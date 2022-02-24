#include "Lamp.h"

void Lamp::off(void) {
	/* IDEMPOTENT */
	/* Fade the lamp out over the number of periods given by "_fadePeriods" member. */
	
	if (_on) // ensure idempotence
	{
		_on = false; // ensure idempotence
		// if fading-in is still in progress (call of 'off' method follows shortly after the call of 'on'),
		_periodsElapsed = (_fade == FADE_IN ? _fadePeriods-_periodsElapsed : 0);
		// start fading-out from the same duty-cycle (eg. 0-1-2 "now set off" 1-0)
		_fade = FADE_OUT; // signal fading to ISR calls
	}
}

void BDE_Lamp::init(uint8_t fadePeriods) {
	_fadePeriods = fadePeriods;							// number of PWM periods needed to fade-in/fade-out
	DDRB |= (1<<PB7);									// set data direction: port B, pin 7
	TCCR1A |= (1<<WGM10);								// set 8 bit fast PWM mode for TC1, OVF at 0x00FF
	TCCR1B |= (1<<WGM12);								// set 8 bit fast PWM mode for TC1, OVF at 0x00FF
	TCCR1B |= (1<<CS12);								// TC1 prescaler 256 for 16 MHz clock >> OVF @ 244 Hz
	OCR1C = 255;										// 0% duty-cycle
	TCCR1A |= (1<<COM1C1) | (1<<COM1C0);				// OC1C (PB7) set on BOTTOM, clear on OCR1C - INVERTED MODE
}

void BDE_Lamp::on(void) {
	/* IDEMPOTENT */
	/* Fade the lamp in over the number of periods given by "_fadePeriods" member. */
	if (!_on) // ensure idempotence
	{
		_on = true;  // ensure idempotence
		// if fading is still in progress (call follows shortly after a previous call to 'on' or 'off'),
		switch (_fade)
		{
			case FADE_OUT: _periodsElapsed = _fadePeriods-_periodsElapsed; break; // start fading-in from the same duty-cycle (eg. 255-254-253 "now set on" 254-255)
			case 0: _periodsElapsed = 0; break; // not fading in our out
		}
		//_periodsElapsed = (_fade == FADE_OUT ? _fadePeriods-_periodsElapsed : 0);
		_fade = FADE_IN; // signal fading to ISR calls
		TIMSK1 |= (1<<OCIE1C); // enable TC1_COMPC interrupt in which 'this.isr' method is called
	}
}

void BDE_Lamp::isr(void) {
	if (_fade)
	{
		if (_fade == FADE_IN)
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR1C = 255 - ++_periodsElapsed*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods)  // if given number of '_fadePeriods' has passed - fading-in is complete
				_fade = 0; // signal for next ISRs
		}
		else // _fade has to be: FADE_OUT
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR1C = 255 - (_fadePeriods - ++_periodsElapsed)*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods) // if given number of '_fadePeriods' has passed - fading-out is complete
				{_fade = 0; TIMSK1 &= ~(1<<OCIE1C);} // disable interrupts
		}
		
	} else
	{
		// OCR has to be updated in case desired brightness was updated from outside the class
		OCR1C = 255 - brightness;
	}
}

void MEZ_Lamp::init(uint8_t fadePeriods) {
	_fadePeriods = fadePeriods;							// number of PWM periods needed to fade-in/fade-out
	DDRB |= (1<<PB6);									// set data direction: port B, pin 6
	TCCR1A |= (1<<WGM10);								// set 8 bit fast PWM mode for TC1, OVF at 0x00FF
	TCCR1B |= (1<<WGM12);								// set 8 bit fast PWM mode for TC1, OVF at 0x00FF
	TCCR1B |= (1<<CS12);								// TC1 prescaler 256 for 16 MHz clock >> OVF @ 244 Hz
	OCR1B = 255;										// 0% duty-cycle
	TCCR1A |= (1<<COM1B1) | (1<<COM1B0);				// OC1B (PB6) set on BOTTOM, clear on OCR1B
}

