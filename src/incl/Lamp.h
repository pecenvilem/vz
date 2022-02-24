#ifndef LAMP_H
#define LAMP_H

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

#define FADE_IN 1
#define FADE_OUT 2

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

class Lamp {
	/* methods */
	public:
		virtual void init(uint8_t fadePeriods) = 0; // initialize and set number of periods needed to fade-in/out
		virtual void on(void) = 0; // fade-in
		virtual void isr(void) = 0; // has to be manually called on output compare interrupt of each PWM period
		// interrupt is disabled when lamp is off - ie. fading out has been completed
		void off(void); // fade-out
		
	/* member variables */
	public:
		// used to set brightness from outside the class - part of the INTERFACE
		uint8_t brightness = 255; // PWM duty cycle (0 to 255)
	protected:
		bool _on = false; // ensures idempotence of 'on' and 'off' methods
		uint8_t _fade; // values: FADE_IN, FADE_OUT - indicates, whether fading is in progress
		uint8_t _fadePeriods; // number of PWM periods needed to fade-in/fade-out
		uint8_t _periodsElapsed; // during fading indicates progress of total '_fadePeriods' needed to complete the fading
}; // Lamp

class BDE_Lamp: public Lamp {
	/* for detailed comments see superclass 'Lamp' */
	/* port: B, pin: 7, Arduino pin: D13, timer: TC1, output compare register: OC1C */
	public:
		void init(uint8_t fadePeriods);
		void on(void);
		void isr(void);
}; // BDE_Lamp

class MEZ_Lamp: public Lamp {
	/* for detailed comments see superclass 'Lamp' */
	/* port: B, pin: 6, Arduino pin: D12, timer: TC1, output compare register: OC1B */
	public:
		void init(uint8_t fadePeriods);
		void on(void);
		void isr(void);
}; // MEZ_Lamp

class VOL_Lamp: public Lamp {
	/* for detailed comments see superclass 'Lamp' */
	/* port: B, pin: 5, Arduino pin: D11, timer: TC1, output compare register: OC1A */
	public:
	void init(uint8_t fadePeriods);
	void on(void);
	void isr(void);
}; // VOL_Lamp

class STU_Lamp: public Lamp {
	/* for detailed comments see superclass 'Lamp' */
	/* port: B, pin: 4, Arduino pin: D10, timer: TC2, output compare register: OC2A */
	public:
	void init(uint8_t fadePeriods);
	void on(void);
	void isr(void);
}; // STU_Lamp

class VYS_Lamp: public Lamp {
	/* for detailed comments see superclass 'Lamp' */
	/* port: H, pin: 6, Arduino pin: D9, timer: TC2, output compare register: OC2B */
	public:
	void init(uint8_t fadePeriods);
	void on(void);
	void isr(void);
}; // VYS_Lamp

#endif	// LAMP_H