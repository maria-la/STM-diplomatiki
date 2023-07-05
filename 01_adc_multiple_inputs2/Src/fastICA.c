#include "arm_math.h"
#include "arm_const_structs.h"
#include "arm_common_tables.h"

// Constants
float32_t TOL = 1e-6;       // Convergence criteria
int MAX_ITERS = 100;    	// Max # iterations

//Expose Functions
void center(float32_t* a, uint32_t blockSize);

void fastICA(float32_t * array, uint32_t  blockSize,  int sigs){
	//Center and whiten data
	center(array, blockSize);
	//whiten(z);
}

void center(float32_t* a, uint32_t blockSize){
	float32_t mean;
	arm_mean_f32(a, blockSize, &mean);
	for(int i; i<blockSize; i++){
		a[i] -= mean;
	}
}

void whiten(float32_t* a, uint32_t blockSize){
	float32_t m[blockSize];


	// 1. Compute covariance matrix
	arm_mat_cmplx_trans_f32(a, a);
	float32_t fact = 1 / (blockSize - 1);
	for(int i; i<blockSize; i++){
			a[i] *= fact;
		}

	// 2. Compute eigenvalues and eigenvectors
	Triplet *svd_ret = svd(x_cov);
	free_mat(x_cov);
	Matrix *u = svd_ret->m1;
	Matrix *s = svd_ret->m2;
	Matrix *vt = svd_ret->m3;

	int n_eig = s->height;
	Matrix *d = new_mat(n_eig, n_eig);
	for (int i = 0; i < n_eig; i++) {
		MAT_CELL(d, i, i) = 1 / SQRT(MAT_CELL(s, i, 0));
	}
	// 3. Compute whitening matrix
	Matrix *tmp = mat_mul(d, vt);
	free_mat(d);
	Matrix *white_mat = mat_mul(u, tmp);
	free_mat(tmp);

	// 4. Whiten data
	Matrix *x_w = mat_mul(white_mat, x);

	// Free memory
	free_triplet(svd_ret, true);

	// Pack Xw and whitening matrix into tuple
	Pair *WhitenData = new_pair(x_w, white_mat);
	return WhitenData;

}
