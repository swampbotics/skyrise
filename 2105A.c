#pragma config(UART_Usage, UART1, uartVEXLCD, baudRate19200, IOPins, None, None)
#pragma config(UART_Usage, UART2, uartNotUsed, baudRate4800, IOPins, None, None)
#pragma config(Sensor, in1,    PowerExpander,  sensorAnalog)
#pragma config(Sensor, in2,    Gyroscope,      sensorGyro)
#pragma config(Sensor, in3,    LineFollowerLeft, sensorLineFollower)
#pragma config(Sensor, in4,    LineFollowerCenter, sensorLineFollower)
#pragma config(Sensor, in5,    LineFollowerRight, sensorLineFollower)
#pragma config(Sensor, in6,    LiftLimitMinA,  sensorAnalog)
#pragma config(Sensor, in7,    LiftLimitMinB,  sensorAnalog)
#pragma config(Sensor, in8,    LiftLimitMaxA,  sensorAnalog)
#pragma config(Sensor, dgtl1,  LiftEncoder,    sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  DriveEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  LiftLimitMaxB,  sensorTouch)
#pragma config(Sensor, dgtl6,  DriveEncoderLeft, sensorQuadEncoder)
#pragma config(Motor,  port1,           LiftLeftA,     tmotorVex393_HBridge, openLoop, encoderPort, dgtl1)
#pragma config(Motor,  port2,           LiftRightA,    tmotorVex393_MC29, openLoop, encoderPort, dgtl1)
#pragma config(Motor,  port3,           LiftLeftB,     tmotorVex393_MC29, openLoop, reversed, encoderPort, dgtl1)
#pragma config(Motor,  port4,           DriveFrontLeft, tmotorVex393_MC29, openLoop, reversed, driveLeft, encoderPort, dgtl3)
#pragma config(Motor,  port5,           DriveFrontRight, tmotorVex393_MC29, openLoop, driveRight, encoderPort, dgtl3)
#pragma config(Motor,  port6,           DriveRearLeft, tmotorVex393_MC29, openLoop, reversed, driveLeft, encoderPort, dgtl3)
#pragma config(Motor,  port7,           DriveRearRight, tmotorVex393_MC29, openLoop, reversed, driveRight, encoderPort, dgtl3)
#pragma config(Motor,  port8,           LiftRightB,    tmotorVex393_MC29, openLoop, reversed, encoderPort, dgtl1)
#pragma config(Motor,  port9,           LiftLeftC,     tmotorVex393_MC29, openLoop, encoderPort, dgtl1)
#pragma config(Motor,  port10,          LiftRightC,    tmotorVex393_HBridge, openLoop, encoderPort, dgtl1)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define HasGyro
#if defined(_DEBUG)
const string FILE = __FILE__;
#endif
#include "core\v3\core.h"
#include "usercontrol\2105A-john.h"
#include "auton\2105A-blue-atloader.h"
#include "auton\2105A-red-atloader-2pylons.h"
#include "misc\2105A-autonmanager.h"

void ResetDriveEncoders() {
	SensorValue[DriveEncoder] = 0;
}

void init() {
	SensorValue[LiftEncoder] = 0;
	ResetDriveEncoders();
	SensorValue[Gyroscope] = 0;
}
