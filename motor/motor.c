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
        //step1 X
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        Delay(PHASE_Delay);

        //step2 X
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        Delay(PHASE_Delay);

        //step3 X
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        Delay(PHASE_Delay);

        //step4 X
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        Delay(PHASE_Delay);
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
        //step1 Y
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        
        //step2 Y
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
        
        //step3 Y
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);      

        //step4 Y
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        Delay(PHASE_Delay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        Delay(PHASE_Delay);
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        Delay(PHASE_Delay);
}

/* This function can control different x-y distance!!! */
void right_up(int y,int x)
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
    }
    }
}

void left_up(int y,int x)
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
    }
    }
}

void right_down(int y,int x)
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
    }
    }
}

void left_down(int y,int x)
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
    }
    }
}

void Moveto(int y,int x)
{
    if(y>=0)
    {
        if(x>=0)
        {
            right_up(y,x);
        }
        else
        {
            right_down(y,-x);
        }
    }
    else
    {
        if(x>=0)
        {
            left_up(-y,x);
        }
        else
        {
            left_down(-y,-x);
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
    Moveto(0,55);
    pen_down();
    Moveto(,);

    
}


//Star
/*
    Moveto(1,43);
    pen_down();
    Moveto(3,6);
    Moveto(2,2);
    Moveto(2,2);
    Moveto(3,3);
    Moveto(3,3);
    Moveto(4,2);
    Moveto(4,2);
    Moveto(5,1);
    Moveto(5,1);
    Moveto(4,0);
    Moveto(2,-1);
    Moveto(6,-2);
    Moveto(3,-2);
    Moveto(5,-2);
    Moveto(4,-3);
    Moveto(3,-4);
    Moveto(2,-4);
    Moveto(3,-3);
    Moveto(0,-1);
    Moveto(1,-5);
    Moveto(0,-4);
    Moveto(0,-8);
    Moveto(-2,-4);
    Moveto(-1,-5);
    Moveto(-3,-4);
    Moveto(-3,-3);
    Moveto(-5,-5);
    Moveto(-4,-2);
    Moveto(-3,-1);
    Moveto(-3,-1);
    Moveto(-7,-1);
    Moveto(-6,0);
    Moveto(-3,1);
    Moveto(-5,2);
    Moveto(-3,1);
    Moveto(-2,1);
    Moveto(-2,1);
    Moveto(-4,4);
    Moveto(-2,2);
    Moveto(-3,4);
    Moveto(-2,4);
    Moveto(-2,4);
    Moveto(0,4);
    Moveto(0,4);
    Moveto(0,2);
    Moveto(0,5);
    Moveto(2,4);
    Moveto(23,0);
    Moveto(8,21);
    Moveto(7,-21);
    Moveto(23,0);
    Moveto(-18,-14);
    Moveto(6,-21);
    Moveto(-18,11);
    Moveto(-19,-12);
    Moveto(5,21);
    Moveto(-17,14);
    pen_up();
*/


//NARUTO
    /*
Moveto(65,65);
    pen_down();
    Moveto(-3,-3);
    Moveto(-3,-2);
    Moveto(-2,-4);
    Moveto(-4,-1);
    Moveto(-5,4);
    Moveto(-3,1);
    Moveto(-4,1);
    Moveto(-2,1);
    Moveto(-5,0);
    Moveto(-5,-1);
    Moveto(-3,0);
    Moveto(-5,-3);
    Moveto(-3,-2);
    Moveto(-6,-8);
    Moveto(0,-5);
    Moveto(-2,-8);
    Moveto(-2,-6);
    Moveto(2,-9);
    Moveto(1,-2);
    Moveto(2,-5);
    Moveto(4,-5);
    Moveto(3,-3);
    Moveto(4,-2);
    Moveto(4,-1);
    Moveto(3,-2);
    Moveto(3,0);
    Moveto(3,0);
    Moveto(4,2);
    Moveto(3,1);
    Moveto(2,2);
    Moveto(2,3);
    Moveto(2,3);
    Moveto(2,5);
    Moveto(1,1);
    Moveto(0,5);
    Moveto(0,5);
    Moveto(0,6);
    Moveto(-1,4);
    Moveto(-2,4);
    Moveto(-1,3);
    Moveto(-3,1);
    Moveto(-5,3);
    Moveto(-5,1);
    Moveto(-5,-1);
    Moveto(-2,-1);
    Moveto(-2,-2);
    Moveto(-4,-4);
    Moveto(-1,-5);
    Moveto(0,-4);
    Moveto(0,-6);
    Moveto(1,-4);
    Moveto(3,-1);
    Moveto(2,-4);
    Moveto(2,0);
    Moveto(5,0);
    Moveto(5,3);
    Moveto(2,4);
    Moveto(0,4);
    Moveto(0,4);
    Moveto(-1,2);
    Moveto(-1,1);
    pen_up();
    Moveto(-32,-2);
    pen_down();
    Moveto(-7,-25);
    Moveto(23,0);

    */