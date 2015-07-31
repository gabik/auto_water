/*
 * tx.h
 *
 * Created: 31/07/2015 16:11:53
 *  Author: Gabi Kazav
 */ 


#ifndef TX_H_
#define TX_H_

#define BTN_PORT	PORTB
#define BTN_PPIN	PINB
#define BTN_DDR		DDRB
#define BTN_PIN		PB2

#define MIN_COUNT_PRESS 10
#define MY_ID 0 // TX module

uint8_t check_button_click(uint8_t port, uint8_t pin);
uint8_t send_counter(uint8_t * clck_count);

#endif /* TX_H_ */