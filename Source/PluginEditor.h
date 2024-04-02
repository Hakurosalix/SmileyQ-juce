/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomVerticalSlider : juce::Slider
{
    CustomVerticalSlider() : juce::Slider(juce::Slider::SliderStyle::LinearVertical,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        
    }
    
};

//==============================================================================
/**
*/
class GraphicEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GraphicEQAudioProcessorEditor (GraphicEQAudioProcessor&);
    ~GraphicEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GraphicEQAudioProcessor& audioProcessor;
    
    CustomVerticalSlider band20Slider,
    band32Slider,
    band64Slider,
    band125Slider,
    band250Slider,
    band500Slider,
    band1kSlider,
    band2kSlider,
    band4kSlider,
    band8kSlider,
    band16kSlider,
    band20kSlider;
    
    std::vector<juce::Component*> getSliders();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEQAudioProcessorEditor)
};
