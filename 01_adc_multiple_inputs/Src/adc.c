#include "stm32f3xx.h"
#include "adc.h"

#define IOPAEN			(1U<<17)
#define ADC12EN			(1U<<28)

#define ADC_CH1 		(1U<<6)
#define ADC_SEQ_LEN_1 	(0x00)

#define CR_ADEN			(1U<<0)
#define CR_ADSTART		(1U<<2)
#define CR_ADCALDIF 	(1U<<30)
#define CR_ADCAL 		(1U<<31)

#define ISR_ADRDY		(1U<<0)

#define CFGR_CONT 		(1U<<13)

#define IER_EOCIE 		(1U<<2)

void pa0_adc_init(void){

	/***Configure the ADC GPIO ***/
	/*Enable clock access to GPIOA*/
	RCC->AHBENR |= IOPAEN;


	/*Set the mode of PA0 to analog*/
	GPIOA->MODER |= (1U<<0);
	GPIOA->MODER |= (1U<<1);

	/***Configure the ADC module***/
	/*Enable clock access to ADC*/
	RCC->AHBENR |= ADC12EN;

	/*Configure ADC clock mode (HCLK/2)*/ //ftiakse /4
	ADC12_COMMON->CCR |= (1U<<17);
	ADC12_COMMON->CCR &=~ (1U<<16);

	/*Enable voltage regulator*/
	ADC1->CR &=~ (1U<<29);
	ADC1->CR |= (1U<<28);

	/*Confirm ADEN is off*/
	while((ADC1->CR & CR_ADEN)){}
	//ADC1->CR &=~ CR_ADEN;

	/*Configure and start calibration*/
	ADC1->CR &= ~CR_ADCALDIF;
	ADC1->CR |= CR_ADCAL;

	//value1 = ADC1->CR;

	/*Wait for calibration to end*/
	while(ADC1->CR & CR_ADCAL){}

	/*Conversion sequence start*/
	ADC1->SQR1 = ADC_CH1;

	/*Conversion sequence length*/
	ADC1->SQR1 &= ~(1U<<0);
	ADC1->SQR1 &= ~(1U<<1);
	ADC1->SQR1 &= ~(1U<<2);
	ADC1->SQR1 &= ~(1U<<3);

	/*Enable ADC module*/
	ADC1->CR |= CR_ADEN;

	/*Wait for ADC to be ready*/
	while(!(ADC1->ISR & ISR_ADRDY)){}
}

void pa0_adc1_interrupt_init(void){

	/***Configure the ADC GPIO ***/
	/*Enable clock access to GPIOA*/
	RCC->AHBENR |= IOPAEN;


	/*Set the mode of PAx to analog*/
	GPIOA->MODER |= (1U<<0);
	GPIOA->MODER |= (1U<<1);

	/***Configure the ADC module***/
	/*Enable clock access to ADC*/
	RCC->AHBENR |= ADC12EN;

	/*Enable ADC end-of-conversion interrupt*/
	ADC1->IER |= IER_EOCIE;

	/*Enable ADC interrupt in NVIC*/
	NVIC_EnableIRQ(ADC1_2_IRQn);

	/*Configure ADC clock mode (HCLK/2)*/
	ADC12_COMMON->CCR |= (1U<<17);
	ADC12_COMMON->CCR &=~ (1U<<16);

	/*Enable voltage regulator*/
	ADC1->CR &=~ (1U<<29);
	ADC1->CR |= (1U<<28);

	/*Confirm ADEN is off*/
	while((ADC1->CR & CR_ADEN)){}

	/*Configure and start calibration*/
	ADC1->CR &= ~CR_ADCALDIF;
	ADC1->CR |= CR_ADCAL;

	/*Wait for calibration to end*/
	while(ADC1->CR & CR_ADCAL){}

	/*Conversion sequence start*/
	ADC1->SQR1 = ADC_CH1;

	/*Conversion sequence length*/
	ADC1->SQR1 &= ~(1U<<0);
	ADC1->SQR1 &= ~(1U<<1);
	ADC1->SQR1 &= ~(1U<<2);
	ADC1->SQR1 &= ~(1U<<3);

	/*Enable ADC module*/
	ADC1->CR |= CR_ADEN;

	/*Wait for ADC to be ready*/
	while(!(ADC1->ISR & ISR_ADRDY)){}
}

void pa1_adc2_interrupt_init(void){

	/***Configure the ADC GPIO ***/
	/*Enable clock access to GPIOA*/
	RCC->AHBENR |= IOPAEN;


	/*Set the mode of PAx to analog*/
	GPIOA->MODER |= (1U<<8);
	GPIOA->MODER |= (1U<<9);

	/***Configure the ADC module***/
	/*Enable clock access to ADC*/
	RCC->AHBENR |= ADC12EN;

	/*Enable ADC end-of-conversion interrupt*/
	ADC2->IER |= IER_EOCIE;

	/*Enable ADC interrupt in NVIC*/
	NVIC_EnableIRQ(ADC1_2_IRQn);

	/*Configure ADC clock mode (HCLK/2)*/
	ADC12_COMMON->CCR |= (1U<<17);
	ADC12_COMMON->CCR &=~ (1U<<16);

	/*Enable voltage regulator*/
	ADC2->CR &=~ (1U<<29);
	ADC2->CR |= (1U<<28);

	/*Confirm ADEN is off*/
	while((ADC2->CR & CR_ADEN)){}

	/*Configure and start calibration*/
	ADC2->CR &= ~CR_ADCALDIF;
	ADC2->CR |= CR_ADCAL;

	/*Wait for calibration to end*/
	while(ADC2->CR & CR_ADCAL){}

	/*Conversion sequence start*/
	ADC2->SQR1 = ADC_CH1;

	/*Conversion sequence length*/
	ADC2->SQR1 &= ~(1U<<0);
	ADC2->SQR1 &= ~(1U<<1);
	ADC2->SQR1 &= ~(1U<<2);
	ADC2->SQR1 &= ~(1U<<3);

	/*Enable ADC module*/
	ADC2->CR |= CR_ADEN;

	/*Wait for ADC to be ready*/
	while(!(ADC2->ISR & ISR_ADRDY)){}
}

void start_conversion1(void){
	/*Enable continuous conversion*/
	//ADC1->CFGR |= CFGR_CONT;
	/*Start the ADC conversion*/
	ADC1->CR |= CR_ADSTART;
}

void start_conversion2(void){
	/*Enable continuous conversion*/
	//ADC2->CFGR |= CFGR_CONT;
	/*Start the ADC conversion*/
	ADC2->CR |= CR_ADSTART;
}


uint16_t adc_read1(void){
	/*Wait for conversion to be complete*/
	while(!(ADC1->ISR & ISR_EOC)){}

	/*Read conversion result*/
	return (ADC1->DR);
}

