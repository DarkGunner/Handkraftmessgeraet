
#include <Arduino.h>
#include "HX711_Lib.h"

HX711_Lib::HX711_Lib() {
}

HX711_Lib::~HX711_Lib() {
}

void HX711_Lib::init(byte dout, byte pd_sck, byte gain) {
	PD_SCK = pd_sck;
	DOUT = dout;

	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);

	set_gain(gain);
}

void HX711_Lib::set_gain(byte gain) {
	switch (gain) {
	case 128:		// Ch A gain 128
		GAIN = 1;
		break;
	case 64:		// Ch A gain 64
		GAIN = 3;
		break;
	case 32:		// Ch B gain 32
		GAIN = 2;
		break;
	}

	digitalWrite(PD_SCK, LOW);
	read_uV();
}

// Read Data

long HX711_Lib::read_uV() {

	wait_ready();

	// Temp Variables
	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;


	noInterrupts();

	// Read 24 bits (3 * 8 bits = 3 bytes)
	data[2] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[1] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[0] = shiftIn(DOUT, PD_SCK, MSBFIRST);

	// Select Channel and Gain
	for (unsigned int i = 0; i < GAIN; i++) {
		digitalWrite(PD_SCK, HIGH);
		digitalWrite(PD_SCK, LOW);
	}


	interrupts();


	// create 32bit int from 24bit data
	if (data[2] & 0x80) {
		filler = 0xFF;
	}
	else {
		filler = 0x00;
	}

	value = (static_cast<unsigned long>(filler) << 24
		| static_cast<unsigned long>(data[2]) << 16
		| static_cast<unsigned long>(data[1]) << 8
		| static_cast<unsigned long>(data[0]));

	return static_cast<long>(value);
}

double HX711_Lib::read_mV() {
	return read_uV() / 1000.00000;
}

// Sensor Status

bool HX711_Lib::is_ready() {
	return digitalRead(DOUT) == LOW;
}

void HX711_Lib::wait_ready(unsigned long delay_ms) {
	while (!is_ready()) {
		delay(delay_ms);
	}
}

bool HX711_Lib::wait_ready_timeout(unsigned long timeout, unsigned long delay_ms) {

	unsigned long now = millis();
	while (millis() - now < timeout) {
		if (is_ready()) {
			return true;
		}
		delay(delay_ms);
	}
	return false;
}

// Sensor Setup

void HX711_Lib::power_down() {
	digitalWrite(PD_SCK, LOW);
	digitalWrite(PD_SCK, HIGH);
}

void HX711_Lib::power_up() {
	digitalWrite(PD_SCK, LOW);
}