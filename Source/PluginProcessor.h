/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class CircularBuffer
{
public:
	CircularBuffer();

	void Init(int size);
	void WriteSample(float sample)
	{
		m_buffer.setSample(0, m_head, sample);
		if (++m_head >= m_size)
			m_head = 0;
	}
	float Read() const
	{
		return m_buffer.getSample(0, m_head);
	}
	float ReadDelay(float sample);
	void Clear();

protected:
	juce::AudioBuffer<float> m_buffer;
	int m_head = 0;
	int m_size = 0;
};

//==============================================================================
/**
*/
class MonoToStereoAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    MonoToStereoAudioProcessor();
    ~MonoToStereoAudioProcessor() override;

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
	juce::AudioProcessorValueTreeState parameters;

	std::atomic<float>* delayParameter = nullptr;
	std::atomic<float>* mixParameter = nullptr;

	CircularBuffer m_buffer = CircularBuffer();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonoToStereoAudioProcessor)
};
