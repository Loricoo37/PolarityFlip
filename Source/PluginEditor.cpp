/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolarityFlipAudioProcessorEditor::PolarityFlipAudioProcessorEditor (PolarityFlipAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //BackgroundImage
    backgroundImage = juce::ImageCache::getFromMemory(
        BinaryData::background_plugin_png,
        BinaryData::background_plugin_pngSize
    );

    //Hasser
    Hasser.setSliderStyle(juce::Slider::LinearHorizontal);
    Hasser.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    Hasser.setRange(-1.0, 1.0, 0.01);
    Hasser.setValue(0.0);
    addAndMakeVisible(Hasser);
    hasserAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts,
            "hasser",
            Hasser
        );

    //HasserLabel
    HasserLabel.setText("Hasser", juce::dontSendNotification);
    HasserLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(HasserLabel);

    hasserAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts,
            "hasser",
            Hasser
        );

    //HasserSwitch
    hasserSwitch.setButtonText("Hasser On");
    hasserSwitch.setClickingTogglesState(true);
    addAndMakeVisible(hasserSwitch);

    hasserSwitchAttachment =
        std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            audioProcessor.apvts,
            "hasserSwitch",
            hasserSwitch
        );

    //LeftSliderLabel
    LlvLabel.setText("Left Level", juce::dontSendNotification);
    LlvLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(LlvLabel);

    //RightSliderLabel
    RlvLabel.setText("Right Level", juce::dontSendNotification);
    RlvLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(RlvLabel);

    //LeftSlider
    LlvSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    LlvSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    LlvSlider.setRange(0.0, 1.3, 0.01);
    LlvSlider.setValue(1.0);
    addAndMakeVisible(LlvSlider);

    LgainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts,
        "Lgain",
        LlvSlider
    );
    
    //RightSlider
    RlvSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    RlvSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    RlvSlider.setRange(0.0, 1.3, 0.01);
    RlvSlider.setValue(1.0);
    addAndMakeVisible(RlvSlider);

    RgainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts,
        "Rgain",
        RlvSlider
    );
    
    //TITLE
    titleLabel.setText("POLARITY FLIP",juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);
    
    //LeftButton
    leftButton.setButtonText("L3fT");
    leftButton.setClickingTogglesState(true);
    addAndMakeVisible(leftButton);

    LflipAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts,
        "flipL",
        leftButton
    );
    
    //RightButton
    rightButton.setButtonText("R1gHt");
    rightButton.setClickingTogglesState(true);
    addAndMakeVisible(rightButton);

    RflipAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts,
        "flipR",
        rightButton
    );

    //=========================================================================
    rightButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::lightblue);
    leftButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::lightskyblue);

    setSize (400, 300);
}

PolarityFlipAudioProcessorEditor::~PolarityFlipAudioProcessorEditor()
{
}

//==============================================================================
void PolarityFlipAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawText ("By phooocuz!!!", 29, 30, 300, 30, juce::Justification::centred, 1);

    g.drawImageWithin(backgroundImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);

    g.saveState();
    g.addTransform(
        juce::AffineTransform::rotation(
            juce::MathConstants<float>::pi / 12.0f, 190.0f, 110.0f
        )
    );
    g.setColour(juce::Colours::lightyellow);
    g.drawText("By phooocuz!!!", 100, 0, 300, 30, juce::Justification::centred);
    g.restoreState();
}

void PolarityFlipAudioProcessorEditor::resized()
{
    titleLabel.setBounds(20, 15, getWidth() - 40, 40);

    leftButton.setBounds(45, 57, 150, 30);
    rightButton.setBounds(195, 57, 150, 30);
    hasserSwitch.setBounds((getWidth() - 200) / 2, 250, 200, 45);

    LlvSlider.setBounds(80, 79, 80, 80);
    RlvSlider.setBounds(220, 79, 80, 80);
    Hasser.setBounds((getWidth() - 264) / 2, 190, 264, 25);

    LlvLabel.setBounds(60, 160, 135, 30);
    RlvLabel.setBounds(200, 160, 135, 30);
    HasserLabel.setBounds((getWidth() - 140) / 2, 210, 140, 55);
}
