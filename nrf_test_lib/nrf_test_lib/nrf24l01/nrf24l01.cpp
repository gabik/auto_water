#define F_CPU 1000000L

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24l01.h"
#include "nrf24l01registers.h"
#include NRF24L01_SPIPATH

static uint8_t nrf24l01_addr0[NRF24L01_ADDRSIZE] = NRF24L01_ADDRP0;
static uint8_t nrf24l01_addrtx[NRF24L01_ADDRSIZE] = NRF24L01_ADDRTX;


/*
 * read one register
 */
uint8_t nrf24l01_readregister(uint8_t reg) {
	nrf24l01_CSNlo; //low CSN
	spi_writereadbyte(NRF24L01_CMD_R_REGISTER | (NRF24L01_CMD_REGISTER_MASK & reg));
    uint8_t result = spi_writereadbyte(NRF24L01_CMD_NOP); //read write register
    nrf24l01_CSNhi; //high CSN
    return result;
}

/*
 * read many registers
 */
void nrf24l01_readregisters(uint8_t reg, uint8_t *value, uint8_t len) {
	uint8_t i = 0;
	nrf24l01_CSNlo; //low CSN
	spi_writereadbyte(NRF24L01_CMD_R_REGISTER | (NRF24L01_CMD_REGISTER_MASK & reg));
	for(i=0; i<len; i++)
		value[i] = spi_writereadbyte(NRF24L01_CMD_NOP); //read write register
	nrf24l01_CSNhi; //high CSN
}

/*
 * write one register
 */
void nrf24l01_writeregister(uint8_t reg, uint8_t value) {
	nrf24l01_CSNlo; //low CSN
	spi_writereadbyte(NRF24L01_CMD_W_REGISTER | (NRF24L01_CMD_REGISTER_MASK & reg));
	spi_writereadbyte(value); //write register
	nrf24l01_CSNhi; //high CSN
}

/*
 * write many registers
 */
void nrf24l01_writeregisters(uint8_t reg, uint8_t *value, uint8_t len) {
	uint8_t i = 0;
	nrf24l01_CSNlo; //low CSN
    spi_writereadbyte(NRF24L01_CMD_W_REGISTER | (NRF24L01_CMD_REGISTER_MASK & reg));
	for(i=0; i<len; i++)
		 spi_writereadbyte(value[i]); //write register
	nrf24l01_CSNhi; //high CSN
}

/*
 * set rx address
 */
void nrf24l01_setrxaddr(uint8_t pipe, uint8_t *addr) {
    	nrf24l01_writeregisters(NRF24L01_REG_RX_ADDR_P0, addr, NRF24L01_ADDRSIZE);
}

/*
 * set tx address
 */
void nrf24l01_settxaddr(uint8_t *addr) {
	nrf24l01_writeregisters(NRF24L01_REG_TX_ADDR, addr, NRF24L01_ADDRSIZE); //set tx address
}

/*
 * flush RX fifo
 */
void nrf24l01_flushRXfifo() {
	nrf24l01_CSNlo; //low CSN
	spi_writereadbyte(NRF24L01_CMD_FLUSH_RX);
	nrf24l01_CSNhi; //high CSN
}

/*
 * flush RX fifo
 */
void nrf24l01_flushTXfifo() {
	nrf24l01_CSNlo; //low CSN
	spi_writereadbyte(NRF24L01_CMD_FLUSH_TX);
	nrf24l01_CSNhi; //high CSN
}

/*
 * set chip as RX
 */
void nrf24l01_setRX() {
	nrf24l01_setrxaddr(0, nrf24l01_addr0); //restore pipe 0 address
	nrf24l01_writeregister(NRF24L01_REG_CONFIG, nrf24l01_readregister(NRF24L01_REG_CONFIG) | (1<<NRF24L01_REG_PRIM_RX)); //prx mode
	nrf24l01_writeregister(NRF24L01_REG_CONFIG, nrf24l01_readregister(NRF24L01_REG_CONFIG) | (1<<NRF24L01_REG_PWR_UP)); //power up
	nrf24l01_writeregister(NRF24L01_REG_STATUS, (1<<NRF24L01_REG_RX_DR) | (1<<NRF24L01_REG_TX_DS) | (1<<NRF24L01_REG_MAX_RT)); //reset status
	nrf24l01_flushRXfifo(); //flush rx fifo
	nrf24l01_flushTXfifo(); //flush tx fifo
	nrf24l01_CEhi; //start listening
	_delay_us(150); //wait for the radio to power up
}

