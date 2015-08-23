/*
 * nrf_ping_RX.c
 *
 * Receiver module (get data and blink its count)
 *
 * Created: 31/07/2015 10:53:54
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L // For Delay.h
#define MY_ID 1 // Reciever #1

#include "../nrf/nrf_operations.h"
#include "nrf_ping_RX.h"
#include "../led_screen/led_screen.h"
#include "../misc/misc.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h>

uint16_t count;
uint8_t data[DATA_PAYLOAD]; // Declare the data buffer

volatile uint16_t timer0_counter;
volatile uint16_t timer0_limit;

void wdt_start()
{
	MCUSR = 0;
	WDTCSR = (1<<WDCE) | (1<<WDE);
	WDTCSR = (1<<WDIE) | (1<<WDP2) | (1<<WDP0);	
	wdt_reset();
}

void setup_rx()
{
	cli();
	LED_DDR |= (1<<LED_PIN);
	LED_PORT &= ~(1<<LED_PIN);
 	nrf_init();
 	_delay_ms(10);
 	nrf_config(0);
	led_screen_init();
	count = 0;
	wdt_start();
	sei();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	goto_sleep();
    /*
	uint8_t reg_val1, val1=0, val2=0;
	uint8_t reg_val5[5];
	for (uint8_t i=0 ; i<=0x17 ; i+=2)
	{
		for (uint8_t j = i ; j<i+2 ; j++)
		{
			poweroff_led();
			if (j == 0x0A || j == 0x0B || j == 0x10)
			{
				nrf_read_register(j, reg_val5, 5) ;
				reg_val1 = reg_val5[4];
			}
			else
				nrf_read_register(j, &reg_val1, 1);
			if (j==i) val1=reg_val1;
			else val2=reg_val1;
		}
		write_to_led_hex(val1, val2, 400);		
		poweroff_led();
	}
    */
}

void goto_sleep()
{
	timer0_stop();
	nrf_powerdown();
	poweroff_led();
	ADCSRA = 0;
	wdt_start();
	sei();
	sleep_cpu();	
}

ISR(TIMER0_COMPA_vect)
{
	timer0_counter++;
	if (timer0_counter >= timer0_limit) 
	{
		timer0_stop();
		while (nrf_is_working());
		goto_sleep();
	}
}

void timer0_init(uint16_t max)
{
	timer0_limit = max;
	timer0_counter = 0;
	TCNT0 = 0;
	TCCR0A |= (1<<WGM01); // CTC
	TCCR0B |= (1 << CS02) | (1<<CS00); // 1M / 1024
	OCR0A = 10; // (1000000 / 1024) = 976hz. we want every 10ms, so (1/976)*1000=1.02459ms, 10ms/1.02459 = 9.76. 10 will give 10.25ms, we will take it.
	TIMSK0 |= (1<<OCIE0A);
	sei();
}

void timer0_stop()
{
	TIMSK0 = 0;
	TCCR0A = 0;
	TCCR0B = 0;
}

void handle_got_data(uint8_t * data)
{
	timer0_stop();
	if (data[0]) count = data[0];
	timer0_init(200); //2000 (2 sec) / 10 (my prescaler) = 200
	while(1) write_to_led(count, 0);
}

ISR(WDT_vect)
{
	wdt_disable();
	timer0_init(12); //120 / 10 (my prescaler) = 12 (~123ms)
	nrf_listen();
	LED_PORT |= (1<<LED_PIN);
	_delay_us(100);
	LED_PORT &= ~(1<<LED_PIN);
	while (!nrf_get(MY_ID, data)) _delay_us(100); // loop till we get data (if timer0 will fire comp0A , powerdown)
	handle_got_data(data);
}

int main_rx(void) { setup_rx(); while(1); }
