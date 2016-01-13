#include "linear_actuator.h"
#include "stm32f4xx.h"                                                                                  
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

typedef enum{
    ACTUATOR_GROUP_A,
    ACTUATOR_GROUP_B
    }
    actuator_group;

typedef enum{
		ACTUATOR_STATE_IDLE,
		ACTUATOR_STATE_MOVE_CW,                               
		ACTUATOR_STATE_MOVE_CCW
                }
                actuator_state_t;
static actuator_state_t actuator_state_A, actuator_state_B;
static actuator_A_cnt = 0, actuator_B_cnt = 0; 
/* limit moving time of linear actuator.
 * this value * the value of "DETECT_LS_POLLING_PERIOD" = working period of Linear Actuator 20151107 
 */

/*Timer handle declare for detect the LS state*/
xTimerHandle detect_LS_Timers;

#define DETECT_LS_POLLING_PERIOD  50//unit : ms

 
static void init_LS(void)
{
        GPIO_InitTypeDef GPIO_InitStruct;
        /* Enable GPIO D clock. */
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

        GPIO_InitStruct.GPIO_Pin =  LS_A_UPPER_PIN | LS_A_LOWER_PIN | LS_B_UPPER_PIN | LS_B_LOWER_PIN; 

        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
        GPIO_Init( LS_READ_PORT, &GPIO_InitStruct ); 
}


/* initialize the control pin(CP) for the linear actuator */
static void init_CP(){
		GPIO_InitTypeDef GPIO_InitStruct;
		/* Enable GPIO E clock. */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		GPIO_InitStruct.GPIO_Pin =  ACTU_A_IN1_PIN| ACTU_A_IN2_PIN | ACTU_B_IN3_PIN | ACTU_B_IN4_PIN ;
	
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;            // Alt Function - Push Pull
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init( ACTU_CONTROL_PORT, &GPIO_InitStruct ); 
		
		GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_A_IN1_PIN,Bit_RESET);
		GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_A_IN2_PIN,Bit_RESET);
		GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_B_IN3_PIN,Bit_RESET);
		GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_B_IN4_PIN,Bit_RESET);	
}

void init_linear_actuator(){
		init_CP();
		init_LS();

        detect_LS_Timers=xTimerCreate("Detect limit Switch state Polling",( DETECT_LS_POLLING_PERIOD), pdTRUE, ( void * ) 3,  detect_LS_Polling);
		xTimerStart( detect_LS_Timers, 0 );
}

void set_linearActuator_A_cmd(int flag){
     if(flag==LINEAR_ACTU_STOP){
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_A_IN1_PIN,Bit_RESET);/* 0 */
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_A_IN2_PIN,Bit_RESET);/* 0 */
         actuator_state_A = ACTUATOR_STATE_IDLE;
     }/* STOP */
     else if(flag==LINEAR_ACTU_CW && get_Linear_Actuator_A_LS_State()!=0x01 && get_Linear_Actuator_A_LS_State()!=0x03){ /*if not upper limited.*/
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_A_IN1_PIN,Bit_SET);/* 1 */
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_A_IN2_PIN,Bit_RESET);/* 0 */
         actuator_state_A = ACTUATOR_STATE_MOVE_CW;
     }/* FORWARD */
     else if(flag==LINEAR_ACTU_CCW && get_Linear_Actuator_A_LS_State()!=0x02 && get_Linear_Actuator_A_LS_State()!=0x03){ /*if not lower limited*/
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_A_IN1_PIN,Bit_RESET);/* 0 */
	 GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_A_IN2_PIN,Bit_SET);/* 1 */
         actuator_state_A = ACTUATOR_STATE_MOVE_CCW;
     }/* BACKWARD */
}

void set_linearActuator_B_cmd(int flag){
     if(flag==LINEAR_ACTU_STOP){
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_B_IN3_PIN,Bit_RESET);/* 0 */
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_B_IN4_PIN,Bit_RESET);/* 0 */
         actuator_state_B = ACTUATOR_STATE_IDLE;
     }/* STOP */
     else if(flag==LINEAR_ACTU_CW && get_Linear_Actuator_B_LS_State()!=0x01 && get_Linear_Actuator_B_LS_State()!=0x03){ /*if not upper limited.*/
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_B_IN3_PIN,Bit_SET);/* 1 */
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_B_IN4_PIN,Bit_RESET);/* 0 */
         actuator_state_B = ACTUATOR_STATE_MOVE_CW;
     }/* FORWARD */
     else if(flag==LINEAR_ACTU_CCW && get_Linear_Actuator_B_LS_State()!=0x02 && get_Linear_Actuator_B_LS_State()!=0x03){ /*if not lower limited*/
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_B_IN3_PIN,Bit_RESET);/* 0 */
         GPIO_WriteBit(ACTU_CONTROL_PORT,ACTU_B_IN4_PIN,Bit_SET);/* 1 */
         actuator_state_B = ACTUATOR_STATE_MOVE_CCW;
     }/* BACKWARD */
}

