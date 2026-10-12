
#include "stm32f4xx.h"
#include "ssd_timer.h"
#include "ssd.h"

void SSD_Timer_Init(void)
{
    // Enable TIM5 clock.
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    // Stop timer while configuring.
    TIM5->CR1 = 0;
    TIM5->PSC = 1599;  // Prescalar | Divide clock by 1600 (16,000,000/1,600 = 10,000 Hz)
    TIM5->ARR = 99;    // Period | Count 100 ticks (10,000/100 = 100)
    // RECALL THAT TO CONVERT Hz to Seconds, you divide 1 by it (or like the inverse)
    // So in this case 1/100 Seconds is 10 Milliseconds (The right amount for this project) 
    TIM5->CNT = 0;
    TIM5->EGR = TIM_EGR_UG;
    TIM5->SR = 0;
    TIM5->DIER |= TIM_DIER_UIE;

    NVIC_SetPriority(TIM5_IRQn, 1);
    NVIC_EnableIRQ(TIM5_IRQn);

    TIM5->CR1 |= TIM_CR1_CEN;
}

void TIM5_IRQHandler(void)
{
    if (TIM5->SR & TIM_SR_UIF)
    {
        TIM5->SR &= ~TIM_SR_UIF;

        SSD_Refresh();
    }
}