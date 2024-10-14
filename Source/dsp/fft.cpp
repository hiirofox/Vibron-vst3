#include "fft.h"

void fft_f32(complex_f32_t* a, int n, int inv)
{
	for (int i = 1, j = 0; i < n - 1; ++i)
	{
		for (int s = n; j ^= s >>= 1, ~j & s;)
			;
		if (i < j)
		{
			complex_f32_t tmp = a[i];
			a[i] = a[j];
			a[j] = tmp;
		}
	}
	complex_f32_t t;
	int p1, p2;
	for (int m = 2; m <= n; m <<= 1)
	{
		complex_f32_t wm = complex_f32_t{ (float)cos(2.0 * M_PI / m), (float)sin(2.0 * M_PI / m) * inv };//Ðý×ªµÄËÙ¶È
		for (int k = 0; k < n; k += m)
		{
			complex_f32_t w = complex_f32_t{ 1.0, 0 };//ÒªÐý×ªµÄÄÇ¸ö(ÆäÊµÊÇ¸ú×Åm)
			p1 = m >> 1;
			for (int j = 0; j < p1; ++j, w = cmul(w, wm))//ÕâÀïÐý×ª
			{
				p2 = k + j;
				t = cmul(w, a[p2 + p1]);
				a[p2 + p1] = csub(a[p2], t);
				a[p2] = cadd(a[p2], t);
			}
		}
	}
}