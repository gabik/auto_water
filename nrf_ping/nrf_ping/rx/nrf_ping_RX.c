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
#include <util/delay.h>

uint16_t count;
uint8_t data[DATA_PAYLOAD]; // Declare the data buffer

void setup_rx()
{
 	nrf_init();
 	_delay_ms(10);
 	nrf_config(0);
	init_button();
	led_screen_init();
	count = 0;
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

void loop_rx()
{
	while (!nrf_get(MY_ID, data)) {write_to_led(count, 0);}
	if (data[0]) count = data[0];
}

int main_rx(void) { setup_rx(); while(1) loop_rx(); }
