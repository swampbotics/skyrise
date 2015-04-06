#pragma config(Sensor, dgtl1,  CollectionCube, sensorDigitalOut)
#pragma config(Sensor, dgtl2,  EncoderRight,   sensorQuadEncoder)
#pragma config(Sensor, dgtl4,  EncoderLeft,    sensorQuadEncoder)
#pragma config(Motor,  port1,           DriveMiddle,   tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           DriveRearLeft, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           DriveFrontRight, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           DriveRearRight, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           DriveFrontLeft, tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port6,           LiftTopLeft,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           LiftBottomLeft, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           LiftTopRight,  tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           LiftBottomRight, tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "misc\2105A-etc.h"

task main() {
	SensorValue[EncoderLeft] = 0;
	SensorValue[EncoderRight] = 0;
	while(true) {
		motor[DriveFrontLeft] = -(vexRT[Ch2] + vexRT[Ch4]);
		motor[DriveFrontRight] = (vexRT[Ch2] - vexRT[Ch4]);
		motor[DriveRearLeft] = -(vexRT[Ch2] + vexRT[Ch4]);
		motor[DriveRearRight] = (vexRT[Ch2] - vexRT[Ch4]);
		motor[DriveMiddle] = vexRT[Ch1];
		if(vexRT[Btn6U] == 1){
			IsStabilizerRunning = false;
			Set_Lift_Target();
			motor[LiftTopLeft] = 127;
			motor[LiftBottomLeft] = 127;
			motor[LiftTopRight] = 127;
			motor[LiftBottomRight] = 127;
			} else if(vexRT[Btn6D] == 1){
			IsStabilizerRunning = false;
			Set_Lift_Target();
			motor[LiftTopLeft] = -127;
			motor[LiftBottomLeft] = -127;
			motor[LiftTopRight] = -127;
			motor[LiftBottomRight] = -127;
			} else {
			if(!IsStabilizerRunning) {
				startTask(Lift_Stabilizer_Left);
				startTask(Lift_Stabilizer_Right);
			}
		}
		if(vexRT[Btn5D] == 1) {
			SensorValue[CollectionCube] = 0;
			} else if(vexRT[Btn5U] == 1) {
			SensorValue[CollectionCube] = 1;
		}
	}
}