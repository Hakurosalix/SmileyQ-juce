/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

// Colors:
// t1: 230u, 195u, 132u
// t2: 149u, 127u, 184u
// t3: 126u, 156u, 216u
// t4: 127u, 180u, 202u
// t5: 122u, 168u, 159u
// t6: 210u, 126u, 153u
// background: 42u, 42u, 55u

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
    //g.setColour (slider.findColour (Slider::backgroundColourId));
    //g.setColour(Colour(130u, 90u, 103u));
    g.setColour(Colour(66u, 56u, 82u));
    g.strokePath (backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });
    
    // Draw notches
    int xOffset = width / 4;
    float yInterval = (startPoint.y - endPoint.y) / 8;
    Rectangle<float> marker = Rectangle<float>(startPoint.x + xOffset, startPoint.y - 0.5, 10, 1);
    for (int j = 0; j < 7; j++) {
        marker.setY(marker.getY() - yInterval);
        g.setColour(Colour(230u, 195u, 132u));
        g.fillRect(marker);
        marker.setX(startPoint.x - xOffset);
        g.fillRect(marker);
        marker.setX(startPoint.x + xOffset);
    }
    
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
    
    // Draw thumb
    g.setColour (Colour(149u, 127u, 184u));
    Rectangle<float> thumb = Rectangle<float> (static_cast<float> (thumbWidth + 5), static_cast<float> (thumbWidth + 15)).withCentre (maxPoint);
    g.fillRect(thumb);
    
    // Thumb grip
    auto bounds = Rectangle<float>(x, y, width, height);
    Rectangle<float> r;
    r.setLeft(thumb.getCentre().getX() - (static_cast<float> (thumbWidth + 5)) / 2);
    r.setRight(thumb.getCentre().getX() + (static_cast<float> (thumbWidth + 5)) / 2);
    r.setTop(thumb.getCentre().getY() - (static_cast<float> (thumbWidth)) / 5);
    r.setBottom(thumb.getCentre().getY() + (static_cast<float> (thumbWidth)) / 5);
    g.setColour(Colours::black);
    g.fillRect(r);
    
    // Draw parameter values
    // If we can cast it, we know we can call member functions:
//    if (auto* cvs = dynamic_cast<CustomVerticalSlider*>(&slider)) {
//        Rectangle<float> k = Rectangle<float> (static_cast<float> (thumbWidth + 5), static_cast<float> (thumbWidth + 15)).withCentre (startPoint);
//        g.setFont(cvs->getTextHeight());
//        auto text = cvs->getDisplayString();
//        auto strWidth = g.getCurrentFont().getStringWidth(text);
//        k.setSize(strWidth + 4, cvs->getTextHeight() + 2);
//        g.fillRect(k);
//        g.setColour(Colours::white);
//        g.drawFittedText(text, k.toNearestInt(), juce::Justification::centred, 1);
//    }
}

juce::String CustomVerticalSlider::getDisplayString() const
{
    return juce::String(getValue());
}

void CustomVerticalSlider::paint(juce::Graphics &g)
{
    using namespace juce;
    
    auto sliderBounds = getSliderBounds();
    
    getLookAndFeel().drawLinearSlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      getPositionOfValue(getValue()),
                                      getPositionOfValue(getMinimum()),
                                      getPositionOfValue(getMaximum()),
                                      getSliderStyle(),
                                      *this);
}

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
    g.fillAll (Colour(42u, 42u, 55u));
    
    auto bounds = getLocalBounds();
    g.setColour(Colours::ghostwhite);
    g.drawRect(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
    
    int yMargin, xMargin, sliderSpace;
    float yMarginMultiplier, xMarginMultiplier;
    yMarginMultiplier = 0.1;
    xMarginMultiplier = 0.075;
    
    yMargin = bounds.getHeight() * yMarginMultiplier;
    xMargin = bounds.getWidth() * xMarginMultiplier;

    bounds.removeFromLeft(xMargin);
    bounds.removeFromRight(xMargin);
    auto parameterTextMargin = bounds.removeFromTop(yMargin);
    bounds.removeFromBottom(yMargin);
    
    sliderSpace = bounds.getWidth() / 12;
    
    for (int i = 0; i < allBandNames.size(); i++) {
        //g.setColour(Colours::black);
        auto sliderTextBounds = parameterTextMargin.removeFromLeft(sliderSpace);
        g.setFont(14);
        auto text = bandLabels[i];
        //auto strWidth = g.getCurrentFont().getStringWidth(text);
        //g.fillRect(sliderTextBounds);
        g.setColour(Colour(230u, 195u, 132u));
        g.drawFittedText(text, sliderTextBounds.toNearestInt(), juce::Justification::centred, 1);
    }
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
    
    for (CustomVerticalSlider* slider : getSliders()) {
        auto sliderBounds = bounds.removeFromLeft(sliderSpace);
        slider->setBounds(sliderBounds);
    }
    
}

std::vector<CustomVerticalSlider*> GraphicEQAudioProcessorEditor::getSliders()
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

