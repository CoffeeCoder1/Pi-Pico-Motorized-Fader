#include "motorcontroller.h"

#include <Arduino.h>

MotorController::MotorController(int forwardPin, int reversePin, int sleepPin) {
  this->forwardPin = forwardPin;
  this->reversePin = reversePin;
  this->sleepPin = sleepPin;
}

void MotorController::begin() {
  pinMode(forwardPin, OUTPUT);
	pinMode(reversePin, OUTPUT);
	pinMode(sleepPin, OUTPUT);
}

void MotorController::setMotorSpeed(int speed) {
  if (speed == 0) {
    digitalWrite(sleepPin, LOW);
  } else {
    digitalWrite(sleepPin, HIGH);
  }

  if (speed > 0) {
		analogWrite(forwardPin, abs(speed));
		digitalWrite(reversePin, LOW);
	} else if (speed < 0) {
		digitalWrite(forwardPin, LOW);
		analogWrite(reversePin, abs(speed));
	} else {
		digitalWrite(forwardPin, LOW);
		digitalWrite(reversePin, LOW);
	}
}