/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
LModelAudioProcessor::LModelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	manager.firEnable = 0;
	for (int i = 0; i < MaxKeyNum; ++i)
	{
		manager.keyState[i] = 0;
	}
	for (int i = 0; i < MaxKeyNum; ++i)
	{
		for (int j = 0; j < MaxEQNodeNum; ++j)
		{
			bpfEnable[i][j] = 0;
			biquadInit(&resos[i].nodes[j]);
		}
	}
	for (int i = 0; i < 65536; ++i)
	{
		cos_table[i] = cosf((float)i * 2.0 * M_PI / 65536.0);
	}

	biquadInit(&lpf1);
	biquadInit(&hpf1);
	biquadInit(&lpf2);
	biquadInit(&hpf2);
	biquadInit(&lpf3);
	biquadInit(&hpf3);
	biquadInit(&lpf4);
	biquadInit(&hpf4);

}


juce::AudioProcessorValueTreeState::ParameterLayout LModelAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<juce::AudioParameterFloat>("harmN", "harmN", 1, MaxEQNodeNum - 1, 1));
	layout.add(std::make_unique<juce::AudioParameterFloat>("freqMix", "freqMix", 0, 1, 1));
	layout.add(std::make_unique<juce::AudioParameterFloat>("mulVal", "mulVal", 0, 2, 1));
	layout.add(std::make_unique<juce::AudioParameterFloat>("powVal", "powVal", 1, 2, 1.5));
	layout.add(std::make_unique<juce::AudioParameterFloat>("decayVal", "decayVal", 0, 1, 0.5));
	layout.add(std::make_unique<juce::AudioParameterFloat>("reso", "reso", 0.25, 1, 0.5));
	layout.add(std::make_unique<juce::AudioParameterFloat>("mix", "mix", 0, 1, 1));
	layout.add(std::make_unique<juce::AudioParameterFloat>("prelp", "prelp", 0, 1, 1));
	layout.add(std::make_unique<juce::AudioParameterFloat>("prehp", "prehp", 0, 1, 0));

	//layout.add(std::make_unique<juce::AudioParameterFloat>("minKey", "minKey", 0, 128, 24));

	return layout;
}

LModelAudioProcessor::~LModelAudioProcessor()
{
}

//==============================================================================
const juce::String LModelAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool LModelAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool LModelAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool LModelAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double LModelAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int LModelAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int LModelAudioProcessor::getCurrentProgram()
{
	return 0;
}

void LModelAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String LModelAudioProcessor::getProgramName(int index)
{
	return "IIRPitchShifter";
}

void LModelAudioProcessor::changeProgramName(int index, const juce::String& newName)
{

}

//==============================================================================
void LModelAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void LModelAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LModelAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
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


void LModelAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	int isMidiUpdata = 0;
	juce::MidiMessage MidiMsg;//先处理midi事件
	int MidiTime;
	juce::MidiBuffer::Iterator MidiBuf(midiMessages);
	while (MidiBuf.getNextEvent(MidiMsg, MidiTime))
	{
		if (MidiMsg.isNoteOn())
		{
			int note = MidiMsg.getNoteNumber() - 24;
			if (note > 0 && note < MaxKeyNum)
			{
				manager.keyState[note] = 1;
				isMidiUpdata = 1;
			}
		}
		if (MidiMsg.isNoteOff())
		{
			int note = MidiMsg.getNoteNumber() - 24;
			if (note > 0 && note < MaxKeyNum)
			{
				manager.keyState[note] = 0;
				isMidiUpdata = 1;

				for (int i = 0; i < MaxEQNodeNum; ++i)
				{
					biquadInit(&resos[note].nodes[i]);
				}
			}
		}
	}
	midiMessages.clear();

	const int numSamples = buffer.getNumSamples();
	float* wavbufl = buffer.getWritePointer(0);
	float* wavbufr = buffer.getWritePointer(1);
	const float* recbufl = buffer.getReadPointer(0);
	const float* recbufr = buffer.getReadPointer(1);

	float SampleRate = getSampleRate();

	float harmN = *Params.getRawParameterValue("harmN");
	float freqMix = *Params.getRawParameterValue("freqMix");
	float mulVal = *Params.getRawParameterValue("mulVal");
	float powVal = *Params.getRawParameterValue("powVal");
	float decayVal = *Params.getRawParameterValue("decayVal");
	float reso = *Params.getRawParameterValue("reso");
	float mix = *Params.getRawParameterValue("mix");
	float prelp = *Params.getRawParameterValue("prelp");
	float prehp = *Params.getRawParameterValue("prehp");

	prelp = prelp * prelp * prelp;
	prehp = prehp * prehp * prehp;
	prelp = prelp * 0.998 + 0.001;
	prehp = prehp * 0.998 + 0.001;

	biquadApplyLPF(&lpf1, prelp, 1.0 / sqrtf(2), 1.0);//pre lp/hp
	biquadApplyHPF(&hpf1, prehp, 1.0 / sqrtf(2), 1.0);
	biquadApplyLPF(&lpf2, prelp, 1.0 / sqrtf(2), 1.0);
	biquadApplyHPF(&hpf2, prehp, 1.0 / sqrtf(2), 1.0);
	biquadApplyLPF(&lpf3, prelp, 1.0 / sqrtf(2), 1.0);
	biquadApplyHPF(&hpf3, prehp, 1.0 / sqrtf(2), 1.0);
	biquadApplyLPF(&lpf4, prelp, 1.0 / sqrtf(2), 1.0);
	biquadApplyHPF(&hpf4, prehp, 1.0 / sqrtf(2), 1.0);

	manager.harmN = harmN;
	manager.harmFreqMix = freqMix;
	manager.harmPowValue = powVal;
	manager.harmMulValue = mulVal;
	manager.harmDecayValue = log10f(decayVal * 9.0 + 1.0);

	float expreso = expf(reso * 7);
	float iirGain = 4.0 * sqrt(sqrt(expreso));

	if (!manager.firEnable)
	{
		for (int i = 0; i < numSamples; ++i)
		{
			tmpbufl[i] = 0;
			tmpbufr[i] = 0;
			StereoSignal tmp = { recbufl[i],recbufr[i] };
			tmp = biquadProcessStereo(&lpf1, tmp);//pre lp/hp
			tmp = biquadProcessStereo(&hpf1, tmp);
			tmp = biquadProcessStereo(&lpf2, tmp);
			tmp = biquadProcessStereo(&hpf2, tmp);
			tmp = biquadProcessStereo(&lpf3, tmp);
			tmp = biquadProcessStereo(&hpf3, tmp);
			tmp = biquadProcessStereo(&lpf4, tmp);
			tmp = biquadProcessStereo(&hpf4, tmp);
			tmpbufl2[i] = tmp.l;
			tmpbufr2[i] = tmp.r;
			tmpbufl3[i] = recbufl[i];
			tmpbufr3[i] = recbufr[i];

		}

		for (int n = 0; n < MaxKeyNum; ++n)
		{
			if (manager.keyState[n])
			{
				for (int m = 0; m < manager.harmN; ++m)
				{
					if (bpfEnable[n][m] == 1)
					{
						StereoSignal tmp = { 0,0 };
						for (int i = 0; i < numSamples; ++i)
						{
							tmp = { tmpbufl2[i],tmpbufr2[i] };
							tmp = biquadProcessStereo(&resos[n].nodes[m], tmp);//3倍采样率
							tmp = biquadProcessStereo(&resos[n].nodes[m], tmp);
							tmp = biquadProcessStereo(&resos[n].nodes[m], tmp);
							//tmpbufl[i] += tmp.l * gains[n][m];
							//tmpbufr[i] += tmp.r * gains[n][m]; 
							tmpbufl[i] += tmp.l;
							tmpbufr[i] += tmp.r;
						}
					}
				}
			}
		}
		for (int i = 0; i < numSamples; ++i)
		{
			wavbufl[i] = tmpbufl[i] * (mix)*iirGain + tmpbufl3[i] * (1.0 - mix);
			wavbufr[i] = tmpbufr[i] * (mix)*iirGain + tmpbufr3[i] * (1.0 - mix);
		}
	}
	else
	{
		for (int i = 0; i < numSamples; ++i)
		{
			tmpbufl[i] = recbufl[i];
			tmpbufr[i] = recbufr[i];

			StereoSignal tmp = { tmpbufl[i],tmpbufr[i] };
			tmp = biquadProcessStereo(&lpf1, tmp);
			tmp = biquadProcessStereo(&hpf1, tmp);
			tmp = biquadProcessStereo(&lpf2, tmp);
			tmp = biquadProcessStereo(&hpf2, tmp);
			tmp = biquadProcessStereo(&lpf3, tmp);
			tmp = biquadProcessStereo(&hpf3, tmp);
			tmp = biquadProcessStereo(&lpf4, tmp);
			tmp = biquadProcessStereo(&hpf4, tmp);

			tmpbufl2[i] = tmp.l;
			tmpbufr2[i] = tmp.r;
		}
		firReso.proc(tmpbufl, tmpbufr,	//mix信号
			tmpbufl2, tmpbufr2,			//要fir的信号
			wavbufl, wavbufr,			//输出
			numSamples, mix, reso * FFT_Length);

	}

	if (procCount++ >= 8 || isMidiUpdata)//更新biquad
	{
		procCount = 0;
		if (manager.firEnable)
		{
			for (int i = 0; i < FFT_Length; ++i)
			{
				firDat[i] = 0;
			}
		}
		for (int i = 0; i < MaxKeyNum; ++i)
		{
			if (manager.keyState[i])
			{
				float f0 = powf(2.0, (float)(i + 72) / 12.0);
				float f0mul = f0;
				float gain = 1.0;
				for (int j = 0; j < manager.harmN; ++j)
				{
					float freq = f0 * ((float)j * manager.harmMulValue + 1) * (1.0 - manager.harmFreqMix) + f0mul * manager.harmFreqMix;
					freq = freq / SampleRate;
					bpfEnable[i][j] = 0;
					if (freq > 0.002 && freq < 0.5)
					{
						if (!manager.firEnable)
						{
							//resos[i].nodes[j];//要更新的biquad

							float gain2 = gain;//妈的不懂怎么补偿
							//用bpf有个问题就是，reso拉高了声音变小
							//补偿有个假设是，频谱中每个频率的能量相同（当作白噪）
							biquadApplyBPF(&resos[i].nodes[j], freq / 3.0, expreso, gain2);
							bpfEnable[i][j] = 1;
						}
						else
						{
							float gain2 = gain / FFT_Length * 16.0;//响度在这调
							for (int x = 0; x < FFT_Length; ++x)
							{
								firDat[x] += cos_table[(uint16_t)((float)x * freq * 32768.0)] * gain2;
								//firDat[x] += cosf((uint16_t)((float)x * M_PI * freq)) * gain;
							}
						}
					}
					gain *= manager.harmDecayValue;
					f0mul *= manager.harmPowValue;
				}
			}
		}
		if (manager.firEnable)
		{
			firReso.updataDirect(firDat, reso * FFT_Length);
		}
	}
}

