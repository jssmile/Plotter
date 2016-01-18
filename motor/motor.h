#define  PHASE_Delay   2


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