/*
 * set chip as TX
 */
void nrf24l01_setTX() {
	nrf24l01_CElo; //stop listening
	nrf24l01_writeregister(NRF24L01_REG_CONFIG, nrf24l01_readregister(NRF24L01_REG_CONFIG) & ~(1<<NRF24L01_REG_PRIM_RX)); //ptx mode
	nrf24l01_writeregister(NRF24L01_REG_CONFIG, nrf24l01_readregister(NRF24L01_REG_CONFIG) | (1<<NRF24L01_REG_PWR_UP)); //power up
	nrf24l01_writeregister(NRF24L01_REG_STATUS, (1<<NRF24L01_REG_RX_DR) | (1<<NRF24L01_REG_TX_DS) | (1<<NRF24L01_REG_MAX_RT)); //reset status
	nrf24l01_flushTXfifo(); //flush tx fifo
	_delay_us(150); //wait for the radio to power up
}

/*
 * get status register
 */
uint8_t nrf24l01_getstatus() {
	uint8_t status = 0;
	nrf24l01_CSNlo; //low CSN
	status = spi_writereadbyte(NRF24L01_CMD_NOP); //get status, send NOP request
	nrf24l01_CSNhi; //high CSN
	return status;
}

/*
 * check if there is data ready
 */
uint8_t nrf24l01_readready(uint8_t* pipe) {
    uint8_t status = nrf24l01_getstatus();
    uint8_t ret = status & (1<<NRF24L01_REG_RX_DR);
    if(ret) {
		//get the pipe number
		if(pipe)
			*pipe = (status >> NRF24L01_REG_RX_P_NO) & 0b111;
    }
    return ret;
}

/*
 * get data
 */
void nrf24l01_read(uint8_t *data) {
	uint8_t i = 0;
	//read rx register
	nrf24l01_CSNlo; //low CSN
    spi_writereadbyte(NRF24L01_CMD_R_RX_PAYLOAD);
    for(i=0; i<NRF24L01_PAYLOAD; i++)
    	data[i] = spi_writereadbyte(NRF24L01_CMD_NOP);
    nrf24l01_CSNhi; //high CSN
    //reset register
    nrf24l01_writeregister(NRF24L01_REG_STATUS, (1<<NRF24L01_REG_RX_DR));
    //handle ack payload receipt
	if (nrf24l01_getstatus() & (1<<NRF24L01_REG_TX_DS))
		nrf24l01_writeregister(NRF24L01_REG_STATUS, (1<<NRF24L01_REG_TX_DS));
}

/*
 * put data
 */
uint8_t nrf24l01_write(uint8_t *data) {
	uint8_t i = 0;

	//set tx mode
	nrf24l01_setTX();

	//write data
	nrf24l01_CSNlo; //low CSN
	spi_writereadbyte(NRF24L01_CMD_W_TX_PAYLOAD);
	for (i=0; i<NRF24L01_PAYLOAD; i++)
		spi_writereadbyte(data[i]);
	nrf24l01_CSNhi; //high CSN

	//start transmission
	nrf24l01_CEhi; //high CE
	_delay_us(15);
	nrf24l01_CElo; //low CE

	//reset PLOS_CNT
	nrf24l01_writeregister(NRF24L01_REG_RF_CH, NRF24L01_CH);

	//power down
	nrf24l01_writeregister(NRF24L01_REG_CONFIG, nrf24l01_readregister(NRF24L01_REG_CONFIG) & ~(1<<NRF24L01_REG_PWR_UP));

	//set rx mode
	nrf24l01_setRX();

	return 1;
}

/*
 * set power level
 */
