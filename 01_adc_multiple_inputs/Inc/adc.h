#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

#define ISR_EOC 		(1U<<2)

void pa0_adc1_interrupt_init(void);
void pa1_adc2_interrupt_init(void);
uint16_t adc_read1(void);
void start_conversion1(void);
void start_conversion2(void);

#endif /* ADC_H_ */
