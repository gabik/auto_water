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

#define DATA_PAYLOAD (nrf_PAYLOAD - 2)

void nrf_send(uint8_t id, uint8_t * data)
void nrf_get(uint8_t my_id, uint8_t * data)

#endif /* NRF_OPERATIONS_H_ */