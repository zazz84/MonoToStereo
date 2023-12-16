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

	static const int N_SLIDERS_COUNT = 2;
	static const int SCALE = 70;
	static const int LABEL_OFFSET = 25;
	static const int SLIDER_WIDTH = 200;
	static const int HUE = 40;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

	typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

private:
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