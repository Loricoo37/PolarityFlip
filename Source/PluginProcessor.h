/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class PolarityFlipAudioProcessor  : public juce::AudioProcessor
{
public:
    juce::AudioProcessorValueTreeState apvts;

    void setFlipL(bool shouldFlip);
    void setFlipR(bool shouldFlip);

    void setGainL(float newGain);
    void setGainR(float newGain);

    void setHassLatency(float latency);
    //==============================================================================
    PolarityFlipAudioProcessor();
    ~PolarityFlipAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

private:
    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    bool flipR = false;
    bool flipL = false;

    float Lgain = 1.0f;
    float Rgain = 1.0f;

    juce::SmoothedValue<float> hassLatency;

    juce::AudioBuffer<float> delayBuffer;
    int delayWritePosition = 0;
    int maxDelaySamples = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolarityFlipAudioProcessor)
};
