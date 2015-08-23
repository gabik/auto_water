/*
 * nrf_operations.h
 *
 * declare NRF operations
 *
 * Created: 31/07/2015 15:09:06
 *  Author: Gabi Kazav
 */ 

#ifndef NRF_OPERATIONS_H_
#define NRF_OPERATIONS_H_

#include <avr/io.h>
#include "nrf.h"
#include "nrf_const.h"

#define FROM_ID_BYTE 0
#define TO_ID_BYTE 1
#define SEQ_BYTE 2
#define DATA_BYTE 3
#define DATA_PAYLOAD (nrf_PAYLOAD - DATA_BYTE - 1)
#define XOR_BYTE DATA_BYTE+DATA_PAYLOAD

#define LED_DDR DDRC
#define LED_PORT PORTC
#define LED_PIN PC0
#define LED_DELAY_SEND 50

#define ACK_WAIT (100-LED_DELAY_SEND)
#define ACKS_N 10

void build_nrf_payload(uint8_t from, uint8_t to, uint8_t seq, uint8_t * data, uint8_t * raw);
void nrf_send_ack(uint8_t from, uint8_t to, uint8_t seq);
uint8_t nrf_send(uint8_t from, uint8_t to, uint8_t * data);
uint8_t nrf_get(uint8_t my_id, uint8_t * data);
uint8_t nrf_get_ack(uint8_t my_id, uint8_t * data, uint8_t * seq);
uint8_t build_xor(uint8_t * data);
uint8_t nrf_is_working();

#endif /* NRF_OPERATIONS_H_ */
