/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LModelAudioProcessorEditor::LModelAudioProcessorEditor(LModelAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setResizable(true, true); // 允许窗口调整大小

	setOpaque(false);  // 允许在边框外面绘制

	//setResizeLimits(64 * 11, 64 * 5, 10000, 10000); // 设置最小宽高为300x200，最大宽高为800x600
	setSize(64 * 13, 64 * 4);

	//constrainer.setFixedAspectRatio(11.0 / 4.0);  // 设置为16:9比例
	//setConstrainer(&constrainer);  // 绑定窗口的宽高限制

	kb.init(&audioProcessor.manager);

	addAndMakeVisible(kb);


	K_HarmN.setText("HarmN");
	K_HarmN.ParamLink(audioProcessor.GetParams(), "harmN");
	addAndMakeVisible(K_HarmN);
	K_FreqMix.setText("FreqMix");
	K_FreqMix.ParamLink(audioProcessor.GetParams(), "freqMix");
	addAndMakeVisible(K_FreqMix);
	K_PowVal.setText("PowVal");
	K_PowVal.ParamLink(audioProcessor.GetParams(), "powVal");
	addAndMakeVisible(K_PowVal);
	K_MulVal.setText("mulVal");
	K_MulVal.ParamLink(audioProcessor.GetParams(), "mulVal");
	addAndMakeVisible(K_MulVal);
	K_DecayVal.setText("DecayVal");
	K_DecayVal.ParamLink(audioProcessor.GetParams(), "decayVal");
	addAndMakeVisible(K_DecayVal);
	K_Reso.setText("Reso");
	K_Reso.ParamLink(audioProcessor.GetParams(), "reso");
	addAndMakeVisible(K_Reso);
	K_Mix.setText("Mix");
	K_Mix.ParamLink(audioProcessor.GetParams(), "mix");
	addAndMakeVisible(K_Mix);
	K_PreLP.setText("preLP");
	K_PreLP.ParamLink(audioProcessor.GetParams(), "prelp");
	addAndMakeVisible(K_PreLP);
	K_PreHP.setText("preHP");
	K_PreHP.ParamLink(audioProcessor.GetParams(), "prehp");
	addAndMakeVisible(K_PreHP);

	startTimerHz(30);

}

LModelAudioProcessorEditor::~LModelAudioProcessorEditor()
{
}

//==============================================================================
void LModelAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colour(0x00, 0x00, 0x00));

	g.fillAll();
	g.setFont(juce::Font("FIXEDSYS", 16.0, 1));
	g.setColour(juce::Colour(0xffffffff));;

	int w = getBounds().getWidth(), h = getBounds().getHeight();

	juce::String tmp = "";
	char number[24];
	for (int i = 0; i < MaxKeyNum; ++i)
	{
		if (audioProcessor.manager.keyState[i])
		{
			sprintf(number, "%d ", i);
			tmp += number;
		}
	}
	g.drawText(tmp, juce::Rectangle<float>(0, h - 16, w, 16), 1);
}

void LModelAudioProcessorEditor::resized()
{
	juce::Rectangle<int> bound = getBounds();
	int x = bound.getX(), y = bound.getY(), w = bound.getWidth(), h = bound.getHeight();
	auto convXY = juce::Rectangle<int>::leftTopRightBottom;
	kb.setBounds(convXY(32, 32, w - 64 - 192, h - 32));

	K_MulVal.setBounds(w - 192 - 32 + 64 * 0, 32 + 64 * 0, 64, 64);
	K_FreqMix.setBounds(w - 192 - 32 + 64 * 1, 32 + 64 * 0, 64, 64);
	K_PowVal.setBounds(w - 192 - 32 + 64 * 2, 32 + 64 * 0, 64, 64);
	K_HarmN.setBounds(w - 192 - 32 + 64 * 0, 32 + 64 * 1, 64, 64);
	K_DecayVal.setBounds(w - 192 - 32 + 64 * 1, 32 + 64 * 1, 64, 64);
	K_Reso.setBounds(w - 192 - 32 + 64 * 2, 32 + 64 * 1, 64, 64);
	K_Mix.setBounds(w - 192 - 32 + 64 * 0, 32 + 64 * 2, 64, 64);
	K_PreLP.setBounds(w - 192 - 32 + 64 * 1, 32 + 64 * 2, 64, 64);
	K_PreHP.setBounds(w - 192 - 32 + 64 * 2, 32 + 64 * 2, 64, 64);
}

#define MaxFRDatLen (32768)
void LModelAudioProcessorEditor::timerCallback()
{
	audioProcessor.manager.firEnable = kb.isFirEnable();
	if (audioProcessor.manager.firEnable)
	{
		K_Reso.setText("firSize");
	}
	else
	{
		K_Reso.setText("Reso");
	}
	repaint();
}
