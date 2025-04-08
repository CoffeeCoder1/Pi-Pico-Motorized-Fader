#!/bin/sh
set -e
arduino-cli compile MotorizedFaderPico
arduino-cli upload -p $1 MotorizedFaderPico
screen arduino-cli monitor -p $1 MotorizedFaderPico
