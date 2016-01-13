/*
 Author: Tony Abboud
 
PWM_Servo Example using the Timer (TIM) peripheral
 
 Wiring Connections:
    Servo 1:
        Vcc     -> 5V
        Gnd     -> Gnd
        Signal  -> PB6 (TIM4 Ch1)
    Servo 2:
        Vcc     -> 5V
        Gnd     -> Gnd
        Signal  -> PB7 (TIM4 Ch2)

 
 
 NOTES:
    TIM4 runs on APB1 which clocks at 42MHz. If Prescaler is set to 1, then clock remains 42MHz
        Otherwise, clock is multiplied by 2 (i.e. 84MHz operation) due to PLL, See reference manual for description. This is why we prescale by 84.
 
 RESOURCES:
    - Page 214 in Ref manual (RM0090) for clock frequency description
    - Page 578 for TIM2-TIM5 information
*/
#include <stdio.h>
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#define  PHASE_vTaskDelay   2

/*volatile uint32_t msTicks;      //counts 1ms timeTicks
void SysTick_Handler(void) {
    msTicks++;
}*/

//  vTaskDelays number of Systicks (happens every 1 ms)
/*static void vTaskDelay(__IO uint32_t dlyTicks){                                              
  uint32_t curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}*/
void H(void);

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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
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
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;    // GPIO_High_Speed
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;         // Weak Pull-up for safety during startup
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Assign Alternate Functions to pins
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
    
    /* Setup TIM / PWM values
     Servo Requirements:  (May be different for your servo)
        - 50Hz (== 20ms) PWM signal
        - 0.6 - 2.1 ms Duty Cycle
     
     1. Determine Required Timer_Freq.
            TIM_Period = (Timer_Freq. / PWM_Freq) - 1
     
            - We need a period of 20ms (or 20000µs) and our PWM_Freq = 50Hz (i.e. 1/20ms)
            - See NOTES, for why we use µs
            TIM_Period = 20000 - 1 = 19999  (since its 0 offset)
     
            Timer_Freq = (TIM_Period + 1) * PWM_Freq.
            Timer_Freq = (19999 + 1) * 50
            Timer_Freq = 1000000 = 1MHz
     
     2. Determine Pre-Scaler
        APB1 clock frequency:
            - SYS_CLK/4 when prescaler == 1 (i.e. 168MHz / 4 = 42MHz)
            - SYS_CLK/2 when prescaler != 1 (i.e. 168MHz / 2 = 84MHz)
     
        Prescaler = APB1_Freq / Timer_Freq
        Prescaler = 84 MHz / 1 MHz
        Prescaler = 84
     
        For our example, we can prescale the TIM clock by 84, which gives us a Timer_Freq of 1MHz
            Timer_Freq = 84 MHz / 84 = 1 MHz
        So the TIMx_CNT register will increase by 1 000 000 ticks every second. When TIMx_CNT is increased by 1 that is 1 µs. So if we want a duty cycle of 1.5ms (1500 µs) then we can set our CCRx register to 1500.
     
     NOTES:
        - TIMx_CNT Register is 16 bits, i.e. we can count from 0 to (2^16)-1 = 65535
        - If the period, TIMx_ARR, is greater than the max TIMx_CNT value (65535), then we need to choose a larger prescaler value in order to slow down the count.
        - We use the µs for a more precise adjustment of the duty cycle
     
     */
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
void clockwise_A(int n)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_9 | GPIO_Pin_10 | \
                   GPIO_Pin_13 | GPIO_Pin_14);
    for(int i = 0; i < n; i++) {
        //step1
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        vTaskDelay(PHASE_vTaskDelay);
        //step2
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        vTaskDelay(PHASE_vTaskDelay);
        //step3
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        vTaskDelay(PHASE_vTaskDelay);
        //step4
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        vTaskDelay(PHASE_vTaskDelay);
    }
}
void counterClockwise_A(int n)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_9 | GPIO_Pin_10 | \
                   GPIO_Pin_13 | GPIO_Pin_14);
    for(int i = 0; i < n; i++) {
         //step1
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        vTaskDelay(PHASE_vTaskDelay);
        //step2
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_13);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_14);
        vTaskDelay(PHASE_vTaskDelay);
        //step3
        GPIO_ResetBits(GPIOG, GPIO_Pin_9);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_10);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        vTaskDelay(PHASE_vTaskDelay);
        //step4
        GPIO_SetBits(GPIOG, GPIO_Pin_9);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_10);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_13);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_14);
        vTaskDelay(PHASE_vTaskDelay);        
    }
}

void clockwise_B(int n)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_5 | GPIO_Pin_6 | \
                   GPIO_Pin_7 | GPIO_Pin_8);
    for(int i = 0; i < n; i++) {
        //step1
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        vTaskDelay(PHASE_vTaskDelay);
        //step2
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        vTaskDelay(PHASE_vTaskDelay);
        //step3
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        vTaskDelay(PHASE_vTaskDelay);
        //step4
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        vTaskDelay(PHASE_vTaskDelay);
    }
}
void counterClockwise_B(int n)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_5 | GPIO_Pin_6 | \
                   GPIO_Pin_7 | GPIO_Pin_8);
    for(int i = 0; i < n; i++) {
        //step1
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        vTaskDelay(PHASE_vTaskDelay);
        //step2
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_6);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_5);
        vTaskDelay(PHASE_vTaskDelay);
        //step3
        GPIO_ResetBits(GPIOG, GPIO_Pin_8);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_7);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        vTaskDelay(PHASE_vTaskDelay);
        //step4
        GPIO_SetBits(GPIOG, GPIO_Pin_8);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_ResetBits(GPIOG, GPIO_Pin_6);
        vTaskDelay(PHASE_vTaskDelay);
        GPIO_SetBits(GPIOG, GPIO_Pin_5);
        vTaskDelay(PHASE_vTaskDelay);
        
    }
}

void pad_init(){
    clockwise_B(65);
    clockwise_A(65);
}

void pen_up(){
    TIM4->CCR1 = 1700;       // 600 == 0.6 ms  -> 0'   
    vTaskDelay(500);
}

void pen_down(){
    TIM4->CCR1 = 900;      // 1500 == 1.5 ms -> 90'
    vTaskDelay(500);
}

//Write H
void H(void){
    for(;;){
    pen_down();
    counterClockwise_A(65);
    counterClockwise_B(65);
    clockwise_A(65);
    clockwise_B(65);
    pen_up();
    }
    vTaskDelete(NULL);
}

int main(void) {
    setSysTick();
    config_PWM();
    gpio_init_motor();
    pen_up();
    pad_init();

    //H();
    /* Create a task to button check. */
    xTaskCreate(H,(signed portCHAR *) "AutoWrite", 512, NULL, tskIDLE_PRIORITY + 5, NULL );

  /* Start running the tasks. */
    vTaskStartScheduler();
    return 0;
}

/* Blank definition */
void vApplicationTickHook(void) {
}

/* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created.  It is also called by various parts of the
   demo application.  If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
void vApplicationMallocFailedHook(void) {
  taskDISABLE_INTERRUPTS();
  for(;;);
}

/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
   task.  It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()).  If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
void vApplicationIdleHook(void) {
}

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName) {
  (void) pcTaskName;
  (void) pxTask;
  /* Run time stack overflow checking is performed if
     configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
     function is called if a stack overflow is detected. */
  taskDISABLE_INTERRUPTS();
  for(;;);
}