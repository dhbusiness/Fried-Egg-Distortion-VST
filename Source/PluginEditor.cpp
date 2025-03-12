/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters/Parameters.h"

//==============================================================================
EggSaturatorAudioProcessorEditor::EggSaturatorAudioProcessorEditor (EggSaturatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
wetDryAttachtment(p._treeState,mixID,wetDrySlider),
toneAttachtment(p._treeState,toneID,toneSlider), 
driveAttachtment(p._treeState,inputID,driveSlider),
satAttachtment(p._treeState,satID,satSlider),
disModelAttachtment(p._treeState,disModelID,disModelCombo)
{
    
    setLookAndFeel(&customLook);
    
    wetDrySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    toneSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    satSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    driveSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    
    wetDrySlider.setRotaryParameters(0,juce::MathConstants<float>::twoPi,true);
    toneSlider.setRotaryParameters(0,juce::MathConstants<float>::twoPi,true);
    satSlider.setRotaryParameters(0,juce::MathConstants<float>::twoPi,true);
    driveSlider.setRotaryParameters(0,juce::MathConstants<float>::twoPi,true);
    
    wetDrySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox , true,0,0);
    toneSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox , true,0,0);
    satSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox , true,0,0);
    driveSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox , true,0,0);
    
    addAndMakeVisible(wetDrySlider);
    addAndMakeVisible(toneSlider);
    addAndMakeVisible(satSlider);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(disModelCombo);
    disModelCombo.setTextWhenNothingSelected("Distortion Model");
    disModelCombo.addItemList(disModels,1);
    
    disModelCombo.addListener(this);
    

    wetDrySlider.addListener(this);
    toneSlider.addListener(this);
    satSlider.addListener(this);
    driveSlider.addListener(this);
   
    
    //controlled assets
    yolk = juce::ImageFileFormat::loadFrom(BinaryData::Egg_Yolk__Feedback_png,BinaryData::Egg_Yolk__Feedback_pngSize);
    heat = juce::ImageFileFormat::loadFrom(BinaryData::Heat_Drive_png, BinaryData::Heat_Drive_pngSize);
    pan = juce::ImageFileFormat::loadFrom(BinaryData::Pan_WetDry_png, BinaryData::Pan_WetDry_pngSize);
    white = juce::ImageFileFormat::loadFrom(BinaryData::Egg_White__Tone_png, BinaryData::Egg_White__Tone_pngSize);
    //Non-moving assets
    contoller = juce::ImageFileFormat::loadFrom(BinaryData::Controller_png, BinaryData::Controller_pngSize);
    screenGrid = juce::ImageFileFormat::loadFrom(BinaryData::Screen_Grid_png, BinaryData::Screen_Grid_pngSize);
    screenGreening = juce::ImageFileFormat::loadFrom(BinaryData::Screen_Greening_png, BinaryData::Screen_Greening_pngSize);
    screenGreenBG = juce::ImageFileFormat::loadFrom(BinaryData::Screen_green_background_png, BinaryData::Screen_green_background_pngSize);
    //Dial assets
    dialHousing = juce::ImageFileFormat::loadFrom(BinaryData::Dial_Housing_png, BinaryData::Dial_Housing_pngSize);
    dialDrive = juce::ImageFileFormat::loadFrom(BinaryData::Drive_Dial_png, BinaryData::Drive_Dial_pngSize);
    dialTone = juce::ImageFileFormat::loadFrom(BinaryData::Tone_Dial_png, BinaryData::Tone_Dial_pngSize);
    dialWetDry = juce::ImageFileFormat::loadFrom(BinaryData::WetDry_Dial_png, BinaryData::WetDry_Dial_pngSize);
    dialFeedback = juce::ImageFileFormat::loadFrom(BinaryData::Feedback_Dial_png, BinaryData::Feedback_Dial_pngSize);

    //for svgs take a look at juce drawable class and loading svg files from binary.. might be worth googling...
    
    
    wetDrySlider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::black);
    toneSlider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::white);
    driveSlider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::red);
    satSlider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::yellow);
//
    wetDrySlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::whitesmoke);
    toneSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::black);
    driveSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::darkred);
    satSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::gold);
//
    wetDrySlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::whitesmoke);
    toneSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::black);
    driveSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::darkred);
    satSlider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::gold);
    
    
    
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (770, 472);
}

EggSaturatorAudioProcessorEditor::~EggSaturatorAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    
}

