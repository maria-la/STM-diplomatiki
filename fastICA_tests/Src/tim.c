#include "stm32f3xx.h"

#define TIM2EN 		(1U<<0)
#define CR1_CEN 	(1U<<0)
#define OC_TOGGLE 	((1U<<4) | (1U<<5))
#define CCER_CC1E 	(1U<<0)

#define IOPAEN		(1U<<17)

#define AFR1_TIM  	(1U<<4)
#define OC_OC1PE  	(1U<<3)
#define CCER_CC1P	(1U<<1)


#define OSPEEDR_5 	(1U<<11)
#define OTYPER_OT5 	~(1U<<5)
#define PUPDR_5 	~(1U<<11)


void tim2_1hz_init(void){

	/*Enable clock access to tim2*/
	RCC->APB1ENR |= TIM2EN;

	/*Set prescaler value*/
	TIM2->PSC = 800 - 1;

	/*Set auto-reload value*/
	TIM2->ARR = 10000 - 1;

	/*Clear counter*/
	TIM2->CNT = 0;

	/*Enable timer*/
	TIM2->CR1 |= CR1_CEN;

}

void tim2_pa1_output_compare(void){

	/*Configure PA1*/
	/*Enable clock access to GPIOA*/
	RCC->AHBENR |= IOPAEN;

	/*Set PA1 mode to alternate function*/
	GPIOA->MODER &=~ (1U<<2);
	GPIOA->MODER |=  (1U<<3);

	/*Set PA1 alternate function type to TIM2_CH2(AF1) */
	GPIOA->AFR[0] = AFR1_TIM;
	GPIOA->OSPEEDR |= OSPEEDR_5;
	GPIOA->OTYPER &= OTYPER_OT5;
	GPIOA->PUPDR &= PUPDR_5;

	/*Enable clock access to tim2*/
	RCC->APB1ENR |= TIM2EN;

	/*Set prescaler value*/
	TIM2->PSC = 800 - 1;

	/*Set auto-reload value*/
	TIM2->ARR = 10000 - 1;


	/*Set output compare toggle mode*/
	TIM2->CCMR1 = OC_TOGGLE;

	/*Enable tim2 ch1 in compare mode*/
	TIM2->CCER &=~ CCER_CC1P;
	TIM2->CCER |= CCER_CC1E;

	/*Clear counter*/
	TIM2->CNT = 0;

	/*Enable timer*/
	TIM2->CR1 |= CR1_CEN;
}
