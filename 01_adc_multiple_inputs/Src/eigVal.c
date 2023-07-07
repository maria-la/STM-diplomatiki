#include <math.h>
#include "arm_math.h"

#define MAX_ITERATIONS 100
#define EPSILON 1e-8

void qr_decomposition(arm_matrix_instance_f32 *matrix, uint16_t size, float32_t *Q, float32_t *R);
void get_eigenvalues(arm_matrix_instance_f32 *matrix, uint16_t size, float32_t *eigenvalues);


void egv(arm_matrix_instance_f32 *pSrc,  float32_t *eigenvalues){
    uint16_t matSize = pSrc->numRows;
    float32_t Q[matSize * matSize];
    float32_t R[matSize * matSize];

    qr_decomposition(pSrc, matSize, Q, R);

    get_eigenvalues(pSrc, matSize, eigenvalues);

}

// Function to compute the QR decomposition of a matrix
void qr_decomposition(arm_matrix_instance_f32 *matrix, uint16_t size, float32_t *Q, float32_t *R) {
    // Initialize Q as the identity matrix
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j) {
                Q[i * size + j] = 1.0;
            } else {
                Q[i * size + j] = 0.0;
            }
        }
    }

    // Perform QR iterations
    for (int iter = 0; iter < MAX_ITERATIONS; iter++) {
        // Compute QR decomposition
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                float32_t sum = 0.0;
                for (int k = 0; k < size; k++) {
                    sum += matrix->pData[i * size + k] * Q[k * size + j];
                }
                R[i * size + j] = sum;
            }
        }

        // Compute the updated matrix
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                float32_t sum = 0.0;
                for (int k = 0; k < size; k++) {
                    sum += Q[i * size + k] * R[k * size + j];
                }
                matrix->pData[i * size + j] = sum;
            }
        }

        // Check for convergence
        float32_t off_diag_sum = 0.0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (i != j) {
                    off_diag_sum += matrix->pData[i * size + j] * matrix->pData[i * size + j];
                }
            }
        }

        if (off_diag_sum < EPSILON) {
            break;
        }
    }
}

// Function to compute eigenvalues from the diagonal of a matrix
void get_eigenvalues(arm_matrix_instance_f32 *matrix, uint16_t size, float32_t *eigenvalues) {
    for (int i = 0; i < size; i++) {
        eigenvalues[i] = matrix->pData[i * size + i];
    }
}
