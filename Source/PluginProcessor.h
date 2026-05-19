/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Builds/VisualStudio2022/Decorrelator.cpp"

//==============================================================================
/**
*/
class NafStereoSeparatorAudioProcessor : public juce::AudioProcessor, private juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    NafStereoSeparatorAudioProcessor();
    ~NafStereoSeparatorAudioProcessor() override;

    //==============================================================================

    void reset() override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

    //==============================================================================
    std::atomic<float>* bypassParam = nullptr;
    std::atomic<float>* widthParam = nullptr;

    std::atomic<float>* e1Param = nullptr;
    std::atomic<float>* e2Param = nullptr;
    std::atomic<float>* e3Param = nullptr;
    std::atomic<float>* e4Param = nullptr;
    std::atomic<float>* e5Param = nullptr;

    std::atomic<float>* d1Param = nullptr;
    std::atomic<float>* d2Param = nullptr;
    std::atomic<float>* d3Param = nullptr;
    std::atomic<float>* d4Param = nullptr;
    std::atomic<float>* d5Param = nullptr;

    std::atomic<float>* g1Param = nullptr;
    std::atomic<float>* g2Param = nullptr;
    std::atomic<float>* g3Param = nullptr;
    std::atomic<float>* g4Param = nullptr;
    std::atomic<float>* g5Param = nullptr;

    juce::AudioProcessorValueTreeState apvts;
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    std::atomic<bool> delayOrGainChanged { false };
    std::atomic<bool> enabledChanged { false };
    Decorrelator decorrelator;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NafStereoSeparatorAudioProcessor)
};
