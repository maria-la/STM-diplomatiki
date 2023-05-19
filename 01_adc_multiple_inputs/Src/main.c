#include <stdint.h>
#include <stdio.h>
#include "stm32f3xx.h"
#include "uart.h"
#include "adc.h"

static void adc1_callback(void);
static void adc2_callback(void);

uint16_t sensor_value1, sensor_value2;

int main(void){

	adc1_interrupt_init_ch1();
	adc2_interrupt_init_ch1();
	start_conversion_dual();

	while(1){

	}


}

static void adc1_callback(void){
	sensor_value1 = ADC1->DR;
	printf("Sensor Value: %d \n\r", (int)sensor_value1);
}

static void adc2_callback(void){
	sensor_value2 = ADC2->DR;
	printf("Sensor Value: %d \n\r", (int)sensor_value2);
}

void ADC1_2_IRQHandler(void){
	///metefere times interrupt se pinaka -- cyclic buffer
	//DMA gia kalutera -- double cyclic buffer

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


