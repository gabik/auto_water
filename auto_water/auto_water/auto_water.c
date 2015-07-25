/*
 * auto_water.c
 *
 * Created: 25/07/2015 21:53:26
 *  Author: Gabi Kazav.
 *
 * Implementing auto self-watering system on attiny85
 * Inputs:
 *  - ADC - soil detector
 *  - ADC - rain detector
 *  - ADC - 10K pot (for calibrating)
 *
 * Outputs:
 *  - LED
 *  - Water pump
 *  - Piezzo
 *
 */ 

#include <avr/io.h>


int main(void)
{
	setup_adc();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}