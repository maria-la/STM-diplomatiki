#include "arm_math.h"
#include "svd.h"
#include "cov.h"

void whitenRows(arm_matrix_instance_f32 *matrix){
    arm_matrix_instance_f32 MatT, cov, U, S, U1;

    float32_t matTdata[numCols*numRows];
    float32_t covData[numCols*numCols];
    float32_t Udata[numCols*numCols];
    float32_t U1data[numCols*numCols];
    float32_t Sdata[numCols*numCols];
    float32_t sigmaDataDiag[numCols];
    int i, k;

    
    arm_mat_init_f32 (&matT, numCols, numRows, matTdata);
    arm_mat_init_f32 (&cov, numCols, numCols, covData);
    arm_mat_init_f32 (&U, numCols, numCols, Udata);
    arm_mat_init_f32 (&U1, numCols, numCols, U1data);
    arm_mat_init_f32 (&S, numCols, numCols, SData);

    arm_mat_trans_f32 (&matrix, &matT);
    cov(&matT, &matrix, &cov);

    svd(&cov, &U, &S);

    for(i=0; i<(numCols*numCols); i+= (numCols+1)){
        arm_sqrt_f32(&S->pData[i], &S->pData[i]);
        S->pData[i] = 1 / S->pData[i];
    }

    arm_mat_mult_f32(&U, &S, &U1);
    arm_mat_trans_f32 (&U, &U);
    arm_mat_mult_f32(&U1, &U, &matOut);
}


