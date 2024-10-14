#include "fir.h"

FirResonator::FirResonator()
{
	for (int i = 0; i < FFT_Length; ++i)
	{
		dat[i].re = dat[i].im = 0;
	}
	for (int i = 0; i < FFT_Length * 2; ++i)
	{
		convDat[i] = 0;
	}
	for (int i = 0; i < FFT_Length * 2; ++i)
	{
		dlyDatL[i] = 0;
		dlyDatR[i] = 0;
	}
}

void FirResonator::init(Resonator* eq)
{
	this->iir = eq;
}

void FirResonator::updataFromFR()//根据频响生成卷积核
{
	if (iir == NULL)return;
	for (int i = 0; i < FFT_Length >> 1; ++i)
	{
		complex_f32_t v = ResonatorGetFreqResponse(iir, (float)i / FFT_Length * 2.0 * M_PI);
		dat[i].re = sqrtf(v.re * v.re + v.im * v.im);
		dat[i].im = 0;
		dat[FFT_Length - i - 1] = dat[i];
	}
	fft_f32(dat, FFT_Length, -1);//ifft
	for (int i = 0; i < FFT_Length; ++i)
	{
		dat[i].re *= (cosf((float)i / FFT_Length * M_PI) + 1.0) * 0.5;
	}
	for (int i = 0; i < FFT_Length; ++i)
	{
		convDat[FFT_Length + i - 1] = dat[i].re;
		convDat[FFT_Length - i - 1] = dat[i].re;
	}
	firSum = 0;
	for (int i = 0; i < FFT_Length * 2; ++i)// to unit
	{
		firSum += convDat[i];
	}
	firSum = 1.0 / firSum;
}
void FirResonator::updataDirect(float* dat, int len)//直接用
{
	for (int i = 0; i < len; ++i)
	{
		dat[i] *= (cosf((float)i / len * M_PI) + 1.0) * 0.5;
	}
	for (int i = 0; i < len; ++i)
	{
		convDat[len + i - 1] = dat[i];
		convDat[len - i - 1] = dat[i];
		//convDat[i] = dat[i];//懒得搓线性了
	}
	/*
	firSum = 0;
	for (int i = 0; i < FFT_Length * 2; ++i)// to unit
	{
		firSum += convDat[i];
	}
	firSum = 1.0 / firSum;*/
	firSum = 1.0;
}

void FirResonator::proc(const float* mixl, const float* mixr,
	const float* inl, const float* inr,
	float* outl, float* outr,
	int numSample, float mix, int len)
{
	len = len * 2;
	for (int i = 0; i < numSample; ++i)
	{
		float suml = 0, sumr = 0;
		float datl = inl[i];
		float datr = inr[i];
		dlyDatL[pos] = datl;
		dlyDatR[pos] = datr;
		for (int j = 0, k = pos; j < len; ++j)
		{
			suml += dlyDatL[k] * convDat[j];
			sumr += dlyDatR[k] * convDat[j];
			k++;
			if (k >= len)k = 0;
		}

		int dlypos1 = (pos + len / 2) % len;//fir mix的延迟补偿
		int dlypos2 = pos % len;
		dlyDryL[dlypos1] = mixl[i];
		dlyDryR[dlypos1] = mixr[i];
		datl = dlyDryL[dlypos2];
		datr = dlyDryR[dlypos2];

		pos++;
		if (pos >= len)pos = 0;

		//lpFirSum += 0.05 * (firSum - lpFirSum);
		//outl[i] = suml * lpFirSum;
		//outr[i] = sumr * lpFirSum;

		outl[i] = suml * firSum * mix + datl * (1.0 - mix);
		outr[i] = sumr * firSum * mix + datr * (1.0 - mix);
	}
}
