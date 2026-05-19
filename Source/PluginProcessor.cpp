/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define MAX_DELAY_MS 30.0f

//==============================================================================
NafStereoSeparatorAudioProcessor::NafStereoSeparatorAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ), 
    apvts(*this, nullptr, "PARAMS", createParameterLayout())
{
    bypassParam = apvts.getRawParameterValue("bypass");
    widthParam = apvts.getRawParameterValue("width");

    e1Param = apvts.getRawParameterValue("e1");
    e2Param = apvts.getRawParameterValue("e2");
    e3Param = apvts.getRawParameterValue("e3");
    e4Param = apvts.getRawParameterValue("e4");
    e5Param = apvts.getRawParameterValue("e5");

    d1Param = apvts.getRawParameterValue("d1");
    d2Param = apvts.getRawParameterValue("d2");
    d3Param = apvts.getRawParameterValue("d3");
    d4Param = apvts.getRawParameterValue("d4");
    d5Param = apvts.getRawParameterValue("d5");

    g1Param = apvts.getRawParameterValue("g1");
    g2Param = apvts.getRawParameterValue("g2");
    g3Param = apvts.getRawParameterValue("g3");
    g4Param = apvts.getRawParameterValue("g4");
    g5Param = apvts.getRawParameterValue("g5");

    apvts.addParameterListener("d1", this);
    apvts.addParameterListener("d2", this);
    apvts.addParameterListener("d3", this);
    apvts.addParameterListener("d4", this);
    apvts.addParameterListener("d5", this);

    apvts.addParameterListener("g1", this);
    apvts.addParameterListener("g2", this);
    apvts.addParameterListener("g3", this);
    apvts.addParameterListener("g4", this);
    apvts.addParameterListener("g5", this);

    apvts.addParameterListener("e1", this);
    apvts.addParameterListener("e2", this);
    apvts.addParameterListener("e3", this);
    apvts.addParameterListener("e4", this);
    apvts.addParameterListener("e5", this);
}

NafStereoSeparatorAudioProcessor::~NafStereoSeparatorAudioProcessor()
{
    apvts.removeParameterListener("d1", this);
    apvts.removeParameterListener("d2", this);
    apvts.removeParameterListener("d3", this);
    apvts.removeParameterListener("d4", this);
    apvts.removeParameterListener("d5", this);

    apvts.removeParameterListener("g1", this);
    apvts.removeParameterListener("g2", this);
    apvts.removeParameterListener("g3", this);
    apvts.removeParameterListener("g4", this);
    apvts.removeParameterListener("g5", this);

    apvts.removeParameterListener("e1", this);
    apvts.removeParameterListener("e2", this);
    apvts.removeParameterListener("e3", this);
    apvts.removeParameterListener("e4", this);
    apvts.removeParameterListener("e5", this);
}

//==============================================================================
const juce::String NafStereoSeparatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NafStereoSeparatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NafStereoSeparatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NafStereoSeparatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NafStereoSeparatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NafStereoSeparatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NafStereoSeparatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NafStereoSeparatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NafStereoSeparatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void NafStereoSeparatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}


void NafStereoSeparatorAudioProcessor::reset()
{
}

//==============================================================================
void NafStereoSeparatorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    decorrelator.prepare(sampleRate, samplesPerBlock, MAX_DELAY_MS);

    decorrelator.resetSize(
        d1Param->load(), d2Param->load(), d3Param->load(), d4Param->load(), d5Param->load(),
        g1Param->load(), g2Param->load(), g3Param->load(), g4Param->load(), g5Param->load());

    decorrelator.resetEnabled(
        e1Param->load() != 0.0, e2Param->load() != 0.0, e3Param->load() != 0.0, e4Param->load() != 0.0, e5Param->load() != 0.0);
}

void NafStereoSeparatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Clear extra outputs, just in case
    for (int ch = totalNumInputChannels; ch < totalNumOutputChannels; ++ch)
    {
        buffer.clear(ch, 0, numSamples);
    }

    if (delayOrGainChanged.exchange(false, std::memory_order_acq_rel))
    {
        decorrelator.resetSize(
            d1Param->load(), d2Param->load(), d3Param->load(), d4Param->load(), d5Param->load(),
            g1Param->load(), g2Param->load(), g3Param->load(), g4Param->load(), g5Param->load());
    }

    if (enabledChanged.exchange(false, std::memory_order_acq_rel))
    {
        decorrelator.resetEnabled(
            e1Param->load() != 0.0, e2Param->load() != 0.0, e3Param->load() != 0.0, e4Param->load() != 0.0, e5Param->load() != 0.0);
    }

    auto* in0Buffer = buffer.getReadPointer(0);
    auto* in1Buffer = (totalNumInputChannels > 1) ? buffer.getReadPointer(1) : nullptr;
    auto* outLBuffer = buffer.getWritePointer(0);
    auto* outRBuffer = buffer.getWritePointer(1);

    const bool bypass = bypassParam->load() != 0.0f;

    if (bypass)
    {
        juce::FloatVectorOperations::copy(outLBuffer, in0Buffer, numSamples);
        juce::FloatVectorOperations::copy(outRBuffer, in1Buffer != nullptr ? in1Buffer : in0Buffer, numSamples);

        return;
    } 

    const float width = widthParam->load();

    for (int i = 0; i < numSamples; ++i)
    {
        // Support stereo in
        const float x = (in1Buffer != nullptr) ? 0.5f * (in0Buffer[i] + in1Buffer[i]) : in0Buffer[i];

        // Decorrelated signal
        const float d = decorrelator.process(x);

        // x +/- d keeps mono sum close to x
        outLBuffer[i] = x + width * d;
        outRBuffer[i] = x - width * d;
    }
}

void NafStereoSeparatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool NafStereoSeparatorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Always outputs stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    {
        return false;
    }

    // Accept mono input OR stereo input
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
    {
        return false;
    }

    return true;
}

//==============================================================================
bool NafStereoSeparatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NafStereoSeparatorAudioProcessor::createEditor()
{
    return new NafStereoSeparatorAudioProcessorEditor (*this);
}

//==============================================================================
void NafStereoSeparatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml && xml->hasTagName(apvts.state.getType()))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}


void NafStereoSeparatorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NafStereoSeparatorAudioProcessor();
}

void NafStereoSeparatorAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    // The thing that sets shouldResetDelay = true (e.g. parameterChanged() or a UI callback) runs on the message thread (or sometimes a host/parameter thread).
    // The place you consume it and actually clear / reset buffers runs in processBlock() on the realtime audio thread.

    if (parameterID == "d1" || parameterID == "d2" || parameterID == "d3" || parameterID == "d4" || parameterID == "d5" ||
        parameterID == "g1" || parameterID == "g2" || parameterID == "g3" || parameterID == "g4" || parameterID == "g5")
    {
        delayOrGainChanged.store(true, std::memory_order_release);
    }

    if (parameterID == "e1" || parameterID == "e2" || parameterID == "e3" || parameterID == "e4" || parameterID == "e5")
    {
        enabledChanged.store(true, std::memory_order_release);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout NafStereoSeparatorAudioProcessor::createParameterLayout()
{
    using APVTS = juce::AudioProcessorValueTreeState;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "width", "Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "e1", "Enabled 1", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "e2", "Enabled 2", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "e3", "Enabled 3", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "e4", "Enabled 4", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "e5", "Enabled 5", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "d1", "Delay 1",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.5f),
        1.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "d2", "Delay 2",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.5f),
        2.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "d3", "Delay 3",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.5f),
        3.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "d4", "Delay 4",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.5f),
        0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "d5", "Delay 5",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.5f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "g1", "Gain 1",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.65f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "g2", "Gain 2",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.70f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "g3", "Gain 3",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.65f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "g4", "Gain 4",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "g5", "Gain 5",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));

    return { params.begin(), params.end() };
}