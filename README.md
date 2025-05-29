# mimicry_plugin
A pitch shifted multi-delay plugin


# Dependencies

General:
- [highway](https://github.com/google/highway)
- [JUCE (v 8.0.7)](https://github.com/juce-framework/JUCE)
- [melatonin inspector](https://github.com/sudara/melatonin_inspector)

Windows/Linux:
- [Intel OneMKL](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-download.html)


Highway, melatonin inspector, and JUCE are included in this repository as git submobules.
If you didn't clone the project recursively, you can get these sources by running: 
```bash
git submodule init
git submodule update --recursive
```


On Linux, JUCE requires some development packages to be installed.
For Ubuntu, you can use:
```bash
sudo apt update
sudo apt install libasound2-dev libjack-jackd2-dev \
    ladspa-sdk \
    libcurl4-openssl-dev  \
    libfreetype-dev libfontconfig1-dev \
    libx11-dev libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
    libwebkit2gtk-4.1-dev \
    libglu1-mesa-dev mesa-common-dev
```

For Fedora, which isn't explicitly supported in the JUCE docs, I was able to build after running:
```bash
sudo dnf install alsa-lib-devel pipewire-jack-audio-connection-kit-devel \
    ladspa-devel \
    libcurl-devel \
    freetype-devel fontconfig-devel \
    libX11-devel libXcomposite-devel libXcursor-devel libXext-devel libXinerama-devel libXrandr-devel libXrender-devel \
    webkit2gtk4.1-devel \
    mesa-libGLU-devel mesa-libGL-devel
```


# Building

On Win/Linux, Intel OneMKL needs to be installed (from the link above).
The CMake script should hopefully detect it with `find_package`.
This requires that certain environment variables are set up
from a script provided by intel. 
See [Windows Instructions](https://www.intel.com/content/www/us/en/docs/oneapi/programming-guide/2024-2/use-the-setvars-and-oneapi-vars-scripts-with-linux.html)
and  [Linux instructions](https://www.intel.com/content/www/us/en/docs/oneapi/programming-guide/2024-2/use-the-setvars-and-oneapi-vars-scripts-with-linux.html).

Note that on Linux, when trying to use CMake tools integrated into an IDE like CLion,
it's helpfule to add a toolchain Settings->Build, Execution, and Deployment->Toolchains
that runs the `/opt/intel/oneapi/setvars.sh` environment file.
Then, build configurations can be set to use this toolchain.

JUCE and Highway are included as git submodules. 
You can pull them with
`git submodule update --init`



Generate a build system using cmake (at least version 3.14). 
For example:
```bash
mkdir build
cd build
cmake ..

```

To generate a Release build, use 
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Check cmake docs for more info about selecting a generator.
For exmaple, to use Ninja:
```bash
cmake .. -G Ninja
```

Then build with
```bash
cmake --build . -t mimicry_VST3
```
