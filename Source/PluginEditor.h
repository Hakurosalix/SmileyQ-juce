/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawLinearSlider (juce::Graphics&,
                                int x, int y, int width, int height,
                                float sliderPos,
                                float minSliderPos,
                                float maxSliderPos,
                                juce::Slider::SliderStyle,
                                juce::Slider&) override;
};

struct CustomVerticalSlider : juce::Slider
{
public:
    CustomVerticalSlider(juce::RangedAudioParameter& rap) :
                                        juce::Slider(juce::Slider::SliderStyle::LinearVertical,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap)
    {
        setLookAndFeel(&lnf);
    }
    
    ~CustomVerticalSlider()
    {
        setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const {return 14;}
    juce::String getDisplayString() const;
    
private:
    LookAndFeel lnf;
    juce::RangedAudioParameter* param;
    juce::String const displayString {"Hz"};
    
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
    
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    Attachment band20SliderAttachment,
                band32SliderAttachment,
                band64SliderAttachment,
                band125SliderAttachment,
                band250SliderAttachment,
                band500SliderAttachment,
                band1kSliderAttachment,
                band2kSliderAttachment,
                band4kSliderAttachment,
                band8kSliderAttachment,
                band16kSliderAttachment,
                band20kSliderAttachment;
    
    std::vector<CustomVerticalSlider*> getSliders();
    
    std::vector<juce::String> const bandLabels {"20", "32", "64", "125",
                                        "250", "500", "1k", "2k",
                                        "4k", "8k", "16k", "20k"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicEQAudioProcessorEditor)
};
