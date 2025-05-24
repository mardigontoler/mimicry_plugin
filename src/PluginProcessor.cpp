#include "PluginProcessor.h"

#include "detect_targets.h"
#include "pitch_functions.h"
#include "PluginEditor.h"
#include "targets.h"
#include "util/mimicry_util.h"


using namespace mimicry;
using namespace juce;


MimicAudioProcessor::MimicAudioProcessor() : AudioProcessor(
        BusesProperties()
                .withInput("Input", AudioChannelSet::mono(), true)
                .withOutput("Output Dry", AudioChannelSet::mono(), true)
        //.withOutput ("Output Wet", AudioChannelSet::stereo(), true)
),
                                             parameters(*this, nullptr, Identifier("Mimicry"), createParameterLayout()),
                                             pitchShifters(numStereoDelayLines)
{

    const uint32_t supported = hwy::SupportedTargets();
    std::cout << "Supported targets: " << std::hex << supported << std::dec << "\n";

    // Check for specific instruction sets
    if (supported & HWY_SSE2) std::cout << "SSE2 is supported\n";
    if (supported & HWY_SSE4) std::cout << "SSE4 is supported\n";
    if (supported & HWY_AVX2) std::cout << "AVX2 is supported\n";
    if (supported & HWY_AVX3_ZEN4) std::cout << "AVX3_ZEN4 is supported\n";


    for (size_t delayIx = 0; delayIx < numStereoDelayLines; delayIx++)
    {
        mDelayLines.emplace_back();
    }

    // set pointers to raw parameter values
    tempoSyncParam = parameters.getRawParameterValue("tempoSync");
    bpmAudioParam = parameters.getParameter("bpm");
    bpmRawParam = parameters.getRawParameterValue("bpm");
    mixParam = parameters.getRawParameterValue("mix");
    divisionParam = parameters.getRawParameterValue("division");

    for (int i = 0; i < numStereoDelayLines; i++)
    {
        delayGainParams.push_back(parameters.getRawParameterValue(String("rhythmGain") + String(i)));
        semitoneParams.push_back(parameters.getRawParameterValue(String("pitchShift") + String(i)));
        feedbackParams.push_back(parameters.getRawParameterValue(String("feedback") + String(i)));
    }
}


MimicAudioProcessor::~MimicAudioProcessor()
= default;


AudioProcessorValueTreeState::ParameterLayout MimicAudioProcessor::createParameterLayout()
{

    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    // fixed params
    params.push_back(
            std::make_unique<AudioParameterFloat>(
                    "bpm",
                    "BPM",
                    MIN_TEMPO,
                    MAX_TEMPO,
                    120.0f)
    );
    params.push_back(
            std::make_unique<AudioParameterBool>(
                    "tempoSync",
                    "Tempo Sync",
                    false)
    );
    params.push_back(
            std::make_unique<AudioParameterFloat>(
                    "mix",
                    "Mix",
                    0.0f,
                    1.0f,
                    0.5f)
    );
    params.push_back(
            std::make_unique<AudioParameterInt>(
                    "division",
                    "Division",
                    1,
                    16,
                    8)
    );
    params.push_back(
            std::make_unique<AudioParameterFloat>(
                    "outputGain",
                    "Output Gain",
                    NormalisableRange<float>(-60.0f, 24.0f, 1.0f),
                    -60.0f,
                    AudioParameterFloatAttributes().withStringFromValueFunction([](float value, int length)
                                                                                {
                                                                                    String s;
                                                                                    if (value <= -60.0f)
                                                                                        s = String("-inf");
                                                                                    s = String(value, 1) + " dB";
                                                                                    s = s.substring(0, length);
                                                                                    return s;
                                                                                }
                    )
            )
    );


    // dynamic parameters; one for each tap on the delay line.
    // First set up gains for each tap
    for (int i = 0; i < numStereoDelayLines; i++)
    {
        params.push_back(std::make_unique<AudioParameterFloat>(
                String("rhythmGain") + String(i),
                String("Rhythm Gain ") + String(i),
                -60.0f,
                24.0f,
                -60.0f));

        // then set up #semitones for each tap
        params.push_back(std::make_unique<AudioParameterInt>(
                String("pitchShift") + String(i),
                String("Pitch Shift ") + String(i),
                -24,
                24,
                0
        ));

        params.push_back(std::make_unique<AudioParameterFloat>(
                String("feedback") + String(i),
                String("Feedback ") + String(i),
                0.0f,
                1.0f,
                0.0f
        ));
    }

    return {params.begin(), params.end()};
}


//==============================================================================
const String MimicAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MimicAudioProcessor::acceptsMidi() const
{
    return false;
}

bool MimicAudioProcessor::producesMidi() const
{
    return false;
}

bool MimicAudioProcessor::isMidiEffect() const
{
    return false;
}

double MimicAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MimicAudioProcessor::getNumPrograms()
{
    return 1;
}

int MimicAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MimicAudioProcessor::setCurrentProgram(int /*index*/)
{
}

const String MimicAudioProcessor::getProgramName(int /*index*/)
{
    return {};
}

void MimicAudioProcessor::changeProgramName(int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void MimicAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mMaxDelayLengthInSamples = static_cast<size_t>( 10 * sampleRate );

    for (auto& delay: mDelayLines)
    {
        delay.reset();
        dsp::ProcessSpec spec{sampleRate, static_cast<uint32>(samplesPerBlock), 1};
        delay.prepare(spec);
        delay.setMaximumDelayInSamples(static_cast<int>(mMaxDelayLengthInSamples));
    }

    delayLineSamples = std::vector<std::vector<float>>(
            numStereoDelayLines,
            std::vector<float>(
                    static_cast<size_t>(samplesPerBlock),
                    0));
}

