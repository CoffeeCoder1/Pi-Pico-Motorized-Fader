#include "controlloop.h"

#include <Arduino.h>

void ControlLoop::begin() {
  pidController.begin(&controlLoopInput, &controlLoopOutput, &controlLoopSetpoint, kP, kI, kD);
	pidController.start();
}

void ControlLoop::update() {
  pidController.compute();
  pidController.debug(&Serial, "faderPID", PRINT_INPUT |
			PRINT_OUTPUT |
			PRINT_SETPOINT |
			PRINT_BIAS |
			PRINT_P |
			PRINT_I |
			PRINT_D);
}

void ControlLoop::updateCoefficients() {
  pidController.setCoefficients(kP, kI, kD);
}

void ControlLoop::setP(double p) {
  kP = p;
  updateCoefficients();
}

void ControlLoop::setI(double i) {
  kI = i;
  updateCoefficients();
}

void ControlLoop::setD(double d) {
  kD = d;
  updateCoefficients();
}

void ControlLoop::setOutputLimits(double min, double max) {
  pidController.setOutputLimits(min, max);
}

void ControlLoop::setWindUpLimits(double min, double max) {
  pidController.setWindUpLimits(min, max);
}

void ControlLoop::setDeadBand(double min, double max) {
  pidController.setDeadBand(min, max);
}

void ControlLoop::set(double setpoint) {
  controlLoopSetpoint = setpoint;
}

void ControlLoop::setFeedback(double feedback) {
  controlLoopInput = feedback;
}

double ControlLoop::getOutput() {
  return controlLoopOutput;
}