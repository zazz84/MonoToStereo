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
	juce::Colour light = juce::Colour::fromHSV(HUE * 0.01f, 0.5f, 0.6f, 1.0f);
	juce::Colour medium = juce::Colour::fromHSV(HUE * 0.01f, 0.5f, 0.5f, 1.0f);
	juce::Colour dark = juce::Colour::fromHSV(HUE * 0.01f, 0.5f, 0.4f, 1.0f);

	getLookAndFeel().setColour(juce::Slider::thumbColourId, dark);
	getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, medium);
	getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, light);

	// Labels
	delayLabel.setText("Delay", juce::dontSendNotification);
	delayLabel.setFont(juce::Font(24.0f * 0.01f * SCALE, juce::Font::bold));
	delayLabel.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(delayLabel);

	mixLabel.setText("Mix", juce::dontSendNotification);
	mixLabel.setFont(juce::Font(24.0f * 0.01f * SCALE, juce::Font::bold));
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

	setSize((int)(SLIDER_WIDTH * 0.01f * SCALE * N_SLIDERS_COUNT), (int)(SLIDER_WIDTH * 0.01f * SCALE));
}

MonoToStereoAudioProcessorEditor::~MonoToStereoAudioProcessorEditor()
{
}

//==============================================================================
void MonoToStereoAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colour::fromHSV(HUE * 0.01f, 0.5f, 0.7f, 1.0f));
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

	left.removeFromBottom((int)(LABEL_OFFSET * 0.01f * SCALE));
	right.removeFromBottom((int)(LABEL_OFFSET * 0.01f * SCALE));

	delayLabel.setBounds(left);
	mixLabel.setBounds(right);
}
