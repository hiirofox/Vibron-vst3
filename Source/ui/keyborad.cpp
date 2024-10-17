#include "keyborad.h"

bool keyColor[12] = { 0,1,0,1,0,0,1,0,1,0,1,0 };//ee

Keyborad::Keyborad()
{
}

void Keyborad::init(ResonatorManager* reso)
{
	this->reso = reso;
}

void Keyborad::paint(juce::Graphics& g)
{
	juce::Rectangle<int> rect = getBounds();
	int w = rect.getWidth(), h = rect.getHeight();
	int h2 = h / 4 * 3;

	float lpos = minNote;
	float rpos = maxNote;
	if (displayMode == 1)//线性的那种显示方式
	{
		lpos = powf(2.0, lpos / 12.0);
		rpos = powf(2.0, rpos / 12.0);
	}
	for (int i = minNote; i < maxNote; ++i)
	{
		if (keyColor[i % 12])
		{
			if (i != selectNum)
				g.setColour(juce::Colour(0xff000000));
			else
				g.setColour(juce::Colour(0xff333333));
		}
		else
		{
			if (i != selectNum)
				g.setColour(juce::Colour(0xffffffff));
			else
				g.setColour(juce::Colour(0xff777777));
		}
		float ipos0 = i;
		float ipos1 = i + 1;
		if (displayMode == 1)
		{
			ipos0 = powf(2.0, ipos0 / 12.0);
			ipos1 = powf(2.0, ipos1 / 12.0);
		}
		float xl = (ipos0 - lpos) / (rpos - lpos) * w;
		float xr = (ipos1 - lpos) / (rpos - lpos) * w;
		g.fillRect(juce::Rectangle<float>(xl, h2, xr - xl - 1, h - h2));
		g.setColour(juce::Colour(0xff00ff00));

		if (reso->keyState[i] == 1)
		{
			if (!keyColor[i % 12])
				g.setColour(juce::Colour(0xffff0000));
			else
				g.setColour(juce::Colour(0xff00ff00));
			g.fillRect(juce::Rectangle<float>((xl + xr) / 2.0 - 4 - 1, h2 + (h - h2) / 2 - 4, 8, 8));
		}
	}

	if (reso->firEnable)
	{
		g.setColour(juce::Colour(0xff00ffff));
	}
	else
	{
		g.setColour(juce::Colour(0xff00ff00));
	}
	int halfKeyWidth = w / (maxNote - minNote) / 2;
	for (int i = 0; i < MaxKeyNum; ++i)
	{
		if (reso->keyState[i])
		{
			float f0 = powf(2.0, (float)i / 12.0);
			float f0mul = f0;
			float gain = 1.0;
			for (int j = 0; j < reso->harmN; ++j)
			{
				float freq = f0 * (j * reso->harmMulValue + 1) * (1.0 - reso->harmFreqMix) + f0mul * reso->harmFreqMix;
				if (displayMode == 0)
				{
					freq = logf(freq) / logf(2.0) * 12.0;
					freq = (freq - minNote) / (maxNote - minNote);
					if (freq >= 0 || freq <= 1.0)
					{
						g.drawLine(freq * w + halfKeyWidth, h2 - 16, freq * w + halfKeyWidth, h2 - gain * h / 3 - 16, 2);
					}
				}
				gain *= reso->harmDecayValue;
				f0mul *= reso->harmPowValue;
			}
		}
	}

	g.setColour(juce::Colour(0xff00ff00));
	juce::Path rectanglePath;
	rectanglePath.addRectangle(0, h2, w, h - h2);
	rectanglePath.addRectangle(0, 0, w, h2 - 16);
	juce::PathStrokeType strokeType(2.0f);
	g.strokePath(rectanglePath, strokeType);
}

void Keyborad::resized()
{
}

void Keyborad::mouseDown(const juce::MouseEvent& event)
{
	juce::Rectangle<int> rect = getBounds();
	int w = rect.getWidth(), h = rect.getHeight();
	int h2 = h / 4 * 3;
	if (event.position.getY() < h2 || event.position.getY() > h ||
		event.position.getX() < 0 || event.position.getX() > w)
	{
		selectNum == -1;
		return;
	}
	if (event.mods.isLeftButtonDown())
	{
		if (selectNum >= 0 && selectNum < MaxKeyNum)
		{
			reso->keyState[selectNum] = reso->keyState[selectNum] != 0 ? 0 : 1;
		}
	}
	if (event.mods.isMiddleButtonDown())
	{
		reso->firEnable = !reso->firEnable;
	}
}

void Keyborad::mouseUp(const juce::MouseEvent& event)
{
}

void Keyborad::mouseMove(const juce::MouseEvent& event)
{
	juce::Rectangle<int> rect = getBounds();
	int w = rect.getWidth(), h = rect.getHeight();
	int h2 = h / 4 * 3;
	if (event.position.getY() < h2 || event.position.getY() > h ||
		event.position.getX() < 0 || event.position.getX() > w)
	{
		selectNum == -1;
		return;
	}
	float lpos = (float)minNote;
	float rpos = (float)maxNote;
	float posx = (float)event.position.getX() / w;
	if (displayMode == 1)
	{
		lpos = powf(2.0, minNote / 12.0);
		rpos = powf(2.0, maxNote / 12.0);
		posx = posx * (rpos - lpos) + lpos;
		selectNum = logf(posx) / logf(2) * 12.0;
	}
	else
	{
		posx = posx * (rpos - lpos) + lpos;
		selectNum = posx;
	}
}

void Keyborad::setDisplayMode(int displayMode)
{
	this->displayMode = displayMode;
}

int Keyborad::getDisplayMode()
{
	return displayMode;
}

int Keyborad::isFirEnable()
{
	return reso->firEnable;
}

void Keyborad::setNoteRange(int minNote, int maxNote)
{
	this->minNote = minNote;
	this->maxNote = maxNote;
}

int Keyborad::getMinNote()
{
	return minNote;
}

int Keyborad::getMaxNote()
{
	return maxNote;
}

