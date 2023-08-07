#include "delay.h"

void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void delay_us(uint32_t us)
{
    uint32_t i = 0;
    while (us--)
    {
        i = 10;
        while (i--)
            ;
    }
}