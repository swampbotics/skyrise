// Initial setup
#pragma systemFile
#pragma platform(VEX2)
#pragma competitionControl(Competition)
#if defined(_DEBUG)
#pragma DebuggerWindows("debugStream")
#ifndef NoLCD
#pragma DebuggerWindows("VexLCD")
#endif
#endif
#ifndef NoLCD
#define LCD_NotUsing_Prompt
#include "lcd_menu.h"
#endif
#include "competition.h"

// Warnings
#if defined(_DEBUG)
#warn("Optimization turned off.")
#endif

#ifndef ROBOTC
#error("This program must be compiled using ROBOTC.")
#endif

// Type Definitions

typedef byte tSpeed;
typedef long tTimer;

typedef enum tDirection {
	STOP,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	CLOCKWISE,
	COUNTERCLOCKWISE
};

typedef enum tVertical {
	UP = 1,
	DOWN = -1,
	VSTOP = 0
};

typedef enum ClawPosition {
	OPEN = 0,
	CLOSE = 1
};

typedef struct {
	tSensors Sensor;
	int IntegralLimit;
	float Kp;
	float Ki;
	float Kd;
} tPID;

// Must be defined per robot
void init();
void ResetDriveEncoders();
bool Lift_TrippedMax();
bool Lift_TrippedMin();

tSensors DriveEncoder, LiftEncoder, LiftLimitMinA, LiftLimitMinB;
tMotor LiftLeftA, LiftLeftB, LiftLeftC, LiftRightA, LiftRightB, LiftRightC;
string POSTErrorText;

tPID PID_Drive, PID_Drive_TurnTo;

// Function definitions

int Auton_GetMultiplier(tDirection Direction, tMotor WhichMotor) {
	switch(Direction) {
	case STOP:
		return 0;
	case FORWARD:
		switch(WhichMotor) {
		case DriveFrontLeft:
			return -1;
		case DriveFrontRight:
			return 1;
		case DriveRearLeft:
			return -1;
		case DriveRearRight:
			return 1;
		}
	case BACKWARD:
		return -Auton_GetMultiplier(FORWARD, WhichMotor);
		/*case LEFT:
#if defined(_DEBUG)
#ifdef MultiDriveEncoders
		writeDebugStreamLine("***ATTENTION: Crawling does not work with multiple encoders b/c I didn't feel like making exceptions.");
#endif
#endif
		switch(WhichMotor) {
		case DriveFrontLeft:
		return 1;
		case DriveFrontRight:
		return 1;
		case DriveRearLeft:
		return -1;
		case DriveRearRight:
		return -1;
		}
		case RIGHT:
#if defined(_DEBUG)
#ifdef MultiDriveEncoders
		writeDebugStreamLine("***ATTENTION: Crawling does not work with multiple encoders b/c I didn't feel like making exceptions.");
#endif
#endif
		return -Auton_GetMultiplier(LEFT, WhichMotor);*/
	case CLOCKWISE:
		return -1;
	case COUNTERCLOCKWISE:
		return 1;
	}
	return 0;
}

// REVIEW HISTORY FOR THIS FUNCTION
void Auton_Drive(tDirection Direction = STOP, tSpeed Speed = 127, int Time = 0) {
	if(Direction == LEFT) {
		motor[DriveCenterA] = -Speed;
		motor[DriveCenterB] = -Speed;
		} else if(Direction == RIGHT) {
		motor[DriveCenterA] = Speed;
		motor[DriveCenterB] = Speed;
		} else {
		motor[DriveFrontLeft] = Speed * Auton_GetMultiplier(Direction,DriveFrontLeft);
		motor[DriveFrontRight] = Speed * Auton_GetMultiplier(Direction,DriveFrontRight);
		motor[DriveRearLeft] = Speed * Auton_GetMultiplier(Direction,DriveRearLeft);
		motor[DriveRearRight] = Speed * Auton_GetMultiplier(Direction,DriveRearRight);
		motor[DriveCenterA] = 0;
		motor[DriveCenterB] = 0;
	}
	if(Time > 0) {
		sleep(Time);
		Auton_Drive();
	}
}