void nrf24l01_setpalevel() {
  uint8_t setup = nrf24l01_readregister(NRF24L01_REG_RF_SETUP);
  setup &= ~((1<<NRF24L01_REG_RF_PWR_LOW) | (1<<NRF24L01_REG_RF_PWR_HIGH));

  if (NRF24L01_RF24_PA == NRF24L01_RF24_PA_MAX) {
	  setup |= (1<<NRF24L01_REG_RF_PWR_LOW) | (1<<NRF24L01_REG_RF_PWR_HIGH);
  } else if (NRF24L01_RF24_PA == NRF24L01_RF24_PA_HIGH) {
	  setup |= (1<<NRF24L01_REG_RF_PWR_HIGH) ;
  } else if (NRF24L01_RF24_PA == NRF24L01_RF24_PA_LOW) {
	  setup |= (1<<NRF24L01_REG_RF_PWR_LOW);
  } else if (NRF24L01_RF24_PA == NRF24L01_RF24_PA_MIN) {
  } else {
	  //default is max power
	  setup |= (1<<NRF24L01_REG_RF_PWR_LOW) | (1<<NRF24L01_REG_RF_PWR_HIGH);
  }

  nrf24l01_writeregister(NRF24L01_REG_RF_SETUP, setup);
}

/*
 * set datarate
 */
void nrf24l01_setdatarate() {
  uint8_t setup = nrf24l01_readregister(NRF24L01_REG_RF_SETUP) ;

  setup &= ~((1<<NRF24L01_REG_RF_DR_LOW) | (1<<NRF24L01_REG_RF_DR_HIGH));
  if(NRF24L01_RF24_SPEED == NRF24L01_RF24_SPEED_250KBPS) {
    setup |= (1<<NRF24L01_REG_RF_DR_LOW);
  } else {
    if (NRF24L01_RF24_SPEED == NRF24L01_RF24_SPEED_2MBPS) {
    	setup |= (1<<NRF24L01_REG_RF_DR_HIGH);
    } else if (NRF24L01_RF24_SPEED == NRF24L01_RF24_SPEED_2MBPS) {
    } else {
    	//default is 1Mbps
    }
  }

  nrf24l01_writeregister(NRF24L01_REG_RF_SETUP, setup);
}

/*
 * set crc length
 */
void nrf24l01_setcrclength() {
  uint8_t config = nrf24l01_readregister(NRF24L01_REG_CONFIG) & ~((1<<NRF24L01_REG_CRCO) | (1<<NRF24L01_REG_EN_CRC));

  if (NRF24L01_RF24_CRC == NRF24L01_RF24_CRC_DISABLED) {
	  //nothing
  } else if (NRF24L01_RF24_CRC == NRF24L01_RF24_CRC_8) {
	  config |= (1<<NRF24L01_REG_EN_CRC);
  } else if (NRF24L01_RF24_CRC == NRF24L01_RF24_CRC_16) {
	  config |= (1<<NRF24L01_REG_EN_CRC);
	  config |= (1<<NRF24L01_REG_CRCO);
  } else {
	  //default is disabled
  }

  nrf24l01_writeregister(NRF24L01_REG_CONFIG, config);
}

void nrf24l01_init(void) {
	NRF24L01_DDR |= (1<<NRF24L01_CSN); //output
	NRF24L01_DDR |= (1<<NRF24L01_CE); //output
	spi_init(); //init spi
	nrf24l01_CElo; //low CE
	nrf24l01_CSNhi; //high CSN
	_delay_ms(5); //wait for the radio to init
	nrf24l01_setpalevel(); //set power level
	nrf24l01_setdatarate(); //set data rate
	nrf24l01_setcrclength(); //set crc length
	nrf24l01_writeregister(NRF24L01_REG_SETUP_RETR, NRF24L01_RETR); // set retries
	nrf24l01_writeregister(NRF24L01_REG_DYNPD, 0); //disable dynamic payloads
	nrf24l01_writeregister(NRF24L01_REG_RF_CH, NRF24L01_CH); //set RF channel
	nrf24l01_writeregister(NRF24L01_REG_RX_PW_P0, NRF24L01_PAYLOAD);
	nrf24l01_writeregister(NRF24L01_REG_EN_RXADDR, (1<<NRF24L01_REG_ERX_P0));
	nrf24l01_writeregister(NRF24L01_REG_EN_AA, 0);
	nrf24l01_setrxaddr(0, nrf24l01_addr0);
	nrf24l01_settxaddr(nrf24l01_addrtx);
	nrf24l01_setRX();
}
