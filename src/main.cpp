#include "controlloop.h"
#include "motorcontroller.h"
#include <Arduino.h>
#include <SerialCommands.h>
#include <SparkFun_ADS1219.h>

// Motor controller
MotorController motor = MotorController(0, 1, 16);

// ADC
SfeADS1219ArdI2C faderADC;

// Initial PID constants
const double kP = 1.25;
const double kI = 64;
const double kD = 0.04;

// Control loop
ControlLoop controlLoop = ControlLoop();

// Command line things
char serial_command_buffer_[32];

SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\n", " ");

void cmd_setP(SerialCommands *sender) {
	controlLoop.setP(atof(sender->Next()));
}

SerialCommand cmd_setP_("setp", cmd_setP);

void cmd_setI(SerialCommands *sender) {
	controlLoop.setI(atof(sender->Next()));
}

SerialCommand cmd_setI_("seti", cmd_setI);

void cmd_setD(SerialCommands *sender) {
	controlLoop.setD(atof(sender->Next()));
}

SerialCommand cmd_setD_("setd", cmd_setD);

void cmd_set(SerialCommands *sender) {
	controlLoop.set(atoi(sender->Next()));
}

SerialCommand cmd_set_("set", cmd_set);

void setup() {
	// Set up serial
	Serial.begin(115200);

	// Set up I2C
	Wire.begin();

	// Set up command line
	serial_commands_.AddCommand(&cmd_setP_);
	serial_commands_.AddCommand(&cmd_setI_);
	serial_commands_.AddCommand(&cmd_setD_);

	// Set up motor driver
	analogWriteFreq(50000); // Make PWM frequency outside the range of human hearing
	motor.begin();

	// Initialize ADC - this also performs a soft reset
	while (!faderADC.begin()) {
		Serial.println("ADC failed to initialize. Please check your wiring! Retrying...");
		delay(1000);
	}

	// Put the ADC into continuous mode
	faderADC.setConversionMode(ADS1219_CONVERSION_CONTINUOUS);

	// Set up PID
	controlLoop.setP(kP);
	controlLoop.setI(kI);
	controlLoop.setD(kD);
	controlLoop.begin();
	controlLoop.setOutputLimits(-255, 255);
	controlLoop.setWindUpLimits(-150, 150);
	controlLoop.setDeadBand(-25, 25);
	controlLoop.set(511);

	// Start the continuous conversions
	faderADC.startSync();
}

void loop() {
	serial_commands_.ReadSerial();

	if (faderADC.dataReady()) {
		// Read from the ADC
		faderADC.readConversion();
		float potMilliVolts = myADC.getConversionMillivolts();

		// Update the control loop
		controlLoop.setFeedback(potMilliVolts);
	}

	// Update the control loop
	controlLoop.update();

	// Set the motor speed accordingly
	motor.setMotorSpeed(controlLoop.getOutput());
}
