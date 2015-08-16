/*
 * nrf_ping_TX.c
 *
 * Transmitter module (on Click send counter++)
 *
 * Created: 31/07/2015 10:53:54
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L // For Delay.h

// #include "../misc/misc.h"
// #include "../led_screen/led_screen.h"
#include "../nrf/nrf_operations.h"
#include "nrf_ping_TX.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define LED_DELAY_OK 100
#define LED_DELAY_ERROR (LED_DELAY_OK*3)

uint8_t clck_count = 0;

uint8_t send_counter(uint8_t clck_count)
{
    uint8_t data[DATA_PAYLOAD];
    for (uint8_t i=0; i<DATA_PAYLOAD ; i++) data[i]=0;
    data[0] = clck_count;
    if (nrf_send(MY_ID, 1, data)) return 1;
    return 0;
}

void setup_tx()
{
	clck_count = 0;
	// init_button();
	nrf_init();
	_delay_ms(10);
	nrf_config(1);
	DDRC |= (1<<PC0);
	PORTC &= ~(1<<PC0);
    /*
	led_screen_init();
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
    nrf_powerdown();
    EIMSK |= (1<<INT0); // INT0 enable
    LED_PORT &= ~(1<<LED_PIN);
    EIFR |= (1<<INTF0); // clear flag
    sei();
    sleep_mode();
}

ISR(INT0_vect) // wake up on click (INT0)
{
    EIMSK &= ~(1<<INT0); // INT0 disable
    if (send_counter(clck_count++)) // If success
    { // blink twice
        LED_PORT |= (1<<LED_PIN);
        _delay_ms(LED_DELAY_OK);
        LED_PORT &= ~(1<<LED_PIN);
        _delay_ms(LED_DELAY_OK);
        LED_PORT |= (1<<LED_PIN);
        _delay_ms(LED_DELAY_OK);
        LED_PORT &= ~(1<<LED_PIN);
    } else { // if failed long blink
        LED_PORT |= (1<<LED_PIN);
        _delay_ms(LED_DELAY_ERROR);
        LED_PORT &= ~(1<<LED_PIN);
    }

    power_down();
}

int main_tx(void) {setup_tx(); power_down(); while(1);  return(0);}
