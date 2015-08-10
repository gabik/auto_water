#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#include <avr/io.h>

//CE and CSN port definitions
#define NRF24L01_DDR DDRB
#define NRF24L01_PORT PORTB
#define NRF24L01_CE PB0
#define NRF24L01_CSN PB1

//define the spi path
#define NRF24L01_SPIPATH "../spi/spi.h" //spi lib path

//CE and CSN functions
#define nrf24l01_CSNhi NRF24L01_PORT |= (1<<NRF24L01_CSN);
#define nrf24l01_CSNlo NRF24L01_PORT &= ~(1<<NRF24L01_CSN);
#define nrf24l01_CEhi NRF24L01_PORT |=  (1<<NRF24L01_CE);
#define nrf24l01_CElo NRF24L01_PORT &= ~(1<<NRF24L01_CE);

//power setup
#define NRF24L01_RF24_PA_MIN 1
#define NRF24L01_RF24_PA_LOW 2
#define NRF24L01_RF24_PA_HIGH 3
#define NRF24L01_RF24_PA_MAX 4
#define NRF24L01_RF24_PA NRF24L01_RF24_PA_MAX

//speed setup
#define NRF24L01_RF24_SPEED_250KBPS 1
#define NRF24L01_RF24_SPEED_1MBPS 2
#define NRF24L01_RF24_SPEED_2MBPS 3
#define NRF24L01_RF24_SPEED NRF24L01_RF24_SPEED_1MBPS

//crc setup
#define NRF24L01_RF24_CRC_DISABLED 1
#define NRF24L01_RF24_CRC_8 2
#define NRF24L01_RF24_CRC_16 3
#define NRF24L01_RF24_CRC NRF24L01_RF24_CRC_DISABLED

//transmission channel
#define NRF24L01_CH 2

//payload lenght
#define NRF24L01_PAYLOAD 4

//auto ack enabled
#define NRF24L01_ACK 0

//auto retransmit delay and count
#define NRF24L01_RETR 0

//enable / disable pipe
#define NRF24L01_ENABLEDP0 1 //pipe 0

//address size
#define NRF24L01_ADDRSIZE 5

//pipe address
#define NRF24L01_ADDRP0 {0xf0, 0xf0, 0xf0, 0xf0, 0xf0} //pipe 0, 5 byte address
#define NRF24L01_ADDRTX {0xf1, 0xf1, 0xf1, 0xf1, 0xf1} //tx default address

extern void nrf24l01_init(void);
extern uint8_t nrf24l01_getstatus();
extern uint8_t nrf24l01_readready();
extern void nrf24l01_read(uint8_t *data);
extern uint8_t nrf24l01_write(uint8_t *data);
extern void nrf24l01_setrxaddr(uint8_t channel, uint8_t *addr);
extern void nrf24l01_settxaddr(uint8_t *addr);

#endif
