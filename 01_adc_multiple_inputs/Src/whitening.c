/*
 * apply whitening on a signal
 * preprossesing change based on PCA
 * */

#include <math.h>
#include "arm_math.h"
#include "eigVal.h"
#include "eigVec.h"


int compare_function(const void *a,const void *b);

arm_matrix_instance_f32 whitening(arm_matrix_instance_f32 *sig, uint16_t noOfSources,uint16_t freqBins){
     	// Create matrix Y and fill with 0s
		arm_matrix_instance_f32 Y;
        float32_t yData[noOfSources * freqBins];
        arm_fill_f32(0, yData,(noOfSources * freqBins));

        // Initialise matrices
        arm_mat_init_f32 (&Y, freqBins, noOfSources, yData);
        arm_matrix_instance_f32 Xi, Xit, V, eigv, D2, eigv2;
        arm_sort_instance_f32 S, step, Yi;

        uint26_t M = sig->numCols;

        float32_t XiData[noOfSources];
        float32_t XitData[noOfSources];
        float32_t VData[noOfSources * noOfSources];
        float32_t D2data[(noOfSources-(M-noOfSources))*(noOfSources-(M-noOfSources))];
        float32_t eigv2Data[(noOfSources-(M-noOfSources))*noOfSources];
        float32_t Yidata[noOfSources-(M-noOfSources)];
        float32_t eigenvalues[noOfSources], dData[noOfSources*noOfSources];
        float32_t step[(noOfSources-(M-noOfSources)) * noOfSources];

        // For every row of Y
        for(int i=0; i<freqBins;i++){
            for(int j=0; j<noOfSources; j++){
                XiData[noOfSources] = sig->pData[i*noOfSources+j];
            }
            arm_mat_init_f32 (&Xi, 1, noOfSources, XiData);
            arm_mat_init_f32 (&Xit, noOfSources, 1, XitData);
            arm_mat_trans_f32 (&Xi, &Xit);

            // Covariance matrix
            arm_mat_init_f32 (&V, noOfSources, noOfSources, VData);
            arm_mat_mult_f32 (&Xit, &Xi, &V);

            // Calculate eigenvalues
            //uint16_t noOfSources = V.numRows;
            egv(&V, eigenvalues);
            
            // Sort eigenvalues in ascending order
            arm_sort_init_f32(S,ARM_SORT_QUICK,ARM_SORT_ASCENDING);
            arm_sort_f32(S, eigenvalues, eigenvalues, noOfSources);

            // Compute eigenvectors
            compute_eigenvectors(eigenvalues, &V, &eigv);

            arm_fill_f32(0, dData,(noOfSources * noOfSources));
            int k = 0;
            for (int j=0; j<noOfSources; j++){
            	dData[k] = eigenvalues[j];
            	k += (noOfSources + 1);
            }

            k = 0;
            int a = 0;
            for(int j=0; j<noOfSources; j++){
            	for(int w=1; w<noOfSources; w++){
            		eigv2Data[k] = eigv->pData[j*noOfSources+w];

            		if(j>=1){
            			arm_sqrt_f32(dData[j*noOfSources+w], &D2data[k]);
            		}
            	}
            }

            arm_mat_init_f32 (&D2, noOfSources-(M-noOfSources), noOfSources-(M-noOfSources), D2data);

            arm_mat_init_f32 (&eigv2, noOfSources, noOfSources-(M-noOfSources), eigv2Data);


            // Generating Y data
            arm_mat_init_f32 (&step, noOfSources-(M-noOfSources), noOfSources, step);
            arm_mat_init_f32 (&Yi, noOfSources-(M-noOfSources), noOfSources, Yidata);

            arm_mat_trans_f32 (&eigv2, &eigv2);
            arm_mat_inverse_f32 (&D2, &D2);

           	arm_mat_mult_f32(&D2, &eigv2, &step);

            arm_mat_mult_f32(&step, &Xit, &Yi);

            for(j=0; j<(noOfSources-(M-noOfSources)); j++){
                yData[i*freqBins+j] = Yidata[j];
            }

        }

    return &Y;
}

