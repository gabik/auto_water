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
 *
 * Connections:
 * 
 * reset	1---8	+5V
 * soil		2---7	pot
 * level	3---6	pump
 * gnd		4---5	led
 *
 Some more connections:
 Soil - input: +5V, GND, output to PB3
 Level -  gnd --L-- +5V
              \||/      Need capacitor
 */ 

#define F_CPU 1000000
#define PIN_POT 1
#define PIN_SOIL 3
#define PIN_LEV PINB4

#define POUT_PUMP PB1
#define POUT_LED PB0

#define NO_WATER_WAIT 5000
#define LOOP_WAIT 1000

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

void setup_adc()
{
	    // AREF = AVcc with Cap on Aref
	    ADMUX = (1<<REFS0);
	    // ADC Enable and prescaler of 16 (1M/16=62.5K)
	    ADCSRA = (1<<ADEN)|(1<<ADPS2);
}

uint16_t adc_read(uint8_t ch)
{
	ch &= 0b00000011;
	ADMUX = (ADMUX & 0xFC)|ch; // clears the bottom 2 bits before ORing
	// start single convertion
	ADCSRA |= (1<<ADSC);
	// wait for conversion to complete, ADSC becomes ’0? again
	while(ADCSRA & (1<<ADSC));
	return (ADC);
}

uint16_t get_pot_val() { return adc_read(PIN_POT); }
uint16_t get_soil_val() { return adc_read(PIN_SOIL); }

bool get_level() 
{ 
	// If HIGH, we have no water
	if (PINB & (1<<PIN_LEV)) 
		return true; 
	else
		return false; 
}

int main(void)
{
	// Start ADC settings
	setup_adc();
	// LED and PUMP as GPIO output and set to off
	DDRB |= (1<<POUT_LED) | (1<<POUT_PUMP);
	PORTB &= ~(1<<POUT_LED);
	PORTB &= ~(1<<POUT_PUMP);
	// Level as input (start at HIGH)
	DDRB &= ~(1<<PIN_LEV);
	PORTB |= (1<<PIN_LEV);
	
    while(1)
    {
		// Check if we have water on bucket
		if (get_level())
		{
			// If yes: Close LED
			PORTB &= ~(1<<POUT_LED);
			// Check if need to water soil and on/off pump
			if (get_pot_val() > get_soil_val()) PORTB |= (1<<POUT_PUMP);
			else PORTB &= ~(1<<POUT_PUMP);
			// wait 1 sec
			_delay_ms(LOOP_WAIT);
		}
		else
		{
			// LED on
			PORTB |= (1<<POUT_LED);
			// Wait N seconds;
			_delay_ms(NO_WATER_WAIT);
		}
    }
}