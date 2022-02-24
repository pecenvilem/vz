#include "Clock.h"

extern Clock clock;
ISR(TIMER5_COMPA_vect)
{
	clock._seconds++;								// increments _seconds counter at 1 Hz rate
}

void Clock::init(void)
{
	OCR5A = 62500;									// OCR reached after 1 second
	TIMSK5 |= (1<<OCIE5A);							// enable OCA interrupt for TC5
	TCCR5B |= (1<<CS52) | (1<<WGM52);				// prescaler 256, CTC mode
}

timestamp Clock::millis(void)
{
	timestamp t = (uint16_t)TCNT5;
	t =  t * 10UL;
	t = t / 625UL;
	t += _seconds * 1000;
	return  t;		// returns time in milliseconds since power-up (*10/625 = /62.5)
}
