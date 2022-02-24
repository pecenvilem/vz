#include "MCP2515.h"

MCP2515::MCP2515(SPI_Device *spi)
{
	_spi = spi;
	_spi->init();
}

void MCP2515::init(void)
{
	_spi->init();
}

void MCP2515::send(TX_BUFFER buffer, uint32_t id, bool rtr, uint8_t dlc, uint8_t *data)
{
	_spi->select();
	switch (buffer)
	{
		case TXB0:
			_spi->transmit(MCP2515_SPI_LOAD_TX_BUFFER | 0b00000000);
			break;
		
		case TXB1:
			_spi->transmit(MCP2515_SPI_LOAD_TX_BUFFER | 0b00000010);
			break;
		
		case TXB2:
			_spi->transmit(MCP2515_SPI_LOAD_TX_BUFFER | 0b00000100);
			break;
	}
	if (id <= 2047)
	{
		_spi->transmit(id>>3);
		_spi->transmit(id<<5);
		_spi->transmit(0x00); _spi->transmit(0x00);											// skip EID
	} else
	{
		_spi->transmit(id>>3);
		_spi->transmit((id<<5) | (1<<4) | (id>>27));
		_spi->transmit(id>>19);
		_spi->transmit(id>>11);
	}
	_spi->transmit(((rtr & 0x01)<<6) | (dlc & 0b00001111));
	for (uint8_t i = 0; i < dlc; i++) _spi->transmit(data[i]);
	_spi->deselect();
	_rts(buffer);
	switch (buffer)
	{
		case TXB0: status_last |= (1<<2); break;
		case TXB1: status_last |= (1<<4); break;
		case TXB2: status_last |= (1<<6); break;
	}
}

void MCP2515::receive(RX_BUFFER buffer, uint32_t *id, bool *rtr, uint8_t *dlc, uint8_t *data)
{
	uint8_t tmp; bool exide = false;
	switch (buffer)
	{
		case RXB0: _spi->select(); _spi->transmit(MCP2515_SPI_READ_RX_BUFFER); break;
		case RXB1: _spi->select(); _spi->transmit(MCP2515_SPI_READ_RX_BUFFER | (1<<2)); break;
	}
	*id = 0;
	*id |= (_spi->transmit(0x00) << 3);
	*id |= ((tmp = _spi->transmit(0x00)) >> 5);
	if (tmp & (1<<3))
	{
		exide = true;
		*id |= ((uint32_t)tmp & 0b00000011) << 27;
		*id |= ((uint32_t)(_spi->transmit(0x00))) << 19;
		*id |= (_spi->transmit(0x00) << 11);
	} else
	{
		if (tmp & (1<<4)) {*rtr = true; _spi->deselect(); return;}
		_spi->transmit(0x00); _spi->transmit(0x00); *rtr = false;
	}
	tmp = _spi->transmit(0x00);
	if (exide && (tmp & (1<<6))) {*rtr = true; _spi->deselect(); return;}
	*rtr = false;
	*dlc = (tmp & 0b00001111);
	for (uint8_t i = 0; i < *dlc; i++) data[i] = _spi->transmit(0x00);
	_spi->deselect();
}

void MCP2515::config(uint8_t cnf3, uint8_t cnf2, uint8_t cnf1)
{
	_spi->select();
	_spi->transmit(MCP2515_SPI_BIT_MODIFY);							// BIT_MODIFY instruction
	_spi->transmit(MCP2515_CNF3);									// CNF3 register address
	_spi->transmit(0b00000111); _spi->transmit(cnf3);				// mask and data
	_spi->deselect();
	
	_spi->select();
	_spi->transmit(MCP2515_SPI_WRITE);								// WRITE instruction
	_spi->transmit(MCP2515_CNF2);									// CNF2 register address
	_spi->transmit(cnf2); _spi->transmit(cnf1);						// CNF2 and CNF1 data
	_spi->deselect();
	
	_spi->select();
	_spi->transmit(MCP2515_SPI_WRITE);								// WRITE instruction
	_spi->transmit(MCP2515_RXB0CTRL);								// RXB0CTRL register address
	_spi->transmit(1<<MCP2515_BUKT);								// enable Roll-over
	_spi->deselect();
}

