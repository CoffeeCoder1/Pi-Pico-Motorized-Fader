#include <Control_Surface.h> // Include the Control Surface library
#include <SerialCommands.h>
#include "motorcontroller.h"
#include "controlloop.h"

// Motor controller
MotorController motor = MotorController(0, 1, 16);

// Initial PID constants
const double kP = 1.25;
const double kI = 64;
const double kD = 0.04;

// Control loop
ControlLoop controlLoop = ControlLoop();

FilteredAnalog<10, 4, uint32_t> faderAnalog = A0;

// Command line things
char serial_command_buffer_[32];

SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\n", " ");

void cmd_setP(SerialCommands* sender) {
	controlLoop.setP(atof(sender->Next()));
}

SerialCommand cmd_setP_("setp", cmd_setP);

void cmd_setI(SerialCommands* sender) {
	controlLoop.setI(atof(sender->Next()));
}

SerialCommand cmd_setI_("seti", cmd_setI);

void cmd_setD(SerialCommands* sender) {
	controlLoop.setD(atof(sender->Next()));
}

SerialCommand cmd_setD_("setd", cmd_setD);

void setup() {
	// Set up serial
	Serial.begin(115200);

	// Set up command line
	serial_commands_.AddCommand(&cmd_setP_);
	serial_commands_.AddCommand(&cmd_setI_);
	serial_commands_.AddCommand(&cmd_setD_);

	// Set up FilteredAnalog
	FilteredAnalog<>::setupADC();

	// Set up motor driver
	analogWriteFreq(50000); // Make PWM frequency outside the range of human hearing
  motor.begin();

	// Set up PID
  controlLoop.setP(kP);
  controlLoop.setI(kI);
  controlLoop.setD(kD);
  controlLoop.begin();
  controlLoop.setOutputLimits(-255, 255);
	controlLoop.setWindUpLimits(-150, 150);
	controlLoop.setDeadBand(-25, 25);
}

void loop() {
	serial_commands_.ReadSerial();

	controlLoop.set(511);
}

void loop1() {
  faderAnalog.update();

	controlLoop.setFeedback(faderAnalog.getValue());
	controlLoop.update();

  motor.setMotorSpeed(controlLoop.getOutput());
}
