#include <stdio.h>
#include <math.h>
#include "arm_math.h"

void local_ILRMA1(float32_t *Sig, uint32_t nIter, uint32_t L, float32_t *Y, float32_t *cost)
{
	uint16_t freqBins = Sig->numRows;
    uint16_t noOfChannels = Sig->numCols;

    arm_matrix_instance_f32 W, Y;
    arm_sort_instance_f32;

    float32_t wData[noOfChannels*noOfChannels];
    float32_t yData[freqBins*noOfChannels];
    float32_t pXData[noOfChannels];

    arm_mat_init_f32 (&W, noOfChannels, noOfChannels, wData);
    arm_mat_init_f32 (&Xit, freqBins, noOfChannels, yData);
    arm_mat_init_f32 (&pX, noOfChannels, freqBins, pXData);

    arm_fill_f32(0, wData,(noOfChannels * noOfChannels));
    arm_fill_f32(0, yData,(freqBins * noOfChannels));

    arm_mat_trans_f32 (&pX, &pX);

    for(int i=0; i<(noOfChannels * noOfChannels); i+=(noOfChannels+1)){
        wData[i] = 1;
    }

    arm_mat_mult_f32(&W, &pX, &Y);
    arm_mat_trans_f32 (&Y, &Y);





}


