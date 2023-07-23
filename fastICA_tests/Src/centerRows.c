#include "arm_math.h"

void centerRows(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *matrixOut){
	uint16_t cols = matrix->numCols;
	uint16_t rows = matrix->numRows;

	float32_t mu;

		int i, j;

		for(i=0; i<rows; i++){
			arm_mean_f32 (&matrix->pData[i * cols], cols, &mu);
			for(j=0; j<cols; j++){
				matrixOut->pData[i * cols + j] = matrix->pData[i * cols + j] - mu;
			}
		}
}
