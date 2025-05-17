cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT supported OUTPUT error)
    if(supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(systematize)

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "") # must be set before project()
    set(CMAKE_XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING[variant=Release] "YES")
    set(CMAKE_XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING[variant=MinSizeRel] "YES")
    set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT[variant=Release] "dwarf-with-dsym")
    set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT[variant=MinSizeRel] "dwarf-with-dsym")
    set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT[variant=RelWithDebInfo] "dwarf-with-dsym")
endif()

# Control whether the CTest module invokes enable_testing().
# Set to false here to prevent numerous unneeded ctest targets from being created when including google highway later
set(BUILD_TESTING OFF CACHE BOOL "Disable CTest testing")

set( PROFILE_BUILD OFF CACHE BOOL "Pass -O0 for profiling debug builds on linux with optimizations" )

if (${PROFILE_BUILD})
    message("Enabling -O0 for a profileing build. Use with Debug configuration")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif()


if ( NOT APPLE)
    message("Finding Intel MKL")
    set(MKL_LINK "static")

    # This is needed to prevent a runtime dependency on a threading library like libopenmp5  that would prevent the plugin
    # from loading on systems without it installed
    set(MKL_THREADING "sequential")

    find_package(MKL CONFIG REQUIRED)
    message(STATUS "${MKL_IMPORTED_TARGETS}") #Provides available list of targets based on input
endif()


if (CMAKE_GENERATOR MATCHES "Visual Studio")
    message(STATUS "Enabling parallel builds for MSVC")
    add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
endif()

project(mimicry VERSION 0.1)

set(SRC ../src)
set(LIB ../lib)

# Present some cached options for customizing builds.
# These can be set in the cmake cache without worrying about accidentally committing them.
# example : EXAMPLE_OPTION will cause a variable with that same name to appear in the variable list after configuration.
# Enabling it will create a preprocessor define with the same name
foreach(compOption
    # Add options here
    )
        set(${compOption} OFF CACHE BOOL "")
        if (${compOption})
            add_compile_definitions(${compOption}=1)
        else()
            add_compile_definitions(${compOption}=0)
        endif()
endforeach()


function (setupXCodeAutomaticSigning target)
    if (APPLE)
        set_property (TARGET ${target} PROPERTY XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "")
        set_property (TARGET ${target} PROPERTY XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "-")
        set_property (TARGET ${target} PROPERTY XCODE_ATTRIBUTE_CODE_SIGN_STYLE Automatic)
    endif()
endfunction()

if (WIN32)
    add_compile_definitions(UNICODE=1)
    add_compile_definitions(_UNICODE=1)
    add_compile_options("$<$<CONFIG:Release>:/Zi>")
    add_link_options("$<$<CONFIG:Release>:/DEBUG>")
    add_link_options("$<$<CONFIG:Release>:/OPT:REF>")
    add_link_options("$<$<CONFIG:Release>:/OPT:ICF>")
endif()


if (APPLE)
    add_compile_options("$<$<CONFIG:Release>:-g>")
endif()


set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(LibsFolderName "Libraries")

set(JUCE_PATH "JUCE" CACHE PATH "Path to JUCE")

set(JUCE_COPY_PLUGIN_AFTER_BUILD ON)
set(JUCE_ENABLE_MODULE_SOURCE_GROUPS ON)

message("JUCE_PATH=${JUCE_PATH}")
add_subdirectory(${JUCE_PATH} build_juce)

set(HWY_ENABLE_CONTRIB OFF CACHE BOOL "Include contrib/")
set(HWY_ENABLE_EXAMPLES OFF CACHE BOOL "Build examples")
set(HWY_ENABLE_INSTALL OFF CACHE BOOL "Install library")
set(HWY_ENABLE_TESTS OFF CACHE BOOL "Enable HWY tests")
add_subdirectory(highway build_highway)


juce_add_binary_data(mimicry_binary_data
    SOURCES
        res/CinzelDecorative-Regular.ttf
        res/Segment7-4Gml.otf
        res/Ubuntu-Medium.ttf
        res/inter/InterVariable.ttf
)



juce_add_plugin(
    mimicry
    VERSION ${PROJECT_VERSION}
    COMPANY_NAME "Mardigon Toler"
    BUNDLE_ID "com.mardigontoler.mimicry"
    FORMATS Standalone VST3 AU
    PLUGIN_CODE "mQwp"

    # these seem to be needed to build on linux for now
    NEEDS_WEB_BROWSER TRUE
    NEEDS_CURL TRUE
)


target_sources(mimicry PRIVATE

    src/PluginProcessor.h
    src/PluginProcessor.cpp
    src/PluginEditor.h
    src/PluginEditor.cpp
#    src/delay/MultiHeadDelayLine.h
#    src/delay/StereoMultiHeadDelayLine.h
    src/gui/title/TitleLookAndFeel.h
    src/gui/DelayControllerLookAndFeel.h
    src/gui/DelayHeadController.cpp
    src/gui/DelayHeadController.h
    src/gui/font.h
    src/gui/colors.h
    src/gui/MimicryLookAndFeel.cpp
    src/gui/MimicryLookAndFeel.h
    src/gui/TempoDisplay.h
    src/gui/TempoDisplay.cpp
    src/gui/WhiteLabelLookAndFeel.h
    src/gui/LabeledSlider.h
    src/gui/LabeledSlider.cpp
    src/pitch/MultiPhaseVocoder.cpp
    src/pitch/MultiPhaseVocoder.h
    src/pitch/pitch_functions.h
    src/pitch/pitch_functions.cpp
    src/util/mimicry_util.h
)

target_link_libraries(mimicry
    PRIVATE
        mimicry_binary_data
        juce::juce_audio_utils
        juce::juce_dsp
        hwy
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

if ( NOT APPLE)
    target_link_libraries(mimicry
        PRIVATE
            MKL::MKL
    )
endif()


target_compile_definitions(mimicry
    PRIVATE
        CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        JUCE_DSP_USE_INTEL_MKL=1
)
target_include_directories(mimicry
    PRIVATE
    src/
    src/pitch
)
target_include_directories(mimicry
    SYSTEM PRIVATE
    highway/hwy
)


add_executable(benchmark_phase_correct
    src/benchmark_phase_correct.cpp
)
target_link_libraries(benchmark_phase_correct
    PRIVATE
    hwy
    mimicry
    juce::juce_audio_utils
    juce::juce_dsp
)
target_include_directories(benchmark_phase_correct
    PRIVATE
    src/
    src/pitch
)
target_compile_definitions(benchmark_phase_correct PRIVATE
    JUCE_WEB_BROWSER=0
)

function(resolve_alias_target output_var target_name)
    if(NOT TARGET "${target_name}")
        set(${output_var} "${target_name}" PARENT_SCOPE)
        return()
    endif()

    get_target_property(alias_target "${target_name}" ALIASED_TARGET)
    if(alias_target)
        set(${output_var} "${alias_target}" PARENT_SCOPE)
    else()
        set(${output_var} "${target_name}" PARENT_SCOPE)
    endif()
endfunction()

# suppress warnings when building thirdarty libs
set(THIRDPARTY_TARGETS
    hwy
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)
foreach(TARGET ${THIRDPARTY_TARGETS})
    if(TARGET ${TARGET})
        resolve_alias_target(actual_target "${TARGET}")
#        target_compile_options(${actual_target} PRIVATE "-w")
        systematize(${actual_target})

    endif()
endforeach()

