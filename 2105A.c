#pragma config(UART_Usage, UART1, uartVEXLCD, baudRate19200, IOPins, None, None)
#pragma config(UART_Usage, UART2, uartNotUsed, baudRate4800, IOPins, None, None)
#pragma config(Sensor, in1,    PowerExpander,  sensorAnalog)
#pragma config(Sensor, in2,    Gyroscope,      sensorGyro)
#pragma config(Sensor, in3,    LineFollowerLeft, sensorLineFollower)
#pragma config(Sensor, in4,    LineFollowerCenter, sensorLineFollower)
#pragma config(Sensor, in5,    LineFollowerRight, sensorLineFollower)
#pragma config(Sensor, dgtl1,  TransmissionPneumatic, sensorDigitalOut)
#pragma config(Sensor, dgtl2,  LiftLimitMin,   sensorTouch)
#pragma config(Sensor, dgtl3,  LiftLimitMax,   sensorTouch)
#pragma config(Sensor, dgtl4,  LiftEncoder,    sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  DriveLeftEncoder, sensorQuadEncoder)
#pragma config(Sensor, dgtl8,  DriveRightEncoder, sensorQuadEncoder)
#pragma config(Sensor, dgtl10, DONOTUSE,       sensorDigitalIn)
#pragma config(Sensor, dgtl11, BattALED,       sensorLEDtoVCC)
#pragma config(Sensor, dgtl12, BattBLED,       sensorLEDtoVCC)
#pragma config(Motor,  port1,           LiftLeftA,     tmotorVex393_HBridge, openLoop, encoderPort, dgtl4)
#pragma config(Motor,  port2,           DriveLeftB,    tmotorVex393_MC29, openLoop, driveLeft, encoderPort, dgtl6)
#pragma config(Motor,  port3,           DriveRightB,   tmotorVex393_MC29, openLoop, driveRight, encoderPort, dgtl8)
#pragma config(Motor,  port4,           LiftRightA,    tmotorVex393_MC29, openLoop, encoderPort, dgtl4)
#pragma config(Motor,  port5,           LiftLeftB,     tmotorVex393_MC29, openLoop, encoderPort, dgtl4)
#pragma config(Motor,  port6,           LiftRightB,    tmotorVex393_MC29, openLoop, encoderPort, dgtl4)
#pragma config(Motor,  port7,           LiftLeftC,     tmotorVex393_MC29, openLoop, encoderPort, dgtl4)
#pragma config(Motor,  port8,           DriveLeftA,    tmotorVex393_MC29, openLoop, driveLeft, encoderPort, dgtl6)
#pragma config(Motor,  port9,           DriveRightA,   tmotorVex393_MC29, openLoop, driveRight, encoderPort, dgtl8)
#pragma config(Motor,  port10,          LiftRightC,    tmotorVex393_HBridge, openLoop, reversed, encoderPort, dgtl4)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// This file should only include functions that are applicable to this robot across drivers and autonomous programs.
// Each driver gets his/her own file with usercontrol task
// Each autonomous function is in its own file that defines only that function
// Autonomous manager should also be in a seperate file?

#define NoProgrammingSkills
const string FILE = __FILE__;
#include "core\v3\core.h"
#include "usercontrol\2105A-john.h"

void ResetDriveEncoders() {}

void init() {
	startTask(BatteryMonitor);
	SensorValue[Gyroscope] = 0;
	SensorValue[BattALED] = 0;
	SensorValue[BattBLED] = 0;
}
