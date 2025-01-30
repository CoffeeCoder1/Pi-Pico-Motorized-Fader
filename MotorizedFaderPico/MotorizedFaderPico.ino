//#include "src/Control-Surface/src/Control_Surface.h" // Include the Control Surface library
#include <Control_Surface.h> // Include the Control Surface library
//#include <MIDI_Interfaces/BluetoothMIDI_Interface.hpp>
#include <ArduPID.h>
#include <SerialCommands.h>

#define FADER_PIN A0
#define MOTOR_PIN_FORWARD 0
#define MOTOR_PIN_BACKWARD 1
#define MOTOR_SLEEP_PIN 16

// Initial PID constants
double kP = 1.25;
double kI = 64;
double kD = 0.04;

double controlLoopSetpoint, controlLoopInput, controlLoopOutput;
ArduPID faderPID;

// Instantiate a MIDI over BLE interface.
//BluetoothMIDI_Interface midi;

//CCPotentiometer potentiometer {
//	FADER_PIN,
//	{MIDI_CC::Channel_Volume, Channel_1},
//};

FilteredAnalog<10, 4, uint32_t> faderAnalog = FADER_PIN;

// Command line things
char serial_command_buffer_[32];

SerialCommands serial_commands_(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\n", " ");

void cmd_setP(SerialCommands* sender) {
	kP = atof(sender->Next());
	faderPID.setCoefficients(kP, kI, kD);
}

SerialCommand cmd_setP_("setp", cmd_setP);

void cmd_setI(SerialCommands* sender) {
	kI = atof(sender->Next());
	faderPID.setCoefficients(kP, kI, kD);
}

SerialCommand cmd_setI_("seti", cmd_setI);

void cmd_setD(SerialCommands* sender) {
	kD = atof(sender->Next());
	faderPID.setCoefficients(kP, kI, kD);
}

SerialCommand cmd_setD_("setd", cmd_setD);

void setup() {
	// Set up serial
	Serial.begin(115200);

	// Set up command line
	serial_commands_.AddCommand(&cmd_setP_);
	serial_commands_.AddCommand(&cmd_setI_);
	serial_commands_.AddCommand(&cmd_setD_);

	// Initialize Control Surface
	//Control_Surface.begin();

	// Set up FilteredAnalog
	FilteredAnalog<>::setupADC();

	// Set up motor driver
	analogWriteFreq(50000); // Make PWM frequency outside the range of human hearing
	pinMode(MOTOR_PIN_FORWARD, OUTPUT);
	pinMode(MOTOR_PIN_BACKWARD, OUTPUT);
	pinMode(MOTOR_SLEEP_PIN, OUTPUT);
	digitalWrite(MOTOR_SLEEP_PIN, HIGH);

	// Set up PID
	faderPID.begin(&controlLoopInput, &controlLoopOutput, &controlLoopSetpoint, kP, kI, kD);
	faderPID.setOutputLimits(-255, 255);
	faderPID.setWindUpLimits(-150, 150);
	faderPID.setDeadBand(-25, 25);
	faderPID.start();
}

void loop() {
	serial_commands_.ReadSerial();

	//Control_Surface.loop(); // Update the Control Surface

	controlLoopSetpoint = 511;

	faderPID.debug(&Serial, "faderPID", PRINT_INPUT |
			PRINT_OUTPUT |
			PRINT_SETPOINT |
			PRINT_BIAS |
			PRINT_P |
			PRINT_I |
			PRINT_D);
}

void loop1() {
  faderAnalog.update();
	controlLoopInput = faderAnalog.getValue();

	faderPID.compute();

	if (controlLoopOutput > 0) {
		analogWrite(MOTOR_PIN_FORWARD, abs(controlLoopOutput));
		digitalWrite(MOTOR_PIN_BACKWARD, LOW);
	} else if (controlLoopOutput < 0) {
		digitalWrite(MOTOR_PIN_FORWARD, LOW);
		analogWrite(MOTOR_PIN_BACKWARD, abs(controlLoopOutput));
	} else {
		digitalWrite(MOTOR_PIN_FORWARD, LOW);
		digitalWrite(MOTOR_PIN_BACKWARD, LOW);
	}
}
