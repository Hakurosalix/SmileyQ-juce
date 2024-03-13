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
    
    updatePeakFilters(chainSettings);
    
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
    
    updatePeakFilters(chainSettings);
    
    
    

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
    
    // Personal Note: This and setStateInformation() below are what enable the plugin parameters to be saved when not opened!
    
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void GraphicEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
        
        auto chainSettings = getChainSettings(apvts);
        updatePeakFilters(chainSettings);
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings chainSettings;
    
//    chainSettings.band20Gain = apvts.getRawParameterValue("Band 20")->load();
//    chainSettings.band32Gain = apvts.getRawParameterValue("Band 32")->load();
//    chainSettings.band64Gain = apvts.getRawParameterValue("Band 64")->load();
//    chainSettings.band125Gain = apvts.getRawParameterValue("Band 125")->load();
//    chainSettings.band250Gain = apvts.getRawParameterValue("Band 250")->load();
//    chainSettings.band500Gain = apvts.getRawParameterValue("Band 500")->load();
//    chainSettings.band1kGain = apvts.getRawParameterValue("Band 1k")->load();
//    chainSettings.band2kGain = apvts.getRawParameterValue("Band 2k")->load();
//    chainSettings.band4kGain = apvts.getRawParameterValue("Band 4k")->load();
//    chainSettings.band8kGain = apvts.getRawParameterValue("Band 8k")->load();
//    chainSettings.band16kGain = apvts.getRawParameterValue("Band 16k")->load();
//    chainSettings.band20kGain = apvts.getRawParameterValue("Band 20k")->load();
    
    for (int i = 0; i < chainSettings.bandGains.size(); ++i) {
        chainSettings.bandGains[i] = apvts.getRawParameterValue(bandNames[i])->load();
    }
    
    return chainSettings;
}

void GraphicEQAudioProcessor::updatePeakFilters(const ChainSettings &chainSettings)
{
    using Coefficients = Filter::CoefficientsPtr;
    std::vector<Coefficients> bandCoefficients;
    
    for (int i = 0; i < bandNames.size(); ++i) {
        bandCoefficients.push_back(juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                    chainSettings.bandFreqs[i],
                                                                                    chainSettings.bandQualities[i],
                                                                                    juce::Decibels::decibelsToGain(chainSettings.bandGains[i])));
    }
    
    *leftChain.get<ChainPositions::band20>().coefficients = *bandCoefficients[0];
    *leftChain.get<ChainPositions::band32>().coefficients = *bandCoefficients[1];
    *leftChain.get<ChainPositions::band64>().coefficients = *bandCoefficients[2];
    *leftChain.get<ChainPositions::band125>().coefficients = *bandCoefficients[3];
    *leftChain.get<ChainPositions::band250>().coefficients = *bandCoefficients[4];
    *leftChain.get<ChainPositions::band500>().coefficients = *bandCoefficients[5];
    *leftChain.get<ChainPositions::band1k>().coefficients = *bandCoefficients[6];
    *leftChain.get<ChainPositions::band2k>().coefficients = *bandCoefficients[7];
    *leftChain.get<ChainPositions::band4k>().coefficients = *bandCoefficients[8];
    *leftChain.get<ChainPositions::band8k>().coefficients = *bandCoefficients[9];
    *leftChain.get<ChainPositions::band16k>().coefficients = *bandCoefficients[10];
    *leftChain.get<ChainPositions::band20k>().coefficients = *bandCoefficients[11];
    
    *rightChain.get<ChainPositions::band20>().coefficients = *bandCoefficients[0];
    *rightChain.get<ChainPositions::band32>().coefficients = *bandCoefficients[1];
    *rightChain.get<ChainPositions::band64>().coefficients = *bandCoefficients[2];
    *rightChain.get<ChainPositions::band125>().coefficients = *bandCoefficients[3];
    *rightChain.get<ChainPositions::band250>().coefficients = *bandCoefficients[4];
    *rightChain.get<ChainPositions::band500>().coefficients = *bandCoefficients[5];
    *rightChain.get<ChainPositions::band1k>().coefficients = *bandCoefficients[6];
    *rightChain.get<ChainPositions::band2k>().coefficients = *bandCoefficients[7];
    *rightChain.get<ChainPositions::band4k>().coefficients = *bandCoefficients[8];
    *rightChain.get<ChainPositions::band8k>().coefficients = *bandCoefficients[9];
    *rightChain.get<ChainPositions::band16k>().coefficients = *bandCoefficients[10];
    *rightChain.get<ChainPositions::band20k>().coefficients = *bandCoefficients[11];
    
}

juce::AudioProcessorValueTreeState::ParameterLayout GraphicEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;
    float rangeStart, rangeEnd, intervalValue, skewFactor, defaultValue;
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