#ifdef HasGyro
void Auton_Drive_TurnTo(tDirection Direction, int Heading = 0, tSpeed Speed = 127) {
	long StartTime = nSysTime;
	writeDebugStream("Request to turn to heading %i", Heading);
	Auton_Drive(Direction, Speed);
	if(Direction == CLOCKWISE) {
		while(SensorValue[Gyroscope] > Heading) {
			//writeDebugStreamLine("TARGET=%i, POSITION=%i", Heading, SensorValue[Gyroscope]);
		}
		} else {
		while(SensorValue[Gyroscope] < Heading) {
			//writeDebugStreamLine("TARGET=%i, POSITION=%i", Heading, SensorValue[Gyroscope]);
		}
	}
	Auton_Drive();
	writeDebugStreamLine(" completed in %ims",(nSysTime - StartTime));
}

void Auton_Drive_TurnTo_PID(tDirection Direction, int Heading = 0, tSpeed MaxSpeed = 127, int RequestedAccuracy = 5, int Timeout = 3000) {
	writeDebugStreamLine("Request to PID turn to heading %i", Heading);
	const int StartTime = nSysTime;
	int Error = 0;
	int LastError = 0;
	int Integral = 0;
	int Derivative = 0;
	int Speed = 0;
	while((nSysTime - StartTime) < Timeout) {
		Error = SensorValue[PID_Drive_TurnTo.Sensor] - Heading;
		if(PID_Drive_TurnTo.Ki != 0 && abs(Error) < PID_Drive_TurnTo.IntegralLimit) {
			Integral += Error;
			} else {
			Integral = 0;
		}
		if(abs(Error) < RequestedAccuracy) {
			break;
		}
		Speed = -Auton_GetMultiplier(Direction,DriveRearRight) * (PID_Drive_TurnTo.Kp * Error) + (Integral * PID_Drive_TurnTo.Ki) + (Derivative * PID_Drive_TurnTo.Kd);
		if(Speed > MaxSpeed)
			Speed = MaxSpeed;
		if(Speed < -MaxSpeed)
			Speed = -MaxSpeed;
		Auton_Drive(Direction, Speed);
		LastError = Error;
		sleep(25);
	}
	Auton_Drive();
#if defined(_DEBUG)
	if(!((nSysTime - StartTime) < Timeout)) {
		writeDebugStreamLine("***WARNING: PID_TurnTo to heading %i timed out after %i ms (gyro reading %i)", Heading, Timeout, SensorValue[PID_Drive_TurnTo.Sensor]);
	}
#endif
}
#endif

void Auton_Drive_Targeted(tDirection Direction, int Distance = 0, tSpeed Speed = 127, int Timeout = 2000, int LeftSpeed = Speed) {
	const int StartTime = nSysTime;
	ResetDriveEncoders();
	Auton_Drive(Direction, Speed, 0);
#if defined(_DEBUG)
	writeDebugStreamLine("Multiplier is %i", Auton_GetMultiplier(Direction,DriveRearRight));
#endif
	switch(-Auton_GetMultiplier(Direction,DriveRearRight)) {
	case -1:
#if defined(_DEBUG)
		writeDebugStreamLine("Current encoder reading is %i, wanting less than %i", SensorValue[DriveEncoder], -Auton_GetMultiplier(Direction,DriveRearRight) * Distance);
#endif
		while(SensorValue[DriveEncoder] > -Auton_GetMultiplier(Direction,DriveRearRight) * Distance && (nSysTime - StartTime) < Timeout) {}
		break;
	case 1:
#if defined(_DEBUG)
		writeDebugStreamLine("Current encoder reading is %i, wanting greater than %i", SensorValue[DriveEncoder], -Auton_GetMultiplier(Direction,DriveRearRight) * Distance);
#endif
		while(SensorValue[DriveEncoder] < -Auton_GetMultiplier(Direction,DriveRearRight) * Distance && (nSysTime - StartTime) < Timeout) {}
		break;
	}
	Auton_Drive();
#if defined(_DEBUG)
	if(!((nSysTime - StartTime) < Timeout)) {
		writeDebugStreamLine("**WARNING: Drive to distance %i timed out after %i ms (encoder reading %i)", Distance, Timeout, SensorValue[DriveEncoder]);
	}
#endif
}

