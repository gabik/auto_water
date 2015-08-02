/*
 * misc.c
 *
 * Created: 01/08/2015 16:30:17
 *  Author: Gabi Kazav
 */ 

#include "../misc/misc.h"
#include <avr/io.h>

#define TST_BTN_DDR	 DDRB
#define TST_BTN_PORT PORTB
#define TST_BTN_PIN  PINB
#define TST_BTN_LEG  PINB6

uint8_t btn_last, btn_prssed, leg, pin;

void init_button()
{
	TST_BTN_DDR &= ~(1<<TST_BTN_LEG);
	TST_BTN_PORT |= (1<<TST_BTN_LEG);
	btn_prssed=0;
	btn_last=0;
}

uint8_t check_button_click()
// Check if PIN is GND (pulled up Input)
{
	if (btn_last) // last pressed
	{
		if (TST_BTN_PIN & (1<<TST_BTN_LEG)) // now not pressed
		{
			btn_last = 0;
			btn_prssed = 0;
		}
	} else { // not pressed
		if (TST_BTN_PIN & (1<<TST_BTN_LEG)) // now also not
		{
			btn_prssed = 0;
			btn_last = 0;
		} else { // now pressed
			btn_prssed++;
			if (btn_prssed > MIN_COUNT_PRESS)
			{
				btn_last = 1;
				return 1;
			}
		}
	}
	return 0;
}