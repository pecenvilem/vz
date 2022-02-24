#ifndef SIMINTERFACE_H
#define SIMINTERFACE_H

#include "MCP2515.h"
#include "Clock.h"
#include <stdbool.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../cfg/can_config.h"

#define SIM_INTERFACE_BUFFER_SIZE 30
#define SIM_INTERFACE_NO_FREE_TX_BUFFER -1
#define SIM_INTERFACE_UNKOWN_VARIABLE_SENT -2

/* container for storing received data, is also used for output variables for convenience */
template<class T>
class Variable
{
public:
	T value = 0, previous = 0;
	timestamp mark = 0; // can be used to record custom timestamp related to this variable
	bool flag = false;
};


class SimInterface
{
public:
	SimInterface(MCP2515 *can, Clock *clock);
	void init(void);
	void loop(void);
	void receive(RX_BUFFER buffer);
	int8_t send(Variable<uint8_t> *variable);
	
	// Simulation variables
	/* INPUT */
	Variable<int16_t> speed;
	Variable<uint8_t> train_line_pressure;
	Variable<uint8_t> brake_cylinder_pressure;
	Variable<uint8_t> signal_code;
	Variable<uint8_t> vigilance_button;
	Variable<uint8_t> driving_leaver;
	Variable<int8_t> direction_leaver;
	Variable<uint8_t> whistle;
	Variable<uint8_t> control_switch;
	Variable<uint8_t> ls_switch;
	Variable<uint8_t> brightness;
	Variable<uint8_t> volume;
	Variable<uint8_t> battery;
	Variable<uint8_t> emp;
	
	/* OUTPUT */
	Variable<uint8_t> ep_valve;
	Variable<uint8_t> ls_indicator;
	
	/* time marks for event timing */
	timestamp _t1, _t2, _t3;

private:
	MCP2515 *_can;
	Clock *_clock;

}; // SimInterface

#endif // SIMINTERFACE_H
