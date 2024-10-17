#include "biquad.h"

// 初始化Biquad滤波器
void biquadInit(Biquad* filter)
{
	filter->b0 = 0;
	filter->b1 = 0;
	filter->b2 = 0;
	filter->a1 = 0;
	filter->a2 = 0;
	filter->dy1l = 0;
	filter->dy2l = 0;
	filter->dx1l = 0;
	filter->dx2l = 0;
	filter->dy1r = 0;
	filter->dy2r = 0;
	filter->dx1r = 0;
	filter->dx2r = 0;

	filter->ctof = 0;
	filter->reso = 0;
	filter->gain = 0;
	filter->type = -1;

}

// 滤波函数
float biquadProcess(Biquad* filter, float input)
{
	float vout = filter->b0 * input +
		filter->b1 * filter->dx1l +
		filter->b2 * filter->dx2l -
		filter->a1 * filter->dy1l -
		filter->a2 * filter->dy2l;
	filter->dy2l = filter->dy1l;
	filter->dy1l = vout;
	filter->dx2l = filter->dx1l;
	filter->dx1l = input;
	return vout;
}

StereoSignal biquadProcessStereo(Biquad* filter, StereoSignal input)
{
	float voutl = filter->b0 * input.l +
		filter->b1 * filter->dx1l +
		filter->b2 * filter->dx2l -
		filter->a1 * filter->dy1l -
		filter->a2 * filter->dy2l;
	filter->dy2l = filter->dy1l;
	filter->dy1l = voutl;
	filter->dx2l = filter->dx1l;
	filter->dx1l = input.l;
	float voutr = filter->b0 * input.r +
		filter->b1 * filter->dx1r +
		filter->b2 * filter->dx2r -
		filter->a1 * filter->dy1r -
		filter->a2 * filter->dy2r;
	filter->dy2r = filter->dy1r;
	filter->dy1r = voutr;
	filter->dx2r = filter->dx1r;
	filter->dx1r = input.r;
	return StereoSignal{ voutl,voutr };
}


//绘制可能需要的，获取biquad的频响

complex_f32_t biquadGetFreqResponse(Biquad* f, float w)
{
	complex_f32_t z1 = complex_f32_t{ cosf(w),-sinf(w) };
	complex_f32_t z2 = cmul(z1, z1);
	complex_f32_t y = cadd(cmulreal(z1, f->b1), cmulreal(z2, f->b2));
	y.re += f->b0;
	complex_f32_t x = cadd(cmulreal(z1, f->a1), cmulreal(z2, f->a2));
	x.re += 1.0;
	return cdiv(y, x);
}
/*
std::complex<float> biquadGetFreqResponse(Biquad* f, float w)
{
	std::complex<float> z1 = { cosf(w),sinf(-w) };
	std::complex<float> z2 = z1 * z1;
	return (z2 * f->b2 + z1 * f->b1 + f->b0) / (z2 * f->a2 + z1 * f->a1 + 1.0f);
}*/

//应用LPF
void biquadApplyLPF(Biquad* filter, float ctof, float reso, float gain) {
	filter->type = 1;
	filter->ctof = ctof;
	filter->reso = reso;
	filter->gain = 1.0;

	float A = 1;
	float omega = M_PI * ctof;
	float alpha = sinf(omega) / (2.0f * reso); // 计算带宽参数
	float cosw0 = cosf(omega);
	float a0 = 1.0f + alpha;
	filter->a1 = -2.0f * cosw0 / a0;
	filter->a2 = (1.0f - alpha) / a0;
	filter->b0 = (1.0f - cosw0) / 2.0f / a0 * A;
	filter->b1 = (1.0f - cosw0) / a0;
	filter->b2 = (1.0f - cosw0) / 2.0f / a0;
}

