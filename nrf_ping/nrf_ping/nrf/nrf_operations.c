/*
 * nrf_operations.c
 *
 * mplement specific NRF operartions
 *
 * Created: 31/07/2015 14:31:52
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L

#include "nrf.h"
#include "nrf_const.h"
#include "nrf_operations.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../led_screen/led_screen.h"

uint8_t tx_seq = 0;
uint8_t MY_ACK = 0b10000001;

volatile uint8_t working = 0;

void nrf_send_ack(uint8_t from, uint8_t to, uint8_t seq)
{
	uint8_t raw[nrf_PAYLOAD];
	uint8_t data_ack[DATA_PAYLOAD];
	for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) data_ack[i] = MY_ACK;
	build_nrf_payload(from, to, seq, data_ack, raw);
	nrf_send_raw(raw);
	write_to_led_hex(raw[0], raw[1], 12);
	//for (uint8_t i =0;i<nrf_PAYLOAD;i+=2) write_to_led_hex(raw[i], raw[i+1], 1000);
}

uint8_t build_xor(uint8_t * data)
{
    uint8_t xor = 0;
    for (uint8_t i = 0 ; i<nrf_PAYLOAD-1 ; i++) xor ^= data[i];
    return xor;
}

void build_nrf_payload(uint8_t from, uint8_t to, uint8_t seq, uint8_t * data, uint8_t * raw)
{
    raw[FROM_ID_BYTE] = from;
	raw[TO_ID_BYTE] = to;
    raw[SEQ_BYTE] = seq;
	for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) raw[i+DATA_BYTE] = data[i];
    raw[XOR_BYTE] = build_xor(raw);
}

uint8_t nrf_send(uint8_t from, uint8_t to, uint8_t * data)
{
	uint8_t got_ack = 0;
	
    if (tx_seq >= 200) tx_seq = 0;
    uint8_t cur_seq = tx_seq++;

	// build raw packet
	uint8_t raw[nrf_PAYLOAD];
    build_nrf_payload(from, to, cur_seq, data, raw);
	
    uint8_t ack_n = 0;
    uint8_t is_ack;	
    while (ack_n++ < ACKS_N)
    {
        nrf_send_raw(raw);
		LED_PORT |= (1<<LED_PIN);
        _delay_ms(LED_DELAY_SEND);		
		LED_PORT &= ~(1<<LED_PIN);
		_delay_ms(ACK_WAIT);		
		uint8_t tmp_data[DATA_PAYLOAD];
		uint8_t seq_ack;
        if (nrf_get_ack(from, tmp_data, &seq_ack))
        {
            is_ack = 1;
            for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) if (tmp_data[i] != MY_ACK) is_ack = 0;
            if ((is_ack) && (cur_seq == seq_ack))
			{
				got_ack = 1;
				ack_n = ACKS_N;
			}
        }
    }
    return got_ack;
}

uint8_t nrf_get(uint8_t my_id, uint8_t * data)
{
	working = 1;
	uint8_t valid = 0;
	
	// Check if we have data 
	if (nrf_data_ready())
	{
		uint8_t raw[nrf_PAYLOAD];
		nrf_get_raw(raw);
		if (raw[TO_ID_BYTE] == my_id)
		{
			uint8_t xor = build_xor(raw);
			if (xor == raw[XOR_BYTE])
			{
				for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) data[i] = raw[i+DATA_BYTE];
				valid = 1;
				_delay_us(500);
                nrf_send_ack(my_id, raw[FROM_ID_BYTE], raw[SEQ_BYTE]);
			}
		}
	}
	working = 0;
	return valid;
}

uint8_t nrf_is_working()
{
	return working;
}

uint8_t nrf_get_ack(uint8_t my_id, uint8_t * data, uint8_t * seq)
{
	uint8_t valid = 0;
	
	if (nrf_data_ready())
	{
		uint8_t raw[nrf_PAYLOAD];
		nrf_get_raw(raw);
		if (raw[TO_ID_BYTE] == my_id)
		{
			for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) data[i] = raw[i+DATA_BYTE];
			*seq = raw[SEQ_BYTE];
			valid = 1;
		}
	}
	return valid;
}
