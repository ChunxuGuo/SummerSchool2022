#include "./fir.h"

coef_t c[N];

void fir(data_t *y, data_t x)
{

#pragma HLS ARRAY_PARTITION variable=c complete dim=0
    static data_t shift_reg[N];
#pragma HLS ARRAY_PARTITION variable=shift_reg complete dim=0
    acc_t acc;
    int i;
    acc = 0;
#pragma HLS PIPELINE II=1
    for (i = N - 1; i >= 0; i--)
    {
        if (i == 0)
        {
            acc += x * c[0];
            shift_reg[0] = x;
        }
        else
        {
            shift_reg[i] = shift_reg[i - 1];
            acc += shift_reg[i] * c[i];
        }
    }
    *y = acc;
}

void fir_wrap(data_t *y, data_t *x, int len, coef_t *coef)
{
#pragma HLS INTERFACE m_axi port=coef offset=slave depth=99
#pragma HLS INTERFACE m_axi port=x offset=slave depth=100
#pragma HLS INTERFACE m_axi port=y offset=slave depth=100
#pragma HLS INTERFACE s_axilite port=len  bundle=CTRL
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL

	data_t res;

	for (int i =0; i < N; i++)
	{
#pragma HLS PIPELINE II=1
		c[i] = *coef++;
	}

	for (int i = 0; i < len; i++)
	{
#pragma HLS PIPELINE II=1
		fir(&res,*x++);
		*y = res;
		y++;
	}

}

