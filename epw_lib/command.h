/* 20150707 wei han for the test of EPW2 */

#ifndef __COMMAND_H__
#define __COMMAND_H__
#include "stm32f4xx.h"

extern uint8_t Receive_String_Ready;

extern void receive_task();

static struct receive_cmd_list{
	volatile unsigned char Identifier[3];
	volatile unsigned char DIR_cmd;
	volatile unsigned char pwm_value;
};

#endif /* __COMMAND_H__ */