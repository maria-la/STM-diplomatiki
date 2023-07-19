#include "arm_math.h"

void normRows(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *matrixOut){
	uint16_t cols = matrix->numCols;
	uint16_t rows = matrix->numRows;
	float32_t sum;

	int i, j;

	for(i=0; i<rows; i++){
		sum = 0;
		for(j=0; j<cols; j++){
			matrixOut->pData[i * cols + j] = matrix->pData[i * cols + j] * matrix->pData[i * cols + j];
			sum += matrixOut->pData[i * cols + j];
		}

		arm_sqrt_f32(sum, &sum);

		arm_scale_f32(&matrix->pData[i*cols], (1/sum), &matrixOut->pData[i*cols], cols);
	}
}
