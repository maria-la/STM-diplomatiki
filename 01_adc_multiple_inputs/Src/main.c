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


	while(1){

		if(k1 != 0){
			int k = k1;
			k1 = 0;

			int i = 0;

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
				input1[i] = (float32_t) (mic1[i]);
				i++;
			}

 			arm_rfft_fast_f32(&audioInput, &input1[i-winLength], &fftOut1[i-winLength], 0);
			arm_cmplx_mag_f32(&fftOut1[i-winLength], &fftOut1[i-winLength], (winLength/2));

			fftOut1[i-winLength] = 0;

		}


		if(k2 != 0){
			int k = k2;
			k2 = 0;

			int i;
			switch (k){
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


			while(i < (winLength*k)){
				input1[i] = (mic2[i]);
				i++;
			}


			arm_rfft_fast_f32(&audioInput, &input2[i-winLength], &fftOut2[i-winLength], 0);
			arm_cmplx_mag_f32(&fftOut2[i-winLength], &fftOut2[i-winLength], (winLength/2));

			fftOut2[i-winLength] = 0;

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
