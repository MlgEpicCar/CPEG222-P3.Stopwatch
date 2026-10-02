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
#include "ssd.h"
#include "ssd_timer.h"
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
volatile uint32_t milliseconds = 0;
uint32_t last_update = 0;

void SysTick_Handler(void)
{
    milliseconds++;
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
    
    SSD_Init();
    int current_number = 0;
    SysTick_Config(SystemCoreClock / 1000);
    SSD_Timer_Init();

    while (1) {

    // PAUSED STATE
        while (state == PAUSE)
        {
            if ((uint32_t)(milliseconds - last_update) >= 10) {
                last_update += 10;
                SSD_DisplayValue(current_number);
            }
            
            // State Switcher
            if (!(GPIOF->IDR & (1U << UP_PIN)))
                state = INCREASE;
            else if (!(GPIOE->IDR & (1U << DOWN_PIN)))
                state = DECREASE;
        }

    // INCREASING STATE
        while (state == INCREASE) {
            if ((uint32_t)(milliseconds - last_update) >= 10) {
                last_update += 10;

                if (current_number < 9999) {
                    current_number++;
                }

                SSD_DisplayValue(current_number);
            }

            // State Switcher
            if (!(GPIOF->IDR & (1U << CENTER_PIN))) {
                state = PAUSE;
            }
            if (!(GPIOE->IDR & (1U << DOWN_PIN)))
                state = DECREASE;
        }

    // DECREASING STATE
        while (state == DECREASE) {
            if ((uint32_t)(milliseconds - last_update) >= 10) {
                last_update += 10;

                if (current_number > 0) {
                    current_number--;
                }

                SSD_DisplayValue(current_number);
            }

            // State Switcher
            if (!(GPIOF->IDR & (1U << CENTER_PIN))) {
                state = PAUSE;
            }
            if (!(GPIOF->IDR & (1U << UP_PIN)))
                state = INCREASE;
        }
    }
}
