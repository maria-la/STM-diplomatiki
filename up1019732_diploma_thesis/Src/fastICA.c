#include "arm_math.h"
#include "whit.h"
#include "centerRows.h"
#include "normRows.h"
#include <stdlib.h>
#include <float.h>
#include <math.h>

// Constants
#define TOL 		1e-6   //Convergence criteria
#define MAX_ITERS 	100    // Max # iterations

void fastICA(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *Zica, uint16_t r){
	arm_matrix_instance_f32 Z, mHelp, W, Sk, G, Gp, mHelp2, Gpt, Wn;

	float32_t Zdata[matrix->numCols*matrix->numRows];
    arm_mat_init_f32 (&Z, matrix->numCols, matrix->numRows, Zdata);
    arm_mat_trans_f32 (matrix, &Z);

	uint16_t numCols = Z.numCols;
	uint16_t numRows = Z.numRows;
	int k = 0;
	int i;
	float32_t sc;

	float32_t mHelpData[numRows*numCols];
	arm_mat_init_f32(&mHelp, numRows, numCols, mHelpData);

	float32_t mHelpData2[numRows*numCols];
	arm_mat_init_f32(&mHelp2, numCols, numRows, mHelpData2);

	float32_t skData[r*numCols];
	arm_mat_init_f32(&Sk, r, numCols, skData);

	float32_t gData[r*numCols];
	arm_mat_init_f32(&G, r, numCols, gData);

	float32_t gpData[r*numCols];
	arm_mat_init_f32(&Gp, r, numCols, gpData);

	float32_t gpDataT[r*numCols];
	arm_mat_init_f32(&Gpt, numCols, r, gpDataT);

	float32_t muData[numCols];

	float32_t wData[r*numRows];
	float32_t wlastData[r*numRows];
	float32_t wData2[r*numRows];
	arm_mat_init_f32(&W, r, numRows, wData);
	arm_mat_init_f32(&Wn, r, numRows, wData2);

	for(i=0; i<(r*numCols); i++){
		wData[i] = ((float32_t)rand()/RAND_MAX);
	}

	centerRows(&Z, &mHelp);
	whitenRows(&mHelp, &Z);

	arm_mat_trans_f32 (&Z, &mHelp2);


	float32_t delta = FLT_MAX;

	while((delta>TOL) & (k<MAX_ITERS)){
		k++;

		arm_copy_f32 (wData, wlastData, (r*numRows));

		arm_mat_mult_f32 (&W, &Z, &Sk);

		for(i=0; i<(r*numCols); i++){
			gData[i] = 4 * powf(skData[i], 3);
			gpData[i] = 12 * powf(skData[i], 2);
		}

		arm_mat_trans_f32 (&Gp, &Gpt);

		arm_mat_mult_f32 (&G, &mHelp2, &W);
		arm_scale_f32 (wData, 1/numCols, wData2, (r*numRows));

		for(i=0; i<r; i++){
			arm_mean_f32 (&gpDataT[i*numCols], numCols, &muData[i]);

			arm_scale_f32(wlastData, muData[i], wData, numRows);
		}

		arm_sub_f32 (wData2, wData, wData, (r*numRows));

		normRows(&W, &Wn);

		decorrelate(&Wn, &W, &Wn);

		for(i=0; i<r; i++){
			arm_dot_prod_f32 ( &wData[i*numRows], &wlastData[i*numRows], numRows, &muData[i]);
		}

		arm_abs_f32 (muData, muData, r);
		arm_scale_f32 (muData, (-1), muData, r);
		arm_offset_f32 (muData, 1, muData, r);

		arm_max_no_idx_f32 (muData, r, &delta);


	}

	arm_mat_mult_f32 (&W, &Z, Zica);


}
