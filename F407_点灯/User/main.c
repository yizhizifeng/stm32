#include "stm32f4xx.h"
#include "Delay.h"

int main(void)
{
    int i;

    Delay_Init(); 
	
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

    GPIOE->MODER   &= ~0x000000FFu;
    GPIOE->MODER   |=  0x00000055u;
    GPIOE->OTYPER  &= ~0x0000000Fu;
    GPIOE->BSRR = 0x0000000Fu;

    while (1)
    {
        for (i = 0; i < 4; i++)
        {
            GPIOE->BSRR = 0x0000000Fu;
            GPIOE->BSRR = 0x00010000u << i;
            Delay_ms(200);
        }
    }
}
