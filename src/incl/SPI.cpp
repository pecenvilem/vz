#include "SPI.h"

SPI_Core::SPI_Core(uint8_t cpol, uint8_t cpha, uint8_t clk_prescl)
{
	_cpol = cpol; _cpha = cpha; _clk_presc = clk_prescl & 0b00000111;
}

uint8_t SPI_Core::transmit(uint8_t data)
{
	if (!_initialised) this->init();										// init SPI module unless it was already done
	//while (!(SPSR & (1<<SPIF)));											// wait if there is a transmission in progress
	SPDR = data;															// writing into SPDR starts transmission
	while (!(SPSR & (1<<SPIF)));											// wait for SPIF to be set after completing transmission
	return SPDR;
}

void SPI_Core::init()
{
	if (_initialised) return;
	DDRB |= (1<<PB2) | (1<<PB1);											// output: MOSI, SCK
	DDRB |= (1<<PB0);														// output: default CS (ATmega will switch to slave, if this pin is an INPUT and falling edge is detected)
	DDRB &= ~(1<<PB3);														// input: MISO
	SPSR = (SPSR & 0b11111110) + (_clk_presc >> 2);
	SPCR = (SPCR & 0b11110000) + (_cpol << 4) + (_cpha << 3) + (_clk_presc & 0b00000011);
	SPCR |= (1<<SPE) | (1<<MSTR);											// enable SPI, master mode
	// send 0x00 - sets SPIF, after transmission is complete
	// ('transmit' method doesn't send data, until SPIF is set, which indicates completion of previous transmission)
	SPDR = 0x00;
	_initialised = true;
}

SPI_Device::SPI_Device(uint16_t *CS_port, uint16_t *CS_ddr, uint8_t CS_pin, SPI_Core *core)
{
	_port = CS_port; _ddr = CS_ddr; _pin = CS_pin; _core = core;
	this->init();
	this->deselect();
}

uint8_t SPI_Device::transmit(uint8_t data)
{
	return _core->transmit(data);
}

void SPI_Device::init()
{
	/* avoid creating unintended falling edge on the pin - before setting it as output, write 1 to the corresponding PORT */
	/* (enables internal pull-up and stays HIGH when output direction is set) */
	*_port |= (1<<_pin);													// enable pull-up
	*_ddr |= (1<<_pin);														// set CS pin as output in the appropriate DDR
}

void SPI_Device::select()
{
	*_port &= ~(1<<_pin);
}

void SPI_Device::deselect()
{
	*_port |= (1<<_pin);
}