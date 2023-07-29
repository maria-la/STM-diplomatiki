#ifndef WHIT_H_
#define WHIT_H_

void whitenRows(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *matrixOut);
void decorrelate(arm_matrix_instance_f32 *matrix, arm_matrix_instance_f32 *matrixOut, arm_matrix_instance_f32 *ref);

#endif /* WHIT_H_ */
