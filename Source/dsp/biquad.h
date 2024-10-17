#pragma once

#include <math.h>
#include "complex.h"
//#include <complex>

#define M_PI 3.1415926535897932384626

typedef struct {
	float l, r;
}StereoSignal;

typedef struct {
	float ctof, reso, gain;//�˲����Ĳ���
	int type;//�˲���������

	float b0, b1, b2;  // �˲����ķ���ϵ��
	float a1, a2;      // �˲����ķ�ĸϵ�� (a0 �Ѿ�����һ��)

	float dy1l, dy2l, dx1l, dx2l;
	float dy1r, dy2r, dx1r, dx2r;
} Biquad;

void biquadInit(Biquad* filter);
float biquadProcess(Biquad* filter, float input);
StereoSignal biquadProcessStereo(Biquad* filter, StereoSignal input);
complex_f32_t biquadGetFreqResponse(Biquad* f, float w);//w:[0,2pi]
//std::complex<float> biquadGetFreqResponse(Biquad* f, float w);//w:[0,2pi]
void biquadApplyLPF(Biquad* filter, float ctof, float reso, float gain);
void biquadApplyBPF(Biquad* filter, float ctof, float reso, float gain);
void biquadApplyHPF(Biquad* filter, float ctof, float reso, float gain);

void biquadApplyLPFNoReso(Biquad* filter, float ctof);
void biquadApplyHPFNoReso(Biquad* filter, float ctof);

void biquadApplyPeaking(Biquad* filter, float ctof, float reso, float gain);
void biquadApplyLowShelf(Biquad* filter, float ctof, float reso, float gain);
void biquadApplyHighShelf(Biquad* filter, float ctof, float reso, float gain);