// 带通滤波器（Bandpass Filter）系数计算
void biquadApplyBPF(Biquad* filter, float ctof, float reso, float gain) {
	filter->type = 6;
	filter->ctof = ctof;
	filter->reso = reso;
	filter->gain = 1.0;

	float omega = M_PI * ctof;
	float A = gain;
	float alpha = sinf(omega) / (2.0f * reso); // 计算带宽参数
	float cosw0 = cosf(omega);
	float a0 = 1.0f + alpha;
	filter->a1 = -2.0f * cosw0 / a0;
	filter->a2 = (1.0f - alpha) / a0;
	filter->b0 = alpha * A;
	filter->b1 = 0.0f;
	filter->b2 = -alpha * A;
}
// 高通滤波器（HPF）系数计算
void biquadApplyHPF(Biquad* filter, float ctof, float reso, float gain) {
	filter->type = 2;
	filter->ctof = ctof;
	filter->reso = reso;
	filter->gain = 1.0;

	float A = 1;
	float omega = M_PI * ctof;
	float alpha = sinf(omega) / (2.0f * reso); // 带宽参数
	float cosw0 = cosf(omega);
	float a0 = 1.0f + alpha;
	filter->a1 = -2.0f * cosw0 / a0;
	filter->a2 = (1.0f - alpha) / a0;
	filter->b0 = (1.0f + cosw0) / 2.0f / a0 * A;
	filter->b1 = -(1.0f + cosw0) / a0;
	filter->b2 = (1.0f + cosw0) / 2.0f / a0;
}

void biquadApplyLPFNoReso(Biquad* filter, float ctof)
{
	filter->type = 20;
	filter->ctof = ctof;
	filter->reso = 0.707;
	filter->gain = 1.0;

	filter->a1 = 2.0 * (ctof - 1.0);
	filter->a2 = (ctof - 1.0) * (ctof - 1.0);
	filter->b0 = ctof * ctof;
	filter->b1 = 0.0;
	filter->b2 = 0.0;
}

void biquadApplyHPFNoReso(Biquad* filter, float ctof)
{
	filter->type = 21;
	filter->ctof = ctof;
	filter->reso = 0.707;
	filter->gain = 1.0;

	filter->a1 = 2.0 * (ctof - 1.0);
	filter->a2 = (ctof - 1.0) * (ctof - 1.0);
	filter->b0 = 1.0 - ctof * ctof;
	filter->b1 = filter->a1;
	filter->b2 = filter->a2;
}

// 峰值滤波器（Peaking Filter）系数计算
void biquadApplyPeaking(Biquad* filter, float ctof, float reso, float gain) {
	filter->type = 3;
	filter->ctof = ctof;
	filter->reso = reso;
	filter->gain = gain;

	float A = gain;
	float omega = M_PI * ctof;
	float alpha = sinf(omega) / (2.0f * reso);
	float cosw0 = cosf(omega);
	float a0 = 1.0f + alpha / A;
	filter->a1 = -2.0f * cosw0 / a0;
	filter->a2 = (1.0f - alpha / A) / a0;
	filter->b0 = (1.0f + alpha * A) / a0;
	filter->b1 = -2.0f * cosw0 / a0;
	filter->b2 = (1.0f - alpha * A) / a0;
}

// 低通搁架滤波器
void biquadApplyLowShelf(Biquad* filter, float ctof, float reso, float gain) {
	filter->type = 4;
	filter->ctof = ctof;
	filter->reso = reso;
	filter->gain = gain;

	float A = gain;
	float omega = M_PI * ctof;
	float alpha = sinf(omega) / (2.0f * reso);
	float cosw0 = cosf(omega);
	float sqrtA = sqrtf(A);
	float a0 = (A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha;
	filter->b0 = A * ((A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha) / a0;
	filter->b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0) / a0;
	filter->b2 = A * ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha) / a0;
	filter->a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cosw0) / a0;
	filter->a2 = ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha) / a0;
}

// 高通搁架滤波器
void biquadApplyHighShelf(Biquad* filter, float ctof, float reso, float gain) {
	filter->type = 5;
	filter->ctof = ctof;
	filter->reso = reso;
	filter->gain = gain;

	float A = gain;
	float omega = M_PI * ctof;
	float alpha = sinf(omega) / (2.0f * reso);
	float cosw0 = cosf(omega);
	float sqrtA = sqrtf(A);
	float a0 = (A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha;
	filter->b0 = A * ((A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * sqrtA * alpha) / a0;
	filter->b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0) / a0;
	filter->b2 = A * ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha) / a0;
	filter->a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0) / a0;
	filter->a2 = ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * sqrtA * alpha) / a0;
}