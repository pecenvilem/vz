/* 
* SimInterface.cpp
*
* Created: 14. 12. 2020 12:33:43
* Author: Vilém
*/

#include "SimInterface.h"

extern SimInterface sim;

 SimInterface::SimInterface(MCP2515 *can, Clock *clock)
 {
	_can = can;
	_clock = clock;
 }

void SimInterface::init(void)
{
	///////////////////////////////////////////////////////////////////////////////////
	DDRC |= (1<<PC4);
	///////////////////////////////////////////////////////////////////////////////////
	
	_t1 = _t2 = _t3 = _clock->millis();
	_can->init();
	_can->reset();
	_can->setMode(CONFIGURATION);
	_can->config(0x03, 0xAC, 0x80);
	DDRE &= ~(1<<PE5);
	_can->interruptEnable((1<<MCP2515_RX0IE) | (1<<MCP2515_RX1IE));
	_can->setMode(NORMAL);
}

int8_t SimInterface::send(Variable<uint8_t> *variable)
/* detects free TX buffers and sends CAN message with the value of the given variable */
/* if no buffer is free, returns error code */
{
	TX_BUFFER buffer;
	uint8_t status;
	uint32_t id; uint8_t dlc; uint8_t data[8];
	
	///////////////////////////////////////////////////////////////////////////////////
	PORTC ^= (1<<PC4);
	///////////////////////////////////////////////////////////////////////////////////
	
	status = _can->status();
	if ((status & 0b00000100) == 0)
	{
		buffer = TXB0;
	} else
	{
		if ((status & 0b00010000) == 0)
		{
			buffer = TXB1;
		} else
		{
			if ((status & 0b01000000) == 0)
			{
				buffer = TXB2;
			} else
			{
				return SIM_INTERFACE_NO_FREE_TX_BUFFER;
			}
		}
	}
	
	
	/* free TX_BUFFER is available - send message now */
	if (variable == (&ep_valve))
	{
		id = CANID_EP_VALVE; dlc = CANDLC_EP_VALVE; data[0] = variable->value;
	} else if (variable == (&ls_indicator))
	{
		id = CANID_LS_INDICATOR; dlc = CANDLC_LS_INDICATOR; data[0] = variable->value;
	}
	else return SIM_INTERFACE_UNKOWN_VARIABLE_SENT;
	_can->send(buffer, id, false, dlc, data);
	return 0;
}

void SimInterface::loop(void)
/* periodically calls takes care of transmission and reception if CAN messages */
{
	timestamp t = _clock->millis();
	/* each 25 ms read current status */
	if (_t1 + 25 < _clock->millis())
	{
		_t1 = t;
		_can->status();
	}
	
	/* each 500 ms read error status */
	if (_t2 + 500 < _clock->millis())
	{
		_t2 = t;
		if (_can->eflg() & (1<<MCP2515_TXBO))			// TEC has reached 255 and device is in BUS-OFF state
		{
			// reset MCP2515
			_can->init();
		}
	}
	
	/* if interrupt signal is LOW, i.e. active, read message/s from buffer/s */
	if (!(PINE & (1<<PE5)))
	{
		if (_can->status_last & (1<<0)) receive(RXB0);
		if (_can->status_last & (1<<1)) receive(RXB1);
		_t1 -= 26; // force status check in the next loop
	}
	
	/* if 100 ms has passed since last transmission of output values, transmit now */
	if (_t3 + 100 < _clock->millis())
	{
		send(&ls_indicator);
		send(&ep_valve);
		_t3 = t;
	}
}

void SimInterface::receive(RX_BUFFER buffer)
/* extracts data from given RX buffer and saves it to the correct container based on the message ID */
/* if RTR was received, immediately sends back requested value */
{
	uint32_t id; bool rtr; uint8_t dlc; uint8_t data[8];
	_can->receive(buffer, &id, &rtr, &dlc, data);
	if (rtr)
	{
		switch(id)
		{
			case CANID_LS_INDICATOR: send(&ls_indicator); break;
			case CANID_EP_VALVE: send(&ep_valve); break;
		}
		return;
	}
	switch(id)
	{
		case CANID_SPEED:
			//speed.previous = speed.value;
			speed.value = (data[0] << 8) | (data[1]);
			//speed.mark = _clock->millis();
			//speed.flag = true;
			break;
		
		case CANID_TRAIN_LINE_PRESSURE:
			//train_line_pressure.previous = train_line_pressure.value;
			train_line_pressure.value = data[0];
			//train_line_pressure.mark = _clock->millis();
			//train_line_pressure.flag = true;
			break;
		
		case CANID_BRAKE_CYLINDER_PRESSURE:
			//brake_cylinder_pressure.previous = brake_cylinder_pressure.value;
			brake_cylinder_pressure.value = data[0];
			//brake_cylinder_pressure.mark = _clock->millis();
			//brake_cylinder_pressure.flag = true;
			break;
		
		case CANID_SIGNAL_CODE:
			//signal_code.previous = signal_code.value;
			signal_code.value = data[0];
			//signal_code.mark = _clock->millis();
			//signal_code.flag = true;
			break;
		
		case CANID_VIGILANCE_BUTTON:
			//vigilance_button.previous = vigilance_button.value;
			vigilance_button.value = data[0];
			//vigilance_button.mark = _clock->millis();
			//vigilance_button.flag = true;
			break;
		
		case CANID_DRIVING_LEAVER:
			//driving_leaver.previous = driving_leaver.value;
			driving_leaver.value = data[0];
			//driving_leaver.mark = _clock->millis();
			//driving_leaver.flag = true;
			break;
		
		case CANID_DIRECTION_LEAVER:
			//direction_leaver.previous = direction_leaver.value;
			direction_leaver.value = (int8_t)data[0];
			//direction_leaver.mark = _clock->millis();
			//direction_leaver.flag = true;
			break;

		case CANID_WHISTLE:
			//whistle.previous = whistle.value;
			whistle.value = data[0];
			//whistle.mark = _clock->millis();
			//whistle.flag = true;
			break;
		
		case CANID_CONTROL_SWITCH:
			//control_switch.previous = control_switch.value;
			control_switch.value = data[0];
			//control_switch.mark = _clock->millis();
			//control_switch.flag = true;
			break;
		
		case CANID_LS_SWITCH:
			//ls_switch.previous = ls_switch.value;
			ls_switch.value = data[0];
			//ls_switch.mark = _clock->millis();
			//ls_switch.flag = true;
			break;
		
		case CANID_BRIGHTNESS:
			//brightness.previous = brightness.value;
			brightness.value = data[0];
			//brightness.mark = _clock->millis();
			//brightness.flag = true;
			break;
			
		case CANID_VOLUME:
			//volume.previous = volume.value;
			volume.value = data[0];
			//volume.mark = _clock->millis();
			//volume.flag = true;
			break;
		
		case CANID_EMP:
			//emp.previous = volume.value;
			emp.value = data[0];
			//emp.mark = _clock->millis();
			//emp.flag = true;
			break;
		
		case CANID_BATTERY:
			//battery.previous = battery.value;
			battery.value = data[0];
			//battery.mark = _clock->millis();
			//battery.flag = true;
			break;
	}
}
