/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NafStereoSeparatorAudioProcessorEditor  : public juce::AudioProcessorEditor,
    private juce::Slider::Listener,
    private juce::ToggleButton::Listener
{
public:
    NafStereoSeparatorAudioProcessorEditor (NafStereoSeparatorAudioProcessor&);
    ~NafStereoSeparatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void addDelaySliders(juce::Slider& dSlider, juce::Slider& gSlider, juce::ToggleButton& eButton, 
        std::unique_ptr<SliderAttachment>& delayAttach, std::unique_ptr<SliderAttachment>& gainAttach, std::unique_ptr<ButtonAttachment>& enabledAttach,
        const char* delayName, const char* gainName, const char* enabledName);
    void setDelaySlidersBounds(juce::Slider& dSlider, juce::Slider& gSlider, juce::ToggleButton& eButton, int height);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    juce::Image background;
    juce::Slider widthSlider;
    juce::Slider d1Slider, d2Slider, d3Slider, d4Slider, d5Slider;
    juce::Slider g1Slider, g2Slider, g3Slider, g4Slider, g5Slider;
    juce::ToggleButton e1Button, e2Button, e3Button, e4Button, e5Button;
    juce::ToggleButton bypassButton;

    std::unique_ptr<SliderAttachment> widthAttach;
    std::unique_ptr<SliderAttachment> delay1Attach;
    std::unique_ptr<SliderAttachment> delay2Attach;
    std::unique_ptr<SliderAttachment> delay3Attach;
    std::unique_ptr<SliderAttachment> delay4Attach;
    std::unique_ptr<SliderAttachment> delay5Attach;
    std::unique_ptr<SliderAttachment> gain1Attach;
    std::unique_ptr<SliderAttachment> gain2Attach;
    std::unique_ptr<SliderAttachment> gain3Attach;
    std::unique_ptr<SliderAttachment> gain4Attach;
    std::unique_ptr<SliderAttachment> gain5Attach;
    std::unique_ptr<ButtonAttachment> enabled1Attach;
    std::unique_ptr<ButtonAttachment> enabled2Attach;
    std::unique_ptr<ButtonAttachment> enabled3Attach;
    std::unique_ptr<ButtonAttachment> enabled4Attach;
    std::unique_ptr<ButtonAttachment> enabled5Attach;

    std::unique_ptr<ButtonAttachment> bypassAttach;

    NafStereoSeparatorAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NafStereoSeparatorAudioProcessorEditor)
};
