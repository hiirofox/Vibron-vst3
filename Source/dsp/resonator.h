#pragma once

#include <math.h>
#include "biquad.h"

#define MaxEQNodeNum 32//用来模拟谐波
typedef struct
{
	Biquad nodes[MaxEQNodeNum];
	int nodeN;
}Resonator;

void ResonatorInit(Resonator* eq, int nodeN);
complex_f32_t ResonatorGetFreqResponse(Resonator* eq, float w);//获取整个EQ的频响
float ResonatorProcess(Resonator* eq, float input);
StereoSignal ResonatorProcessStereo(Resonator* eq, StereoSignal input);


#define MaxKeyNum 128//每个按键对应一个 一共有128个键
typedef struct
{
	int keyState[MaxKeyNum];
	int harmN;				//谐波个数(不超过MaxEQNodeNum)
	float harmFreqMix;		//谐波频率关系(0:线性->1:指数)
	float harmPowValue;		//谐波指数频率的系数(例如2倍频是2)
	float harmMulValue;		//谐波线性频率的系数(例如1是隔一个1倍？)
	float harmDecayValue;	//对于上一个谐波增益的衰减(0->1)

	int firEnable;
}ResonatorManager;

