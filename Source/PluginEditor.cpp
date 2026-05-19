/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

// Add this to build stage
// copy /Y "$(OutDir)\NafStereoSeparator.dll" "C:\Program Files\Common Files\VST3\NafStereoSeparator.vst3"


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NafStereoSeparatorAudioProcessorEditor::NafStereoSeparatorAudioProcessorEditor (NafStereoSeparatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    background = juce::ImageCache::getFromMemory(
        BinaryData::NafaisStereoSeparator_png,
        BinaryData::NafaisStereoSeparator_pngSize
    );
     
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
    setResizable(true, true);
    setResizeLimits(480, 400, 1000, 400);

    // Attach must be done before the other settings
    widthAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, "width", widthSlider);

    widthSlider.setSliderStyle(juce::Slider::LinearBar);
    widthSlider.setRange(0.0, 1.0, 0.01);
    widthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    widthSlider.setPopupDisplayEnabled(true, false, this);
    widthSlider.setTextValueSuffix("");
    widthSlider.setValue(0.5);
    widthSlider.addListener(this);

    // Make visible
    addAndMakeVisible(&widthSlider);

    // delay and gain
    addDelaySliders(d1Slider, g1Slider, e1Button, delay1Attach, gain1Attach, enabled1Attach, "d1", "g1", "e1");
    addDelaySliders(d2Slider, g2Slider, e2Button, delay2Attach, gain2Attach, enabled2Attach, "d2", "g2", "e2");
    addDelaySliders(d3Slider, g3Slider, e3Button, delay3Attach, gain3Attach, enabled3Attach, "d3", "g3", "e3");
    addDelaySliders(d4Slider, g4Slider, e4Button, delay4Attach, gain4Attach, enabled4Attach, "d4", "g4", "e4");
    addDelaySliders(d5Slider, g5Slider, e5Button, delay5Attach, gain5Attach, enabled5Attach, "d5", "g5", "e5");

    // Attach must be done before the other settings
    bypassAttach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "bypass", bypassButton);

    // Make visible
    addAndMakeVisible(&bypassButton);
}

NafStereoSeparatorAudioProcessorEditor::~NafStereoSeparatorAudioProcessorEditor()
{
}

//==============================================================================

void NafStereoSeparatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    int halfWidth = (getWidth() - 30 - 30) / 2;
    int width = (getWidth() - 20); 

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText("Width",   10, 0, width, 30, juce::Justification::left, 1);
    g.drawFittedText("Delay 1", 10, 50, halfWidth, 30, juce::Justification::left, 1);
    g.drawFittedText("Gain",    30 + 20 + halfWidth, 50, width, 30, juce::Justification::left, 1);
    g.drawFittedText("Delay 2", 10, 100, halfWidth, 30, juce::Justification::left, 1);
    g.drawFittedText("Gain",    30 + 20 + halfWidth, 100, width, 30, juce::Justification::left, 1);
    g.drawFittedText("Delay 3", 10, 150, halfWidth, 30, juce::Justification::left, 1);
    g.drawFittedText("Gain",    30 + 20 + halfWidth, 150, width, 30, juce::Justification::left, 1);
    g.drawFittedText("Delay 4", 10, 200, halfWidth, 30, juce::Justification::left, 1);
    g.drawFittedText("Gain",    30 + 20 + halfWidth, 200, width, 30, juce::Justification::left, 1);
    g.drawFittedText("Delay 5", 10, 250, halfWidth, 30, juce::Justification::left, 1);
    g.drawFittedText("Gain",    30 + 20 + halfWidth, 250, width, 30, juce::Justification::left, 1);
    g.drawFittedText("Bypass",  10, 300, width, 30, juce::Justification::left, 1);

    g.drawImageAt(background, 10, 360, false);
}

