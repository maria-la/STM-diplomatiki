//
// Created by nihil on 20/09/21.
//

#include <stdlib.h>
#include "../include/linalg.h"
#include "../include/utils.h"
#include "../include/random.h"
#include "../include/sorting.h"

/*
 * Compute a Householder reflection for a given real vector
 */
Matrix *generate_householder(Matrix *x, fp *tau) {
    aassert(is_vector(x), "The matrix should be a vector (i.e. either its height or width should be equal to 1).");
    fp x0 = MAT_CELL(x, 0, 0);
    Matrix *v = scale(x, 1 / (x0 + (fp) sgn(x0) * norm(x)));
    MAT_CELL(v, 0, 0) = 1;
    *tau = 2 / dot(v, v);

    return v;
}

/*
 * Reduce the given real matrix to Hessenberg form by means of Householder reflections
 */
Matrix *to_hessenberg(Matrix *m) {
    aassert(is_square(m), "Hessenberg transformation can only be applied to square matrices.");
    int n = m->height;
    Matrix *h = copy_mat(m);

    for (int k = 0; k < n - 2; k++) {
        // Extract k-th column
        Matrix *h_k = read_slice(h, k + 1, n - 1, k, k);  // column vector
        // Generate k-th Householder reflection, which can be applied as follows: PX = X - tau * v @ v.T @ X
        fp tau;
        Matrix *v_k = generate_householder(h_k, &tau);
        free_mat(h_k);

        // Apply it from the left:
        // H_k = H_k - tau * v_k @ v_k.T @ H_k
        h_k = read_slice(h, k + 1, n - 1, k, n - 1);
        Matrix *tmp1 = mat_mul_trans1(v_k, h_k);
        Matrix *tmp2 = outer(v_k, tmp1);
        scale_(tmp2, tau);
        sub_mat_(h_k, tmp2);
        write_slice(h, h_k, k + 1, k);
        free_mat(h_k);
        free_mat(tmp1);
        free_mat(tmp2);

        // Apply it from the right:
        // H_k = H_k - tau * (H_k @ v_k) @ v_k.T
        h_k = read_slice(h, 0, n - 1, k + 1, n - 1);
        tmp1 = mat_mul(h_k, v_k);
        tmp2 = mat_mul_trans2(tmp1, v_k);
        scale_(tmp2, tau);
        sub_mat_(h_k, tmp2);
        write_slice(h, h_k, 0, k + 1);
        free_mat(h_k);
        free_mat(tmp1);
        free_mat(tmp2);

        free_mat(v_k);
    }

    return h;
}

/*
 * QR decomposition using Householder reflections
 */
Pair *qr_decomposition(Matrix *m) {
    int n = m->height;
    Matrix *r = copy_mat(m);
    Matrix *q = eye(n);
    for (int k = 0; k < n; k++) {
        // Extract k-th column
        Matrix *r_k = read_slice(r, k, n - 1, k, k);
        // Generate Householder reflection
        fp tau;
        Matrix *v_k = generate_householder(r_k, &tau);
        free_mat(r_k);

        // Compute H_k
        Matrix *h = eye(n);
        Matrix *h_k = read_slice(h, k, n - 1, k, n - 1);
        Matrix *tmp = mat_mul_trans2(v_k, v_k);
        scale_(tmp, tau);
        sub_mat_(h_k, tmp);
        write_slice(h, h_k, k, k);
        free_mat(h_k);
        free_mat(tmp);

        // Q(k + 1) = Q(k) @ H(k)
        tmp = mat_mul(q, h);
        free_mat(q);
        q = tmp;

        // R(k + 1) = H(k) @ R(k)
        tmp = mat_mul(h, r);
        free_mat(r);
        r = tmp;

        // Free memory
        free_mat(h);
        free_mat(v_k);
    }
    // Make R upper triangular
    tri_up(r);

    // Pack Q and R into tuple
    Pair *qr = new_pair(q, r);

    return qr;
}

/*
 * Compute the eigenvalues of a square matrix by means of QR decomposition with shift and Hessenberg reduction
 */
