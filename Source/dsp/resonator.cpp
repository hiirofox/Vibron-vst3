#include "resonator.h"

void ResonatorInit(Resonator* eq, int nodeN)
{
	for (int i = 0; i < MaxEQNodeNum; ++i)
	{
		biquadInit(&eq->nodes[i]);
	}
	eq->nodeN = nodeN;
}

complex_f32_t ResonatorGetFreqResponse(Resonator* eq, float w)
{
	complex_f32_t dat = complex_f32_t{ 0,0 };
	for (int i = 0; i < eq->nodeN; ++i)//滤波器并联等于传递函数相加
	{
		dat = cadd(dat, biquadGetFreqResponse(&eq->nodes[i], w));
	}
	return dat;
}


float ResonatorProcess(Resonator* eq, float input)
{
	float out = 0;
	for (int i = 0; i < eq->nodeN; ++i)
	{
		out += biquadProcess(&eq->nodes[i], input);
	}
	return out;
}

StereoSignal ResonatorProcessStereo(Resonator* eq, StereoSignal input)
{
	StereoSignal out = { 0,0 };
	for (int i = 0; i < eq->nodeN; ++i)
	{
		StereoSignal tmp = biquadProcessStereo(&eq->nodes[i], input);
		out.l += tmp.l;
		out.r += tmp.r;
	}
	return out;
}