//==============================================================================
bool LModelAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LModelAudioProcessor::createEditor()
{
	return new LModelAudioProcessorEditor(*this);

	//return new juce::GenericAudioProcessorEditor(*this);//自动绘制(调试)
}

//==============================================================================

void LModelAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.

	// 创建一个 XML 节点
	juce::XmlElement xml("LMEQ_Settings");

	juce::MemoryBlock eqDataBlock;
	eqDataBlock.append(&manager, sizeof(ResonatorManager));
	juce::String base64Data = eqDataBlock.toBase64Encoding();
	xml.setAttribute("VIB_MANAGER", base64Data);//添加resonance数据

	auto state = Params.copyState();
	xml.setAttribute("Knob_Data", state.toXmlString());//添加旋钮数据

	juce::String xmlString = xml.toString();
	destData.append(xmlString.toRawUTF8(), xmlString.getNumBytesAsUTF8());
}

void LModelAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	  // 将 data 转换为字符串以解析 XML
	juce::String xmlString(static_cast<const char*>(data), sizeInBytes);

	// 解析 XML
	std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(xmlString));
	if (xml == nullptr || !xml->hasTagName("LMEQ_Settings"))
	{
		DBG("Error: Unable to load XML settings");
		return;
	}

	juce::String base64Data = xml->getStringAttribute("VIB_MANAGER");
	juce::MemoryBlock eqDataBlock;
	eqDataBlock.fromBase64Encoding(base64Data);
	if (eqDataBlock.getData() != NULL)
	{
		std::memcpy(&manager, eqDataBlock.getData(), sizeof(ResonatorManager));
	}

	auto KnobDataXML = xml->getStringAttribute("Knob_Data");
	Params.replaceState(juce::ValueTree::fromXml(KnobDataXML));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new LModelAudioProcessor();
}
