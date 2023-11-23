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
class MonoToStereoAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MonoToStereoAudioProcessorEditor (MonoToStereoAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~MonoToStereoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

	typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MonoToStereoAudioProcessor& audioProcessor;

	juce::AudioProcessorValueTreeState& valueTreeState;

	juce::Label delayLabel;
	juce::Label mixLabel;
	juce::Slider delaySlider;
	juce::Slider mixSlider;

	std::unique_ptr<SliderAttachment> delaySliderAttachment;
	std::unique_ptr<SliderAttachment> mixSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MonoToStereoAudioProcessorEditor)
};
