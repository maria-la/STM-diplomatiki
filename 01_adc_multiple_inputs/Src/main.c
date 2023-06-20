#include "main.h"

uint16_t m1,m2;

int main(void){


	RCC->AHBENR |= IOPAEN;

	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &=~ (1U<<11);

	fpu_init();
	adc1_interrupt_init_ch1();
	adc2_interrupt_init_ch1();
	start_conversion_dual();

	arm_rfft_fast_init_128_f32(&audioInput1);
	while(1){

	}


}

static void adc1_callback(void){
	sensor_value1 = ADC1->DR;

	/*Move values to mem*/
	dma_init_ch1((uint32_t)&ADC1->DR, (uint32_t)mic1, winLength);
}

static void adc2_callback(void){
	sensor_value2 = ADC2->DR;

	/*Move values to mem*/
	dma_init_ch2((uint32_t)&ADC2->DR, (uint32_t)mic2, winLength);
}

static void dma_ch1_callback(void){
	GPIOA->ODR |= LED_PIN;
	m1 = ADC1->DR;

	for(int i=0;i<winLength; i++){
			input1[i] = (float32_t) (mic1[i]);
		}


	arm_rfft_fast_f32(&audioInput1, input1, fftOut1, 0);
	arm_cmplx_mag_f32(fftOut1, fftOut1, (winLength/2));

	fftOut1[0] = 0;

}

static void dma_ch1_callback_h(void){
	//GPIOA->ODR |= LED_PIN;
	m2 = ADC1->DR;
}

static void dma_ch2_callback(void){
	GPIOA->ODR |= LED_PIN;
	m1 = ADC1->DR;
}

static void dma_ch2_callback_h(void){
	//GPIOA->ODR |= LED_PIN;
	m2 = ADC1->DR;
}

void ADC1_2_IRQHandler(void){
	/*Check if eoc in SR*/
	if((ADC1->ISR & ISR_EOC)!=0){
		/*Clear EOC*/
		ADC1->ISR &=~ ISR_EOC;

		//Do something
		adc1_callback();
	}

	if((ADC2->ISR & ISR_EOC)!=0){
			/*Clear EOC*/
			ADC2->ISR &=~ ISR_EOC;

			//Do something
			adc2_callback();
		}
}



void DMA1_CH1_IRQHandler(void){
	/*Check for transfer complete interrupt*/
	if(DMA1->ISR & ISR_TCIF1){
		/*Clear interrupt flag*/
		DMA1->IFCR |= IFCR_CTCIF1;

		//Do something
		dma_ch1_callback();

	}

	if(DMA1->ISR & ISR_HTIF1){
			/*Clear interrupt flag*/
			DMA1->IFCR |= IFCR_CHTIF1;

			//Do something
			dma_ch1_callback_h();

		}

}

void DMA1_CH2_IRQHandler(void){
	/*Check for transfer complete interrupt*/
	if(DMA1->ISR & ISR_TCIF2){
		/*Clear interrupt flag*/
		DMA1->IFCR |= IFCR_CTCIF2;

		//Do something
		dma_ch2_callback();

	}

	if(DMA1->ISR & ISR_HTIF2){
			/*Clear interrupt flag*/
			DMA1->IFCR |= IFCR_CHTIF2;

			//Do something
			dma_ch2_callback_h();

		}

}
