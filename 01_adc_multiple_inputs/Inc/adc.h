#ifndef ADC_H_
#define ADC_H_

//#include <stdint.h>

#define ISR_EOC 		(1U<<2)

void adc1_interrupt_init_ch1(void);
void adc2_interrupt_init_ch1(void);
uint16_t adc_read1(void);
void start_conversion_dual(void);

#endif /* ADC_H_ */