Matrix *solve_eig_vals(Matrix *m, fp tol, int max_iter) {
    // Tri-diagonalize matrix using Hessenberg
    Matrix *t = to_hessenberg(m);
    int n = t->height;
    Matrix *eig_vals = new_vec(n);  // column vector

    int k = n - 1;
    int i = 0;
    while (k > 0 && i < max_iter) {
        // Obtain the shift from the lower right corner of the matrix.
        Matrix *mu = eye(k + 1);
        scale_(mu, MAT_CELL(t, k, k));

        // Shift T matrix and perform QR on shifted matrix
        sub_mat_(t, mu);
        Pair *qr = qr_decomposition(t);
        Matrix *q = qr->m1;
        Matrix *r = qr->m2;

        // Multiply R*Q and shift back result
        free_mat(t);
        t = mat_mul(r, q);
        add_mat_(t, mu);

        // Free memory
        free_mat(mu);
        free_pair(qr, true);

        if (ABS(MAT_CELL(t, k, k - 1)) < tol) {
            MAT_CELL(eig_vals, k, 0) = MAT_CELL(t, k, k);
            Matrix *tmp = read_slice(t, 0, k - 1, 0, k - 1);
            // Free T data field
            free(t->data);
            // Manually change T fields to Tmp
            t->height = tmp->height;
            t->width = tmp->width;
            t->data = tmp->data;
            // Free Tmp struct (but not data field)
            free(tmp);

            k--;
        }
        i++;
    }
    MAT_CELL(eig_vals, 0, 0) = MAT_CELL(t, 0, 0);

    free_mat(t);

    return eig_vals;
}

/*
 * Compute the eigenvector associated to an eigenvalue by means of inverse iteration
 */
Matrix *inv_iter(fp eig_val, Matrix *m, fp tol, int max_iter) {
    // Perturb lambda to prevent the computed matrix from becoming singular
    fp lambda = eig_val + uniform_rand() * 1e-6f;
    // Compute M' = M - lambda * I
    Matrix *lambda_i = eye(m->height);
    scale_(lambda_i, lambda);
    Matrix *m_prime = sub_mat(m, lambda_i);

    // Initialize vector randomly
    Matrix *eig_vec = mat_randn(m->height, 1);  // column vector
    Matrix *prev;

    int i = 0;
    do {
        if (i > 0)
            free_mat(prev);

        // Save previous estimate and compute the new one
        prev = copy_mat(eig_vec);
        free_mat(eig_vec);
        eig_vec = lin_solve(m_prime, prev);

        // If the first entry of the current estimate is negative,
        // swap the sign to improve convergence
        if (MAT_CELL(eig_vec, 0, 0) < 0) {
            scale_(eig_vec, -1);
        }

        // Normalize estimate
        fp v_norm = norm(eig_vec);
        scale_(eig_vec, 1 / v_norm);

        i++;
    } while ((!are_equal(eig_vec, prev, tol)) && i < max_iter);

    free_mat(lambda_i);
    free_mat(m_prime);
    free_mat(prev);

    return eig_vec;
}

/*
 * Compute the eigenvectors of a square matrix given the eigenvalues
 */
Matrix *solve_eig_vecs(Matrix *eig_vals, Matrix *m, fp tol, int max_iter) {
    Matrix *eig_vecs = new_mat(m->height, m->width);
    // Iterate over eigenvalues
    for (int i = 0; i < eig_vals->height; i++) {
        // Extract current eigenvalue
        fp eig_val = MAT_CELL(eig_vals, i, 0);  // eig_vals is a column vector
        // Compute current eigenvector
        Matrix *eig_vec = inv_iter(eig_val, m, tol, max_iter);
        // Insert it into i-th column of output matrix
        paste_col(eig_vecs, eig_vec, i);
        free_mat(eig_vec);
    }

    return eig_vecs;
}

/*
 * Compute the eigenvalues and eigenvectors of a square matrix by means of QR decomposition
 */
Pair *solve_eig(Matrix *m) {
    fp tol = 1e-7f;
    int max_iter = 3000;
    Matrix *eig_vals = solve_eig_vals(m, tol, max_iter);
    Matrix *eig_vecs = solve_eig_vecs(eig_vals, m, tol, max_iter);

    // Pack eigenvalues and eigenvectors into a pair
    Pair *eigen = new_pair(eig_vals, eig_vecs);

    return eigen;
}

/*
 * Solve a linear system Ux = y, where U is upper triangular, by means of back-substitution
 */
Matrix *back_substitution(Matrix *u, Matrix *y) {
    // NB: both X and Y are column vectors
    int n_eq = y->height;
    Matrix *x = new_vec(n_eq);

    // Iterate over the cells of y backwards
    for (int i = n_eq - 1; i >= 0; i--) {
        fp back_substitute = 0;
        // Iterate over the subsequent cells and accumulate back substitutions
        for (int j = i + 1; j < n_eq; j++) {
            back_substitute += MAT_CELL(x, j, 0) * MAT_CELL(u, i, j);
        }
        // Compute i-th solution
        MAT_CELL(x, i, 0) = (MAT_CELL(y, i, 0) - back_substitute) / MAT_CELL(u, i, i);
    }

    return x;
}