void NafStereoSeparatorAudioProcessorEditor::resized()
{
    // sets the position and size of the slider with arguments (x, y, width, height)
    widthSlider.setBounds (10, 25,  getWidth() - 20, 20);
    setDelaySlidersBounds(d1Slider, g1Slider, e1Button, 75);
    setDelaySlidersBounds(d2Slider, g2Slider, e2Button, 125);
    setDelaySlidersBounds(d3Slider, g3Slider, e3Button, 175);
    setDelaySlidersBounds(d4Slider, g4Slider, e4Button, 225);
    setDelaySlidersBounds(d5Slider, g5Slider, e5Button, 275);
    bypassButton.setBounds   (10, 320, 25, 30);
}

void NafStereoSeparatorAudioProcessorEditor::addDelaySliders(juce::Slider& dSlider, juce::Slider& gSlider, juce::ToggleButton& eButton, 
    std::unique_ptr<SliderAttachment>& delayAttach, std::unique_ptr<SliderAttachment>& gainAttach, std::unique_ptr<ButtonAttachment>& enabledAttach,
    const char* delayName, const char* gainName, const char* enabledName)
{
    // Attach must be done before the other settings
    delayAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, delayName, dSlider);

    dSlider.setSliderStyle(juce::Slider::LinearBar);
    dSlider.setRange(0.0, 30.0, 0.01);
    dSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    dSlider.setPopupDisplayEnabled(true, false, this);
    dSlider.setTextValueSuffix(" ms");

    // Make visible
    addAndMakeVisible(&dSlider);

    // Attach must be done before the other settings
    gainAttach = std::make_unique<SliderAttachment>(audioProcessor.apvts, gainName, gSlider);

    gSlider.setSliderStyle(juce::Slider::LinearBar);
    gSlider.setRange(-1.0, 1.0, 0.01);
    gSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    gSlider.setPopupDisplayEnabled(true, false, this);
    gSlider.setTextValueSuffix("");

    // Make visible
    addAndMakeVisible(&gSlider);

    // Attach must be done before the other settings
    enabledAttach = std::make_unique<ButtonAttachment>(audioProcessor.apvts, enabledName, eButton);

    // Make visible
    addAndMakeVisible(&eButton);
}

void NafStereoSeparatorAudioProcessorEditor::setDelaySlidersBounds(juce::Slider& dSlider, juce::Slider& gSlider, juce::ToggleButton& eButton, int height)
{
    int halfWidth = (getWidth() - 30 - 30) / 2;
    dSlider.setBounds(30 + 10, height, halfWidth, 20);
    gSlider.setBounds(30 + 20 + halfWidth, height, halfWidth, 20);
    eButton.setBounds(10, height-5, 25, 30);
}

void NafStereoSeparatorAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    //if (slider == &widthSlider)
    //{
    //    audioProcessor.width = slider->getValue();
    //}
    //else
    //{
    //    audioProcessor.d1 = d1Slider.getValue();
    //    audioProcessor.d2 = d2Slider.getValue();
    //    audioProcessor.d3 = d3Slider.getValue();
    //    audioProcessor.d4 = d4Slider.getValue();
    //    audioProcessor.d5 = d5Slider.getValue();

    //    audioProcessor.g1 = g1Slider.getValue();
    //    audioProcessor.g2 = g2Slider.getValue();
    //    audioProcessor.g3 = g3Slider.getValue();
    //    audioProcessor.g4 = g4Slider.getValue();
    //    audioProcessor.g5 = g5Slider.getValue();

    //    audioProcessor.sizeChanged = true;
    //}
}

void NafStereoSeparatorAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    //if (button == &bypassButton)
    //{
    //    audioProcessor.bypass = button->getToggleState();
    //}
    //else
    //{
    //    audioProcessor.e1 = e1Button.getToggleState();
    //    audioProcessor.e2 = e2Button.getToggleState();
    //    audioProcessor.e3 = e3Button.getToggleState();
    //    audioProcessor.e4 = e4Button.getToggleState();
    //    audioProcessor.e5 = e5Button.getToggleState();

    //    audioProcessor.enabledChanged = true;
    //}
}