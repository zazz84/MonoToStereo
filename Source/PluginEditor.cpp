/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MonoToStereoAudioProcessorEditor::MonoToStereoAudioProcessorEditor (MonoToStereoAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
	// Labels
	delayLabel.setText("Delay", juce::dontSendNotification);
	delayLabel.setFont(juce::Font(24.0f, juce::Font::bold));
	delayLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(delayLabel);

	mixLabel.setText("Mix", juce::dontSendNotification);
	mixLabel.setFont(juce::Font(24.0f, juce::Font::bold));
	mixLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(mixLabel);

	// Sliders
	delaySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	addAndMakeVisible(delaySlider);
	delaySliderAttachment.reset(new SliderAttachment(valueTreeState, "delay", delaySlider));

	mixSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
	addAndMakeVisible(mixSlider);
	mixSliderAttachment.reset(new SliderAttachment(valueTreeState, "mix", mixSlider));

	setSize(400, 250);
}

MonoToStereoAudioProcessorEditor::~MonoToStereoAudioProcessorEditor()
{
}

//==============================================================================
void MonoToStereoAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkslateblue);
}

void MonoToStereoAudioProcessorEditor::resized()
{
	int width = getWidth();
	int height = getHeight();

	juce::Rectangle<int> left;
	left.setSize(width / 2, height);
	left.setPosition(0, 0);

	juce::Rectangle<int> right;
	right.setSize(width / 2, height);
	right.setPosition(width / 2, 0);

	delaySlider.setBounds(left);
	mixSlider.setBounds(right);

	left.removeFromBottom(20);
	right.removeFromBottom(20);

	delayLabel.setBounds(left);
	mixLabel.setBounds(right);
}
