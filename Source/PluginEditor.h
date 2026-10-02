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
class PolarityFlipAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PolarityFlipAudioProcessorEditor (PolarityFlipAudioProcessor&);
    ~PolarityFlipAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PolarityFlipAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Label LlvLabel;   
    juce::Label RlvLabel;
    juce::Label HasserLabel;

    juce::TextButton leftButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> LflipAttachment;
    juce::TextButton rightButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> RflipAttachment;
    juce::ToggleButton hasserSwitch;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> hasserSwitchAttachment;

    juce::Slider LlvSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> LgainAttachment;
    juce::Slider RlvSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> RgainAttachment;

    juce::Slider Hasser;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hasserAttachment;

    juce::Image backgroundImage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolarityFlipAudioProcessorEditor)
};