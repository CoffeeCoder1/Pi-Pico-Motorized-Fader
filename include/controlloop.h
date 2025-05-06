#pragma once

#include <ArduPID.h>

class ControlLoop {
private:
	double controlLoopSetpoint;
	double controlLoopInput;
	double controlLoopOutput;
	ArduPID pidController;

	double kP;
	double kI;
	double kD;

	void updateCoefficients();

public:
	ControlLoop() = default;

	void begin();
	void update();
	void setP(double p);
	void setI(double i);
	void setD(double d);
	void setOutputLimits(double min, double max);
	void setWindUpLimits(double min, double max);
	void setDeadBand(double min, double max);

	/**
	 * Sets the target of the PID loop.
	 */
	void set(double setpoint);
	/**
	 * Sets the current position feedback of the PID loop.
	 */
	void setFeedback(double feedback);
	/**
	 * Gets the output value of the PID loop.
	 */
	double getOutput();
};
