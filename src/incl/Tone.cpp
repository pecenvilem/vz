#include "Tone.h"

extern Tone tone;

ISR(TIMER0_OVF_vect)
/* called on every TC0 overflow, e.i. after each PWM period */
{
	tone.index += tone.increment;
	OCR0B = tone._table[(tone.index>>8)];
}

void Tone::init(void)
{
	TIMSK0 &= ~(1<<TOIE0); // disable OVF interrupt
	DDRG |= (1<<PG5); // set output data direction on PG5
	volume(255); // initialize the table from source_table
	TCCR0A |= (1<<COM0B1) | (1<<WGM01) | (1<<WGM00); // PWM mode
	OCR0A = 0xFF; // TOP - 0x7F for 125 kHz sample rate, 0xFF for 65.2 kHz
	TCCR0B |= (1<<WGM02); // PWM mode
	TCCR0B |= (1<<CS00); // prescaler
	OCR0B = 127; // initial sample - at the mid-point of possible voltage swing
}


void Tone::volume(uint8_t volume)
/* scales the wave sample data and loads them into the table */
{
	for (uint8_t i = 0; i < TONE_SAMPLES_PER_WAVE - 1; i++)
	{
		_table[i] = _table_source[i] * volume / 255 + 43 * (255 - volume) / 255;
	}
}

void Tone::on(void) {TIMSK0 |= (1<<TOIE0);} // PWM duty-cycle starts to be updated by ISR
	
void Tone::off(void) {TIMSK0 &= ~(1<<TOIE0);} // PWM duty-cycle stops being updated by ISR