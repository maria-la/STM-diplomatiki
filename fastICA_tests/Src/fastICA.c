#include "arm_math.h"
#include "whit.h"
#include "centerRows.h"
#include "normRows.h"

// Constants
#define TOL 		1e-6   //Convergence criteria
#define MAX_ITERS 	100    // Max # iterations

void fastICA(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *Zica, uint16_t r){
	arm_matrix_instance_f32 Z, mHelp;

	float32_t Zdata[matrix->numCols*matrix->numRows];
    arm_mat_init_f32 (&Z, matrix->numCols, matrix->numRows, Zdata);
    arm_mat_trans_f32 (matrix, &Z);

	uint16_t numCols = Z.numCols;
	uint16_t numRows = Z.numRows;

	float32_t mHelpData[numRows*numCols];
	arm_mat_init_f32(&mHelp, numRows, numCols, mHelpData);

	centerRows(&Z, &mHelp);
	whitenRows(&mHelp, &Z);


}
