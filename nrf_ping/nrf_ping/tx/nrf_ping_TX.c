/*
 * nrf_ping_TX.c
 *
 * Transmitter module (on Click send counter++)
 *
 * Created: 31/07/2015 10:53:54
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L // For Delay.h

#include "../nrf/nrf_operations.h"
#include "tx.h"
#include "../misc/misc.h"
#include "nrf_ping_TX.h"
#include <avr/io.h>
#include <util/delay.h>

uint8_t clck_count;

void setup_tx()
{
	clck_count = 0;
	init_button();
	nrf_init();
	_delay_ms(10);
	nrf_config(1);
	DDRD |= (1<<PD0);
	PORTD &= ~(1<<PD0);
}

void loop_tx()
{
	// uint8_t data[DATA_PAYLOAD]; // Declare the data buffer
	if (check_button_click())
	{
		PORTD |= (1<<PD0);
		send_counter(clck_count++);
		_delay_ms(500);
		PORTD &= ~(1<<PD0);
	}
}

int main_tx(void) {setup_tx(); while(1) loop_tx(); }