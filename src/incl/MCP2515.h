#ifndef MCP2515_H
#define MCP2515_H

#include <stdbool.h>
#include "SPI.h"

/* MCP2515 register addresses */
#define MCP2515_CANSTAT 0x0E
#define MCP2515_CANCTRL 0x0F
#define MCP2515_CNF3 0x28
#define MCP2515_CNF2 0x29
#define MCP2515_CNF1 0x2A
#define MCP2515_CANINTE 0x2B
#define MCP2515_CANINTF 0x2C
#define MCP2515_EFLG 0x2F
#define MCP2515_RXB0CTRL 0x60

/* MCP2515 CANCSTAT register bit names */
#define MCP2515_OPMOD0 5

/* MCP2515 CANCSTAT register bit names */
#define MCP2515_REQOP0 5

/* MCP2515 EFLG register bit names */
#define MCP2515_RX1OVR 7										// RX1 overflow error flag (EFLG)
#define MCP2515_RX0OVR 6										// RX0 overflow error flag (EFLG)
#define MCP2515_TXBO 5											// bus-off state (after TEC has reached 255)

/* MCP2515 CANINTE register bit names */
#define MCP2515_MERRE 7											// MSG error interrupt enable bit (CANINTE)
#define MCP2515_WAKIE 6											// wake-up interrupt enable bit (CANINTE)
#define MCP2515_ERRIE 5											// error interrupt enable bit (CANITE)
#define MCP2515_TX2IE 4											// TXB2 empty interrupt bit (CANINTE)
#define MCP2515_TX1IE 3											// TXB1 empty interrupt bit (CANINTE)
#define MCP2515_TX0IE 2											// TXB0 empty interrupt bit (CANINTE)
#define MCP2515_RX1IE 1											// RXB1 full interrupt bit (CANINTE)
#define MCP2515_RX0IE 0											// RXB0 full interrupt bit (CANINTE)

/* MCP2515 CANINTF register bit names */
#define MCP2515_MERRF 7											// MSG error interrupt flag (CANINTF)
#define MCP2515_WAKIF 6											// wake-up interrupt flag (CANINTF)
#define MCP2515_ERRIF 5											// error interrupt flag (CANINTF)
#define MCP2515_TX2IF 4											// TXB2 empty interrupt flag (CANINTF)
#define MCP2515_TX1IF 3											// TXB1 empty interrupt flag (CANINTF)
#define MCP2515_TX0IF 2											// TXB0 empty interrupt flag (CANINTF)
#define MCP2515_RX1IF 1											// RXB1 full interrupt flag (CANINTF)
#define MCP2515_RX0IF 0											// RXB0 full interrupt flag (CANINTF)

/* MCP2515 RXB0CTRL register bit names */
#define MCP2515_BUKT 2

/* SPI instruction codes */
#define MCP2515_SPI_RESET 0xC0
#define MCP2515_SPI_READ 0x03
#define MCP2515_SPI_READ_RX_BUFFER 0x90
#define MCP2515_SPI_WRITE 0x02
#define MCP2515_SPI_LOAD_TX_BUFFER 0x40
#define MCP2515_SPI_RTS 0x80
#define MCP2515_SPI_READ_STATUS 0xA0
#define MCP2515_SPI_RX_STATUS 0xB0
#define MCP2515_SPI_BIT_MODIFY 0x05

typedef enum FILTER: uint8_t
{
	RXF0 = 0x00, RXF1 = 0x04,									// filters for RXB0
	RXF2 = 0x08, RXF3 = 0x10, RXF4 =0x14, RXF5 = 0x18			// filters for RXB1
} FILTER;

typedef enum MASK: uint8_t
{
	RXM0 = 0x20,												// mask for RXB0
	RXM1 = 0x24													// mask for RXB1
} MASK;

typedef enum RX_BUFFER: uint8_t
{
	RXB0 = 0x60, RXB1 = 0x70
} RX_BUFFER;

typedef enum TX_BUFFER: uint8_t
{
	TXB0 = 0x30, TXB1 = 0x40, TXB2 = 0x50
} TX_BUFFER;

typedef enum MODE: uint8_t
{
	NORMAL = 0x00, SLEEP = 0x01, LOOPBACK = 0x02,
	LISTEN_ONLY = 0x03, CONFIGURATION = 0x04
} MODE;

class MCP2515
{
public:
	MCP2515(SPI_Device *spi);
	void init(void);
	void send(TX_BUFFER buffer, uint32_t id, bool rtr, uint8_t dlc, uint8_t *data);
	void receive(RX_BUFFER buffer, uint32_t *id, bool *rtr, uint8_t *dlc, uint8_t *data);
	void config(uint8_t cnf3, uint8_t cnf2, uint8_t cnf1);
	void setFilter(FILTER f, uint16_t sid, uint8_t exide, uint32_t eid);
	void setMask(MASK m, uint16_t sid, uint32_t eid);
	void reset(void);
	void interruptEnable(uint8_t value);
	void setMode(MODE m);
	uint8_t status(void);
	uint8_t rxStatus(void);
	uint8_t interruptFlag(void);
	uint8_t eflg(void);
	void clearInterruptFlag(uint8_t filter);
	uint8_t status_last;
	
private:
	SPI_Device *_spi;
	void _rts(TX_BUFFER buffer);
	void _bit_modify(uint8_t address, uint8_t mask, uint8_t data);
	

}; // MCP2515

#endif // MCP2515_H
