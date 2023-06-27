#include "arm_math.h"

#define NUM_FREQ_BINS 128
#define NUM_VOICES 2

void ilrma(const float32_t* audio_signal, float32_t* separated_voices) {
    float32_t X[NUM_FREQ_BINS];
    float32_t Y[NUM_FREQ_BINS * NUM_VOICES];
    float32_t W[NUM_FREQ_BINS * NUM_VOICES];
    float32_t V[NUM_FREQ_BINS * NUM_VOICES];
    float32_t P[NUM_FREQ_BINS * NUM_VOICES];
    float32_t P_t[NUM_FREQ_BINS * NUM_VOICES];

    // Copy input audio signal to X
    arm_copy_f32(audio_signal, X, NUM_FREQ_BINS);

    // Perform ILRMA iterations in the frequency domain
    int max_iterations = 100;
    int iteration = 0;

    do {
        // Estimate source signals in the frequency domain
        for (int i = 0; i < NUM_VOICES; i++) {
            arm_dot_prod_f32(W + (i * NUM_FREQ_BINS), X, NUM_FREQ_BINS, Y + (i * NUM_FREQ_BINS));
        }

        // Update W matrix
        for (int i = 0; i < NUM_VOICES; i++) {
            arm_dot_prod_f32(V + (i * NUM_FREQ_BINS), Y + (i * NUM_FREQ_BINS), NUM_FREQ_BINS, P + (i * NUM_FREQ_BINS));
        }
        arm_matrix_instance_f32 P_instance = {NUM_FREQ_BINS, NUM_VOICES, P};
        arm_matrix_instance_f32 P_t_instance = {NUM_VOICES, NUM_FREQ_BINS, P_t};
        arm_mat_inverse_f32(&P_instance, &P_t_instance);
        for (int i = 0; i < NUM_VOICES; i++) {
            arm_dot_prod_f32(X, P_t + (i * NUM_FREQ_BINS), NUM_FREQ_BINS, W + (i * NUM_FREQ_BINS));
        }

        // Update V matrix
        for (int i = 0; i < NUM_VOICES; i++) {
            arm_dot_prod_f32(Y + (i * NUM_FREQ_BINS), W + (i * NUM_FREQ_BINS), NUM_FREQ_BINS, V + (i * NUM_FREQ_BINS));
        }

        iteration++;
    } while (iteration < max_iterations);

    // Copy separated voice signals to the output array
    for (int i = 0; i < NUM_VOICES; i++) {
        arm_copy_f32(Y + (i * NUM_FREQ_BINS), separated_voices + (i * NUM_FREQ_BINS), NUM_FREQ_BINS);
    }
}
