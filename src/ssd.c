
#include "stm32f4xx.h"
#include "ssd.h"

// Segment GPIO pins
#define PIN_A 9 // Port G
#define PIN_B 12 // Port F
#define PIN_C 13 // Port F
#define PIN_D 14 // Port G
#define PIN_E 8 // Port E
#define PIN_F 15 // Port F
#define PIN_G 4 // Port B
#define PIN_DecimalPoint 14 // Port F

// Digit select pins
#define PIN_DIGIT0 10 // Port E
#define PIN_DIGIT1 7 // Port E
#define PIN_DIGIT2 5 // Port B
#define PIN_DIGIT3 3 // Port B

static const uint8_t digit[11] = {
    0b1000000, // 0
    0b1111001, // 1
    0b0100100, // 2
    0b0110000, // 3
    0b0011001, // 4
    0b0010010, // 5
    0b0000010, // 6
    0b1111000, // 7
    0b0000000, // 8
    0b0010000, // 9
    0b1111111  // Completely Blank
};

void SSD_Init(void)
{
    // Enable GPIO clocks: B, E, F, G
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN|
                    RCC_AHB1ENR_GPIOEEN|
                    RCC_AHB1ENR_GPIOFEN|
                    RCC_AHB1ENR_GPIOGEN;

    // SET SHIT TO OUTPUT
    GPIOB->MODER &= ~((3U << (PIN_DIGIT2 * 2))|
                    (3U << (PIN_DIGIT3 * 2))|
                    (3U << (PIN_G * 2)));
    GPIOB->MODER |= (1U << (PIN_DIGIT2 * 2))|
                    (1U << (PIN_DIGIT3 * 2))|
                    (1U << (PIN_G * 2));

    GPIOE->MODER &= ~((3U << (PIN_DIGIT0 * 2))|
                    (3U << (PIN_DIGIT1 * 2))|
                    (3U << (PIN_E * 2)));
    GPIOE->MODER |= (1U << (PIN_DIGIT0 * 2))|
                    (1U << (PIN_DIGIT1 * 2))|
                    (1U << (PIN_E * 2));

    GPIOF->MODER &= ~((3U << (PIN_B * 2))|
                    (3U << (PIN_C * 2))|
                    (3U << (PIN_F * 2))|
                    (3U << (PIN_DecimalPoint * 2)));
    GPIOF->MODER |= (1U << (PIN_B * 2))|
                    (1U << (PIN_C * 2))|
                    (1U << (PIN_F * 2))|
                    (1U << (PIN_DecimalPoint * 2));

    GPIOG->MODER &= ~((3U << (PIN_A * 2))|
                    (3U << (PIN_D * 2)));
    GPIOG->MODER |= (1U << (PIN_A * 2))|
                    (1U << (PIN_D * 2));
}

void SSD_DisplayValue(uint16_t value)
{
    GPIOG->BSRR = (1U << (PIN_A));
}