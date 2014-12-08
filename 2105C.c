#pragma config(Motor,  port1,           left_back,     tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           left_mid,      tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           left_front,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           right_front,   tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           right_mid,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           sail,          tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port10,          right_back,    tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "core\v3\competition.h"

void pre_auton() {}
void go_forward(int time);
void turn_left(int time);
void go_back(int time);

void go_forward(int time){
	motor[left_front] = -127;
	motor[left_mid] = -127;
	motor[left_back] = -127;
	motor[right_front] = 127 ;
	motor[right_mid] = 127;
	motor[right_back] = 127;
	wait1Msec(time);
}

void go_back(int time){
	motor[left_front] = 127;
	motor[left_mid] = 127;
	motor[left_back] = 127;
	motor[right_front] = -127 ;
	motor[right_mid] = -127;
	motor[right_back] = -127;
	wait1Msec(time);
}

void turn_left(int time){
	motor[left_front] = 127;
	motor[left_mid] = 127;
	motor[left_back] = 127;
	motor[right_front] = 127 ;
	motor[right_mid] = 127;
	motor[right_back] = 127;
	wait1Msec(time);
}
task autonomous {
	go_forward(1950);
	turn_left(900);
	go_forward(1600);
	turn_left(1200);
	go_forward(2800);
	go_back(1000);
	allMotorsOff();
}

task usercontrol {
	{
		while (true)
		{
			motor[left_front] = -vexRT[Ch2] - vexRT[Ch1];
			motor[left_mid] = -vexRT[Ch2] - vexRT[Ch1];
			motor[left_back] = -vexRT[Ch2] - vexRT[Ch1];

			motor[right_front] = vexRT[Ch2] - vexRT[Ch1];
			motor[right_mid] = vexRT[Ch2] - vexRT[Ch1];
			motor[right_back] = vexRT[Ch2] - vexRT[Ch1];

			if(vexRT[Btn6U] == 1){
				motor[sail] = 127;
				} else if(vexRT[Btn6D] == 1){
				motor[sail] = -127;
				} else {
				motor[sail] = 0;
			}

		}

	}
}
