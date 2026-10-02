
#include "stm32f4xx.h"
#include "ssd_timer.h"
#include "ssd.h"

void SSD_Timer_Init(void)
{
    // Enable TIM5 clock.
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    // Stop timer while configuring.
    TIM5->CR1 = 0;
    TIM5->PSC = 8999;
    TIM5->ARR = 1; // Clock speed thing, slower = faster
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