#include "main.h"

int main(void){

	// fastICA-----------------------------//
	Matrix *x1;
	FastICAStrategy strategy = Parallel;
	GFunc g_function = LogCosh;
	fp threshold = 1e-4f;
	int max_iter = 3000;
	int s_len = 10;
	int s_rate = 100;
	bool add_noise = true;
	bool verbose = false;


	RCC->AHBENR |= IOPAEN;

	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);

	fpu_init();
	uart2_tx_init();
	adc1_interrupt_init_ch1();
	adc2_interrupt_init_ch1();

	dma_init_ch1((uint32_t)&ADC1->DR, (uint32_t)mic1, winLengthDouble);
	dma_init_ch2((uint32_t)&ADC2->DR, (uint32_t)mic2, winLengthDouble);

	start_conversion_dual();

	arm_rfft_fast_init_128_f32(&audioInput);
	int n_samples = s_len * s_rate;

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

			x1 = from_array( &fftOut1[i-winLength], winLength, 1);
			//print_mat(x);
			//x.data = &fftOut1[i-winLength];
			// Perform FastICA


		    // Create matrix S of original signals (n_components, n_samples)
		    //Matrix *s = generate_signals(n_samples, (fp) s_len, add_noise);
		    // Standardize signal
		    Matrix *s_std = col_std(x1);
		    div_col_(x1, s_std);

		    //fp a_data[] = {1, 1, 1, 0.5f, 2, 1, 1.5f, 1, 2};
		    //Matrix *a = from_array(a_data, 3, 3);

		    // Create observation X by mixing signal S with matrix A (n_components, n_samples)
		    //Matrix *x = mat_mul(a, s);


		    // Perform FastICA
		    Matrix *s_ = fast_ica(x1, true, strategy, g_function, threshold, max_iter);

			//Matrix *s_ = fast_ica(x, true, strategy, g_function, threshold, max_iter);
			//print_mat(s_);
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


			//y = from_array( &fftOut1[i-winLength], winLength, 1);
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
