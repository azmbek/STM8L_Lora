#include "ebox_pwm.h"

void pwm_config(uint16_t frq,uint16_t duty)
{
    uint32_t period  = 0;
    uint32_t prescaler = 0;
    uint16_t pulse = 0;

      
    /* Enable TIM1 clock */
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
    
    //千分之一精度方案
    for(prescaler = 0; prescaler <= 0xffff; prescaler++)
    {
        period = 16000000 / (prescaler+1) / frq;
        if((0xffff >= period) && (period >= 1000))
        {
            break;
        }
    }
    
    if(prescaler == 65536)//分配失败
    {
        //执行百分之一精度方案
        for(prescaler = 1; prescaler <= 0xffff; prescaler++)
        {
            period = 16000000 / prescaler / frq;
            if((0xffff >= period) && (period >= 100))
            {
            break;
            }
        }
    }
    
    if(duty > 1000)
        duty = 1000;

    pulse = (uint16_t) (( duty * period ) / 1000);
    /*
    - TIM1CLK = 2 MHz
    - TIM1 counter clock = TIM1CLK / TIM1_PRESCALER+1 = 2 MHz/1+1 = 1 MHz
    */
    /* Time base configuration */  
    TIM1_TimeBaseInit(prescaler, TIM1_CounterMode_Up, period, 0);
    
    TIM1->ARRH = (uint8_t)(period >> 8);
    TIM1->ARRL = (uint8_t)(period);
    TIM1->PSCRH = (uint8_t)(prescaler >> 8);
    TIM1->PSCRL = (uint8_t)(prescaler);

    /*
    - The TIM1 CCR1 register value is equal to 32768: 
    - CC1 update rate = TIM1 counter clock / CCR1_Val = 30.51 Hz,
    - So the TIM1 Channel 1 generates a periodic signal with a frequency equal to 15.25 Hz.
    */
    /* Toggle Mode configuration: Channel1 */
    TIM1_OC1Init(TIM1_OCMode_Toggle, TIM1_OutputState_Enable, TIM1_OutputNState_Disable,
               pulse, TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set,
               TIM1_OCNIdleState_Set);
    TIM1_OC1PreloadConfig(ENABLE);
    TIM1_ARRPreloadConfig(ENABLE);
    /* Enable TIM1 outputs */
    TIM1_CtrlPWMOutputs(ENABLE);
    /* TIM1 enable counter */
    //TIM1_Cmd(ENABLE);
    TIM1->CR1 |= TIM1_CR1_CEN;

}