void Auton_Drive_Targeted_PID(tDirection Direction, int Distance, tSpeed MaxSpeed = 127, int RequestedAccuracy = 5, int Timeout = 3000) {
	const int StartTime = nSysTime;
	ResetDriveEncoders();
	int Error = 0;
	int LastError = 0;
	int Integral = 0;
	int Derivative = 0;
	int Speed = 0;
#if defined(_DEBUG)
	writeDebugStreamLine("Multiplier is %i", -Auton_GetMultiplier(Direction,DriveRearRight));
#endif
	//writeDebugStreamLine("START-DISTANCE=%i",Distance);
	while((nSysTime - StartTime) < Timeout) {
		//writeDebugStreamLine("Sensor=%i",SensorValue[PID_Drive.Sensor]);
		Error = SensorValue[PID_Drive.Sensor] - (-Auton_GetMultiplier(Direction,DriveRearRight)) * Distance;
		//writeDebugStreamLine("Target=%i",(-Auton_GetMultiplier(Direction,DriveRearRight)) * Distance);
		//writeDebugStreamLine("Error=%i",Error);
		if(PID_Drive.Ki != 0 && abs(Error) < PID_Drive.IntegralLimit) {
			Integral += Error;
			} else {
			Integral = 0;
		}
		if(abs(Error) < RequestedAccuracy) {
			break;
		}
		Speed = (-Auton_GetMultiplier(Direction,DriveRearRight)) * ((PID_Drive.Kp * Error) + (Integral * PID_Drive.Ki) + (Derivative * PID_Drive.Kd));
		//writeDebugStreamLine("Speed=%i",Speed);
		//writeDebugStreamLine("");
		if(Speed > MaxSpeed)
			Speed = MaxSpeed;
		if(Speed < -MaxSpeed)
			Speed = -MaxSpeed;
		Auton_Drive(Direction, Speed);
		LastError = Error;
		sleep(25);
		//writeDebugStreamLine("%i",Speed);
	}
	//writeDebugStreamLine("END-DISTANCE=%i",Distance);
	Auton_Drive();
#if defined(_DEBUG)
	if(!((nSysTime - StartTime) < Timeout)) {
		writeDebugStreamLine("***WARNING: PID_Drive to distance %i timed out after %i ms (encoder reading %i)", Distance, Timeout, SensorValue[DriveEncoder]);
	}
#endif
}

#ifndef NoLCD
void Auton_WaitForKeyPress(int Sleep = 0) {
	LCD.Display.Paused = true;
	while(nLCDButtons == 0) {}
	while(nLCDButtons != 0) {}
	sleep(Sleep);
	LCD.Display.Paused = false;
}
#endif

void selftest(const string nexttest) {
	while(nLCDButtons != 0) {}
#if defined(_DEBUG)
	writeDebugStreamLine("PASSED");
	writeDebugStream(nexttest);
#endif
}

