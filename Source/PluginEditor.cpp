/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GraphicEQAudioProcessorEditor::GraphicEQAudioProcessorEditor (GraphicEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
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
    int topMargin, sideMargin, sliderSpace;
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    
    // 12 sliders, evenly spaced...

    
    auto bounds = getLocalBounds();
    
    topMargin = bounds.getHeight() * 0.1;
    sideMargin = bounds.getWidth() * 0.075;
    
    bounds.removeFromTop(topMargin);
    bounds.removeFromBottom(topMargin);
    bounds.removeFromLeft(sideMargin);
    bounds.removeFromRight(sideMargin);
    
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
