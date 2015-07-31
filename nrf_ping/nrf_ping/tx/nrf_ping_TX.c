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
#include "nrf_ping_TX.h"
#include <avr/io.h>
#include <util/delay.h>

void setup_tx()
{
	BTN_DDR &= ~(1<<BTN_PIN); // set BTN as input
	BTN_PORT |= (1<<BTN_PIN); // pull up
	nrf_init();
	_delay_ms(10);
	nrf_config();
}

void loop_tx(uint8_t * clck_count)
{
	// uint8_t data[DATA_PAYLOAD]; // Declare the data buffer
	while (!check_button_click(BTN_PPIN, BTN_PIN)) {} // Loop till btn clicked
	send_counter(clck_count);
	_delay_ms(500);
}

int main_tx(void) { uint8_t clck_count = 0; setup_tx(); while(1) loop_tx(&clck_count); }