void pre_auton() {
	bStopTasksBetweenModes = true;
#ifndef NoLCD
	clearLCDLine(0);
	clearLCDLine(1);
	bLCDBacklight = true;
	displayLCDCenteredString(0, "POST IN PROGRESS");
	displayLCDCenteredString(1, "Please stand by");
#endif
#if defined(_DEBUG)
	clearDebugStream();
	writeDebugStreamLine(FILE);
	writeDebugStreamLine(" - Debug			 ON");
#ifdef ProgrammingSkills
	writeDebugStreamLine(" - Progskill	 ON");
#endif
#ifndef ProgrammingSkills
	writeDebugStreamLine(" - Progskill	OFF");
#endif
#ifndef NoInit
	writeDebugStreamLine(" - Init				 ON");
#endif
#ifdef NoInit
	writeDebugStreamLine(" - Init				OFF");
#endif
#ifndef NoLCD
	writeDebugStreamLine(" - LCD				 ON");
#endif
#ifdef NoLCD
	writeDebugStreamLine(" - LCD				OFF");
#endif
	writeDebugStreamLine(" - Batt A   %1.2fv", (float)nImmediateBatteryLevel / (float)1000);
#ifndef NoPowerExpander
	writeDebugStreamLine(" - Batt B   %1.2fv", (float)SensorValue[PowerExpander] / (float)280); // TBD: Verify magic number
#endif
	writeDebugStreamLine(" - Backup   %1.2fv", (float)BackupBatteryLevel / (float)1000);
#endif
	bPlaySounds = false;
#ifndef NoLCD
	LCD.Display.BattA = true;
#ifndef NoPowerExpander
	LCD.Display.BattB = PowerExpander;
#endif
#ifdef NoPowerExpander
	LCD.Display.BattB = (tSensors)-1; // (something) means cast to type something. Cancels warning of assigning char to tSensors variable.
#endif
	LCD.Display.Backup = true;
#endif
	SensorValue[SolenoidCubes] = 1;
#ifdef BatteryIndicators
	writeDebugStreamLine("Testing indicators...");
	TestIndicators();
	startTask(BatteryIndicate);
#endif
	if(!bIfiRobotDisabled) {
#if defined(_DEBUG)
		writeDebugStreamLine("Not disabled: exiting");
#endif
#ifndef NoLCD
		clearLCDLine(0);
		clearLCDLine(1);
		init();
		startTask(LCD_Display);
#endif
		return;
	}
#if defined(_DEBUG)
	writeDebugStreamLine("POST started");
#endif
#if defined(_DEBUG)
	writeDebugStream("9v connection: ");
#endif
	// POWER ON SELF-TEST
	while(((float)BackupBatteryLevel / (float)1000) < 2 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST ERROR");
		displayLCDCenteredString(1, "9V not connected");
	}
	selftest("9V voltage: ");
	while(((float)BackupBatteryLevel / (float)1000) < 8.7 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST WARNING");
		sprintf(POSTErrorText, "9v batt: %1.2fv", ((float)BackupBatteryLevel / (float)1000));
		displayLCDCenteredString(1, POSTErrorText);
	}
	selftest("Cortex connected: ");
	while(((float)nImmediateBatteryLevel / (float)1000) < 2 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST ERROR");
		displayLCDCenteredString(1, "No Cortex power");
	}
	selftest("Cortex voltage: ");
	while(((float)nImmediateBatteryLevel / (float)1000) < 8.5 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST WARNING");
		sprintf(POSTErrorText, "A batt: %1.2fv", ((float)nImmediateBatteryLevel / (float)1000));
		displayLCDCenteredString(1, POSTErrorText);
	}
#ifndef NoPowerExpander
	selftest("Power expander connected: ");
	while(((float)SensorValue[PowerExpander] / (float)280) < 2 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST ERROR");
		displayLCDCenteredString(1, "No batt B power");
	}
	selftest("Power expander voltage: ");
	while(((float)SensorValue[PowerExpander] / (float)280) < 8.5 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST WARNING");
		sprintf(POSTErrorText, "B batt: %1.2fv", ((float)SensorValue[PowerExpander] / (float)280));
		displayLCDCenteredString(1, POSTErrorText);
	}
#endif
	/*#ifndef NoLiftLimits
	selftest("Lift limits: ");
	while(!Lift_TrippedMin() || Lift_TrippedMax() && nLCDButtons == 0 && bIfiRobotDisabled) {
	displayLCDCenteredString(0, "POST ERROR");
	displayLCDCenteredString(1, "Check lift limits");
	}
#endif*/
	selftest("VEXnet link: ");
	while(!bVEXNETActive && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST ERROR");
		displayLCDCenteredString(1, "VEXnet link");
	}
#ifdef Pneumatics
	//selftest("Pneumatics: ");
	//SensorValue[Solenoid] = 1;
	//sleep(1000);
	SensorValue[SolenoidA] = 0;
#endif
	selftest("Drive encoder: ");
	clearLCDLine(0);
	clearLCDLine(1);
	bLCDBacklight = true;
	displayLCDCenteredString(0, "POST IN PROGRESS");
	displayLCDCenteredString(1, "Please stand by");
#ifndef NoDriveEncoder
	sleep(1000);
	SensorValue[LiftEncoder] = 0;
	SensorValue[DriveEncoder] = 0;
#endif
#ifdef HasGyro
	SensorValue[Gyroscope] = 0;
#endif
#ifndef NoDriveEncoder
	sleep(500);
	while(SensorValue[DriveEncoder] != 0 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST ERROR");
		displayLCDCenteredString(1, "Check drive enc");
	}
#endif
#ifndef OneLiftEncoder
	selftest("Lift encoder: ");
	while(SensorValue[LiftEncoder] != 0 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST ERROR");
		displayLCDCenteredString(1, "Check lift enc");
	}
#endif
#ifdef HasGyro
	selftest("Gyroscope: ");
	while(SensorValue[Gyroscope] != 0 && nLCDButtons == 0 && bIfiRobotDisabled) {
		displayLCDCenteredString(0, "POST ERROR");
		sprintf(POSTErrorText, "Gyro: %i", SensorValue[Gyroscope]);
		displayLCDCenteredString(1, POSTErrorText);
	}
#endif
#if defined(_DEBUG)
	writeDebugStreamLine("PASSED");
#endif
	while(nLCDButtons != 0) {}
#ifndef NoLCD
#if defined(_DEBUG)
	writeDebugStreamLine("Menu launched");
#endif
	LCD_Menu();
	clearLCDLine(0);
	clearLCDLine(1);
	/*displayLCDCenteredString(0, "Did you put air");
	displayLCDCenteredString(1, "in it?       YES");
	while(nLCDButtons != 4 && bIfiRobotDisabled) {}
	while(nLCDButtons == 4 && bIfiRobotDisabled) {}
	clearLCDLine(0);
	clearLCDLine(1);
	displayLCDCenteredString(0, "Are you sure?");
	displayLCDCenteredString(1, "YES");
	while(nLCDButtons != 2 && bIfiRobotDisabled) {}
	while(nLCDButtons == 2 && bIfiRobotDisabled) {}*/
	clearLCDLine(0);
	clearLCDLine(1);
	displayLCDCenteredString(0, "Position robot");
	displayLCDCenteredString(1, "press any key");
	while(nLCDButtons == 0 && bIfiRobotDisabled) {}
	while(nLCDButtons != 0 && bIfiRobotDisabled) {}
	clearLCDLine(0);
	clearLCDLine(1);
	displayLCDString(0, 0, "*** KEEP BACK ***");
	displayLCDString(1, 0, "   CALIBRATING");

	/*// Next 6 lines are verbatim from gyro sample programs
	//Completely clear out any previous sensor readings by setting the port to "sensorNone"
	SensorType[Gyroscope] = sensorNone;
	wait1Msec(1000);
	//Reconfigure Analog Port 8 as a Gyro sensor and allow time for ROBOTC to calibrate it
	SensorType[Gyroscope] = sensorGyro;
	wait1Msec(2000);*/

	clearLCDLine(0);
	clearLCDLine(1);
	startTask(LCD_Display);
#endif
#ifndef NoInit
#if defined(_DEBUG)
	writeDebugStreamLine("Starting init()");
#endif
	init();
#endif
#if defined(_DEBUG)
	writeDebugStreamLine("Ready to roll!");
#endif
}

