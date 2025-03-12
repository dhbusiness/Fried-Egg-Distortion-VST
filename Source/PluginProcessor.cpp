/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EggSaturatorAudioProcessor::EggSaturatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
    , _treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif

//DELAY STUFF
//,pluginstate(*this,nullptr, "DelayState", {
 //   std::make_unique<juce::AudioParameterFloat>("Feedback","Feedback ",juce::NormalisableRange<float>(0.,1.),0.33,juce::AudioParameterFloatAttributes().withLabel("%").withStringFromValueFunction([](auto x, auto){return juce::String(x*100);}))})

   // leftDelay(10),
    //rightDelay(10)
//DELAY STUFF END

{
    _treeState.addParameterListener(satID, this);
    _treeState.addParameterListener(disModelID, this);
    _treeState.addParameterListener(inputID, this);
    _treeState.addParameterListener(mixID, this);
    _treeState.addParameterListener(outputID, this);
    _treeState.addParameterListener(toneID, this);
}

EggSaturatorAudioProcessor::~EggSaturatorAudioProcessor()
{
    _treeState.removeParameterListener(satID, this);
    _treeState.removeParameterListener(disModelID, this);
    _treeState.removeParameterListener(inputID, this);
    _treeState.removeParameterListener(mixID, this);
    _treeState.removeParameterListener(outputID, this);
    _treeState.removeParameterListener(toneID, this);
}

juce::AudioProcessorValueTreeState::ParameterLayout EggSaturatorAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    //UI INPUTS -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    juce::StringArray disModels = {"Hard","Soft","Saturation"};
    auto pDriveModel = std::make_unique < juce::AudioParameterChoice>(disModelID,disModelName, disModels,1);    
    auto pDrive = std::make_unique<juce::AudioParameterFloat>(inputID, inputName, 0.0f, 24.0f, 0.0f);
    auto pMix = std::make_unique<juce::AudioParameterFloat>(mixID, mixName, 0.0f, 1.0f, 1.0f);
    auto pOutput = std::make_unique<juce::AudioParameterFloat>(outputID, outputName, -24.0f, 24.0f, 1.0f);
    auto pSat = std::make_unique<juce::AudioParameterFloat>(satID, satName, 1.0f, 24.0f, 1.0f);
    auto pTone = std::make_unique<juce::AudioParameterFloat>(toneID, toneName, 10.0f, 22500.0f, 12500.0f);

    params.push_back(std::move(pDriveModel));
    params.push_back(std::move(pDrive));
    params.push_back(std::move(pMix));
    params.push_back(std::move(pOutput));
    params.push_back(std::move(pSat));
    params.push_back(std::move(pTone));


    return {params.begin(), params.end() };
    //UI INPUTS -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}

void EggSaturatorAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    updateParameters();
}

void EggSaturatorAudioProcessor::updateParameters()
{
    auto model = static_cast<int>(_treeState.getRawParameterValue(disModelID)->load());
    switch(model)
    {
    case 0: _distortionModule.setDistortionModel(Distortion<float>::DistortionModel::kHard); break;
    case 1: _distortionModule.setDistortionModel(Distortion<float>::DistortionModel::kSoft); break;
    case 2: _distortionModule.setDistortionModel(Distortion<float>::DistortionModel::kSaturation); break;

    }

    _distortionModule.setDrive(_treeState.getRawParameterValue(inputID)->load());
    _distortionModule.setMix(_treeState.getRawParameterValue(mixID)->load());
    _distortionModule.setOutput(_treeState.getRawParameterValue(outputID)->load());
}

//==============================================================================
const juce::String EggSaturatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EggSaturatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EggSaturatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EggSaturatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EggSaturatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EggSaturatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EggSaturatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EggSaturatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EggSaturatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void EggSaturatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EggSaturatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    //init spec for dsp modules
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    
    currentSampleRate = sampleRate;
    
    spec.numChannels = getTotalNumOutputChannels();

    _distortionModule.prepare(spec);
    updateParameters();
    
    
    
    
    //TONE CONTROl
    _toneFilter.prepare(spec);
    _toneFilter.setEnabled(true);
    _toneFilter.setMode(juce::dsp::LadderFilterMode::LPF24);
    //TONE CONTROL
}

void EggSaturatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
   // leftDelay.clearBuffer();
   // rightDelay.clearBuffer();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EggSaturatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void EggSaturatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    // DELAY STUFF
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    auto wetBuffer(buffer);
    
    juce::dsp::AudioBlock<float> block {wetBuffer};
    _distortionModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    
    juce::dsp::AudioBlock<float> toneblock {buffer};
    
    auto toneParam = _treeState.getParameter(toneID);
    auto val = toneParam->getValue();
    auto satParam = _treeState.getParameter(satID);
    auto satVal = satParam->getValue();
    
    _toneFilter.setCutoffFrequencyHz(toneParam->convertFrom0to1(val));
    
    _toneFilter.setDrive(satParam->convertFrom0to1(satVal));
    
    _toneFilter.process(juce::dsp::ProcessContextReplacing<float>(toneblock));
    
    
    auto mixParam = _treeState.getParameter(mixID);
    auto mixVal = mixParam->getValue();
    
    for(auto i = 0 ; i < buffer.getNumChannels();++i)
    {
        for(auto n = 0 ; n < buffer.getNumSamples(); ++n)
        {
            auto dryValue = buffer.getSample(i, n);
            auto wetValue = wetBuffer.getSample(i, n);
            buffer.setSample(i, n, ( wetValue * mixVal) + (dryValue * (1.0-mixVal)) );
        }
    }
    
}

//==============================================================================
bool EggSaturatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EggSaturatorAudioProcessor::createEditor()
{
   return new EggSaturatorAudioProcessorEditor (*this);
   // return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void EggSaturatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EggSaturatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EggSaturatorAudioProcessor();
}
