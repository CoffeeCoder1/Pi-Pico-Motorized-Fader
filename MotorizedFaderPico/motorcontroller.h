#pragma once

class MotorController {

private:
  int forwardPin;
  int reversePin;
  int sleepPin;

public:
  MotorController(int forwardPin, int reversePin, int sleepPin);

  /**
   * Sets up the motor controller. This should be called once at initialization time.
   */
  void begin();
  /**
   * Sets the motor's speed [-255, 255].
   */
  void setMotorSpeed(int speed);

};