//==============================================================================
void EggSaturatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    
    auto bounds = getLocalBounds().toFloat();
    
    g.fillAll (juce::Colour (0xff323e44));

     {
         int x = 92, y = 112;
         juce::Colour fillColour1 = juce::Colour (0xff767672), fillColour2 = juce::Colour (0xff3e3e3e);
         //[UserPaintCustomArguments] Customize the painting arguments here..
         //[/UserPaintCustomArguments]
         g.setGradientFill (juce::ColourGradient (fillColour1,
                                              472.0f - 92.0f + x,
                                              256.0f - 112.0f + y,
                                              fillColour2,
                                              1296.0f - 92.0f + x,
                                              128.0f - 112.0f + y,
                                              false));
         g.fillRect (0,0, 770, 472);
     }
   // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::gold);
    g.setFont (36.0f);
    g.drawFittedText ("Fried Egg Distortion", getLocalBounds(), juce::Justification::topLeft,36);
    
    
    //g.drawImage(yolk, bounds.reduced(100));
    //Non-moving assets
    
    auto wetDryParam =  audioProcessor._treeState.getParameter(mixID);
    auto wetDryValue = wetDryParam->getValue();
    
    auto toneParam =  audioProcessor._treeState.getParameter(toneID);
    auto toneValue = toneParam->getValue();
    
    auto driveParam =  audioProcessor._treeState.getParameter(inputID);
    auto driveValue = driveParam->getValue();
    
    auto satParam =  audioProcessor._treeState.getParameter(satID);
    auto satValue = satParam->getValue();
    
    
    
    
    g.drawImage(contoller, 0, 3.37, contoller.getWidth(), contoller.getHeight(), 0, 0, contoller.getWidth(), contoller.getHeight());
    g.drawImage(screenGreenBG, 0, 92, screenGreenBG.getWidth(), screenGreenBG.getHeight(), 0, 0, screenGreenBG.getWidth(), screenGreenBG.getHeight());
    g.drawImage(screenGrid, 0, 92, screenGrid.getWidth(), screenGrid.getHeight(), 0, 0, screenGrid.getWidth(), screenGrid.getHeight());
    //dial assets
    g.drawImage(dialHousing, 375.21, 69.67, dialHousing.getWidth(), dialHousing.getHeight(), 0, 0, dialHousing.getWidth(), dialHousing.getHeight());
    g.drawImage(dialDrive, 585.66, 171.88, dialDrive.getWidth(), dialDrive.getHeight(), 0, 0, dialDrive.getWidth(), dialDrive.getHeight());
    g.drawImage(dialTone, 475.91, 281.63, dialTone.getWidth(), dialTone.getHeight(), 0, 0, dialTone.getWidth(), dialTone.getHeight());
    g.drawImage(dialWetDry, 366.16, 171.88, dialWetDry.getWidth(), dialWetDry.getHeight(), 0, 0, dialWetDry.getWidth(), dialWetDry.getHeight());
    g.drawImage(dialFeedback, 475.91, 62.14, dialFeedback.getWidth(), dialFeedback.getHeight(), 0, 0, dialFeedback.getWidth(), dialFeedback.getHeight());

    
    
    juce::Image secondPan = pan.createCopy();
    secondPan.multiplyAllAlphas(wetDryValue);
    
    juce::Image secondHeat = heat.createCopy();
    secondHeat.multiplyAllAlphas(driveValue);
    
    juce::Image secondWhite = white.createCopy();
    secondWhite.multiplyAllAlphas(toneValue);
    
    juce::Image secondYolk = yolk.createCopy();
    secondYolk.multiplyAllAlphas(satValue);
    
    //DRAWING CONTROLLED ASSETS HERE
    g.drawImage(secondPan, 37.67, 113.37, pan.getWidth(), pan.getHeight(), 0, 0, pan.getWidth(), pan.getHeight());
    g.drawImage(secondHeat, 114.4, 124.08, heat.getWidth(), heat.getHeight(), 0, 0, heat.getWidth(), heat.getHeight());
    g.drawImage(secondWhite, 148.31, 161.55, white.getWidth(), white.getHeight(), 0, 0, white.getWidth(), white.getHeight());
    g.drawImage(secondYolk, 178.64, 184.75, yolk.getWidth(), yolk.getHeight(), 0, 0, yolk.getWidth(), yolk.getHeight());
    //Non-moving assets
    g.drawImage(screenGreening, 0, 92, screenGreening.getWidth(), screenGreening.getHeight(), 0, 0, screenGreening.getWidth(), screenGreening.getHeight());
 

}

void EggSaturatorAudioProcessorEditor::resized()
{

 // This is generally where you'll want to lay out the positions of any
// subcomponents in your editor..

// wetDrySlider.setBounds(366.16, 171.88, dialWetDry.getWidth(), dialWetDry.getHeight()); //+35 PX TO X & Y when /5
    wetDrySlider.setBounds(401, 207, dialWetDry.getWidth() * 0.5, dialWetDry.getHeight() * 0.5);

    //toneSlider.setBounds( 475.91, 281.63, dialTone.getWidth(), dialTone.getHeight() );
    toneSlider.setBounds(510.91, 316.63, dialTone.getWidth() * 0.5, dialTone.getHeight() * 0.5);
    //    feedbackSlider.setBounds( 475.91, 62.14, dialFeedback.getWidth(), dialFeedback.getHeight() );

    satSlider.setBounds(510.91, 97.14, dialFeedback.getWidth() * 0.5, dialFeedback.getHeight() * 0.5);
    //    driveSlider.setBounds( 585.66, 171.88, dialDrive.getWidth(), dialDrive.getHeight() );

    driveSlider.setBounds(620.66, 206.88, dialDrive.getWidth() * 0.5, dialDrive.getHeight() * 0.5);
    
    disModelCombo.setBounds(86.75, 54.0, 173.5, 26.0);

}


void EggSaturatorAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    repaint();
};
void EggSaturatorAudioProcessorEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) {
    //DBG("Combo Changed");
    
};
