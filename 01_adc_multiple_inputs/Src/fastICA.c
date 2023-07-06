#include <stdio.h>
#include "arm_math.h"

#define TOL 1e-6f         // Convergence criteria
#define MAX_ITERS 100    // Max # iterations

void centerRows(float32_t* Z, float32_t* mu, uint32_t d, uint32_t n) {
    for (uint32_t i = 0; i < d; i++) {
        float32_t sum = 0;
        for (uint32_t j = 0; j < n; j++) {
            sum += Z[i * n + j];
        }
        mu[i] = sum / n;
        for (uint32_t j = 0; j < n; j++) {
            Z[i * n + j] -= mu[i];
        }
    }
}

void normRows(float32_t* X, uint32_t d, uint32_t n) {
    for (uint32_t i = 0; i < d; i++) {
        float32_t sum = 0;
        for (uint32_t j = 0; j < n; j++) {
            sum += X[i * n + j] * X[i * n + j];
        }
        float32_t norm = sqrtf(sum);
        for (uint32_t j = 0; j < n; j++) {
            X[i * n + j] /= norm;
        }
    }
}

void fastICA(float32_t* Z, uint32_t d, uint32_t n, uint32_t r, char* type, uint32_t flag, float32_t* Zica, float32_t* W, float32_t* T, float32_t* mu) {
    // Parse inputs
    if (type == NULL) {
        // Default type
        type = "kurtosis";
    }

    // Set algorithm type
    int USE_KURTOSIS;
    char algoStr[12];
    if (strncmp(type, "kurtosis", 1) == 0) {
        // Kurtosis
        USE_KURTOSIS = 1;
        strcpy(algoStr, "kurtosis");
    }
    else if (strncmp(type, "negentropy", 1) == 0) {
        // Negentropy
        USE_KURTOSIS = 0;
        strcpy(algoStr, "negentropy");
    }
    else {
        // Unsupported type
        printf("Unsupported type '%s'", type);
        return;
    }

    // Center and whiten data
    float32_t Zc[d * n];
    centerRows(Z, mu, d, n);

    // Calculate the covariance matrix
    float32_t R[d * d];
    arm_matrix_instance_f32 Zmat;
    arm_matrix_instance_f32 Rmat;
    arm_status status;

    arm_mat_init_f32(&Zmat, n, d, Z);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix initialization error: %d\n", status);
        return;
    }

    arm_mat_init_f32(&Rmat, d, d, R);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix initialization error: %d\n", status);
        return;
    }

    arm_mat_mult_f32(&Zmat, &Zmat, &Rmat);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix multiplication error: %d\n", status);
        return;
    }

    float32_t scale = 1.0f / (float32_t)(n - 1);
    status = arm_mat_scale_f32(&Rmat, scale, &Rmat);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix scaling error: %d\n", status);
        return;
    }

    // Whiten data
    float32_t U[d * d];
    float32_t S[d];
    float32_t V[d * d];
    arm_matrix_instance_f32 Umat;
    arm_matrix_instance_f32 Smat;
    arm_matrix_instance_f32 Vmat;
    arm_matrix_instance_f32 Tmat;
    arm_matrix_instance_f32 Zwmat;

    arm_mat_init_f32(&Umat, d, d, U);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix initialization error: %d\n", status);
        return;
    }

    arm_mat_init_f32(&Smat, d, 1, S);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix initialization error: %d\n", status);
        return;
    }

    arm_mat_init_f32(&Vmat, d, d, V);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix initialization error: %d\n", status);
        return;
    }

    arm_mat_svd_f32(&Rmat, &Umat, &Smat, &Vmat);
    if (status != ARM_MATH_SUCCESS) {
        printf("SVD error: %d\n", status);
        return;
    }

    arm_mat_mult_f32(&Umat, &Smat, &Tmat);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix multiplication error: %d\n", status);
        return;
    }

    arm_mat_mult_f32(&Tmat, &Zmat, &Zwmat);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix multiplication error: %d\n", status);
        return;
    }

    // Normalize rows to unit norm
    normRows(Zw, r, n);

    // Perform Fast ICA
    if (flag) {
        // Prepare status updates
        char fmt[3];
        sprintf(fmt, "%%0%dd", (int)(ceil(log10(MAX_ITERS + 1))));
        char str[50];
        sprintf(str, "Iter %s: max(1 - |<w%s, w%s>|) = %%0.4g\n", fmt, fmt, fmt);
        printf("***** Fast ICA (%s) *****\n", algoStr);
    }

    float32_t W[r * d]; // Random initial weights
    for (uint32_t i = 0; i < r * d; i++) {
        W[i] = (float32_t)rand() / RAND_MAX;
    }

    uint32_t k = 0;
    float32_t delta = INFINITY;
    while (delta > TOL && k < MAX_ITERS) {
        k++;

        // Update weights
        float32_t Wlast[r * d];
        memcpy(Wlast, W, r * d * sizeof(float32_t));
        float32_t Sk[r * n];
        arm_matrix_instance_f32 Wmat;
        arm_matrix_instance_f32 Zcwmat;
        arm_matrix_instance_f32 Skmat;

        arm_mat_init_f32(&Wmat, r, d, W);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix initialization error: %d\n", status);
            return;
        }

        arm_mat_init_f32(&Zcwmat, n, r, Zw);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix initialization error: %d\n", status);
            return;
        }

        arm_mat_init_f32(&Skmat, r, n, Sk);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix initialization error: %d\n", status);
            return;
        }

        arm_mat_mult_f32(&Wmat, &Zcwmat, &Skmat);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix multiplication error: %d\n", status);
            return;
        }

        float32_t G[r * n];
        float32_t Gp[r * n];

        if (USE_KURTOSIS) {
            // Kurtosis
            for (uint32_t i = 0; i < r * n; i++) {
                G[i] = 4 * Sk[i] * Sk[i] * Sk[i];
                Gp[i] = 12 * Sk[i] * Sk[i];
            }
        }
        else {
            // Negentropy
            for (uint32_t i = 0; i < r * n; i++) {
                float32_t expVal = expf(-0.5f * Sk[i] * Sk[i]);
                G[i] = Sk[i] * expVal;
                Gp[i] = (1 - Sk[i] * Sk[i]) * expVal;
            }
        }

        arm_matrix_instance_f32 Gmat;
        arm_matrix_instance_f32 Gpmat;

        arm_mat_init_f32(&Gmat, r, n, G);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix initialization error: %d\n", status);
            return;
        }

        arm_mat_init_f32(&Gpmat, r, 1, Gp);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix initialization error: %d\n", status);
            return;
        }

        arm_mat_mult_f32(&Gmat, &Zcwmat, &Wmat);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix multiplication error: %d\n", status);
            return;
        }

        arm_mat_scale_f32(&Wmat, 1.0f / n, &Wmat);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix scaling error: %d\n", status);
            return;
        }

        arm_mat_sub_f32(&Wmat, &Gpmat, &Wmat);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix subtraction error: %d\n", status);
            return;
        }

        normRows(W, r, d);

        // Decorrelate weights
        float32_t Wtemp[r * r];
        arm_matrix_instance_f32 Wtempmat;

        arm_mat_init_f32(&Wtempmat, r, r, Wtemp);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix initialization error: %d\n", status);
            return;
        }

        arm_mat_mult_f32(&Wmat, &Wmat, &Wtempmat);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix multiplication error: %d\n", status);
            return;
        }

        arm_mat_mult_f32(&Umat, &Wtempmat, &Wmat);
        if (status != ARM_MATH_SUCCESS) {
            printf("Matrix multiplication error: %d\n", status);
            return;
        }

        memcpy(W, Wtemp, r * d * sizeof(float32_t));

        // Update convergence criteria
        delta = 0.0f;
        for (uint32_t i = 0; i < r * d; i++) {
            delta += 1.0f - fabsf(W[i] * Wlast[i]);
        }
        delta /= r;

        if (flag) {
            printf(str, k, k, k - 1, delta);
        }
    }

    if (flag) {
        printf("\n");
    }

    // Independent components
    arm_matrix_instance_f32 Zcwmat;
    arm_matrix_instance_f32 Wmat;
    arm_matrix_instance_f32 Zicamat;

    status = arm_mat_init_f32(&Zcwmat, n, d, Zw);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix initialization error: %d\n", status);
        return;
    }

    arm_mat_init_f32(&Wmat, d, r, W);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix initialization error: %d\n", status);
        return;
    }

    arm_mat_init_f32(&Zicamat, r, n, Zica);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix initialization error: %d\n", status);
        return;
    }

    arm_mat_mult_f32(&Wmat, &Zcwmat, &Zicamat);
    if (status != ARM_MATH_SUCCESS) {
        printf("Matrix multiplication error: %d\n", status);
        return;
    }
}
