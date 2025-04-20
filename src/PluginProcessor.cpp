#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "util/mimicry_util.h"


using namespace mimicry;
using namespace juce;


MimicAudioProcessor::MimicAudioProcessor(): AudioProcessor (
    BusesProperties()
        .withInput  ("Input",  AudioChannelSet::mono(), true)
        .withOutput ("Output Dry", AudioChannelSet::mono(), true)
        //.withOutput ("Output Wet", AudioChannelSet::stereo(), true)
    ),
    parameters(*this, nullptr, Identifier("Mimicry"), createParameterLayout()),
    multiDelayLines(numStereoDelayLines)

{
    // set pointers to raw parameter values
    tempoSyncParam = parameters.getRawParameterValue("tempoSync");
    bpmAudioParam = parameters.getParameter("bpm");
    bpmRawParam = parameters.getRawParameterValue("bpm");
    mixParam = parameters.getRawParameterValue("mix");
    divisionParam = parameters.getRawParameterValue("division");

    for (int i = 0; i < numStereoDelayLines; i++) {
        delayGainParams.push_back(parameters.getRawParameterValue(String("rhythmGain") + String(i)));
        semitoneParams.push_back(parameters.getRawParameterValue(String("pitchShift") + String(i)));
    }
}

MimicAudioProcessor::~MimicAudioProcessor()
= default;



AudioProcessorValueTreeState::ParameterLayout MimicAudioProcessor::createParameterLayout(){

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
            2)
    );


    // dynamic parameters; one for each tap on the delay line.
    // First set up gains for each tap
    for (int i = 0; i < numStereoDelayLines; i++) {
        params.push_back(std::make_unique<AudioParameterFloat>(
            String("rhythmGain") + String(i),
            String("Rhythm Gain ") + String(i),
            0.0f,
            1.0f,
            0.0f));

        // then set up #semitones for each tap
        params.push_back(std::make_unique<AudioParameterInt>(
            String("pitchShift") + String(i),
            String("Pitch Shift ") + String(i),
            -24,
            24,
            0
            ));
    }

    return { params.begin(), params.end() };
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

void MimicAudioProcessor::setCurrentProgram (int index)
{
}

const String MimicAudioProcessor::getProgramName (int index)
{
    return {};
}

void MimicAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MimicAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const auto maxDelayLength = static_cast<size_t>( 5.0 * sampleRate ); // TODO update this

    multiDelayLines.resize(maxDelayLength);
    for (size_t i = 0; i < multiDelayLines.getNumHeads(); i++) {
        multiDelayLines.clear();
    }

    //stereoDelayLines[3].setEnabled(true);

}

void MimicAudioProcessor::releaseResources()
{
    
}


bool MimicAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{

    const AudioChannelSet& mainInputChannelSet = layouts.getMainInputChannelSet();
    const AudioChannelSet& mainOutputChannelSet = layouts.getMainOutputChannelSet();

    // only support mono in and out
    if (mainInputChannelSet != AudioChannelSet::mono())
        return false;
    
    // either way, all buses must have same layout
    return mainInputChannelSet == mainOutputChannelSet;
}




void MimicAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto bufferSize = buffer.getNumSamples();

    static int blockCounter = -1;
    blockCounter = (blockCounter + 1) % 100;
    const bool timedDebug = blockCounter == 0;

    const bool tempoSync = static_cast<bool>(*tempoSyncParam);
    const float mix = *mixParam;
    const double sampleRate = getSampleRate();

    double bpm = *bpmRawParam;


    if (tempoSync) {

        std::optional<double> bpmFromPlayhead = std::nullopt;

        AudioPlayHead* playHead = getPlayHead();
        if (playHead != nullptr) {
            if (auto optPlayheadPosition = playHead->getPosition(); optPlayheadPosition.hasValue())
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

        bpmAudioParam->setValueNotifyingHost(static_cast<float>(bpm / MAX_TEMPO));

    }

    const float beatDivider = *divisionParam;

    // calculate delay size from the tempo and set all the delay lines
    const long samplesPerSubdivision = mimicry_util::getSamplesPerSubdivision(bpm, sampleRate, 1.0f/beatDivider);

    //if (timedDebug)DBG("tempo sync " << (tempoSync ? "true" : "false"));
    if (timedDebug)
    {
        //getStateXML();
    }

    // update the delay lines with parameter values
    for (size_t i = 0; i < multiDelayLines.getNumHeads(); i++) {
        //
        multiDelayLines.setNumDelaySamples(i, i * samplesPerSubdivision, static_cast<size_t>(sampleRate));
        const float gain = *(delayGainParams[i]);
        multiDelayLines.setGain(i, gain);

        const int semitones = static_cast<int>(*semitoneParams[i]);
        pitchShifters[i].setPitchShiftSemitones(static_cast<float>(semitones));
    }


    // get the next block of delayed samples from the delay lines.
    // set the right and left channels from the right and left delay lines
    for (auto channel = 0; channel < totalNumInputChannels; channel++) {
        if (channel == 0) { // limit to mono
            for (auto i = 0; i < bufferSize; i++) {
                float inputSample = buffer.getSample(channel, i);
                multiDelayLines.pushNextSample(inputSample);
                float summedDelayLinesSample = 0;
                for (int headIndex = 0; headIndex < multiDelayLines.getNumHeads(); headIndex++){


                    // route delay head into the associated pitch shifter
                    int pitchShifterIndex = headIndex;
                    pitchShifters[pitchShifterIndex].pushSample(multiDelayLines.getNextDelayedSample(headIndex));
                    //summedDelayLinesSample += multiDelayLines.getNextDelayedSample(headIndex);

                }
                // now get the next sample from each pitch shifter and add to sum
                for(auto & pitchShifter : pitchShifters){
                    summedDelayLinesSample += pitchShifter.nextSample();
                }
                const float mixedSample = ((1 - mix) * inputSample) + (mix * summedDelayLinesSample);
                buffer.setSample(channel, i, mixedSample);
            }
        }
        else {
            //buffer.clear(channel, 0, buffer.getNumSamples());
        }
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

}





//==============================================================================
bool MimicAudioProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* MimicAudioProcessor::createEditor()
{
    return new MimicAudioProcessorEditor (*this, parameters);
}


std::unique_ptr<XmlElement> MimicAudioProcessor::getStateXML() {
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xmlState(state.createXml());
    xmlState->setAttribute("version", 1);
    #if JUCE_DEBUG
    DBG("serializing state");
    DBG(xmlState->toString());
    #endif
    return xmlState;
}

//==============================================================================
void MimicAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // host calls this to get the state
    std::unique_ptr<XmlElement> stateXml = getStateXML();
    copyXmlToBinary(*stateXml, destData);
}

void MimicAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // host calls this to load state
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    #if JUCE_DEBUG
    DBG("loading state");
    DBG(xmlState->toString());
    #endif
    if (xmlState != nullptr)
        if (xmlState->hasTagName(parameters.state.getType())) {
            ValueTree valueTree = ValueTree::fromXml(*xmlState);

            parameters.replaceState(valueTree);
        }
}


AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MimicAudioProcessor();
}
