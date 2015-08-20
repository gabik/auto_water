/*
 * nrf_ping_RX.h
 *
 * Created: 31/07/2015 17:05:55
 *  Author: Gabi Kazav
 */ 


#ifndef NRF_PING_RX_H_
#define NRF_PING_RX_H_

#include <avr/io.h>

int main_rx(void);
void setup_rx();
void goto_sleep();
void timer0_init(uint16_t max);
void timer0_stop();
void handle_got_data(uint8_t * data);

#endif /* NRF_PING_RX_H_ */
