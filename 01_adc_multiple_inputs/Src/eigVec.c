#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"

void compute_eigenvectors(float32_t *eigenvalues, arm_matrix_instance_f32 *pSrc, arm_matrix_instance_f32 *eigenvectors) {
    uint16_t matSize = pSrc->numRows;
    for (int i = 0; i < matSize; i++) {
        float32_t lambda = eigenvalues[i];

        // Solve (A - lambda * I) * eigenvector = 0
        for (int j = 0; j < matSize; j++) {
            eigenvectors->pData[i * matSize + j] = pSrc->pData[i * matSize + j] - lambda * (i == j ? 1.0 : 0.0);
        }
    }
}
