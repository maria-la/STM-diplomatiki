/*
 * apply whitening on a signal
 * preprossesing change based on PCA
 * */

#include <math.h>
#include "arm_math.h"
#include "eigVal.h"
#include "eigVec.h"


int compare_function(const void *a,const void *b);

void whitening(arm_matrix_instance_f32 *sig, uint16_t noOfSources,uint16_t freqBins, uint16_t dnum){
     	// Create matrix Y and fill with 0s
		arm_matrix_instance_f32 Y;
        float32_t yData[noOfSources * freqBins];
        arm_fill_f32(0, yData,(noOfSources * freqBins));

        // Initialise mat Y
        arm_mat_init_f32 (&Y, freqBins, noOfSources, yData);

        // For every row of Y
        for(int i=0; i<freqBins;i++){
            arm_matrix_instance_f32 Xi, Xit, V, eigv, D2, eigv2;
            float32_t XiData[noOfSources];
            float32_t XitData[noOfSources];
            for(int j=0; j<noOfSources; j++){
                XiData[noOfSources] = sig->pData[j*noOfSources];
            }
            arm_mat_init_f32 (&Xi, 1, noOfSources, XiData);
            arm_mat_init_f32 (&Xit, noOfSources, 1, XitData);
            arm_mat_trans_f32 (&Xi, &Xit);

            // Covariance matrix
            float32_t VData[noOfSources * noOfSources];
            arm_mat_init_f32 (&V, noOfSources, noOfSources, VData);
            arm_mat_mult_f32 (&Xit, &Xi, &V);

            // Calculate eigenvalues
            uint16_t matSize = V.numRows;
            float32_t eigenvalues[matSize];
            egv(&V, eigenvalues);
            
            // Sort eigenvalues in ascending order
            const arm_sort_instance_f32 S;
            arm_sort_init_f32(S,ARM_SORT_QUICK,ARM_SORT_ASCENDING);
            arm_sort_f32(S, eigenvalues, eigenvalues, matSize);

            // Compute eigenvectors
            compute_eigenvectors(eigenvalues, &V, &eigv);

            float32_t D2data[(dnum-1)*(dnum-1)], dData[matSize*matSize];
            arm_fill_f32(0, dData,(matSize * matSize));
            int k = 0;
            for (int j=0; j<matSize; j++){
            	dData[k] = eigenvalues[j];
            	k += (matSize + 1);
            }


           /* for(int j=0; j<((dnum-1)*(dnum-1)); j++){
            	D2data[j] = sig->pData[j*noOfSources];
            }*/


          /*  for(int j=0; j<noOfSources; j++){
            	XiData[noOfSources] += sig->pData[j*noOfSources];
            }*/

            k = 0;
            int a = 0;
            for(int j=0; j<matSize; j++){
            	for(int w=(noOfSources-dnum+1); w<matSize; w++){
            		eigv2Data[k] = eigv->pData[j*matSize+w];

            		if(j>=(noOfSources-dnum+1)){
            			arm_sqrt_f32(dData[j*matSize+w], &D2data[k]);
            		}
            	}
            }

            arm_mat_init_f32 (&D2, (dnum-1), (dnum-1), D2data);

            float32_t eigv2Data[noOfSources*(dnum-1)];
            arm_mat_init_f32 (&eigv2, (dnum-1), (dnum-1), eigv2Data);


            // Generating Y data
            arm_mat_trans_f32 (&eigv2, &eigv2);


        }

}

