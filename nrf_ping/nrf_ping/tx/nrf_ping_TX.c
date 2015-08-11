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
#include "../led_screen/led_screen.h"
#include <avr/io.h>
#include <util/delay.h>

uint8_t clck_count = 0;

void setup_tx()
{
	clck_count = 0;
	init_button();
	nrf_init();
	_delay_ms(10);
	nrf_config(1);
	DDRC |= (1<<PC0);
	PORTC &= ~(1<<PC0);
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
}


void loop_tx()
{
	if (check_button_click())
		send_counter(clck_count++);
}

int main_tx(void) {setup_tx(); while(1) loop_tx(); }