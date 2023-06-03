#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include <stdio.h>
#include "stm32f3xx.h"
#include "uart.h"
#include "adc.h"
#include "dma.h"



#define IOPAEN			(1U<<17)
#define PIN5 			(1U<<5)

#define LED_PIN			PIN5

#define IFCR_CTCIF1 	(1U<<1)
#define IFCR_CHTIF1 	(1U<<2)

#define ISR_TCIF1 		(1U<<1)
#define ISR_HTIF1 		(1U<<2)

#define IFCR_CTCIF2 	(1U<<5)
#define IFCR_CHTIF2 	(1U<<6)

#define ISR_TCIF2 		(1U<<5)
#define ISR_HTIF2 		(1U<<6)


static void adc1_callback(void);
static void adc2_callback(void);

static void dma_ch1_callback(void);
static void dma_ch1_callback_h(void);

static void dma_ch2_callback(void);
static void dma_ch2_callback_h(void);


uint16_t mic1[100], mic2[100];

uint16_t sensor_value1, sensor_value2;


#endif /* MAIN_H_ */