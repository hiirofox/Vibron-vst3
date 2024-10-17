#pragma once

#include <JuceHeader.h>
#include <math.h>
#include "../dsp/resonator.h"

class Keyborad :public juce::Component
{
private:
	int displayMode = 0;//0:exp,1:linear
	int minNote = 24, maxNote = 60;//显示的部分(4个8度)
	int selectNum;//鼠标悬停对应的键
	
	ResonatorManager* reso = NULL;
public:
	Keyborad();
	void init(ResonatorManager* reso);
	//juce:
	void paint(juce::Graphics& g) override;
	void resized() override;


	void mouseDown(const juce::MouseEvent& event) override; //重写鼠标按下的回调
	//void mouseDrag(const juce::MouseEvent& event) override; // 重写鼠标拖动的回调
	void mouseUp(const juce::MouseEvent& event) override; // 重写鼠标释放的回调
	void mouseMove(const juce::MouseEvent& event) override; // 重写鼠标移动的回调
	//void mouseDoubleClick(const juce::MouseEvent& event) override; // 重写鼠标双击的回调
	//void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;//鼠标滚轮

	void setDisplayMode(int displayMode);
	int getDisplayMode();

	int isFirEnable();

	void setNoteRange(int minNote, int maxNote);
	int getMinNote();
	int getMaxNote();

};