void MEZ_Lamp::on(void) {
	/* IDEMPOTENT */
	/* Fade the lamp in over the number of periods given by "_fadePeriods" member. */
	if (!_on) // ensure idempotence
	{
		_on = true;  // ensure idempotence
		// if fading is still in progress (call follows shortly after a previous call to 'on' or 'off'),
		switch (_fade)
		{
			case FADE_OUT: _periodsElapsed = _fadePeriods-_periodsElapsed; break; // start fading-in from the same duty-cycle (eg. 255-254-253 "now set on" 254-255)
			case 0: _periodsElapsed = 0; break; // not fading in our out
		}
		//_periodsElapsed = (_fade == FADE_OUT ? _fadePeriods-_periodsElapsed : 0);
		_fade = FADE_IN; // signal fading to ISR calls
		TIMSK1 |= (1<<OCIE1B); // enable TC1_COMPB interrupt in which 'this.isr' method is called
	}
}

void MEZ_Lamp::isr(void) {
	if (_fade)
	{
		if (_fade == FADE_IN)
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR1B = 255 - ++_periodsElapsed*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods)  // if given number of '_fadePeriods' has passed - fading-in is complete
			_fade = 0; // signal for next ISRs
		}
		else // _fade has to be: FADE_OUT
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR1B = 255 - (_fadePeriods - ++_periodsElapsed)*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods) // if given number of '_fadePeriods' has passed - fading-out is complete
			{_fade = 0; TIMSK1 &= ~(1<<OCIE1B);} // disable interrupts
		}
		
	} else
	{
		// OCR has to be updated in case desired brightness was updated from outside the class
		OCR1B = 255 - brightness;
	}
}

void VOL_Lamp::init(uint8_t fadePeriods) {
	_fadePeriods = fadePeriods;							// number of PWM periods needed to fade-in/fade-out
	DDRB |= (1<<PB5);									// set data direction: port B, pin 5
	TCCR1A |= (1<<WGM10);								// set 8 bit fast PWM mode for TC1, OVF at 0x00FF
	TCCR1B |= (1<<WGM12);								// set 8 bit fast PWM mode for TC1, OVF at 0x00FF
	TCCR1B |= (1<<CS12);								// TC1 prescaler 256 for 16 MHz clock >> OVF @ 244 Hz
	OCR1A = 255;										// 0% duty-cycle
	TCCR1A |= (1<<COM1A1) | (1<<COM1A0);				// OC1A (PB5) set on BOTTOM, clear on OCR1A
}

void VOL_Lamp::on(void) {
	/* IDEMPOTENT */
	/* Fade the lamp in over the number of periods given by "_fadePeriods" member. */
	if (!_on) // ensure idempotence
	{
		_on = true;  // ensure idempotence
		// if fading is still in progress (call follows shortly after a previous call to 'on' or 'off'),
		switch (_fade)
		{
			case FADE_OUT: _periodsElapsed = _fadePeriods-_periodsElapsed; break; // start fading-in from the same duty-cycle (eg. 255-254-253 "now set on" 254-255)
			case 0: _periodsElapsed = 0; break; // not fading in our out
		}
		//_periodsElapsed = (_fade == FADE_OUT ? _fadePeriods-_periodsElapsed : 0);
		_fade = FADE_IN; // signal fading to ISR calls
		TIMSK1 |= (1<<OCIE1A); // enable TC1_COMPA interrupt in which 'this.isr' method is called
	}
}

void VOL_Lamp::isr(void) {
	if (_fade)
	{
		if (_fade == FADE_IN)
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR1A = 255 - ++_periodsElapsed*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods)  // if given number of '_fadePeriods' has passed - fading-in is complete
			_fade = 0; // signal for next ISRs
		}
		else // _fade has to be: FADE_OUT
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR1A = 255 - (_fadePeriods - ++_periodsElapsed)*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods) // if given number of '_fadePeriods' has passed - fading-out is complete
			{_fade = 0; TIMSK1 &= ~(1<<OCIE1A);} // disable interrupts
		}
			
	} else
	{
		// OCR has to be updated in case desired brightness was updated from outside the class
		OCR1A = 255 - brightness;
	}
}

void STU_Lamp::init(uint8_t fadePeriods) {
	_fadePeriods = fadePeriods;							// number of PWM periods needed to fade-in/fade-out
	DDRB |= (1<<PB4);									// set data direction: port B, pin 4
	TCCR2A |= (1<<WGM21) | (1<<WGM20);					// set fast PWM mode for TC2, OVF at 0x00FF
	TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);		// TC2 prescaler 256 for 16 MHz clock >> OVF @ 244 Hz
	OCR2A = 255;										// 0% duty-cycle
	TCCR2A |= (1<<COM2A1) | (1<<COM2A0);				// OC2A (PB4) set on BOTTOM, clear on OCR1C
}

