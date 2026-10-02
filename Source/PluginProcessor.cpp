/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PolarityFlipAudioProcessor::PolarityFlipAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
      apvts(*this,nullptr,"Parameters",createParameterLayout())
#else
     :apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

juce::AudioProcessorValueTreeState::ParameterLayout
PolarityFlipAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            "hasser",
            "Hasser",
            -1.0f,
            1.0f,
            0.0f
        )
    );
    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            "Rgain",
            "Right Gain",
            0.0f,
            1.3f,
            1.0f
        )
    );
    layout.add(
        std::make_unique<juce::AudioParameterFloat>(
            "Lgain",
            "Left Gain",
            0.0f,
            1.3f,
            1.0f
        )
    );
    layout.add(
        std::make_unique<juce::AudioParameterBool>(
            "flipL",
            "Flip Left",
            false
        )
    );
    layout.add(
        std::make_unique<juce::AudioParameterBool>(
            "flipR",
            "Flip Right",
            false
        )
    );
    layout.add(
        std::make_unique<juce::AudioParameterBool>(
            "hasserSwitch",
            "Hasser Switch",
            true
        )
    );
    return layout;
}

PolarityFlipAudioProcessor::~PolarityFlipAudioProcessor()
{
}

//==============================================================================
const juce::String PolarityFlipAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PolarityFlipAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PolarityFlipAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PolarityFlipAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PolarityFlipAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PolarityFlipAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PolarityFlipAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PolarityFlipAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PolarityFlipAudioProcessor::getProgramName (int index)
{
    return {};
}

void PolarityFlipAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PolarityFlipAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    hassLatency.reset(sampleRate, 0.002);
    hassLatency.setCurrentAndTargetValue(0.0f);
    maxDelaySamples = static_cast<int>(sampleRate * 0.050);
    delayBuffer.setSize(2, maxDelaySamples + samplesPerBlock);
    delayBuffer.clear();
    delayWritePosition = 0;
}

void PolarityFlipAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PolarityFlipAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PolarityFlipAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    float currentHassLatency = hassLatency.getNextValue();
    const int delaySamples = static_cast<int>(std::abs(currentHassLatency) * maxDelaySamples);

    auto* hasserParameter = apvts.getRawParameterValue("hasser");
    hassLatency.setTargetValue(hasserParameter->load());

    auto* RgainParameter = apvts.getRawParameterValue("Rgain");
    float currentRgain = RgainParameter->load();

    auto* LgainParameter = apvts.getRawParameterValue("Lgain");
    float currentLgain = LgainParameter->load();

    auto* RflipParameter = apvts.getRawParameterValue("flipR");
    bool currentFlipR = RflipParameter->load() > 0.5f;

    auto* LflipParameter = apvts.getRawParameterValue("flipL");
    bool currentFlipL = LflipParameter->load() > 0.5f;

    auto* hasserSwitchParameter = apvts.getRawParameterValue("hasserSwitch");
    bool currentHasserSwitchState = hasserSwitchParameter->load() > 0.5f;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const int delayBufferLength = delayBuffer.getNumSamples();

        int readPosition = delayWritePosition - delaySamples;

        if (readPosition < 0)
            readPosition += delayBufferLength;

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            float inputSample = buffer.getSample(channel, sample);

            delayBuffer.setSample(
                channel,
                delayWritePosition,
                inputSample
            );

            float delayedSample =
                delayBuffer.getSample(channel, readPosition);

            // Hasser
            if (currentHasserSwitchState)
            {
                if (currentHassLatency < 0.0f && channel == 0)
                {
                    buffer.setSample(channel, sample, delayedSample);
                }

                if (currentHassLatency > 0.0f && channel == 1)
                {
                    buffer.setSample(channel, sample, delayedSample);
                }
            }

            // Gain + Polarity
            if (channel == 0)
            {
                buffer.setSample(
                    channel,
                    sample,
                    buffer.getSample(channel, sample) * currentLgain
                );

                if (currentFlipL)
                {
                    buffer.setSample(
                        channel,
                        sample,
                        -buffer.getSample(channel, sample)
                    );
                }
            }

            if (channel == 1)
            {
                buffer.setSample(
                    channel,
                    sample,
                    buffer.getSample(channel, sample) * currentRgain
                );

                if (currentFlipR)
                {
                    buffer.setSample(
                        channel,
                        sample,
                        -buffer.getSample(channel, sample)
                    );
                }
            }
        }

        // 移动 delay buffer 的写入位置
        delayWritePosition++;

        if (delayWritePosition >= delayBufferLength)
            delayWritePosition = 0;
    }
}

//==============================================================================
bool PolarityFlipAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PolarityFlipAudioProcessor::createEditor()
{
    return new PolarityFlipAudioProcessorEditor (*this);
}

//==============================================================================
void PolarityFlipAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();

    std::unique_ptr<juce::XmlElement>xml = state.createXml();
    copyXmlToBinary(*xml, destData);
}

void PolarityFlipAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement>xml = getXmlFromBinary(data, sizeInBytes);
    if (xml != nullptr)
    {
        if (xml->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolarityFlipAudioProcessor();
}

//==============================================================================
//妈妈生的
void PolarityFlipAudioProcessor::setFlipL(bool shouldFlip)
{
    flipL = shouldFlip;
}

void PolarityFlipAudioProcessor::setFlipR(bool shouldFlip)
{
    flipR = shouldFlip;
}

void PolarityFlipAudioProcessor::setGainL(float newGain)
{
    Lgain = newGain;
}

void PolarityFlipAudioProcessor::setGainR(float newGain)
{
    Rgain = newGain;
}

void PolarityFlipAudioProcessor::setHassLatency(float latency)
{
    hassLatency.setTargetValue(latency);
}