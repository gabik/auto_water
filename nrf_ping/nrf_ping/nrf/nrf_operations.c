/*
 * nrf_operations.c
 *
 * mplement specific NRF operartions
 *
 * Created: 31/07/2015 14:31:52
 *  Author: Gabi Kazav
 */ 

#define F_CPU 1000000L
#define ACK_WAIT 100
#define ACKS_N 10

#include "nrf.h"
#include "nrf_const.h"
#include "nrf_operations.h"
#include <avr/io.h>
#include <util/delay.h>
#include "../led_screen/led_screen.h"

uint8_t tx_seq = 0;
uint8_t MY_ACK[DATA_PAYLOAD] = {0b10000000, 0b10000001};
uint8_t tmp_data[DATA_PAYLOAD];

void nrf_send_ack(uint8_t from, uint8_t to, uint8_t seq)
{
	uint8_t raw[nrf_PAYLOAD];
    build_nrf_payload(from, to, cur_seq, MY_ACK, raw);
}

uint8_t build_xor(uint8_t * data)
{
    uint8_t xor = 0;
    for (uint8_t i = 0 ; i<nrf_PAYLOAD-1 ; i++) xor ^= data[i];
    return xor
}

void build_nrf_payload(uint8_t from, uint8_t to, uint8_t seq, uint8_t * data, uint8_t * raw)
{
    raw[FROM_ID_BYTE] = from;
	raw[TO_ID_BYTE] = to;
    raw[SEQ_BYTE] = cur_seq;
	for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) raw[i+DATA_BYTE] = data[i];
    raw[XOR_BYTE] = build_xor(raw);
}

uint8_t nrf_send(uint8_t from, uint8_t to, uint8_t * data)
{
    if (tx_seq >= 200) tx_seq = 0;
    uint8_t cur_seq = tx_seq++;

	// build raw packet
	uint8_t raw[nrf_PAYLOAD];
    build_nrf_payload(from, to, cur_seq, data, raw);
	
    uint8_t ack_n = 0;
    while (ack_n++ < ACKS_N)
    {
        nrf_send_raw(raw);
        _delay_ms(ACK_WAIT);
        uint8_t is_ack = 0;
        if (nrf_get(from, tmp_data))
        {
            is_ack = 0;
            for (uint8_t i=0 ; i<DATA_PAYLOAD ; i++) if (tmp_data[i] != MY_ACK[i]) is_ack = 0;
            ack_seq = tmp_data[SEQ_BYTE];
            if ((is_ack) && (cur_seq == ack_seq)) return(1);
        }
    }
    return (0);
}

uint8_t nrf_get(uint8_t my_id, uint8_t * data)
{
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
                nrf_send_ack(my_id, raw[FROM_ID_BYTE], raw[SEQ_BYTE]);
			}
		}
	}
	return valid;
}