void Auton_Lift(tVertical Direction = VSTOP, tSpeed Speed = 127, int Time = 0, bool OverrideLimits = false) {
	if(Lift_TrippedMax() && Direction == UP) {
		Auton_Lift();
		return;
	}
	if(Lift_TrippedMin() && !OverrideLimits && Direction == DOWN) {
		Auton_Lift();
		return;
	}
	motor[LiftLeftA] = Direction * Speed;
	motor[LiftLeftB] = Direction * Speed;
	motor[LiftLeftC] = Direction * Speed;
	motor[LiftRightA] = Direction * Speed;
	motor[LiftRightB] = Direction * Speed;
	motor[LiftRightC] = Direction * Speed;
	if (Time > 0) {
		sleep(Time);
		Auton_Lift();
	}
}

// if sensorvalue == newposition nothing will happen
void Auton_Lift_Targeted(tVertical Direction, int NewPosition = 0, tSpeed Speed = 127, int Timeout = 4000) {
	const int StartTime = nSysTime;
#if defined(_DEBUG)
	writeDebugStreamLine("Request to move lift to position %i at speed %i",NewPosition,Speed);
#endif
	if(NewPosition == 0) {
#if defined(_DEBUG)
		writeDebugStreamLine("Running down to 0 at speed %i", Speed);
#endif
		Auton_Lift(DOWN, Speed);
		while(!Lift_TrippedMin() && vexRT[Btn6U] != 1 && vexRT[Btn6D] != 1 && (nSysTime - StartTime) < Timeout) {
			Auton_Lift(DOWN, Speed);
		}
#if defined(_DEBUG)
		writeDebugStreamLine("Stopping lift");
#endif
		Auton_Lift();
#if defined(_DEBUG)
		writeDebugStreamLine("Done in %i ms", nSysTime - StartTime);
#endif
#if defined(_DEBUG)
		writeDebugStreamLine("Done in %i ms", nSysTime - StartTime);
		if(!((nSysTime - StartTime) < Timeout)) {
			writeDebugStreamLine("**WARNING: Lift to position %i timed out after %i ms", NewPosition, Timeout);
		}
#endif
		return;
	}
	Auton_Lift(Direction, Speed);
	if(Direction == UP) {
#if defined(_DEBUG)
		writeDebugStreamLine("Running UP");
#endif
		while(-SensorValue[LiftEncoder] < NewPosition && !Lift_TrippedMax() && vexRT[Btn6U] != 1 && vexRT[Btn6D] != 1 && (nSysTime - StartTime) < Timeout) {
			Auton_Lift(Direction, Speed);
		}
#if defined(_DEBUG)
		writeDebugStreamLine("Stopping lift");
#endif
		Auton_Lift();
#if defined(_DEBUG)
		writeDebugStreamLine("Done in %i ms", nSysTime - StartTime);
#endif
		} else if(Direction == DOWN) {
#if defined(_DEBUG)
		writeDebugStreamLine("Running DOWN");
#endif
		while(-SensorValue[LiftEncoder] > NewPosition && !Lift_TrippedMin() && vexRT[Btn6U] != 1 && vexRT[Btn6D] != 1 && (nSysTime - StartTime) < Timeout) {
			Auton_Lift(Direction, Speed);
		}
#if defined(_DEBUG)
		writeDebugStreamLine("Stopping lift");
#endif
		Auton_Lift();
#if defined(_DEBUG)
		writeDebugStreamLine("Done in %i ms", nSysTime - StartTime);
		if(!((nSysTime - StartTime) < Timeout)) {
			writeDebugStreamLine("**WARNING: Lift to position %i timed out after %i ms", NewPosition, Timeout);
		}
#endif
	}
}

