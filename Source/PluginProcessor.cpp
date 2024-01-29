/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GraphicEQAudioProcessor::GraphicEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

GraphicEQAudioProcessor::~GraphicEQAudioProcessor()
{
}

//==============================================================================
const juce::String GraphicEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GraphicEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GraphicEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GraphicEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GraphicEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GraphicEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GraphicEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GraphicEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GraphicEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void GraphicEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GraphicEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    auto chainSettings = getChainSettings(apvts);
    
    auto band20Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 
                                                                                  chainSettings.band20Freq,
                                                                                  chainSettings.band20Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band20Gain));
    auto band32Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                                  chainSettings.band32Freq,
                                                                                  chainSettings.band32Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band32Gain));
    auto band64Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                                  chainSettings.band64Freq,
                                                                                  chainSettings.band64Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band64Gain));
    auto band125Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                                  chainSettings.band125Freq,
                                                                                  chainSettings.band125Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band125Gain));
    auto band250Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                                  chainSettings.band250Freq,
                                                                                  chainSettings.band250Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band250Gain));
    auto band500Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,
                                                                                  chainSettings.band500Freq,
                                                                                  chainSettings.band500Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band500Gain));
    
    *leftChain.get<ChainPositions::band20>().coefficients = *band20Coefficients;
    *leftChain.get<ChainPositions::band32>().coefficients = *band32Coefficients;
    *leftChain.get<ChainPositions::band64>().coefficients = *band64Coefficients;
    *leftChain.get<ChainPositions::band125>().coefficients = *band125Coefficients;
    *leftChain.get<ChainPositions::band250>().coefficients = *band250Coefficients;
    *leftChain.get<ChainPositions::band500>().coefficients = *band500Coefficients;
    
    *rightChain.get<ChainPositions::band20>().coefficients = *band20Coefficients;
    *rightChain.get<ChainPositions::band32>().coefficients = *band32Coefficients;
    *rightChain.get<ChainPositions::band64>().coefficients = *band64Coefficients;
    *rightChain.get<ChainPositions::band125>().coefficients = *band125Coefficients;
    *rightChain.get<ChainPositions::band250>().coefficients = *band250Coefficients;
    *rightChain.get<ChainPositions::band500>().coefficients = *band500Coefficients;
    
    
    
}

void GraphicEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GraphicEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GraphicEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    auto chainSettings = getChainSettings(apvts);
    
    auto band20Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                  chainSettings.band20Freq,
                                                                                  chainSettings.band20Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band20Gain));
    auto band32Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                  chainSettings.band32Freq,
                                                                                  chainSettings.band32Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band32Gain));
    auto band64Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                  chainSettings.band64Freq,
                                                                                  chainSettings.band64Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band64Gain));
    auto band125Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                  chainSettings.band125Freq,
                                                                                  chainSettings.band125Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band125Gain));
    auto band250Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                  chainSettings.band250Freq,
                                                                                  chainSettings.band250Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band250Gain));
    auto band500Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                  chainSettings.band500Freq,
                                                                                  chainSettings.band500Quality,
                                                                                  juce::Decibels::decibelsToGain(chainSettings.band500Gain));
    
    *leftChain.get<ChainPositions::band20>().coefficients = *band20Coefficients;
    *leftChain.get<ChainPositions::band32>().coefficients = *band32Coefficients;
    *leftChain.get<ChainPositions::band64>().coefficients = *band64Coefficients;
    *leftChain.get<ChainPositions::band125>().coefficients = *band125Coefficients;
    *leftChain.get<ChainPositions::band250>().coefficients = *band250Coefficients;
    *leftChain.get<ChainPositions::band500>().coefficients = *band500Coefficients;
    
    *rightChain.get<ChainPositions::band20>().coefficients = *band20Coefficients;
    *rightChain.get<ChainPositions::band32>().coefficients = *band32Coefficients;
    *rightChain.get<ChainPositions::band64>().coefficients = *band64Coefficients;
    *rightChain.get<ChainPositions::band125>().coefficients = *band125Coefficients;
    *rightChain.get<ChainPositions::band250>().coefficients = *band250Coefficients;
    *rightChain.get<ChainPositions::band500>().coefficients = *band500Coefficients;
    
    
    
    

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    
    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
}

//==============================================================================
bool GraphicEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GraphicEQAudioProcessor::createEditor()
{
    // return new SmileyQAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void GraphicEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GraphicEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings chainSettings;
    
    chainSettings.band20Gain = apvts.getRawParameterValue("Band 20")->load();
    chainSettings.band32Gain = apvts.getRawParameterValue("Band 32")->load();
    chainSettings.band64Gain = apvts.getRawParameterValue("Band 64")->load();
    chainSettings.band125Gain = apvts.getRawParameterValue("Band 125")->load();
    chainSettings.band250Gain = apvts.getRawParameterValue("Band 250")->load();
    chainSettings.band500Gain = apvts.getRawParameterValue("Band 500")->load();
    chainSettings.band1kGain = apvts.getRawParameterValue("Band 1k")->load();
    chainSettings.band2kGain = apvts.getRawParameterValue("Band 2k")->load();
    chainSettings.band4kGain = apvts.getRawParameterValue("Band 4k")->load();
    chainSettings.band8kGain = apvts.getRawParameterValue("Band 8k")->load();
    chainSettings.band16kGain = apvts.getRawParameterValue("Band 16k")->load();
    chainSettings.band20kGain = apvts.getRawParameterValue("Band 20k")->load();
    
    return chainSettings;
}

juce::AudioProcessorValueTreeState::ParameterLayout GraphicEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;
    float rangeStart, rangeEnd, intervalValue, skewFactor, defaultValue;
    std::vector<juce::String> bandNames {"Band 20", "Band 32", "Band 64", "Band 125",
                                        "Band 250", "Band 500", "Band 1k", "Band 2k",
                                        "Band 4k", "Band 8k", "Band 16k", "Band 20k"};
    rangeStart = -12.f;
    rangeEnd = 12.f;
    intervalValue = 0.5f;
    skewFactor = 1.f;
    defaultValue = 0.0f;
    
    // All bands should probably be duplicates with different names... need to figure out
    //  how to set their frequencies when get to DSP
    for (juce::String bandName : bandNames) {
        parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(bandName, 1),
                                                                        bandName,
                                                                        juce::NormalisableRange<float>(rangeStart, rangeEnd, intervalValue, skewFactor),
                                                                        defaultValue));
    }
    
    return parameterLayout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GraphicEQAudioProcessor();
}
