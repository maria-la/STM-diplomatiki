#include "main.h"

uint16_t m1,m2;

int main(void){


	RCC->AHBENR |= IOPAEN;

	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);

	fpu_init();
	adc1_interrupt_init_ch1();
	adc2_interrupt_init_ch1();

	dma_init_ch1((uint32_t)&ADC1->DR, (uint32_t)mic1, winLengthDouble);
	dma_init_ch2((uint32_t)&ADC2->DR, (uint32_t)mic2, winLengthDouble);

	start_conversion_dual();

	arm_rfft_fast_init_128_f32(&audioInput);
	arm_rfft_fast_init_64_f32(&audioInput2);

	arm_matrix_instance_f32 A,V;
	//arm_rfft_fast_init_128_f32(&audioInput2);

/*	float32_t aData[4] = {1,2,3,4};
	float32_t uData[4];
	float32_t sData[2];
	float32_t vData[4];
	float32_t atData[4];

	float32_t covData[4];

	arm_matrix_instance_f32 A, U, S, V, At, covm;
	arm_mat_init_f32 (&A, 2, 2, aData);
	//arm_mat_init_f32 (&At, 2, 2, atData);
	//arm_mat_init_f32 (&U, 2, 2, uData);
	arm_mat_init_f32 (&V, 2, 2, vData);
	//arm_mat_init_f32 (&covm, 2, 2, covData);

	covMat(aData, vData, 2, 2);
	centerRows(&A, &V);
	whitenRows(&A, &V);
	fastICA(&A, &V, 2);*/
	/*float32_t vData[128*2];
	//float32_t outData[128*2];
	float32_t aData[128];

	arm_mat_init_f32 (&A, 128, 1, aData);
	//vData[128*2];
	arm_mat_init_f32 (&V, 2, 128, vData);
	for(int a=0;a<128; a++ ){
		aData[a] = (sawtooth[a]+square[a])/2;
	}

	fastICA(&A, &V, 2);*/
	float32_t vData[128*2];
	float32_t outData[128*2];

	int i = 0;
	int j = 0;

	while(1){

		if(k1 != 0){
			int k = k1;
			k1 = 0;
			i = 0;
			j = 0;

			switch (k){
			    case 1:
			      break;

			    case 2:
			      i = winLength;
			      break;

			    default:
			      printf("Error in k1 no");
			      break;
			}


			while(i < (winLength*k)){
				input1[i] =  ((sawtooth[i/k]+square[i/k])/2);
				i++;
			}

 			arm_rfft_fast_f32(&audioInput, &input1[i-winLength], &fftOut1[i-winLength], 0);
			//arm_cmplx_mag_f32(&fftOut1[i-winLength], &fftOut1[i-winLength], (winLength/2));

			///fftOut1[i-winLength] = 0;

		/*	for(j = 0; j<(winLength/2); j++){
				fftOut1[i-j-1] = fftOut1[j];
			}*/
 			arm_mat_init_f32 (&A, 128, 1, &fftOut1[i-winLength]);
 			//vData[128*2];
 			arm_mat_init_f32 (&V, 2, 128, vData);

 			fastICA(&A, &V, 2);
			arm_rfft_fast_f32 (&audioInput2, vData, outData, 1);

		}


		if(k2 != 0){
			int kk = k2;
			k2 = 0;
			i = 0;
			j = 0;

			switch (kk){
				case 1:
				  i = 0;
				  break;

				case 2:
				  i = winLength;
				  break;

				default:
				  printf("Error in k2 no");
				  break;
			}


			while(i < (winLength*kk)){
				input2[i] = (mic2[i]);
				i++;
			}


			arm_rfft_fast_f32(&audioInput, &input2[i-winLength], &fftOut2[i-winLength], 0);
			arm_cmplx_mag_f32(&fftOut2[i-winLength], &fftOut2[i-winLength], (winLength/2));

			fftOut2[i-winLength] = 0;

			for(j = 0; j<(winLength/2); j++){
				fftOut2[i-j-1] = fftOut2[j];
			}

			}
	}


}


void ADC1_2_IRQHandler(void){
	/*Clear EOC ADC1*/
	ADC1->ISR &=~ ISR_EOC;

	/*Clear EOC ADC2*/
	ADC2->ISR &=~ ISR_EOC;

}



void DMA1_CH1_IRQHandler(void){
	/*Check for transfer complete interrupt*/
	if(DMA1->ISR & ISR_TCIF1){
		/*Clear interrupt flag*/
		DMA1->IFCR |= IFCR_CTCIF1;

		k1 = 2; //flag

	}
	else if(DMA1->ISR & ISR_HTIF1){
			/*Clear interrupt flag*/
			DMA1->IFCR |= IFCR_CHTIF1;

			k1 = 1; //flag

	}

}

void DMA1_CH2_IRQHandler(void){
	/*Check for transfer complete interrupt*/
	if(DMA1->ISR & ISR_TCIF2){
		/*Clear interrupt flag*/
		DMA1->IFCR |= IFCR_CTCIF2;

		k2 = 2; //flag

	}

	else if(DMA1->ISR & ISR_HTIF2){
			/*Clear interrupt flag*/
			DMA1->IFCR |= IFCR_CHTIF2;

			k2 = 1; //flag

		}

}
