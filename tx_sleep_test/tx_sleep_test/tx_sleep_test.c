/*
 * tx_sleep_test.c
 *
 * Created: 15/08/2015 20:04:11
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define LED_DDR DDRD
#define LED_PORT PORTD
#define LED_PIN PD4
#define LED_DELAY 200

void setup()
{
	// Interrupt pin
	DDRD &= ~(1<<PD2); // INT0
	PORTD |= (1<<PD2); // Set HIGH	
	EICRA |= (1<<ISC01); // Wake on LOW
	
	// Sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);	
	
	// LED
	LED_DDR |= (1<<LED_PIN);
	LED_PORT &= ~(1<<LED_PIN);
	
	sei();
}

void power_down()
{
	// NRF to sleep
	EIMSK |= (1<<INT0); // INT0 enable
	LED_PORT &= ~(1<<LED_PIN);
	EIFR |= (1<<INTF0); // clear flag	
	sei();
	sleep_mode();
}

ISR(INT0_vect) // wake up on click (INT0)
{
	EIMSK &= ~(1<<INT0); // INT0 disable
	LED_PORT |= (1<<LED_PIN);
	_delay_ms(LED_DELAY);
	LED_PORT &= ~(1<<LED_PIN);
	_delay_ms(LED_DELAY);
	power_down();
}

int main(void)
{
	setup();
	power_down();
    while(1);
	return(0);
}