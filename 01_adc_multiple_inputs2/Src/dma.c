#include "stm32f3xx.h"

#define RCC_DMA1EN 					(1U<<0)

#define DMAEN  						(1U<<0)
#define DMA_CH_EN 					(1U<<0)
#define DMA_MEM_INC					(1U<<7)
#define DMA_DIR_PERIPH_TO_MEM  		~(1U<<4)
#define DMA_CIRC 					(1U<<5)
#define DMA_CR_TCIE 				(1U<<1)
#define DMA_CR_HTIE					(1U<<2)

#define CFGR_DMAEN 				(1U<<0)
#define CFGR_DMACFG				(1U<<1)




void dma_init_ch1(uint32_t per,uint32_t mem, uint32_t len){

	/*Enable clock access to DMA*/
	RCC->AHBENR |= DMAEN;

	/*Disable DMA1*/
	DMA1_Channel1->CCR &=~ DMA_CH_EN;

	while(DMA1_Channel1->CCR & DMA_CH_EN){}

	/*Clear all interrupt flags of ch1*/
	DMA1->IFCR |= (1U<<0);
	DMA1->IFCR |= (1U<<1);
	DMA1->IFCR |= (1U<<2);
	DMA1->IFCR |= (1U<<3);

	DMA1_Channel1->CCR |= (1U<<12); //priority
	DMA1_Channel1->CCR |= (1U<<13);

	DMA1_Channel1->CCR |= (1U<<10); //transfer size
	DMA1_Channel1->CCR |= (1U<<8);

	/*Set the peripheral buffer*/
	DMA1_Channel1->CPAR = per;

	/*Set the memory buffer*/
	DMA1_Channel1->CMAR = mem;

	/*Set length*/
	DMA1_Channel1->CNDTR = len;

	/*Enable circular mode*/
	DMA1_Channel1->CCR |= DMA_CIRC;

	/*Enable memory increment*/
	DMA1_Channel1->CCR |= DMA_MEM_INC;

	/*Configure transfer direction*/
	DMA1_Channel1->CCR &= DMA_DIR_PERIPH_TO_MEM;

	/*Enable DMA transfer compete interrupt*/
	DMA1_Channel1->CCR |= DMA_CR_TCIE;

	/*Enable DMA half transfer compete interrupt*/
	DMA1_Channel1->CCR |= DMA_CR_HTIE;


	/*Enable DMA1*/
	DMA1_Channel1->CCR |= DMA_CH_EN;

	/*Enable adc1 transmitter DMA*/
	ADC1->CFGR |= CFGR_DMAEN;

	/*Adc1 transmitter DMA to circular*/
	ADC1->CFGR |= CFGR_DMACFG;

	/*DMA Interrupt Enable in NVIC*/
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void dma_init_ch2(uint32_t per,uint32_t mem, uint32_t len){

	/*Enable clock access to DMA*/
	RCC->AHBENR |= DMAEN;

	/*Disable DMA1*/
	DMA1_Channel2->CCR &=~ DMA_CH_EN;

	while(DMA1_Channel2->CCR & DMA_CH_EN){}

	/*Clear all interrupt flags of ch1*/
	DMA1->IFCR |= (1U<<4);
	DMA1->IFCR |= (1U<<5);
	DMA1->IFCR |= (1U<<6);
	DMA1->IFCR |= (1U<<7);

	DMA1_Channel2->CCR |= (1U<<12); //priority
	DMA1_Channel2->CCR |= (1U<<13);

	DMA1_Channel2->CCR |= (1U<<10); //transfer size
	DMA1_Channel2->CCR |= (1U<<8);

	/*Set the peripheral buffer*/
	DMA1_Channel2->CPAR = per;

	/*Set the memory buffer*/
	DMA1_Channel2->CMAR = mem;

	/*Set length*/
	DMA1_Channel2->CNDTR = len;

	/*Enable circular mode*/
	DMA1_Channel2->CCR |= DMA_CIRC;

	/*Enable memory increment*/
	DMA1_Channel2->CCR |= DMA_MEM_INC;

	/*Configure transfer direction*/
	DMA1_Channel2->CCR &= DMA_DIR_PERIPH_TO_MEM;

	/*Enable DMA transfer compete interrupt*/
	DMA1_Channel2->CCR |= DMA_CR_TCIE;

	/*Enable DMA half transfer compete interrupt*/
	DMA1_Channel2->CCR |= DMA_CR_HTIE;


	/*Enable DMA1*/
	DMA1_Channel2->CCR |= DMA_CH_EN;

	/*Enable adc2 transmitter DMA*/
	ADC2->CFGR |= CFGR_DMAEN;

	/*Adc2 transmitter DMA to circular*/
	ADC2->CFGR |= CFGR_DMACFG;

	/*DMA Interrupt Enable in NVIC*/
	NVIC_EnableIRQ(DMA1_Channel2_IRQn);
}

