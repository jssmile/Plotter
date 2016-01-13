/*=============================================================================
  *
  * @file     : linear_actuator.h
  * @author        : skyline
  * @data       : 2014/3/10
  * @brief   : linear_actuator.c header file
  *
  *============================================================================*/
#ifndef __LINEAR_ACTUATOR_H__
#define __LINEAR_ACTUATOR_H__

#include "stm32f4xx_syscfg.h"


#define ACTU_CONTROL_PORT             GPIOE

#define ACTU_A_IN1_PIN              GPIO_Pin_0
#define ACTU_A_IN2_PIN              GPIO_Pin_1
#define ACTU_B_IN3_PIN              GPIO_Pin_2
#define ACTU_B_IN4_PIN              GPIO_Pin_3


/*
 *Limit Switch
 */
#define LS_READ_PORT              GPIOD
#define LS_A_UPPER_PIN              GPIO_Pin_1
#define LS_A_LOWER_PIN              GPIO_Pin_2
#define LS_B_UPPER_PIN              GPIO_Pin_3
#define LS_B_LOWER_PIN              GPIO_Pin_4

enum{
  LINEAR_ACTU_STOP,
  LINEAR_ACTU_CW,
  LINEAR_ACTU_CCW
};

struct limit_switch_info{
  uint8_t actuatorA_LS_state;
  uint8_t actuatorB_LS_state;
};

static void init_LS();
static void init_CP();
void init_linear_actuator();
static void detect_LS_Polling();
void set_linearActuator_A_cmd(int flag);
void set_linearActuator_B_cmd(int flag);
int get_Linear_Actuator_A_LS_State();
int get_Linear_Actuator_B_LS_State();

#endif /* __LINEAR_ACTUATOR_H__ */
