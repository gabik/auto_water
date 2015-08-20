/*
 * rx_sleep_test.c
 *
 * Created: 20/08/2015 15:16:20
 *  Author: Gabi Kazav
 */ 


#define F_CPU 1000000L
	
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED_DDR DDRD
#define LED_PORT PORTD
#define LED_PIN PD4

void setup()
{
	cli();
	LED_DDR |= (1<<LED_PIN);
	LED_PORT &= ~(1<<LED_PIN);
	MCUSR = 0;
	WDTCSR = (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP0);
	sei();	
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
}

ISR(WDT_vect)
{
	LED_PORT |= (1<<LED_PIN);
	_delay_us(100);
	LED_PORT &= ~(1<<LED_PIN);
	sei();
	sleep_cpu();
}

int main(void)
{
	setup();
	sleep_cpu();
}