void MimicAudioProcessor::releaseResources()
{

}


bool MimicAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{

    const AudioChannelSet& mainInputChannelSet = layouts.getMainInputChannelSet();
    const AudioChannelSet& mainOutputChannelSet = layouts.getMainOutputChannelSet();

    // only support mono in and out
    if (mainInputChannelSet != AudioChannelSet::mono())
        return false;

    // either way, all buses must have same layout
    return mainInputChannelSet == mainOutputChannelSet;
}


void MimicAudioProcessor::processBlock(AudioBuffer<float>& ioAudioBuffer, MidiBuffer& /*midiMessages*/)
{
#if JUCE_DEBUG
    return;
#else
    ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto bufferSize = static_cast<size_t>(ioAudioBuffer.getNumSamples());

    static int blockCounter = -1;
    blockCounter = (blockCounter + 1) % 100;
    const bool timedDebug = blockCounter == 0;

    const bool tempoSync = *tempoSyncParam > 0.5f;
    const float mix = *mixParam;
    const double sampleRate = getSampleRate();

    const double bpmParamVal = *bpmRawParam;

    double bpm = bpmParamVal;

    if (tempoSync) {

        std::optional<double> bpmFromPlayhead = std::nullopt;

        if (const AudioPlayHead* audioPlayHead = getPlayHead(); audioPlayHead != nullptr) {
            if (auto optPlayheadPosition = audioPlayHead->getPosition(); optPlayheadPosition.hasValue())
            {
                if (auto optPlayheadBpm = optPlayheadPosition->getBpm(); optPlayheadBpm.hasValue())
                {
                    // set the bpm parameter directly to communicate with the editor
                    bpmFromPlayhead = *optPlayheadBpm;
                }
            }
        }

        if (bpmFromPlayhead.has_value())
        {
            bpm = *bpmFromPlayhead;
        }
        else
        {
            bpm = 120.0f; // default if trying to sync in a host that doesn't support it
        }
    }

    const float beatDivider = *divisionParam;

    // calculate delay size from the tempo and set all the delay lines
    const size_t samplesPerSubdivision = mimicry_util::getSamplesPerSubdivision(bpm, sampleRate, 1.0f/beatDivider);

    //if (timedDebug)DBG("tempo sync " << (tempoSync ? "true" : "false"));
    if (timedDebug)
    {
        //getStateXML();
    }

    jassert(nextDelayLineSamples.size() == mDelayLines.size());

    for (size_t ix = 0; ix < mDelayLines.size(); ix++) {
        auto& delay = mDelayLines[ix];

        size_t delaySamples =  ix * samplesPerSubdivision;
        delaySamples = std::clamp<size_t>(delaySamples, 0, mMaxDelayLengthInSamples);

        delay.setDelay(static_cast<float>(delaySamples));

        const auto semitones = static_cast<int>(*semitoneParams[ix]);
        pitchShifters.setPitchShiftSemitones(ix, static_cast<float>(semitones));
    }


    for (auto channel = 0; channel < totalNumInputChannels; channel++) {
        if (channel == 0) { // limit to mono
            for (size_t i = 0; i < bufferSize; i++) {

                const float rawInputSample = ioAudioBuffer.getSample(channel, static_cast<int>(i));

                // feedback delay line
                float inputWithFeedback = rawInputSample;
                for (size_t headIndex = 0; headIndex < mDelayLines.size(); headIndex++)
                {
                    auto& delay = mDelayLines[headIndex];
                    const auto delayLineSample = delay.popSample(channel);
                    delayLineSamples[headIndex][i] = delayLineSample;
                    inputWithFeedback += delayLineSample * *(feedbackParams[headIndex]);
                }

                pitchShifters.pushSample(inputWithFeedback);

                float summedDelayLinesSample = 0;
                for (size_t headIndex = 0; headIndex < mDelayLines.size(); headIndex++)
                {
                    // feed each pitch shifter output into the corresponding delay line
                    auto& delay = mDelayLines[headIndex];

                    const auto nextPitchShifterSample = pitchShifters.nextSample(headIndex);

                    delay.pushSample(channel, nextPitchShifterSample);

                    auto delayedSample = delayLineSamples[headIndex][i];
                    const auto gain = Decibels::decibelsToGain<float>(*(delayGainParams[headIndex]), -60.0f);
                    delayedSample *= gain;

                    summedDelayLinesSample += delayedSample;
                }

                const float mixedSample = ((1 - mix) * inputWithFeedback) + (mix * summedDelayLinesSample);
                ioAudioBuffer.setSample(channel, static_cast<int>(i), mixedSample);
            }
        }
        else {
            //buffer.clear(channel, 0, buffer.getNumSamples());
        }
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        ioAudioBuffer.clear (i, 0, ioAudioBuffer.getNumSamples());

#endif

}


//==============================================================================
bool MimicAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* MimicAudioProcessor::createEditor()
{
    return new MimicAudioProcessorEditor(*this, parameters);
}


std::unique_ptr<XmlElement> MimicAudioProcessor::getStateXML()
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xmlState(state.createXml());
    xmlState->setAttribute("version", 1);
#if JUCE_DEBUG
    // DBG("serializing state");
    // DBG(xmlState->toString());
#endif
    return xmlState;
}

//==============================================================================
void MimicAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // host calls this to get the state
    std::unique_ptr<XmlElement> stateXml = getStateXML();
    copyXmlToBinary(*stateXml, destData);
}

void MimicAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // host calls this to load state
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
#if JUCE_DEBUG
    // DBG("loading state");
    // DBG(xmlState->toString());
#endif
    if (xmlState != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            ValueTree valueTree = ValueTree::fromXml(*xmlState);

            parameters.replaceState(valueTree);
        }
}


AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MimicAudioProcessor();
}
