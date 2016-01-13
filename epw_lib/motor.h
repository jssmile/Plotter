/* 20150708 wei han for the test of EPW2 */
#ifndef __MOTOR_H__
#define __MOTOR_H__

/* Define all pins of the motor of EPW2 */
#define MOTOR_PWM_PORT				GPIOD
#define MOTOR_LEFT_PWM_PIN			GPIO_Pin_14 //Red  TIM4_CH3
#define MOTOR_RIGHT_PWM_PIN			GPIO_Pin_15 //Blue TIM4_CH4

extern void init_motor(void);
extern void testMotor(uint32_t SpeedValue_left, uint32_t SpeedValue_right);

#endif 
/* __MOTOR_H__ */
