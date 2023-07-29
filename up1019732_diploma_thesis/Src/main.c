#include "main.h"

int main(void){

	//Inittialise Floating Point Unit
	fpu_init();

	//Initialise Analog to Digital Converters 1 and 2
	adc1_interrupt_init_ch1();
	adc2_interrupt_init_ch1();

	//Initialise Direct Memory Acces from the ADCx outputs to vectors mic1, mic2
	dma_init_ch1((uint32_t)&ADC1->DR, (uint32_t)mic1, winLengthDouble);
	dma_init_ch2((uint32_t)&ADC2->DR, (uint32_t)mic2, winLengthDouble);

	//Start ADC in dual mode (ADC1-ADC2 work concurrently)
	start_conversion_dual();

	while(1){

		// mic1+mic2 full flag
		if(k1*k2 != 0){
			k = k1;
			k1 = 0;
			k2 = 0;
			i = 0;

			switch (k){
				//when half of the buffer is full
			    case 1:
			      break;

				//when the buffer is full
			    case 2:
			      i = winLength;
			      break;

			    default:
			      printf("Error in k no");
			      break;
			}

			i /= k;

			//Transform data from uint16_t to float32_t
			while(i < (winLength*k)){
				// changed mic1/mic2 to data from known signals for testing purposes
				input[i] =  (((sawtooth[i]+square[i])/2)+(0.3*sawtooth[i]+0.7*square[i]))/2;
				i++;
			}

			//Initialise necessary Matrices for FastICA
 			arm_mat_init_f32 (&A, 128, 1, input);
 			arm_mat_init_f32 (&V, 2, 128, vData);

 			//perform signal Seperation
 			fastICA(&A, &V, 2);

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
