#include "stm32f3xx.h"

void fpu_init(void){
	// Enable the FPU
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));

	// Initialize FPU lazy stacking mode
	FPU->FPCCR |= FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk;

}
