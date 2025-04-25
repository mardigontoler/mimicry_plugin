
#pragma once

#include "delay/MultiHeadDelayLine.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "pitch/MultiPhaseVocoder.h"

namespace mimicry {
    // constants
    constexpr int numStereoDelayLines = 16;
    constexpr double MIN_TEMPO = 30.0f; // bpm
    constexpr double MAX_TEMPO = 200.0f;

}
//==============================================================================
/**
*/
class MimicAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    MimicAudioProcessor();
    ~MimicAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    std::unique_ptr<juce::XmlElement> getStateXML();


private:

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioProcessorValueTreeState parameters;
    
//    MultiHeadDelayLine<float>   multiDelayLines;
	std::vector<juce::dsp::DelayLine<float>> mDelayLines;
	size_t mMaxDelayLengthInSamples{};

    MultiPhaseVocoder pitchShifters;

    // audio parameter value pointers
    std::atomic<float>* bpmRawParam = nullptr; // tempo
    std::atomic<float>* tempoSyncParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* divisionParam = nullptr;

    // array of atomic pointers to delay and pitch shift params
    std::vector<std::atomic<float>*> delayGainParams;
    std::vector<std::atomic<float>*> semitoneParams; // value tree will give us float even for int parameters
	std::vector<std::atomic<float>*> feedbackParams;

	std::array<float, mimicry::numStereoDelayLines> nextDelayLineSamples;


	juce::RangedAudioParameter* bpmAudioParam; // so we can set this param internally

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MimicAudioProcessor)
};

