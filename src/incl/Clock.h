#ifndef CLOCK_H
#define CLOCK_H

#include <avr/io.h>
#include <avr/interrupt.h>

typedef uint32_t timestamp;				// represents a timestamp

class Clock
{
public:
	void init(void);
	timestamp millis(void);
	volatile uint32_t _seconds = 0;					// incremented by ISR at 1 Hz rate

}; // Clock

#endif //CLOCK_H
