/*
 * nrf_const.h
 *
 * NRF24l01 constsnts
 *
 * Created: 31/07/2015 10:55:34
 *  Author: Gabi Kazav
 */ 


#ifndef NRF_CONST_H_
#define NRF_CONST_H_


/* Register Memory Table (9.1 - page 54 */
#define CONFIG      0x00
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0

#define EN_AA       0x01
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0

#define EN_RXADDR   0x02
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0

#define SETUP_AW    0x03
#define AW          0 // 2 bit (0:1)

#define SETUP_RETR  0x04
#define ARD         4 // 4 bit (4:7)
#define ARC         0 // 4 bit (0:3)

#define RF_CH       0x05
#define RF_CH_VAL	0 // 7 bit (0:6)

#define RF_SETUP    0x06
#define RF_DR_LOW   5
#define PLL_LOCK    4
#define RF_DR_HIGH  3  
#define RF_PWR      1 // 2 bit (1:2) - 11 = 0dBm
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2
#define LNA_HCURR   0

#define STATUS      0x07
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1 // 3 bit (1:3)
#define TX_FULL     0

#define OBSERVE_TX  0x08
#define PLOS_CNT    4
#define ARC_CNT     0

#define RPD         0x09
#define RPD_VAL		0

#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10

#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define PX_PW		0 // 6 bit (0:5)

#define FIFO_STATUS 0x17
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0

#define DYNPD 0x1C

/* Instruction Mnemonics */
#define R_REGISTER    0x00 // & 5 bit val (000A AAAA)
#define W_REGISTER    0x20 // & 5 bit val (001A AAAA)
#define REGISTER_MASK 0x1F // this is the 5 bit (5xA's) mask
#define R_RX_PAYLOAD  0x61 
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define NOP           0xFF

#endif /* NRF_CONST_H_ */