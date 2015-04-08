#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in7,    PylonPot,       sensorPotentiometer)
#pragma config(Sensor, in8,    Gyro,           sensorGyro)
#pragma config(Sensor, dgtl1,  SolenoidCubes,  sensorDigitalOut)
#pragma config(Sensor, dgtl2,  SolenoidPylons, sensorDigitalOut)
#pragma config(Sensor, dgtl3,  LiftMinA,       sensorTouch)
#pragma config(Sensor, dgtl4,  LiftMinB,       sensorTouch)
#pragma config(Sensor, dgtl5,  MainBattLEDGreen, sensorLEDtoVCC)
#pragma config(Sensor, dgtl6,  MainBattLEDRed, sensorLEDtoVCC)
#pragma config(Sensor, dgtl7,  BackupBattLEDGreen, sensorLEDtoVCC)
#pragma config(Sensor, dgtl8,  BackupBattLEDRed, sensorLEDtoVCC)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_4,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           DriveCenter,   tmotorVex393_HBridge, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port2,           LiftLeftTop,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           LiftRightTop,  tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           DriveFrontLeft, tmotorVex393_MC29, openLoop, reversed, driveLeft)
#pragma config(Motor,  port5,           DriveFrontRight, tmotorVex393_MC29, openLoop, reversed, driveRight, encoderPort, I2C_2)
#pragma config(Motor,  port6,           DriveRearLeft, tmotorVex393_MC29, openLoop, reversed, driveLeft)
#pragma config(Motor,  port7,           DriveRearRight, tmotorVex393_MC29, openLoop, reversed, driveRight)
#pragma config(Motor,  port8,           LiftLeftBottom, tmotorVex393_MC29, openLoop, encoderPort, I2C_4)
#pragma config(Motor,  port9,           LiftRightBottom, tmotorVex393_MC29, openLoop, encoderPort, I2C_3)
#pragma config(Motor,  port10,          PylonMotor,    tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "misc\2105A-stabilizers.h"

int Pylon_Target = 0;
task PylonLock;
const tSensors PylonSolenoid = SolenoidPylons;

task main {
	startTask(PylonLock);
	nMotorEncoder[LiftLeftBottom] = 0;
	nMotorEncoder[LiftRightBottom] = 0;
	clearDebugStream();
	Set_Lift_Target();
	while(true) {
		motor[DriveFrontLeft]  = -(vexRT[Ch2] + vexRT[Ch4]);
		motor[DriveFrontRight] =  (vexRT[Ch2] - vexRT[Ch4]);
		motor[DriveRearLeft]   = -(vexRT[Ch2] + vexRT[Ch4]);
		motor[DriveRearRight]  =  (vexRT[Ch2] - vexRT[Ch4]);
		motor[DriveCenter]     =   vexRT[Ch1];
		if(vexRT[Btn6U] == 1) {
			Set_Lift_Target();
			IsStabilizerRunning = false;
			motor[LiftLeftTop]     =  127;
			motor[LiftLeftBottom]  =  127;
			motor[LiftRightTop]    =  127;
			motor[LiftRightBottom] =  127;
			} else if(vexRT[Btn6D] == 1) {
			IsStabilizerRunning = false;
			Set_Lift_Target();
			motor[LiftLeftTop]     = -127;
			motor[LiftLeftBottom]  = -127;
			motor[LiftRightTop]    = -127;
			motor[LiftRightBottom] = -127;
			} else {
			if(!IsStabilizerRunning) {
				Set_Lift_Target();
				/*motor[LiftTopLeft]     = 0;
				motor[LiftBottomLeft]  = 0;
				motor[LiftTopRight]    = 0;
				motor[LiftBottomRight] = 0;*/
				startTask(Lift_Stabilizer_Left);
				startTask(Lift_Stabilizer_Right);
			}
		}
		SensorValue[SolenoidCubes] = vexRT[Btn5D];
		SensorValue[SolenoidPylons] = vexRT[Btn5U];
		if(vexRT[Btn7L] == 1) {
			Pylon_Target = 1000;
			} else if(vexRT[Btn7R] == 1) {
			Pylon_Target = 3000;
			} else {
			Pylon_Target = 1800;
		}
	}
}

task PylonLock {
	const float PKp = 1;
	const float PKd = 1.5;
	const float PKi = 0.001;
	const tSensors PsensorRight = PylonPot;
	const int PrightAdjust = 0; // -350;
	const int PrightMultiplier = 1;
	const int PIntegralLimit = 200;
	const int PMax = 3200;
	const int PMin = 700;
	bool PMinOverride = false;
	const tSensors sensor = PsensorRight;
	const tMotor motor1 = PylonMotor;
	const int offset = PrightAdjust;
	const int mult = PrightMultiplier;
	int error = 0, previous_error = 0, speed = 0, derivative = 0, integral = 0;
	int last_nSysTime = nSysTime;
	while(true) {
		if(((mult * SensorValue[sensor]) + offset) > PMax || (((mult * SensorValue[sensor]) + offset) < PMin && !PMinOverride)) {
			motor[motor1] = 0;
			error = 0;
			integral = 0;
			derivative = 0;
			previous_error = 0;
			continue;
		}
		error = (Pylon_Target - ((mult * SensorValue[sensor]) + offset));
		if(abs(error) < PIntegralLimit) {
			integral += error / divisible(nSysTime - last_nSysTime);
			} else {
			integral = 0;
		}
		derivative = (error - previous_error) / divisible(nSysTime - last_nSysTime);
		last_nSysTime = nSysTime;
		previous_error = error;
		speed = PKp * error + PKd * derivative + PKi * integral;
		motor[motor1] = Normalize(speed);
}
}
