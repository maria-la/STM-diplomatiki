#include "main.h"

uint16_t m1,m2;

int main(void){

	adc1_interrupt_init_ch1();
	adc2_interrupt_init_ch1();
	start_conversion_dual();

	//dma_init_ch1((uint32_t)&ADC1->DR, (uint32_t)&mic1, 100);


	while(1){

	}


}

static void adc1_callback(void){
	sensor_value1 = ADC1->DR;
	printf("Sensor Value: %d \n\r", (int)sensor_value1);

	dma_init_ch1((uint32_t)&ADC1->DR, (uint32_t)mic1, 100);
}

static void adc2_callback(void){
	sensor_value2 = ADC2->DR;
	printf("Sensor Value: %d \n\r", (int)sensor_value2);
	//dma_init_ch1((uint32_t)&ADC1->DR, (uint32_t)mic2, 100);
}

static void dma_callback(void){
	GPIOA->ODR |= LED_PIN;
	m1 = ADC1->DR;
}

static void dma_callback_h(void){
	GPIOA->ODR |= LED_PIN;
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
		dma_callback();

	}

	if(DMA1->ISR & ISR_HTIF1){
			/*Clear interrupt flag*/
			DMA1->IFCR |= IFCR_CHTIF1;

			//Do something
			dma_callback_h();

		}

}
