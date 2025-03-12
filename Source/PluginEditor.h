/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "EggLook.h"

//==============================================================================
/**
*/
class EggSaturatorAudioProcessorEditor  : public juce::AudioProcessorEditor , public juce::Slider::Listener , public juce::ComboBox::Listener
{
public:
    EggSaturatorAudioProcessorEditor (EggSaturatorAudioProcessor&);
    ~EggSaturatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged (juce::Slider* slider)override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EggSaturatorAudioProcessor& audioProcessor;
    
    
    juce::Image yolk;
    juce::Image heat;
    juce::Image white;
    juce::Image pan;
    juce::Image contoller;
    juce::Image screenGrid;
    juce::Image screenGreening;
    juce::Image screenGreenBG;
    juce::Image dialHousing;
    juce::Image dialDrive;
    juce::Image dialTone;
    juce::Image dialFeedback;
    juce::Image dialWetDry;
    
    
    EggLook customLook;

    juce::Slider wetDrySlider;
    juce::Slider toneSlider;
    juce::Slider driveSlider;
    juce::Slider satSlider;
    
    juce::ComboBox disModelCombo;
    
    juce::AudioProcessorValueTreeState::SliderAttachment wetDryAttachtment, toneAttachtment, driveAttachtment, satAttachtment;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment disModelAttachtment;
    
    juce::StringArray disModels = { "Hard","Soft","Saturation" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EggSaturatorAudioProcessorEditor)
};
