/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct ChainSettings {
    float band20Gain {0}, band32Gain {0}, band64Gain {0}, band125Gain {0},
    band250Gain {0}, band500Gain {0}, band1kGain {0}, band2kGain {0},
    band4kGain {0}, band8kGain {0}, band16kGain {0}, band20kGain {0};
    
    float band20Freq {20.f}, band32Freq {32.f}, band64Freq {64.f}, band125Freq {125.f},
    band250Freq {250.f}, band500Freq {500.f}, band1kFreq {1000.f}, band2kFreq {2000.f},
    band4kFreq {4000.f}, band8kFreq {8000.f}, band16kFreq {16000.f}, band20kFreq {20000.f};
    
    float band20Quality {1.0f}, band32Quality {1.0f}, band64Quality {1.0f}, band125Quality {1.0f},
    band250Quality {0.5f}, band500Quality {0.5f}, band1kQuality {0.5f}, band2kQuality {1.0f},
    band4kQuality {1.0f}, band8kQuality {1.0f}, band16kQuality {1.0f}, band20kQuality {1.0f};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

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
    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter, Filter>;
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
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEQAudioProcessor)
};
