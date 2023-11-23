/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CircularBuffer::CircularBuffer()
{
}

void CircularBuffer::Init(int size)
{
	m_head = 0;
	m_size = size;
	m_buffer.setSize(1, size);
}

void CircularBuffer::Clear()
{
	m_head = 0;
	m_buffer.clear();
}

float CircularBuffer::ReadDelay(float sample)
{
	const int bufferSize = m_size;
	const float readIdx = m_head + bufferSize - sample;

	const int flr = static_cast<int>(readIdx);
	const int iPrev = flr < bufferSize ? flr : flr - bufferSize;
	int iNext = flr + 1;
	iNext = iNext < bufferSize ? iNext : iNext - bufferSize;

	const float weight = readIdx - flr;
	return m_buffer.getSample(0, iPrev) * (1.f - weight) + m_buffer.getSample(0, iNext) * weight;
}

//==============================================================================
MonoToStereoAudioProcessor::MonoToStereoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
	parameters
	(
		*this,
		nullptr,
		juce::Identifier("MonoToStereo"),
		{ std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "delay", 1 }, "Drive", 0.0f, 10.0f, 5.0f),
		std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { "mix", 1 }, "Volume", 0.0f, 1.0f, 0.5f), }
	)
{
	delayParameter = parameters.getRawParameterValue("delay");
	mixParameter = parameters.getRawParameterValue("mix");
}

MonoToStereoAudioProcessor::~MonoToStereoAudioProcessor()
{
}

//==============================================================================
const juce::String MonoToStereoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MonoToStereoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MonoToStereoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MonoToStereoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MonoToStereoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MonoToStereoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MonoToStereoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MonoToStereoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MonoToStereoAudioProcessor::getProgramName (int index)
{
    return {};
}

void MonoToStereoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MonoToStereoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	int samples =(int)(sampleRate * 0.001f * 10.0f);
	m_buffer.Init(samples);
	m_buffer.Clear();
}

void MonoToStereoAudioProcessor::releaseResources()
{
	m_buffer.Clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MonoToStereoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MonoToStereoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.

	int samples = buffer.getNumSamples();
	auto* leftChannel = buffer.getWritePointer(0);
	auto* rightChannel = buffer.getWritePointer(1);
	float delaySamples = std::max((float)(getSampleRate() * delayParameter->load() * 0.001f), 2.0f);

	float mix = mixParameter->load();
	float invertMix = 1.0f - mix;

	for (int sample = 0; sample < samples; ++sample)
	{
		float delayedSample = m_buffer.ReadDelay(delaySamples);
		float inLeft = leftChannel[sample];
		float inRight = rightChannel[sample];
		m_buffer.WriteSample(inLeft + inRight);
	
		float mixOut = delayedSample * mix;

		leftChannel[sample] = -mixOut + inLeft * invertMix;
		rightChannel[sample] = mixOut + inRight * invertMix;
	}
}

//==============================================================================
bool MonoToStereoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MonoToStereoAudioProcessor::createEditor()
{
    return new MonoToStereoAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void MonoToStereoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void MonoToStereoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters.state.getType()))
			parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MonoToStereoAudioProcessor();
}
