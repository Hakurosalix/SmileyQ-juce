/*
  ==============================================================================

    This file contains the framework code for a JUCE Graphic EQ plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ChainSettings {
    std::vector<float> bandGains {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<float> bandFreqs {20.f, 32.f, 64.f, 125.f, 250.f, 500.f, 1000.f, 2000.f, 4000.f, 8000.f, 16000.f, 20000.f};
    std::vector<float> bandQualities {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

static std::vector<juce::String> allBandNames {"Band 20", "Band 32", "Band 64", "Band 125",
                                    "Band 250", "Band 500", "Band 1k", "Band 2k",
                                    "Band 4k", "Band 8k", "Band 16k", "Band 20k"};

//==============================================================================
/**
*/
class GraphicEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GraphicEQAudioProcessor();
    ~GraphicEQAudioProcessor() override;

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
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    
    // Specific to cut filters with 4 options for filters i.e. 12 dB/oct, 24 dB/oct, etc
    // using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    
    // Mono Chain represents our mono signal path
    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter>;
    // Need 2 for stereo
    MonoChain leftChain, rightChain;
    
    // needed for indices of chain parameters when updating chain values in DSP
    enum ChainPositions
    {
        band20,
        band32,
        band64,
        band125,
        band250,
        band500,
        band1k,
        band2k,
        band4k,
        band8k,
        band16k,
        band20k
    };
    
    void updatePeakFilters(const ChainSettings& chainSettings);
    
    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Filter::CoefficientsPtr& old, const Coefficients& replacements);
    static void updateChainCoefficients(MonoChain& monoChain, std::vector<Coefficients>& bandCoefficients);
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEQAudioProcessor)
};
