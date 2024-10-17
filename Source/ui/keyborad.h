#pragma once

#include <JuceHeader.h>
#include <math.h>
#include "../dsp/resonator.h"

class Keyborad :public juce::Component
{
private:
	int displayMode = 0;//0:exp,1:linear
	int minNote = 24, maxNote = 60;//��ʾ�Ĳ���(4��8��)
	int selectNum;//�����ͣ��Ӧ�ļ�
	
	ResonatorManager* reso = NULL;
public:
	Keyborad();
	void init(ResonatorManager* reso);
	//juce:
	void paint(juce::Graphics& g) override;
	void resized() override;


	void mouseDown(const juce::MouseEvent& event) override; //��д��갴�µĻص�
	//void mouseDrag(const juce::MouseEvent& event) override; // ��д����϶��Ļص�
	void mouseUp(const juce::MouseEvent& event) override; // ��д����ͷŵĻص�
	void mouseMove(const juce::MouseEvent& event) override; // ��д����ƶ��Ļص�
	//void mouseDoubleClick(const juce::MouseEvent& event) override; // ��д���˫���Ļص�
	//void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;//������

	void setDisplayMode(int displayMode);
	int getDisplayMode();

	int isFirEnable();

	void setNoteRange(int minNote, int maxNote);
	int getMinNote();
	int getMaxNote();

};