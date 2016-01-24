#include <FreeRTOS.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <motor.h>
#include <stdio.h>
#include <string.h>

int x[167];
int y[167];

volatile uint32_t msTicks;      //counts 1ms timeTicks
void SysTick_Handler(void) {
    msTicks++;
}
//  Delays number of Systicks (happens every 1 ms)
static void Delay(__IO uint32_t dlyTicks){                                              
  uint32_t curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

void setSysTick(void){
    // ---------- SysTick timer (1ms) -------- //
    if (SysTick_Config(SystemCoreClock / 1000)) {
        // Capture error
        while (1){};
    }
}

void gpio_init_motor(){
    // AHB clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_13 | GPIO_Pin_14 |  GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void config_PWM(void) {

    // Structures for configuration
    GPIO_InitTypeDef            GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_OCInitTypeDef           TIM_OCInitStructure;
    
    // TIM4 Clock Enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // GPIOB Clock Enable
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    // Initalize PB6 (TIM4 Ch1) and PB7 (TIM4 Ch2)
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;    // GPIO_High_Speed
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;         // Weak Pull-up for safety during startup
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Assign Alternate Functions to pins
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
    
    uint16_t PrescalerValue = (uint16_t) 84;

    // Time Base Configuration
    TIM_TimeBaseStructure.TIM_Period        = 19999;
    TIM_TimeBaseStructure.TIM_Prescaler     = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // Common TIM Settings
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0;                        // Initial duty cycle
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    
    // Channel 1
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
  
    // Channel 2
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    
    // Start timer
    TIM_Cmd(TIM4, ENABLE);
}

/*
 * The sequence of control signals for 4 control wires is as follows:
 *
 * Step C0 C1 C2 C3
 *    1  1  0  1  0
 *    2  0  1  1  0
 *    3  0  1  0  1
 *    4  1  0  0  1
*/

void X_clockwise()
{
        //step1 X
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        //step2 X
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        //step3 X
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        //step4 X
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
}

void X_counterclockwise()
{
        //step1 X
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        
        //step2 X
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        
        //step3 X
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);      

        //step4 X
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
}

void Moveto(int x)
{
    if(x>=0)
    {
        for(int i=0;i<x;++i)
        {
        X_counterclockwise();
        }
    }
    else
    {
        x=-x;
        for(int i=0;i<x;++i)
        {
        X_clockwise();
        }
    }
        
}

void pad_init(){
    Moveto(-65);
    Delay(500);
}

void pen_up(){
    TIM4->CCR1 = 1700;       // 600 == 0.6 ms  -> 0'   
    Delay(500);
}

void pen_down(){
    TIM4->CCR1 = 900;      // 1500 == 1.5 ms -> 90'
    Delay(500);
}

void draw()
{
    //Leave the gcode below
}



void read_and_draw()
{
  char lineSize[10]; 
  char *token;
  int count=0;

  FILE *fp; 
  fp=fopen("./input.txt","r"); 
  
  if(fp!=NULL){        
        while(fgets(lineSize,10,fp))
        { 
             printf("hole line=%s",lineSize);

             token = strtok(lineSize," "); 
             printf("getx=%s\n",token);
             int x=atoi(token);
             printf("x=%d\n",x);


             token = strtok(NULL,"\n");
             printf("gety=%s\n",token);
             int y=atoi(token);
             printf("y=%d\n",y);
             
             Moveto(x);
             if(abs(x)<=abs(y))
             {
                Delay(32*(abs(y)-abs(x)));
             }
             count++;  
             printf("count=%d\n",count);
        } 
   }
}