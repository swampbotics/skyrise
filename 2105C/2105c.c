#pragma config(Motor,  port1,           DriveLeftc,    tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           DriveRighta,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           DriveLefta,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           DriveLeftb,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           DriveRightb,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           Sail,          tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port10,          DriveRightc,   tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "core\v3\competition.h"

void pre_auton() {}
{
	task autonomous {
		motor[DriveLefta] = 127;
		motor[Driveleftb] = 127;
		motor[DriveLeftc] = 127;
		motor[DriveRighta] = -127;
		motor[Driverightb] = -127;
		motor[DriveRightc] = -127;
		wait1Msec(5000);
	}
	while(true) {
		motor[DriveLefta] = vexRT[Ch3] + vexRT[Ch4];
		motor[DriveLeftb] = vexRT[Ch3] + vexRT[Ch4];
		motor[DriveLeftc] = vexRT[Ch3] + vexRT[Ch4];
		motor[DriveRighta] = vexRT[Ch3] - vexRT[Ch4];
		motor[DriveRightb] = vexRT[Ch3] - vexRT[Ch4];
		motor[DriveRightc] = vexRT[Ch3] - vexRT[Ch4];

		//Sail
		if(vexRT[Btn6D] == 1) {
			motor[Sail] = -127;
			} else if(vexRT[Btn6U] == 1) {
			motor[Sail] = 127;
			} else {
			motor[Sail] = 0;
		}
	}
}
