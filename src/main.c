/****************************************************************
* Author: Carlos Munar (MlgEpicCar)
* Project 3 - Stopwatch, 10/1/26
*
* This program activates when a directional button is pressed.
* if Up is pressed then the SSD will count upwards until 99.99
* if Down is pressed then the SSD will count downwards until 0.00
* if Left is pressed then the SSD will flip instantly to 0.00
* if Right is pressed then the SSD will flip instantly to 99.99
****************************************************************/

#include "stm32f4xx.h"
#include <stdbool.h>

/* State Definitions*/
#define DECREASE -1
#define PAUSE     0
#define INCREASE  1

/* Button Definitions */
#define BUTTON_PORT_F_LCU GPIOF // Port for Left, Center, & Up Buttons
#define BUTTON_PORT_E_RD GPIOE // Port for Right & Down Buttons

#define UP_PIN     7
#define DOWN_PIN   5
#define LEFT_PIN   9
#define RIGHT_PIN  6
#define CENTER_PIN 8

volatile int state = PAUSE;

uint16_t read_pot(void) {
    /* Start conversion */
    ADC1->CR2 |= ADC_CR2_SWSTART;

    /* Wait until conversion is complete */
    while (!(ADC1->SR & ADC_SR_EOC));

    /* Return 12-bit ADC result */
    return ADC1->DR;
}

int main(void)
{
    /* Enable clock for GPIOC, GPIOD, GPIOE, and GPIOF */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN |
                     RCC_AHB1ENR_GPIODEN|
                     RCC_AHB1ENR_GPIOEEN|
                     RCC_AHB1ENR_GPIOFEN;

    /* Configure Buttons */
    BUTTON_PORT_F_LCU->MODER &= ~((3U << (LEFT_PIN * 2))|
                                (3U << (CENTER_PIN * 2))|
                                   (3U << (UP_PIN * 2)));  
    BUTTON_PORT_E_RD->MODER &= ~((3U << (RIGHT_PIN * 2))|
                                 (3U << (DOWN_PIN * 2)));
    
    while (1) {

    // PAUSED STATE
        while (state == PAUSE)
        {
            // State Switcher
            if (!(GPIOF->IDR & (1U << UP_PIN)))
                state = INCREASE;
            else if (!(GPIOE->IDR & (1U << DOWN_PIN)))
                state = DECREASE;
        }

    // INCREASING STATE
        while (state == INCREASE) {

            // State Switcher
            if (!(GPIOF->IDR & (1U << CENTER_PIN))) {
                state = PAUSE;
            }
            if (!(GPIOE->IDR & (1U << DOWN_PIN)))
                state = DECREASE;
        }

    // DECREASING STATE
        while (state == DECREASE) {
            // State Switcher
            if (!(GPIOF->IDR & (1U << CENTER_PIN))) {
                state = PAUSE;
            }
            if (!(GPIOF->IDR & (1U << UP_PIN)))
                state = INCREASE;
        }
    }
}
