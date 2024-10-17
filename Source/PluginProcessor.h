/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "dsp\resonator.h"
#include "dsp\fir.h"
//==============================================================================
/**
*/
class LModelAudioProcessor : public juce::AudioProcessor
{
public:


	//==============================================================================
	LModelAudioProcessor();
	~LModelAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//==============================================================================
	juce::AudioProcessorValueTreeState& GetParams()
	{
		return Params;
	}

	Resonator resos[MaxKeyNum];
	FirResonator firReso;
	ResonatorManager manager;
	int procCount = 0;//数proc次数，用来更新biquad参数的
	int bpfEnable[MaxKeyNum][MaxEQNodeNum];
	float tmpbufl[4800];
	float tmpbufr[4800];
	float tmpbufl2[4800];
	float tmpbufr2[4800];
	float tmpbufl3[4800];
	float tmpbufr3[4800];
	//int firEnable = 0;
	float firDat[FFT_Length];

	float cos_table[65536];

	Biquad lpf1, hpf1;
	Biquad lpf2, hpf2;
	Biquad lpf3, hpf3;
	Biquad lpf4, hpf4;
	Biquad lsf1, hsf1;
	Biquad lsf2, hsf2;
	Biquad lsf3, hsf3;
	Biquad lsf4, hsf4;
private:
	//Synth Param
	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	juce::AudioProcessorValueTreeState Params{ *this, nullptr, "Parameters", createParameterLayout() };

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LModelAudioProcessor)
};

