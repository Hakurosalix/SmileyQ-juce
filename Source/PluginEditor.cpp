/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawLinearSlider(juce::Graphics &g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPos,
                                   float minSliderPos,
                                   float maxSliderPos,
                                   juce::Slider::SliderStyle style,
                                   juce::Slider &slider)
{
    using namespace juce;

    auto trackWidth = jmin (6.0f, (float) width * 0.25f);

    Point<float> startPoint ((float) x + (float) width * 0.5f,
                             (float) (height + y));

    Point<float> endPoint (startPoint.x,
                            (float) y);

    Path backgroundTrack;
    backgroundTrack.startNewSubPath (startPoint);
    backgroundTrack.lineTo (endPoint);
    g.setColour (slider.findColour (Slider::backgroundColourId));
    g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

    Path valueTrack;
    Point<float> minPoint, maxPoint;

    auto kx = ((float) x + (float) width * 0.5f);
    auto ky = sliderPos;

    minPoint = startPoint;
    maxPoint = { kx, ky };

    auto thumbWidth = getSliderThumbRadius (slider);

    valueTrack.startNewSubPath (minPoint);
    valueTrack.lineTo (maxPoint);
    g.setColour (slider.findColour (Slider::trackColourId));
    g.strokePath (valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

    g.setColour (slider.findColour (Slider::thumbColourId));
    g.fillRect(Rectangle<float> (static_cast<float> (thumbWidth + 5), static_cast<float> (thumbWidth + 15)).withCentre (maxPoint));
}

//void CustomVerticalSlider::paint(juce::Graphics &g)
//{
//    using namespace juce;
//    
//    auto sliderBounds = getSliderBounds();
//    getLookAndFeel().drawLinearSlider(g,
//                                      sliderBounds.getX(),
//                                      sliderBounds.getY(),
//                                      sliderBounds.getWidth(),
//                                      sliderBounds.getHeight(),
//                                      getPositionOfValue(getValue()),
//                                      getPositionOfValue(getMinimum()),
//                                      getPositionOfValue(getMaximum()),
//                                      getSliderStyle(),
//                                      *this);
//}

juce::Rectangle<int> CustomVerticalSlider::getSliderBounds() const
{
    return getLocalBounds();
}
//==============================================================================
GraphicEQAudioProcessorEditor::GraphicEQAudioProcessorEditor (GraphicEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    band20Slider(*audioProcessor.apvts.getParameter(allBandNames[0])),
    band32Slider(*audioProcessor.apvts.getParameter(allBandNames[1])),
    band64Slider(*audioProcessor.apvts.getParameter(allBandNames[2])),
    band125Slider(*audioProcessor.apvts.getParameter(allBandNames[3])),
    band250Slider(*audioProcessor.apvts.getParameter(allBandNames[4])),
    band500Slider(*audioProcessor.apvts.getParameter(allBandNames[5])),
    band1kSlider(*audioProcessor.apvts.getParameter(allBandNames[6])),
    band2kSlider(*audioProcessor.apvts.getParameter(allBandNames[7])),
    band4kSlider(*audioProcessor.apvts.getParameter(allBandNames[8])),
    band8kSlider(*audioProcessor.apvts.getParameter(allBandNames[9])),
    band16kSlider(*audioProcessor.apvts.getParameter(allBandNames[10])),
    band20kSlider(*audioProcessor.apvts.getParameter(allBandNames[11])),
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
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);
    
    auto bounds = getLocalBounds();
    g.setColour(Colours::ghostwhite);
    g.drawRect(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
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
