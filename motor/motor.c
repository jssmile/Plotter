#include <FreeRTOS.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>
#include <motor.h>

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
        //step1 
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        Delay(PHASE_Delay);
        //step2
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        Delay(PHASE_Delay);
        //step3
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        Delay(PHASE_Delay);
        //step4
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        Delay(PHASE_Delay);
}

void X_counterclockwise()
{
        int t2 =2;
        //step1 X
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        Delay(t2);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        Delay(t2);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        Delay(t2);
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        Delay(t2);

        //step2 X
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        Delay(t2);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        Delay(t2);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        Delay(t2);
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        Delay(t2);

        //step3 X
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        Delay(t2);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        Delay(t2);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        Delay(t2);
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        Delay(t2);

        //step4 X
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        Delay(t2);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        Delay(t2);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        Delay(t2);
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        Delay(t2);
}

void Y_clockwise()
{
    //step1
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        //step2
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        //step3
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        //step4
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
}

void Y_counterclockwise()
{
        int t1 = 2;
        //step1 Y
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(t1);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(t1);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(t1);
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(t1);
        
        //step2 Y
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(t1);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(t1);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(t1);
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(t1);
        
        //step3 Y
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(t1);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(t1);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(t1);
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(t1);      

        //step4 Y
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(t1);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(t1);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(t1);
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(t1);
}

/* This function can control different x-y distance!!! */
void right_up(int x, int y)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_5 | GPIO_Pin_6 | \
                   GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | \
                   GPIO_Pin_13 | GPIO_Pin_14);
        int over = 0;

        if(x>y){
        for(int i=0; i<x; ++i){
        X_counterclockwise();
        over = over + y;
        if (over>=x){
        over = over - x;
        Y_counterclockwise();                
            }
        Delay(2);
        }
    }
    else{
        for(int i=0; i<y; ++i){
        Y_counterclockwise();
        over = over + x;
        if (over>=x) {
        over = over - y;
        X_counterclockwise();                
        }
        Delay(2);
    }
    }
}

void left_up(int x, int y)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_5 | GPIO_Pin_6 | \
                   GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | \
                   GPIO_Pin_13 | GPIO_Pin_14);
    int over = 0;

        if(x>y){
        for(int i=0; i<x; ++i){
        X_counterclockwise();
        over = over + y;
        if (over>=x){
        over = over - x;
        Y_clockwise();                
            }
        Delay(2);
        }
    }
    else{
        for(int i=0; i<y; ++i){
        Y_clockwise();
        over = over + x;
        if (over>=x) {
        over = over - y;
        X_counterclockwise();                
        }
        Delay(2);
    }
    }
}

void right_down(int x, int y)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_5 | GPIO_Pin_6 | \
                   GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | \
                   GPIO_Pin_13 | GPIO_Pin_14);
    int over = 0;

        if(x>y){
        for(int i=0; i<x; ++i){
        X_clockwise();
        over = over + y;
        if (over>=x){
        over = over - x;
        Y_counterclockwise();                
            }
        Delay(2);
        }
    }
    else{
        for(int i=0; i<y; ++i){
        Y_counterclockwise();
        over = over + x;
        if (over>=x) {
        over = over - y;
        X_clockwise();                
        }
        Delay(2);
    }
    }
}

void left_down(int x, int y)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_5 | GPIO_Pin_6 | \
                   GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | \
                   GPIO_Pin_13 | GPIO_Pin_14);
    int over = 0;

        if(x>y){
        for(int i=0; i<x; ++i){
        X_clockwise();
        over = over + y;
        if (over>=x){
        over = over - x;
        Y_clockwise();                
            }
        Delay(2);
        }
    }
    else{
        for(int i=0; i<y; ++i){
        Y_clockwise();
        over = over + x;
        if (over>=x) {
        over = over - y;
        X_clockwise();                
        }
        Delay(2);
    }
    }
}

void pad_init(){
    left_down(65,65);
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
    right_down(16,32);
    pen_down();
    left_up(8,8);
    left_down(8,8);
    right_down(8,8);
    right_up(8,8);
    pen_up();
    left_up(5,2);
    pen_down();
    right_up(5,0);
    pen_up();
    left_down(3,3);
    pen_down();
    pen_up();
    left_up(3,3);
    pen_down();
    left_down(10,5);
    right_up(10,0);
    pen_up();
    pad_init();
    right_down(16,32);
    pen_down();
    right_up(32,0);
    pen_up();
    left_up(32,0);
    pen_down();
    right_up(16,20);
    right_down(0,16);
    right_up(8,10);
    right_down(0,10);
    right_up(8,10);
    right_down(0,14);
    pen_up();
    pad_init();
    right_down(16,32);
    pen_down();
    right_down(16,20);
    right_up(0,16);
    right_down(8,10);
    right_up(0,10);
    right_down(8,10);
    right_up(0,14);
    right_down(10,10);
    left_up(5,5);
    right_up(5,5);
    left_up(5,0);
    right_up(0,8);
    left_down(10,10);
    pen_up();
}

void test()
{
    right_up(32,32);
    Delay(500);
    pen_down();
    right_up(32,32);
    left_down(32,32);
    right_down(32,32);
    left_up(32,32);
    left_down(32,32);
    right_up(32,32);

}