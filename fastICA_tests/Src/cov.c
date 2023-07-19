#include "arm_math.h"

static float cov(float x[], float y[], uint16_t length) ;
static float var(float x[], uint16_t length);

void covMat(float X[], float Y[], uint16_t row, uint16_t column) {
	/* Turn X into transpose because it's easier to handle the rows */
	arm_matrix_instance_f32 xMat, xMatT;
	float Xt[column*row];
	arm_mat_init_f32(&xMat, row, column, X);
	arm_mat_init_f32(&xMatT, row, column, Xt);
	arm_mat_trans_f32 (&xMat, &xMatT);

	/* Compute the diagonal variance of X */
	uint16_t i, j, k;
	float* X0 = Xt;
	float* Y0 = Y;
	for (i = 0; i < column; i++) {
		//arm_var_f32 (Xt, row, &Y[i]);
		Y[i] = var(Xt, row);
		X += row;
		Y += column;
	}

	/* Reset index */
	X = X0;
	Y = Y0;

	/* Compute variances of each row of X */
	uint32_t position;
	for (j = 0; j < column - 1; j++) {
		k = 0;
		for (i = j + 1; i < column; i++) {
			position = i * column + j;
			k++;
			Y[position] = cov(X, &X[row * k], row);

			/* Symmetric */
			Y[j * column + i] = Y[position];
		}
		X += row;
	}
	/* Reset index */
	X = X0;

}

static float cov(float x[], float y[], uint16_t length) {
	float x_mu, y_mu;
	arm_mean_f32 (x, length, &x_mu);
	arm_mean_f32 (y, length, &y_mu);

	float sigma = 0.0f;
	uint16_t i;
	for (i = 0; i < length; i++) {
		sigma += (x[i] - x_mu)*(y[i] - y_mu);
	}
	return sigma / ((float)(length - 1));
}

static float var(float x[], uint16_t length) {
	float mu;
	arm_mean_f32 (x, length, &mu);

	float sigma = 0.0f;
	float p;
	uint16_t i;
	for (i = 0; i < length; i++) {
		p = x[i] - mu;
		sigma += p * p;
	}
	return sigma / ((float)(length - 1));
}
