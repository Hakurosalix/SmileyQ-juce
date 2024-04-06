/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GraphicEQAudioProcessorEditor::GraphicEQAudioProcessorEditor (GraphicEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    band20SliderAttachment(audioProcessor.apvts, allBandNames[0], band20Slider),
    band32SliderAttachment(audioProcessor.apvts, allBandNames[1], band32Slider),
    band64SliderAttachment(audioProcessor.apvts, allBandNames[2], band64Slider),
    band125SliderAttachment(audioProcessor.apvts, allBandNames[3], band125Slider),
    band250SliderAttachment(audioProcessor.apvts, allBandNames[4], band250Slider),
    band500SliderAttachment(audioProcessor.apvts, allBandNames[5], band500Slider),
    band1kSliderAttachment(audioProcessor.apvts, allBandNames[6], band1kSlider),
    band2kSliderAttachment(audioProcessor.apvts, allBandNames[7], band2kSlider),
    band4kSliderAttachment(audioProcessor.apvts, allBandNames[8], band4kSlider),
    band8kSliderAttachment(audioProcessor.apvts, allBandNames[9], band8kSlider),
    band16kSliderAttachment(audioProcessor.apvts, allBandNames[10], band16kSlider),
    band20kSliderAttachment(audioProcessor.apvts, allBandNames[11], band20kSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    for (auto* slider : getSliders()) {
        addAndMakeVisible(slider);
    }
    
    setSize (800, 300);
}

GraphicEQAudioProcessorEditor::~GraphicEQAudioProcessorEditor()
{
}

//==============================================================================
void GraphicEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void GraphicEQAudioProcessorEditor::resized()
{
    int yMargin, xMargin, sliderSpace;
    float yMarginMultiplier, xMarginMultiplier;
    yMarginMultiplier = 0.1;
    xMarginMultiplier = 0.075;
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // 12 sliders, evenly spaced...
    auto bounds = getLocalBounds();
    
    yMargin = bounds.getHeight() * yMarginMultiplier;
    xMargin = bounds.getWidth() * xMarginMultiplier;
    
    bounds.removeFromTop(yMargin);
    bounds.removeFromBottom(yMargin);
    bounds.removeFromLeft(xMargin);
    bounds.removeFromRight(xMargin);
    
    sliderSpace = bounds.getWidth() / 12;
    
    for (juce::Component* slider : getSliders()) {
        slider->setBounds(bounds.removeFromLeft(sliderSpace));
    }
    
}

std::vector<juce::Component*> GraphicEQAudioProcessorEditor::getSliders()
{
    return
    {
        &band20Slider,
        &band32Slider,
        &band64Slider,
        &band125Slider,
        &band250Slider,
        &band500Slider,
        &band1kSlider,
        &band2kSlider,
        &band4kSlider,
        &band8kSlider,
        &band16kSlider,
        &band20kSlider
    };
}
