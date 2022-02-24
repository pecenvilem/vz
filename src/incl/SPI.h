#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <stdbool.h>

#define SPI_SCK_prescaler2		0b100
#define SPI_SCK_prescaler4		0b000
#define SPI_SCK_prescaler8		0b101
#define SPI_SCK_prescaler16		0b001
#define SPI_SCK_prescaler32		0b110
#define SPI_SCK_prescaler64		0b010
#define SPI_SCK_prescaler128	0b011

#define SPI_CPOL_SCK_idle_LOW	0
#define SPI_CPOL_SCK_idle_HIGH	1
#define SPI_CPHA_sample_edege_LEADING	0
#define SPI_CPAH_sample_edege_TRAILING	1

class SPI_Core
{
public:
	SPI_Core(uint8_t cpol, uint8_t cpha, uint8_t clk_prescl);
	uint8_t transmit(uint8_t data);
	void init();
private:
	uint8_t _cpol;
	uint8_t _cpha;
	uint8_t _clk_presc;
	bool _initialised;
};

class SPI_Device
{
public:
	SPI_Device(uint16_t *CS_port, uint16_t *CS_ddr, uint8_t CS_pin, SPI_Core *core);
	uint8_t transmit(uint8_t data);
	void init(void);
	void select(void);
	void deselect(void);
private:
	SPI_Core *_core;
	uint16_t *_port, *_ddr;
	uint8_t _pin;
};

#endif // SPI_H
