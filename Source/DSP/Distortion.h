/*
  ==============================================================================

    Distortion.h
    Created: 12 Nov 2022 2:06:53pm
    Author:  Deivids

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

template <typename SampleType>

class Distortion
{
public:
    
    Distortion();

    void prepare(juce::dsp::ProcessSpec& spec);

    void reset();

    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        auto val = 0.0 ;
        for (size_t n = 0; n < numSamples; ++n)
        {

            for (size_t ch = 0; ch < numChannels; ++ch)
            {
                val =processSample(inputBlock.getChannelPointer(ch)[n], (int)ch);
                val = _dcFilter.processSample((int)ch, val);
                outputBlock.getChannelPointer(ch)[n] = val;
              
                
                //  outputBlock.getChannelPointer(ch)[n] +=
                //outputBlock.getChannelPointer(ch)[n] =
//                outputBlock.getChannelPointer(ch)[n] = processSample(ch, inputBlock.getChannelPointer(ch)[n]);
            }
        }
        

    }

    SampleType processSample(SampleType inputSample, int channel) noexcept
    {
        switch (_model)
        {
        case DistortionModel::kHard:
        {
            return processHardClipper(inputSample);
            break;

        }
        case DistortionModel::kSoft:
        {
            return processSoftClipper(inputSample);
            break;

        }
        case DistortionModel::kSaturation:
        {
            return processSaturation(inputSample, channel);
            break;

        }

        }
    }
    // DSP PROCESSING
    SampleType processHardClipper(SampleType inputSample)
    {
        auto wetSignal = inputSample * juce::Decibels::decibelsToGain(_input.getNextValue());
        
        if (std::abs(wetSignal) > 0.99)
        {
            wetSignal *= 0.99 / std::abs(wetSignal);
        }

        auto mix = (1.0 - _mix.getNextValue()) * inputSample + wetSignal * _mix.getNextValue();

        return mix * juce::Decibels::decibelsToGain(_output.getNextValue());
    }


    SampleType processSoftClipper(SampleType inputSample)
    {
        auto wetSignal = inputSample * juce::Decibels::decibelsToGain(_input.getNextValue());

        wetSignal = _piDivisor * std::atan(wetSignal);

        wetSignal *= 2.0;

        wetSignal *= juce::Decibels::decibelsToGain(_input.getNextValue() * -0.25);

        if (std::abs(wetSignal) > 0.99)
        {
            wetSignal *= 0.99 / std::abs(wetSignal);
        }

        auto mix = (1.0 - _mix.getNextValue()) * inputSample + wetSignal * _mix.getNextValue();

        return mix * juce::Decibels::decibelsToGain(_output.getNextValue());
    }


    SampleType processSaturation(SampleType inputSample, int channel)
    {

        auto drive = juce::jmap(_input.getNextValue(), 0.0f, 24.0f, 0.0f, 24.0f);

        auto wetSignal = inputSample * juce::Decibels::decibelsToGain(drive);

        if (wetSignal >= 0.0)
        {
            wetSignal = std::tanh(wetSignal);
        }
        else
        {
            wetSignal = std::tanh(std::sinh(wetSignal)) - 0.2 * wetSignal * std::sin(juce::MathConstants<float>::pi * wetSignal);
        }
        
 

        wetSignal *= 1.15;
        wetSignal *= juce::Decibels::decibelsToGain(_input.getNextValue() * -0.05);

        auto mix = (1.0 - _mix.getNextValue()) * inputSample + wetSignal * _mix.getNextValue();

        return mix * juce::Decibels::decibelsToGain(_output.getNextValue());
    }
    //DSP PROCESSING


    enum class DistortionModel
    {
        kHard,
        kSoft,
        kSaturation
    };

    void setDrive(SampleType newDrive);
    void setMix(SampleType newMix);
    void setOutput(SampleType newOutput);

    void setDistortionModel(DistortionModel newModel);


private:
    juce::SmoothedValue<float> _input;
    juce::SmoothedValue<float> _mix;
    juce::SmoothedValue<float> _output;

    juce::dsp::LinkwitzRileyFilter<float> _dcFilter;


    float _piDivisor = 2.0 / juce::MathConstants<float>::pi;

    float _sampleRate = 48000.0f;

    DistortionModel _model = DistortionModel::kHard;

};
