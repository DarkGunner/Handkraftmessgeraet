// HX711_Lib.h

#ifndef _HX711_LIB_h
#define _HX711_LIB_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class HX711_Lib
{
private:
	byte PD_SCK;
	byte DOUT;
	byte GAIN;

public:

	HX711_Lib();

	virtual ~HX711_Lib();

	// init Sensor
	// CH A : 128 or 64 gain; CH B 32 gain
	void init(byte dout, byte pd_sck, byte gain = 128);

	// Check Sensor is ready
	bool is_ready();
	void wait_ready(unsigned long delay_ms = 0);
	bool wait_ready_timeout(unsigned long timeout = 1000, unsigned long delay_ms = 0);

	// CH A : 128 or 64 gain; CH B 32 gain
	void set_gain(byte gain = 128);

	// read uV
	long read_uV();

	// read mV
	double read_mV();

	void power_down();
	void power_up();
};


#endif