/*
 * Solve a linear system Ax = b by means of QR decomposition:
 * A = QR => QRx = b and, thanks to the orthogonality of Q, Rx = Q.Tb => Rx = y
 */
Matrix *lin_solve(Matrix *a, Matrix *b) {
    aassert(is_col_vector(b), "Matrix B should be a column vector.");
    // Decompose matrix A using QR
    Pair *qr = qr_decomposition(a);
    Matrix *q = qr->m1;
    Matrix *r = qr->m2;

    // Multiply the transpose of Q and b
    Matrix *y = mat_mul_trans1(q, b);
    // Solve Rx = y by means of back-substitution
    Matrix *x = back_substitution(r, y);

    // Free memory
    free_pair(qr, true);
    free_mat(y);

    return x;
}

void eigen_sort(Matrix *eig_vals, Matrix *eig_vecs) {
    int n_eig = eig_vals->height;
    // Sort eigenvalues in descending order
    int* sort_idx = quick_sort(eig_vals->data, n_eig, true);

    // Reorder eig_vecs according to sort_idx
    Matrix *eig_vecs_copy = copy_mat(eig_vecs);
    for (int i = 0; i < n_eig; i++) {
        int idx = sort_idx[i];
        Matrix *eig_vec = extract_col(eig_vecs_copy, idx);
        write_slice(eig_vecs, eig_vec, 0, i);

        free_mat(eig_vec);
    }

    // Free memory
    free(sort_idx);
    free_mat(eig_vecs_copy);
}

/*
 * Compute the Singular Value Decomposition of a given matrix
 */
Triplet *svd(Matrix *x) {
    // Compute x.T @ x
    Matrix *c = mat_mul_trans1(x, x);
    // Find eigenvalues and eigenvectors
    Pair *eigen = solve_eig(c);
    free_mat(c);
    Matrix *eig_vals = eigen->m1;
    Matrix *eig_vecs = eigen->m2;
    // Sort eigenvalues and eigenvectors
    eigen_sort(eig_vals, eig_vecs);

    // Compute matrix of singular values
    Matrix *s = new_mat(x->height, x->width);
    Matrix *s_inv = new_mat(x->width, x->height);
    for (int i = 0; i < eig_vals->height; i++) {
        fp s_val = SQRT(ABS(MAT_CELL(eig_vals, i, 0)));
        MAT_CELL(s, i, i) = s_val;
        MAT_CELL(s_inv, i, i) = 1 / s_val;
    }
    Matrix *s_diag = diagonal(s);
    free_mat(s);

    // Compute right singular vectors
    Matrix *eig_vec_norm = row_norm(eig_vecs);
    Matrix *v = div_row(eig_vecs, eig_vec_norm);
    free_mat(eig_vec_norm);
    Matrix *vt = transpose(v);

    // Compute left singular vectors
    Matrix *tmp = mat_mul(v, s_inv);
    free_mat(v);
    free_mat(s_inv);
    Matrix *u = mat_mul(x, tmp);
    free_mat(tmp);

    free_pair(eigen, true);

    // Pack U, S and Vt in a triplet
    Triplet *svd_ret = new_triplet(u, s_diag, vt);
    return svd_ret;
}

/*
 * Given a (n_features, n_samples) matrix, compute the (n_features, n_features) covariance matrix
 */
Matrix *covariance(Matrix *x, bool center_data) {
    int n_samples = x->width;
    Matrix *x_cov;

    if (center_data) {  // center data first
        // Compute mean and center data
        Matrix *x_mean = col_mean(x);
        Matrix *x_c = sub_col(x, x_mean);

        // Compute covariance matrix
        x_cov = mat_mul_trans2(x_c, x_c);  // transpose handled internally for efficiency reasons

        // Free memory
        free_mat(x_c);
        free_mat(x_mean);
    } else {  // data already centered
        // Compute covariance matrix
        x_cov = mat_mul_trans2(x, x);  // transpose handled internally for efficiency reasons
    }

    // Normalize
    fp fact = 1 / ((fp) n_samples - 1);
    scale_(x_cov, fact);

    return x_cov;
}
