#include "arm_math.h"
#include "eigVec.h"
#include "eigVal.h"

void svd(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *U, arm_matrix_instance_f32 *sigma){
    uint16_t numRows = matrix->numRows;
    uint16_t numCols = matrix->numCols;
    arm_matrix_instance_f32 matT, AtA, eigV, U1, At, sigmaDiag;

    float32_t matTdata[numCols*numRows];
    float32_t AtAdata[numCols*numCols];
    float32_t eigvData[numRows*numRows];
    float32_t dData[numCols*numCols];
    float32_t sigmaData[numCols];
    float32_t sigmaDataDiag[numCols*numCols];
    float32_t u1Data[numRows*numCols];
    float32_t uData[numRows*numCols];
    int i;

    arm_mat_init_f32 (&matT, numCols, numRows, matTdata);
    arm_mat_init_f32 (&AtA, numCols, numCols, AtAdata);

    arm_mat_trans_f32 (&matrix, &matT);
    arm_mat_mult_f32(&matT, &matrix, &AtA);


    // Calculate eigenvalues
    float32_t eigenvalues[numCols];         
    egv(&AtA, eigenvalues);
    
    // Sort eigenvalues in ascending order
    arm_sort_init_f32(S,ARM_SORT_QUICK,ARM_SORT_DECENDING);
    arm_sort_f32(S, eigenvalues, eigenvalues, numCols);

    // Compute eigenvectors
    arm_mat_init_f32 (&eigv, numCols, numCols, eigv2Data);
    compute_eigenvectors(eigenvalues, &AtA, &eigv);

    arm_fill_f32(0, dData,(numCols * numCols));

    // Compute singular values and eliminate small values
    for(i=0; i<(numCols); i++){
        arm_sqrt_f32(&eigenvalues[i],&sigma->pData[i]);
    }

    arm_mat_init_f32 (&U1, numRows, numCols, u1Data);
    arm_mat_init_f32 (&U1t, numCols, numRows, u1tData);
    arm_mat_mult_f32(&matrix, &eigv, &U1);

    arm_mat_init_f32 (&sigmaDiag, numCols, numCols, sigmaDataDiag);
    arm_fill_f32(0, sigmaDataDiag,(numCols * numCols));

    int k = 0;
    for(i=0; i<(numCols*numCols); i+= (numCols+1)){
        sigmaDataDiag[i] = sigma->pData[k];
        k++;
    }

    //arm_mat_init_f32 (&U, numRows, numCols, uData);
    arm_mat_trans_f32 (&sigma, &sigma);
    arm_mat_mult_f32(&U1, &sigma, &U);
    
}