void MCP2515::setFilter(FILTER f, uint16_t sid, uint8_t exide, uint32_t eid)
{
	_spi->select();
	_spi->transmit(MCP2515_SPI_WRITE);
	_spi->transmit(f);												// address
	_spi->transmit(sid>>3);											// top 8 MSB of SID
	_spi->transmit((sid<<5) | ((exide & 0x01)<<4) | (eid>>16));		// SID2, SID1, SID0, _, EXIDE, _, EID17, EID16
	_spi->transmit(eid>>8);											// EID15, EID14, EID13, EID12, EID11, EID10, EID9, EID8
	_spi->transmit(eid);											// EID7, EID6, EID5, EID4, EID3, EID2, EI1, EI0
	_spi->deselect();
}

void MCP2515::setMask(MASK m, uint16_t sid, uint32_t eid)
{
	_spi->select();
	_spi->transmit(MCP2515_SPI_WRITE);
	_spi->transmit(m);												// address
	_spi->transmit(sid>>3);											// top 8 MSB of SID
	_spi->transmit((sid<<5) | (eid>>16));							// SID2, SID1, SID0, _, _, _, EID17, EID16
	_spi->transmit(eid>>8);											// EID15, EID14, EID13, EID12, EID11, EID10, EID9, EID8
	_spi->transmit(eid);											// EID7, EID6, EID5, EID4, EID3, EID2, EI1, EI0
	_spi->deselect();
}

void MCP2515::reset(void)
{
	_spi->select();
	_spi->transmit(MCP2515_SPI_RESET);
	_spi->deselect();
}

void MCP2515::interruptEnable(uint8_t value)
{
	_spi->select();
	_spi->transmit(MCP2515_SPI_WRITE);
	_spi->transmit(MCP2515_CANINTE);
	_spi->transmit(value);
	_spi->deselect();
}

void MCP2515::setMode(MODE m)
{
	uint8_t canstat = 0xFF;
	do {
		/* write code for required mode into CANCTRL register */
		_spi->select();
		_spi->transmit(MCP2515_SPI_WRITE);
		_spi->transmit(MCP2515_CANCTRL);
		_spi->transmit(m << MCP2515_OPMOD0);						// mode selection is performed by setting 3 MSB in CANCTRL register
		_spi->deselect();
		/* verify, that the device has switched to that mode */
		_spi->select();
		_spi->transmit(MCP2515_SPI_READ);
		_spi->transmit(MCP2515_CANSTAT);
		canstat = _spi->transmit(0x00);
		_spi->deselect();
	} while ((canstat >> MCP2515_OPMOD0) != m);						// loop until the mode changes
	
}

uint8_t MCP2515::status(void)
{
	_spi->select();
	_spi->transmit(MCP2515_SPI_READ_STATUS);
	status_last = _spi->transmit(0x00);
	_spi->deselect();
	return status_last;
}

uint8_t MCP2515::rxStatus(void)
{
	uint8_t tmp;
	_spi->select();
	_spi->transmit(MCP2515_SPI_RX_STATUS);
	tmp = _spi->transmit(0x00);
	_spi->deselect();
	return tmp;
}

uint8_t MCP2515::interruptFlag(void)
{
	uint8_t tmp;
	_spi->select();
	_spi->transmit(MCP2515_SPI_READ);
	_spi->transmit(MCP2515_CANINTF);
	tmp = _spi->transmit(0x00);
	_spi->deselect();
	return tmp;
}

uint8_t MCP2515::eflg(void)
{
	uint8_t tmp;
	_spi->select();
	_spi->transmit(MCP2515_SPI_READ);
	_spi->transmit(MCP2515_EFLG);
	tmp = _spi->transmit(0x00);
	_spi->deselect();
	return tmp;
}

void MCP2515::clearInterruptFlag(uint8_t filter)
{
	_bit_modify(MCP2515_CANINTF, filter, 0x00);
}

void MCP2515::_rts(TX_BUFFER buffer)
{
	_spi->select();
	switch (buffer)
	{
		case TXB0: _spi->transmit(MCP2515_SPI_RTS | 0b00000001); break;
		case TXB1: _spi->transmit(MCP2515_SPI_RTS | 0b00000010); break;
		case TXB2: _spi->transmit(MCP2515_SPI_RTS | 0b00000100); break;
	}
	_spi->deselect();
}

void MCP2515::_bit_modify(uint8_t address, uint8_t mask, uint8_t data)
{
	_spi->select();
	_spi->transmit(MCP2515_SPI_BIT_MODIFY);
	_spi->transmit(address);
	_spi->transmit(mask);
	_spi->transmit(data);
	_spi->deselect();
}

