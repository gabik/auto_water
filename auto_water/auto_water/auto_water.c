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

 To reduce power:
 1) Turn off everything
 2) Turn ADC off
 3) All pins to input [DDR as 0] Pull up [PORT as 1]
 4) Disable BOD [Check for Vcc level]
 5) Work at 3.3V
 6) Work at 1Mhz [or even 128Khz]
 7) prescaler for the click by 8 [128Khz] - if 1Mhz clock.
 8) PRR : TIMER1 shutdown, USI shutdown

 Vcc = 3.3V
 Watchdog enabled
 1Mhz clock
 power down
 should take 4uA
 Power up should take 1mA

 FUSES:
 BOD disable
 1MHz clock (Or 128Khz)
 x8 prescaler [128Khz] (x1 if 128Khz clock)
 */ 

#define F_CPU 128000
#define PIN_POT 1
#define PIN_SOIL 3
#define PIN_LEV PINB4

#define POUT_PUMP PB1
#define POUT_LED PB0

#define WATER_PUMP_SECONDS 10

#define ADC_READ_COUNTS 10

#define WATCHDOG_LOOPS 12

#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

uint8_t watchdog_counter;

void delay_seconds(int delay){
	// Clock = 128kHz , prescaler = 1024 => 128Hz
	TCCR0A = (1<<WGM01); // CTC mode
	TCCR0B = (1<<CS00)|(1<<CS02); // Prescaler 1024
	TCNT0 = 0; // init counter
	OCR0A = f_CPU/1024-1; // CTC top to get 1 second.
	while (delay-- > 0) while (!(TIFR & 1<<OCF0A));
}

void setup_adc()
{
	    // AREF = AVcc
	    //ADMUX = 0;
	    // ADC Enable and prescaler of 2 [62.5K] - the rest is 0.
	    ADCSRA = (1<<ADEN);
	    // REF is Vcc
	    ADMUX &= ~((1<<REFS0) | (1<<REFS1));
}

uint16_t adc_read(uint8_t ch)
{
	ch &= 0b00000011;
	ADMUX = (ADMUX & 0xFC)|ch; // clears the bottom 2 bits and set the channel got as arg
	/*
	   MUX[3:0]
		0000 ADC0 (PB5)
		0001 ADC1 (PB2)
		0010 ADC2 (PB4)
		0011 ADC3 (PB3)
	 */
	// start single conversion
	ADCSRA |= (1<<ADSC);
	// wait for conversion to complete, ADSC becomes 0 again
	while(ADCSRA & (1<<ADSC));
	return (ADC);
}

uint16_t get_adc_val(uint8_t adc_id)
{ 
	uint16_t total_adc = 0;
	for (uint8_t i=0 ; i<ADC_READ_COUNTS; i++) { total_adc += adc_read(adc_id); }
	return (uint16_t)(total_adc / ADC_READ_COUNTS);
}

bool get_level() 
{ 
	// If HIGH, we have no water
	if (PINB & (1<<PIN_LEV)) 
		return false; 
	else
		return true; 
}

void sleep_me(uint8_t sec)
{
	ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA

	// Set LED and PUMP as input to save power
	DDRB &= ~(1<<POUT_LED);
	DDRB &= ~(1<<POUT_PUMP);
	PORTB |= (1<<POUT_LED);
	PORTB |= (1<<POUT_PUMP);

	// PRR - close timers , ADC and USI
	PRR |= 0b00001111;

	sei();
	sleep_mode();
}

void setup()
{
	cli();
	// Low-power 128Khz clock / 256 = 500hz - I am not sure it works, I think it is for the 8Mhz oscillator.
	//CLKPR=(1<<CLKPCE);
	//CLKPR=4;   // CLKPS[3:0] = 1000

    // Disable and clear all Watchdog settings. Nice to get a clean slate when dealing with interrupts 
    WDTCR = (1<<WDCE)|(1<<WDE);
    WDTCR = 0;
	// Setup watchdog for sleeping.
	// Counter to 0,
	// register WDTCR bits WDP[3:0] to 1001 => 128Khz/1024K = 1/0.125 [8 sec]
	// Disable watchdog and enable interrupts
	watchdog_counter = 0;
	WDTCR |= (1<<WDP3) | (1<<WDP0);
	WDTCR &= ~(1<<WDP2);
	WDTCR &= ~(1<<WDP1);
	WDTCR |= (1<<WDIE);

	// Set sleep mode to POWER DOWN
	MCUCR |= (1 << SM1);
	// Enable sleeps (No need, sleep_mode oed SE, sleep, -SE)
	//MCUCR |= (1 << SE);
	//MCUCR &= ~(1 << SE);

	// Level as input (start at HIGH)
	DDRB &= ~(1<<PIN_LEV);
	PORTB |= (1<<PIN_LEV);
}

// watchdog interrupt
ISR(WDT_vect) {
  watchdog_counter++;
  // WD disable the interupts flag each time, we need to enable it.
  WDTCR |= (1<<WDIE);
}

int main(void)
{
	setup();
    while(1)
    {
    	// Wait 1 minute [WATCHDOG_LOOPS * 8sec sleep]
    	if (watchdog_counter >= WATCHDOG_LOOPS)
    	{
			// Check if we have water on bucket
			if (get_level())
			{
                // Led off
				PORTB &= ~(1<<POUT_LED);
				setup_adc(); //Enable ADC and output ports once I am up
				// Check if need to water soil and on/off pump
				if (get_adc_val(PIN_POT) > get_adc_val(PIN_SOIL))
				{
					// PUMP as GPIO output and set to on
					DDRB |= (1<<POUT_PUMP);
					PORTB |= (1<<POUT_PUMP);
				} else {
                    // Pump off
                    PORTB &= ~(1<<POUT_PUMP);
                }
			}
			else
			{
				// No water - LED for 10 seconds
				// LED as GPIO output and set to off
				DDRB |= (1<<POUT_LED);
				// LED on
				PORTB |= (1<<POUT_LED);
			}
    	}
    	sleep_me();
    }
}
