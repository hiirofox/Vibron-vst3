#pragma once

#include <math.h>
#include "biquad.h"

#define MaxEQNodeNum 32//����ģ��г��
typedef struct
{
	Biquad nodes[MaxEQNodeNum];
	int nodeN;
}Resonator;

void ResonatorInit(Resonator* eq, int nodeN);
complex_f32_t ResonatorGetFreqResponse(Resonator* eq, float w);//��ȡ����EQ��Ƶ��
float ResonatorProcess(Resonator* eq, float input);
StereoSignal ResonatorProcessStereo(Resonator* eq, StereoSignal input);


#define MaxKeyNum 128//ÿ��������Ӧһ�� һ����128����
typedef struct
{
	int keyState[MaxKeyNum];
	int harmN;				//г������(������MaxEQNodeNum)
	float harmFreqMix;		//г��Ƶ�ʹ�ϵ(0:����->1:ָ��)
	float harmPowValue;		//г��ָ��Ƶ�ʵ�ϵ��(����2��Ƶ��2)
	float harmMulValue;		//г������Ƶ�ʵ�ϵ��(����1�Ǹ�һ��1����)
	float harmDecayValue;	//������һ��г�������˥��(0->1)

	int firEnable;
}ResonatorManager;

