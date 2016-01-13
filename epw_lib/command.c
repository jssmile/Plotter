#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "stm32f4xx_usart.h"
#include "uart.h"
#include "command.h"
#include "motor.h"
#include "linear_actuator.h"
/*parse*/
#include "string.h"

extern uint32_t SpeedValue_left;
extern uint32_t SpeedValue_right;
uint32_t inc = 1;

struct receive_cmd_list * receive_cmd_type;

void receive_task(){
	//int i, j;
	//while(1){
		if(Receive_String_Ready){
			if(received_string[0] == '+'){
				SpeedValue_left += inc;
				SpeedValue_right += inc;
				testMotor(SpeedValue_left,SpeedValue_right);
				USART_puts(USART3, "left:");
				USART_putd(USART3, SpeedValue_left);
				USART_puts(USART3, " right:");
				USART_putd(USART3, SpeedValue_right);
				USART_puts(USART3, "\r\n");
			}else if(received_string[0] == '-'){
				SpeedValue_left -= inc;
				SpeedValue_right -= inc;
				testMotor(SpeedValue_left,SpeedValue_right);
				USART_puts(USART3, "left:");
				USART_putd(USART3, SpeedValue_left);
				USART_puts(USART3, " right:");
				USART_putd(USART3, SpeedValue_right);
				USART_puts(USART3, "\r\n");
			}else if(received_string[0] == 'e'){
				getEncoder();
			}else if(received_string[0] == 'u'){
				if(received_string[1] == 'a'){
				    USART_puts(USART3, "Actu_A_up");
				    set_linearActuator_A_cmd(LINEAR_ACTU_CW);
				    USART_puts(USART3, "\r\n");
				}else if(received_string[1] == 'b'){
				    USART_puts(USART3, "Actu_B_up");
				    set_linearActuator_B_cmd(LINEAR_ACTU_CW);
				    USART_puts(USART3, "\r\n");
				}
			}else if(received_string[0] == 'd'){
				if(received_string[1] == 'a'){
				    USART_puts(USART3, "Actu_A_down");
				    set_linearActuator_A_cmd(LINEAR_ACTU_CCW);
				    USART_puts(USART3, "\r\n");
				}else if(received_string[1] == 'b'){
				    USART_puts(USART3, "Actu_B_down");
				    set_linearActuator_B_cmd(LINEAR_ACTU_CCW);
				    USART_puts(USART3, "\r\n");
				}
			}else{
				USART_puts(USART3, received_string);
				USART_puts(USART3, "\r\n");
			}
			//}
			/*Receive_String_Ready = 0;
			for( i = 0 ; i< MAX_STRLEN ; i++){
				received_string[i]= 0;*/
		}
	//}
}
/*
void parseString(unsigned char *received_string){
	char *word = strtok(received_string,' ');
	receive_cmd_type->Identifier[0] = word;
	if (word = 'c'){
			word = strtok(NULL,' ');
			//receive_cmd_type->DIR_cmd = word;
			//word = strtok(NULL,' ');
			//receive_cmd_type->pwm_value = word;
	}
}*/
