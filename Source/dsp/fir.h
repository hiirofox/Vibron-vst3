#pragma once

#include <JuceHeader.h>
#include "resonator.h"
#include "complex.h"
#include "fft.h"

#define FFT_Length 2048//fir_length = fft_length*2 ÒòÎªÒª¶Ô³Æ
class FirResonator
{
private:
	Resonator* iir = NULL;
	complex_f32_t dat[FFT_Length];//H(z)
	float convDat[FFT_Length * 2];
	float dlyDatL[FFT_Length * 2];
	float dlyDatR[FFT_Length * 2];
	float dlyDryL[FFT_Length * 2];
	float dlyDryR[FFT_Length * 2];
	int pos = 0;
	float firSum = 0;
	float lpFirSum = 0;
public:
	FirResonator();
	void init(Resonator* eq);
	void updataFromFR();//iir->fir
	void updataDirect(float* dat, int len);//sin->fir
	void proc(const float* mixl, const float* mixr,
		const float* inl, const float* inr,
		float* outl, float* outr,
		int numSample, float mix, int len);
};