void STU_Lamp::on(void) {
	/* IDEMPOTENT */
	/* Fade the lamp in over the number of periods given by "_fadePeriods" member. */
	if (!_on) // ensure idempotence
	{
		_on = true;  // ensure idempotence
		// if fading is still in progress (call follows shortly after a previous call to 'on' or 'off'),
		switch (_fade)
		{
			case FADE_OUT: _periodsElapsed = _fadePeriods-_periodsElapsed; break; // start fading-in from the same duty-cycle (eg. 255-254-253 "now set on" 254-255)
			case 0: _periodsElapsed = 0; break; // not fading in our out
		}
		//_periodsElapsed = (_fade == FADE_OUT ? _fadePeriods-_periodsElapsed : 0);
		_fade = FADE_IN; // signal fading to ISR calls
		TIMSK2 |= (1<<OCIE2A); // enable TC2_COMPA interrupt in which 'this.isr' method is called
	}
}

void STU_Lamp::isr(void) {
	if (_fade)
	{
		if (_fade == FADE_IN)
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR2A = 255 - ++_periodsElapsed*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods)  // if given number of '_fadePeriods' has passed - fading-in is complete
			_fade = 0; // signal for next ISRs
		}
		else // _fade has to be: FADE_OUT
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR2A = 255 - (_fadePeriods - ++_periodsElapsed)*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods) // if given number of '_fadePeriods' has passed - fading-out is complete
			{_fade = 0; TIMSK2 &= ~(1<<OCIE2A);} // disable interrupts
		}
		
	} else
	{
		// OCR has to be updated in case desired brightness was updated from outside the class
		OCR2A = 255 - brightness;
	}
}

void VYS_Lamp::init(uint8_t fadePeriods) {
	_fadePeriods = fadePeriods;							// number of PWM periods needed to fade-in/fade-out
	DDRH |= (1<<PH6);									// set data direction: port H, pin 6
	TCCR2A |= (1<<WGM21) | (1<<WGM20);					// set fast PWM mode for TC2, OVF at 0x00FF
	TCCR2B |= (1<<CS22) | (1<<CS21) | (1<<CS20);		// TC2 prescaler 256 for 16 MHz clock >> OVF @ 244 Hz
	OCR2B = 255;										// 0% duty-cycle
	TCCR2A |= (1<<COM2B1) | (1<<COM2B0);				// OC2B (PH6) set on BOTTOM, clear on OCR1C
}

void VYS_Lamp::on(void) {
	/* IDEMPOTENT */
	/* Fade the lamp in over the number of periods given by "_fadePeriods" member. */
	if (!_on) // ensure idempotence
	{
		_on = true;  // ensure idempotence
		// if fading is still in progress (call follows shortly after a previous call to 'on' or 'off'),
		switch (_fade)
		{
			case FADE_OUT: _periodsElapsed = _fadePeriods-_periodsElapsed; break; // start fading-in from the same duty-cycle (eg. 255-254-253 "now set on" 254-255)
			case 0: _periodsElapsed = 0; break; // not fading in our out
		}
		//_periodsElapsed = (_fade == FADE_OUT ? _fadePeriods-_periodsElapsed : 0);
		_fade = FADE_IN; // signal fading to ISR calls
		TIMSK2 |= (1<<OCIE2B); // enable TC2_COMPB interrupt in which 'this.isr' method is called
	}
}

void VYS_Lamp::isr(void) {
	if (_fade)
	{
		if (_fade == FADE_IN)
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR2B = 255 - ++_periodsElapsed*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods)  // if given number of '_fadePeriods' has passed - fading-in is complete
			_fade = 0; // signal for next ISRs
		}
		else // _fade has to be: FADE_OUT
		{
			// PWM in INVERTED mode (255 - 0% duty-cycle)
			OCR2B = 255 - (_fadePeriods - ++_periodsElapsed)*brightness/_fadePeriods;
			if (_periodsElapsed >= _fadePeriods) // if given number of '_fadePeriods' has passed - fading-out is complete
			{_fade = 0; TIMSK2 &= ~(1<<OCIE2B);} // disable interrupts
		}
			
	} else
	{
		// OCR has to be updated in case desired brightness was updated from outside the class
		OCR2B = 255 - brightness;
	}
}