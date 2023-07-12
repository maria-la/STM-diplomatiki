#include "arm_math.h"


void cov(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *matrixT, arm_matrix_instance_f32 *cov){
    //% Compute the mean vector
    arm_matrix_instance_f32 matrixCenT;
    uint16_t numCols = matrix->numCols;
    uint16_t numRows = matrix->numRows;

    float32_t mean_vec[numCols];
    float32_t matrixCenTdata[numRows];


    arm_mat_init_f32 (&matrixCenT, numRows, numCols, matrixCenTdata);

    arm_mean_f32(matrixT->pData, numRows, mean_vec);

    //% Subtract the mean vector from each row
    int i, j;
    for(i=0; i<numRows; i++){
        for(j=0; j<numCols; j++){
            matrix->pData[i*numRows+j] -= mean_vec[j];
        }
    }
    
    

    //% Compute the covariance matrix
    arm_mat_trans_f32 (&matrix, &matrixCenT);
    arm_mat_mult_f32(&matrixCenT, &matrix, &cov);
    arm_mat_scale_f32 (&cov, (1/(numRows-1)), &cov);

}
