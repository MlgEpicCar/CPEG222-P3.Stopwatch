/****************************************************************
* Author: Carlos Munar (MlgEpicCar)
* Project 3 - Stopwatch, 10/1/26
*
* 
****************************************************************/

#include "stm32f4xx.h"
#include <stdbool.h>

#define VPOT_PIN 2

/* State Definitions*/
#define SHIFT_LEFT  -1
#define PAUSE        0
#define SHIFT_RIGHT  1

/* Button Definitions */
#define LEFT_PIN 9 // GPIOF
#define RIGHT_PIN 6 // NOTE: Right uses GPIOE (for some fucking reason)
#define CENTER_PIN 8 // also GPIOF
    
/* Switch Definitions */
#define S1 8
#define S2 9
#define S3 10
#define S4 11
#define SWITCH_PORT GPIOC

/* Unlike the NUCLEO-F446ZE onboard LEDs, the CPEG222 Shield LEDs are PD0-7 (making it easy)*/
#define LED_PORT GPIOD

volatile int state = PAUSE;
uint8_t led_pattern = 0;

uint16_t read_pot(void) {
    /* Start conversion */
    ADC1->CR2 |= ADC_CR2_SWSTART;

    /* Wait until conversion is complete */
    while (!(ADC1->SR & ADC_SR_EOC));

    /* Return 12-bit ADC result */
    return ADC1->DR;
}

void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        for (volatile uint32_t i = 0; i < 16000; i++)
        {
            if (!(GPIOF->IDR & (1U << LEFT_PIN)))
                state = SHIFT_LEFT;
            if (!(GPIOF->IDR & (1U << CENTER_PIN))) {
                led_pattern = 0;
                state = PAUSE;
            }
            if (!(GPIOE->IDR & (1U << RIGHT_PIN)))
                state = SHIFT_RIGHT;
        }
    }
}

int main(void)
{
    /* Enable clock for GPIOC, GPIOD, GPIOE, and GPIOF */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN |
                     RCC_AHB1ENR_GPIODEN|
                     RCC_AHB1ENR_GPIOEEN|
                     RCC_AHB1ENR_GPIOFEN;

    /* Clear PD0-7 so we can set them to general purpose output */
    LED_PORT->MODER &= ~((3U << (0 * 2)) |
                        (3U << (1 * 2)) |
                        (3U << (2 * 2)) |
                        (3U << (3 * 2)) |
                        (3U << (4 * 2)) |
                        (3U << (5 * 2)) |
                        (3U << (6 * 2)) |
                        (3U << (7 * 2)));
    /* Set PD0-7 as general purpose output (01) */
    LED_PORT->MODER |= (1U << (0 * 2))|
                    (1U << (1 * 2))|
                    (1U << (2 * 2))|
                    (1U << (3 * 2))|
                    (1U << (4 * 2))|
                    (1U << (5 * 2))|
                    (1U << (6 * 2))|
                    (1U << (7 * 2));

    /* Ensure all LEDs start off */
    LED_PORT->BSRR = (1U << (0 + 16))|
                  (1U << (1 + 16))|
                  (1U << (2 + 16))|
                  (1U << (3 + 16))|
                  (1U << (4 + 16))|
                  (1U << (5 + 16))|
                  (1U << (6 + 16))|
                  (1U << (7 + 16));

    /* Configure Buttons */
    GPIOF->MODER &= ~((3U << (LEFT_PIN * 2))|
                     (3U << (CENTER_PIN * 2)));  
    GPIOE->MODER &= ~(3U << (RIGHT_PIN * 2)); // RIGHT uses it's own GPIO port 

    /* Configure Switches */
    SWITCH_PORT->MODER &= ~((3U << (S1 * 2))|
                     (3U << (S2 * 2))|
                     (3U << (S3 * 2))|
                     (3U << (S4 * 2))); 
    
    /* Configure PC2 as analog input */
    GPIOC->MODER |= (3U << (VPOT_PIN * 2));

    /* Analog to Digital Converter Shenanigans */
    /* Enable ADC1 clock */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    /* ADC channel 12 */
    ADC1->SQR3 = 12;
    /* One conversion */
    ADC1->SQR1 = 0;
    /* Enable ADC */
    ADC1->CR2 |= ADC_CR2_ADON;

    //volatile int state = PAUSE;
    //uint8_t led_pattern = 0;
    
    while (1) {

        while (state == PAUSE)
        {
            // Turn on/off LEDs when switches are #switched
            if (SWITCH_PORT->IDR & (1U << S1))
                led_pattern |= (1U << 0);
            else
                led_pattern &= ~(1U << 0);

            if (SWITCH_PORT->IDR & (1U << S2))
                led_pattern |= (1U << 1);
            else
                led_pattern &= ~(1U << 1);

            if (SWITCH_PORT->IDR & (1U << S3))
                led_pattern |= (1U << 2);
            else
                led_pattern &= ~(1U << 2);

            if (SWITCH_PORT->IDR & (1U << S4))
                led_pattern |= (1U << 3);
            else
                led_pattern &= ~(1U << 3);

            // Display the pattern
            LED_PORT->ODR = led_pattern;

            // Trigger Left/Right Movement when a directional button is pressed 
            if (!(GPIOF->IDR & (1U << LEFT_PIN)))
                state = SHIFT_LEFT;
            else if (!(GPIOE->IDR & (1U << RIGHT_PIN)))
                state = SHIFT_RIGHT;
        }

        while (state == SHIFT_LEFT) {
            if (!(GPIOF->IDR & (1U << CENTER_PIN))) {
                led_pattern = 0;
                state = PAUSE;
            }
            if (!(GPIOE->IDR & (1U << RIGHT_PIN)))
                state = SHIFT_RIGHT;

            // memorize leftmost bit and place it on the right
            uint8_t left_bit = led_pattern & 0x80;
            led_pattern <<= 1;
            if (left_bit)
            {
                led_pattern |= 0x01;
            }
            LED_PORT->ODR = led_pattern;

            // speed limit
            uint16_t pot_val = read_pot(); // 0–4095
            uint32_t delay = 1 + (pot_val * 149) / 4095; // maps to ~1–150 ms
            delay_ms(delay);
        }

        while (state == SHIFT_RIGHT) {
            if (!(GPIOF->IDR & (1U << CENTER_PIN))) {
                led_pattern = 0;
                state = PAUSE;
            }
            if (!(GPIOF->IDR & (1U << LEFT_PIN)))
                state = SHIFT_LEFT;

            // memorize rightmost bit and place it on the left
            uint8_t right_bit = led_pattern & 0x01;
            led_pattern >>= 1;
            if (right_bit)
            {
                led_pattern |= 0x80;
            }
            LED_PORT->ODR = led_pattern;
            
            // speed limit
            uint16_t pot_val = read_pot(); // 0–4095
            uint32_t delay = 1 + (pot_val * 149) / 4095; // maps to ~1–150 ms
            delay_ms(delay);
        }
    }
}
