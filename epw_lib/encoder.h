#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "stm32f4xx_gpio.h"
#include "FreeRTOS.h"

#define ENCODER_PORT 		GPIOA

#define ENCODER_LEFT_A_PIN	GPIO_Pin_0
#define ENCODER_RIGHT_A_PIN	GPIO_Pin_1
#define ENCODER_LEFT_B_PIN	GPIO_Pin_2
#define ENCODER_RIGHT_B_PIN	GPIO_Pin_3

void init_encoder(void);
void init_encoder_exti(void);

void EXTI0_IRQHandler();
void EXTI1_IRQHandler();
void detachEXTI(uint32_t EXTI_LineX);
void attachEXTI(uint32_t EXTI_LineX);
void getEncoder(void);
#endif /* __ENCODER_H__ */