bool Lift_TrippedMin() {
	return SensorValue[LiftLimitMinA] == 1 || SensorValue[LiftLimitMinB] == 1;
}

bool Lift_TrippedMax() {
	return ((SensorValue[I2C_3] + SensorValue[I2C_4]) / 2) > 1400;
}

#ifdef Pneumatics
void Claw(ClawPosition Position) {
	SensorValue[SolenoidA] = Position;
	if(LastPosition != Position) {
		LastPosition = Position;
		if(Position == OPEN) {
			NumberOfOpens++;
			TimeClosed += LastSwitched - nSysTime;
			} else {
			NumberOfCloses++;
			TimeOpen += LastSwitched - nSysTime;
		}
		LastSwitched = nSysTime;
	}
}
#endif

#if defined(_DEBUG)
void AutonDataDump() {
	writeDebugStreamLine("Autonomous finished");
	writeDebugStreamLine(" - Total time (estimated): %i:%2i.%3i",LCD_Timer_Mins(0),LCD_Timer_Secs(0,true),LCD_Timer_Msecs(0,true));
	writeDebugStreamLine(" - Batt A   %1.2fv", (float)nImmediateBatteryLevel / (float)1000);
#ifndef NoPowerExpander
	writeDebugStreamLine(" - Batt B   %1.2fv", (float)SensorValue[PowerExpander] / (float)280);
#endif
	writeDebugStreamLine(" - Backup   %1.2fv", (float)BackupBatteryLevel / (float)1000);
	LCD.Display.Paused = true;
	ResetDriveEncoders();
}
#endif
