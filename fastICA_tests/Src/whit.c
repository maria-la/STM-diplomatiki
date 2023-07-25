#include "arm_math.h"
#include "svd_golub_reinsch.h"
#include "cov.h"

void decorrelate(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *out, arm_matrix_instance_f32 *ref);

void whitenRows(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *matrixOut){
    arm_matrix_instance_f32 matT, covm;
    uint16_t numCols = matrix->numCols;
    uint16_t numRows = matrix->numRows;

    float32_t matTdata[numCols*numRows];
    float32_t covData[numRows*numRows];
    
    arm_mat_init_f32 (&matT, numCols, numRows, matTdata);
    arm_mat_init_f32 (&covm, numRows, numRows, covData);

    arm_mat_trans_f32 (matrix, &matT);
    covMat(matTdata, covData, numCols, numRows);

    decorrelate(&covm, matrixOut, matrix);

}


void decorrelate(arm_matrix_instance_f32 *in, arm_matrix_instance_f32 *out, arm_matrix_instance_f32 *ref){
	arm_matrix_instance_f32 U, S, U1, T, Ut;
    int i,k;
    float32_t mul = 1;
    float32_t add = 0;

    uint16_t numCols = in->numCols;
    uint16_t numRows = in->numRows;

	float32_t Udata[numRows*numRows];
	float32_t U1data[numRows*numRows];
	float32_t Utdata[numRows*numRows];
	float32_t Tdata[numRows*numRows];
	float32_t Sdata[numRows*numCols];
	float32_t Vdata[numCols*numCols];
	float32_t sigmaDataDiag[numRows];

    arm_mat_init_f32 (&U, numRows, numRows, Udata);
    arm_mat_init_f32 (&U1, numRows, numRows, U1data);
    arm_mat_init_f32 (&Ut, numRows, numRows, Utdata);
    arm_mat_init_f32 (&S, numRows, numCols, Sdata);
    arm_mat_init_f32 (&T, numRows, numRows, Tdata);


	svd_golub_reinsch(in->pData, numRows, numCols, Udata, sigmaDataDiag, Vdata);
	arm_scale_f32(Udata, (-1), Udata, (numRows*numRows));
	arm_scale_f32(sigmaDataDiag, (-1), Sdata, (numRows));
	arm_scale_f32(Vdata, (-1), Vdata, (numRows*numRows));

	arm_fill_f32 (0, Sdata, (numRows*numRows));
	k= 0;
	for(i=0; i<numRows; i++){
		mul = mul*sigmaDataDiag[i];
		add += sigmaDataDiag[i];
	}
	if(mul==0) {
		for(i=0; i<(numRows*numRows); i+= (numRows+1)){
			if(sigmaDataDiag[k]==0){
				arm_sqrt_f32(add, &S.pData[i]);
				S.pData[i] =1/ (S.pData[i]* 4.0955e-09);
			}
			else {
				S.pData[i] = 0;
			}
			k++;
		}
	}
	else {
		for(i=0; i<(numRows*numRows); i+= (numRows+1)){
			arm_sqrt_f32(sigmaDataDiag[k], &S.pData[i]);
			S.pData[i] = 1 / S.pData[i];
			k++;
		}
	}

	arm_mat_mult_f32(&U, &S, &U1);
	arm_mat_trans_f32 (&U, &Ut);
	arm_mat_mult_f32(&U1, &Ut, &T);

	arm_mat_mult_f32(&T, ref, out);
}