static void detect_LS_Polling(){    
    /*detect actuator A LS*/
    switch(actuator_state_A){
        case ACTUATOR_STATE_IDLE:
           break;//wait the next state into.
        case ACTUATOR_STATE_MOVE_CW:
           actuator_A_cnt ++;
           if(get_Linear_Actuator_A_LS_State()==0x01 || get_Linear_Actuator_A_LS_State()==0x03){
               set_linearActuator_A_cmd(LINEAR_ACTU_STOP);
               actuator_state_A=ACTUATOR_STATE_IDLE;
           }
           if(actuator_A_cnt >= 20){
               set_linearActuator_A_cmd(LINEAR_ACTU_STOP);
               actuator_state_A=ACTUATOR_STATE_IDLE;
               actuator_A_cnt = 0;
           }
           break;
        case ACTUATOR_STATE_MOVE_CCW:
            actuator_A_cnt ++;
            if(get_Linear_Actuator_A_LS_State()==0x02 || get_Linear_Actuator_A_LS_State()==0x03){
               set_linearActuator_A_cmd(LINEAR_ACTU_STOP);
               actuator_state_A=ACTUATOR_STATE_IDLE;
            }
            if(actuator_A_cnt >= 20){
               set_linearActuator_A_cmd(LINEAR_ACTU_STOP);
               actuator_state_A=ACTUATOR_STATE_IDLE;
               actuator_A_cnt = 0;
            }
           break;
        default:
           break;
    }
    
    /*detect actuator B LS*/
    switch(actuator_state_B){
       case ACTUATOR_STATE_IDLE:
           break;//wait the next command into.
       case ACTUATOR_STATE_MOVE_CW:
           actuator_B_cnt ++;
           if(get_Linear_Actuator_B_LS_State()==0x01 || get_Linear_Actuator_B_LS_State()==0x03){
               set_linearActuator_B_cmd(LINEAR_ACTU_STOP);
               actuator_state_B=ACTUATOR_STATE_IDLE;
           }
           if(actuator_B_cnt >= 20){
               set_linearActuator_B_cmd(LINEAR_ACTU_STOP);
               actuator_state_B=ACTUATOR_STATE_IDLE;
               actuator_B_cnt = 0;
           }
           break;
       case ACTUATOR_STATE_MOVE_CCW:
           actuator_B_cnt ++;
           if(get_Linear_Actuator_B_LS_State()==0x02 || get_Linear_Actuator_B_LS_State()==0x03){
               set_linearActuator_B_cmd(LINEAR_ACTU_STOP);
               actuator_state_B=ACTUATOR_STATE_IDLE;
           }
           if(actuator_B_cnt >= 20){
               set_linearActuator_B_cmd(LINEAR_ACTU_STOP);
               actuator_state_B=ACTUATOR_STATE_IDLE;
               actuator_B_cnt = 0;
           }
           break;
       default: 
           break;
   }
}

int get_Linear_Actuator_A_LS_State(void){
    static int actuator_A_LS_state;
    actuator_A_LS_state  = GPIO_ReadInputDataBit(LS_READ_PORT, LS_A_UPPER_PIN)? 0x01 : 0x00; /*upper limited*/
    actuator_A_LS_state |= GPIO_ReadInputDataBit(LS_READ_PORT, LS_A_LOWER_PIN)? 0x02 : 0x00; /*lower limited*/
    /*
     * 00 :  normal range.
     * 01 :  upper limited
     * 02 :  lower limited
     * 03 :  both limited, but isn't impossible in actually.
     */ 
    return actuator_A_LS_state;
}
int get_Linear_Actuator_B_LS_State(void){
    static int actuator_B_LS_state;
    actuator_B_LS_state  = GPIO_ReadInputDataBit(LS_READ_PORT, LS_B_UPPER_PIN)? 0x01 : 0x00; /*upper limited*/
    actuator_B_LS_state |= GPIO_ReadInputDataBit(LS_READ_PORT, LS_B_LOWER_PIN)? 0x02 : 0x00; /*lower limited*/
    /*
     * 00 :  normal range.
     * 01 :  upper limited
     * 02 :  lower limited
     * 03 :  both limited, but isn't impossible in actually.
     */ 
    return actuator_B_LS_state;
}


