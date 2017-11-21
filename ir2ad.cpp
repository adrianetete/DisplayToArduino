#define MIN_WIN_VER 0x0501

#ifndef WINVER
#	define WINVER			MIN_WIN_VER
#endif

#ifndef _WIN32_WINNT
#	define _WIN32_WINNT		MIN_WIN_VER 
#endif

#pragma warning(disable:4996) //_CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <signal.h>
#include <time.h>

#include "irsdk_defines.h"
#include "irsdk_client.h"
#include "serial.h"

// for timeBeginPeriod
#pragma comment(lib, "Winmm")

// bool, driver is in the car and physics are running
// shut off motion if this is not true
irsdkCVar g_playerInCar("IsOnTrack");
irsdkCVar g_playerInGarage("IsInGarage");

irsdkCVar g_drsStatus("DRS_Status");
irsdkCVar g_engineWarnings("EngineWarnings");

// Fuel
irsdkCVar g_fuelPitService("PitSvFuel");
irsdkCVar g_fuelLevel("FuelLevel");

// float, cars velocity in m/s
irsdkCVar g_carVelX("VelocityX");
irsdkCVar g_carVelY("VelocityY");
irsdkCVar g_carVelZ("VelocityZ");

irsdkCVar g_carShiftPCT("ShiftIndicatorPct");
irsdkCVar g_ShiftGrindRPM("ShiftGrindRPM");
irsdkCVar g_ShiftPowerPct("ShiftPowerPct");

irsdkCVar g_carSpeed("Speed");
irsdkCVar g_carGear("Gear");
irsdkCVar g_LapLastTime("LapLastLapTime");
irsdkCVar g_DeltaToBestLap("LapDeltaToSessionBestLap");
irsdkCVar g_DeltaToBestLap_DD("LapDeltaToBestLap_DD");

Serial serial;
int maxScreen = 3;
int showScreen = 1;

int speed;
int fuel;
int gear;
int drs;
float rpmPercent;
int delta;
float lastTime;
float lapCurrentTime;
byte deltaSign;

// Session String
const int MAX_STR = 512;
char tstr[MAX_STR];
char tpath[MAX_STR];
int driverCarIdx = 0;

// Pit Fuel
float pitFuel = 0;
float  fuelLevel = 0;

byte convertIntToDigit(int num) {

	switch (num)
	{
	case -1: return 0xBF; break;
	case 0: return 0xC0; break;
	case 1: return 0xF9; break;
	case 2: return 0xA4; break;
	case 3: return 0xB0; break;
	case 4: return 0x99; break;
	case 5: return 0x92; break;
	case 6: return 0x82; break;
	case 7: return 0xF8; break;
	case 8: return 0x80; break;
	case 9: return 0x90; break;
	default: return 0xFF; break;
	}
}

byte convertIntToDigit2(int num) {

	switch (num)
	{
	case -1: return 0xBF; break;
	case 0: return 0xFF; break;
	case 1: return 0xF9; break;
	case 2: return 0xA4; break;
	case 3: return 0xB0; break;
	case 4: return 0x99; break;
	case 5: return 0x92; break;
	case 6: return 0x82; break;
	case 7: return 0xF8; break;
	case 8: return 0x80; break;
	case 9: return 0x90; break;
	default: return 0xFF; break;
	}
}

byte convertIntToDigitPoint(int num) {

	switch (num)
	{
	case -1: return 0xBF; break;
	case 0: return 0x40; break;
	case 1: return 0x79; break;
	case 2: return 0x24; break;
	case 3: return 0x30; break;
	case 4: return 0x19; break;
	case 5: return 0x12; break;
	case 6: return 0x02; break;
	case 7: return 0x78; break;
	case 8: return 0x00; break;
	case 9: return 0x10; break;
	default: return 0xFF; break;
	}
}

byte convertGearToDigit(int num) {

	switch (num)
	{
	case -1: return 0x88; break;
	case 0: return 0xBF; break;
	case 1: return 0xF9; break;
	case 2: return 0xA4; break;
	case 3: return 0xB0; break;
	case 4: return 0x99; break;
	case 5: return 0x92; break;
	case 6: return 0x82; break;
	case 7: return 0xF8; break;
	case 8: return 0x00; break;
	default: return 0xFF; break;
	}
}

byte convertRPMToLEDS1(float rpmPerc) {

	byte res = 0x00;

	if ((rpmPerc >= 0.01) && (rpmPerc < 7.14)) {
		res = 0x80;
	}
	else if ((rpmPerc >= 7.14) && (rpmPerc < 7.14*2)) {
		res = 0xC0;
	}
	else if ((rpmPerc >= 7.14*2) && (rpmPerc < 7.14*3)) {
		res = 0xE0;
	}
	else if ((rpmPerc >= 7.14*3) && (rpmPerc < 7.14*4)) {
		res = 0xF0;
	}
	else if ((rpmPerc >= 7.14*4) && (rpmPerc < 7.14*5)) {
		res = 0xF8;
	}
	else if ((rpmPerc >= 7.14*5) && (rpmPerc < 7.14*6)) {
		res = 0xFC;
	}
	else if ((rpmPerc >= 7.14*6) && (rpmPerc < 7.14*7)) {
		res = 0xFE;
	}
	else if (rpmPerc >= 7.14*7) {
		res = 0xFF;
	}
	else {
		res = 0x00;
	}

	// En caso de que se active el PitLimiter
	if ((g_engineWarnings.getInt() == 16) || (g_engineWarnings.getInt() == 48)) res = 0x22;
	return res;
}

byte convertRPMToLEDS2(float rpmPerc) {

	byte res = 0x00;

	if ((rpmPerc >= 7.14*8) && (rpmPerc < 7.14*9)) {
		res = 0x80;
	}
	else if ((rpmPerc >= 7.14*9) && (rpmPerc < 7.14*10)) {
		res = 0xC0;
	}
	else if ((rpmPerc >= 7.14*10) && (rpmPerc < 7.14*11)) {
		res = 0xE0;
	}
	else if ((rpmPerc >= 7.14*11) && (rpmPerc < 7.14*12)) {
		res = 0xF0;
	}
	else if ((rpmPerc >= 7.14*12) && (rpmPerc < 7.14*13)) {
		res = 0xF8;
	}
	else if ((rpmPerc >= 7.14*13) && (rpmPerc < 7.14*14)) {
		res = 0xFC;
	}
	else if (rpmPerc >= 7.14*14) {
		res = 0xFF;
	}
	else {
		res = 0x00;
	}

	// En caso de que se active el PitLimiter
	if ((g_engineWarnings.getInt() == 16) || (g_engineWarnings.getInt() == 48)) res = 0x20;
	return res;
}

byte convertRPMToLEDS3(float rpmPerc, int drs) {

	byte res = 0x00;

	if ((rpmPerc >= 0.01) && (rpmPerc < 4.0)) {
		res = 0x08;
	}
	else if ((rpmPerc >= 4.0) && (rpmPerc < 7.0)) {
		res = 0x0C;
	}
	else if ((rpmPerc >= 7.0) && (rpmPerc < 10.0)) {
		res = 0x0E;
	}
	else if ( rpmPerc >= 10.0 ) {
		res = 0x0F;
	}

	if (drs == 1) res = res | 0x80;
	if (drs == 2) res = res | 0xC0;
	if (drs == 3) res = res | 0xF0;

	// En caso de que se active el PitLimiter
	if ( (g_engineWarnings.getInt() == 16) || (g_engineWarnings.getInt() == 48) ) res = 0x22;
	return res;
}

byte convertRPMToLEDS4(float rpmPerc) {

	byte res = 0x00;

	if ((rpmPerc >= 10.0) && (rpmPerc < 14.0)) {
		res = 0x80;
	}
	else if (rpmPerc >= 14.0) {
		res = 0xFF;
	}
	else {
		res = 0x00;
	}

	// En caso de que se active el PitLimiter
	if ( (g_engineWarnings.getInt() == 16) || (g_engineWarnings.getInt() == 48) ) res = 0x20;
	return res;
}

void thread_getKey(void *arg) {
	
		
}

void ex_program(int sig) 
{
	(void)sig;

	printf("recieved ctrl-c, exiting\n\n");

	serial.closeSerial();
	timeEndPeriod(1);

	signal(SIGINT, SIG_DFL);
	exit(0);
}

bool init()
{
	// trap ctrl-c
	signal(SIGINT, ex_program);
	printf("press enter to exit:\n\n");
	
	// bump priority up so we get time from the sim
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	// ask for 1ms timer so sleeps are more precise
	timeBeginPeriod(1);
	
	// enumerate serial ports and pick the highest port
	//****FixMe, in a real program you would want to handle the port going away and coming back again in a graceful manner.
	int portList[32];
	int portCount = 32;
	//int port = serial.enumeratePorts(portList, &portCount);
	int port = 3;
	// open serial, hopefully this is the arduino
	if(serial.openSerial(port, CBR_9600))
			
		return true;
	else
		printf("failed to open port %d\n", port);

	return false;
}

void monitorConnectionStatus()
{
	// keep track of connection status
	bool isConnected = irsdkClient::instance().isConnected();
	static bool wasConnected = !isConnected;
	if(wasConnected != isConnected)
	{
		if(isConnected)
			printf("Connected to iRacing\n");
		else
			printf("Lost connection to iRacing\n");
		wasConnected = isConnected;
	}
}

void run()
{
	// wait up to 16 ms for start of session or new data
	if(irsdkClient::instance().waitForData(16))
	{
		// and grab the data
		if(g_playerInCar.getBool())
		{
			/*
			****** Caracteres Display *******
			0b11000000;  0xC0;  // caracter[ 0 ]
			0b11111001;  0xF9;  // caracter[ 1 ]
			0b10100100;  0xA4;  // caracter[ 2 ]
			0b10110000;  0xB0;  // caracter[ 3 ]
			0b10011001;  0x99;  // caracter[ 4 ]
			0b10010010;  0x92;  // caracter[ 5 ]
			0b10000010;  0x82;  // caracter[ 6 ]
			0b11111000;  0xF8;  // caracter[ 7 ]
			0b10000000;  0x80;  // caracter[ 8 ]
			0b10010000;  0x90;  // caracter[ 9 ]
			0b10111111;  0xBF;  // caracter[ - ]
			0b10001000;  0x88;  // caracter[ R ]
			*/

			speed = static_cast<int>(g_carSpeed.getFloat() * 3.6 + 0.5);
			lastTime = g_LapLastTime.getFloat() * 1000;
			gear = g_carGear.getInt();
			rpmPercent = g_carShiftPCT.getFloat() * 100;
			
			delta = static_cast<int>(g_DeltaToBestLap.getFloat() * 1000 + 0.5);

			if (delta < 0) {
				deltaSign = 0xBF;
			}
			else {
				deltaSign = 0xFF;
			}

			delta = abs(delta);
						
			// Cast time to MM:SS
			lastTime = lastTime + 40000 * (((int)lastTime / 10000) / 6);
			if (lastTime <= 0) lastTime = 0;

			pitFuel = g_fuelPitService.getFloat();
			fuelLevel = g_fuelLevel.getFloat() * 10;

			printf("PitFuel: %f, FuelLevel: %f - Screen: %d \n", pitFuel, fuelLevel, showScreen);

			//printf("SpeedInt: %d, Gear: %d \n", speed, gear);
			
			// Receive carIdx in "tstr" for getting car name
			if (1 == irsdkClient::instance().getSessionStrVal("DriverInfo:DriverCarIdx:", tstr, MAX_STR))
			{
				// Cast carId to int
				driverCarIdx = atoi(tstr);

				// Format the string "tpath" with de carIdx
				_snprintf(tpath, MAX_STR, "DriverInfo:Drivers:CarIdx:{%d}CarScreenNameShort:", driverCarIdx);
				tpath[MAX_STR - 1] = '\0';

				// Get the info in tstr
				irsdkClient::instance().getSessionStrVal(tpath, tstr, MAX_STR);
			}

			if (strcmp(tstr, "Mclaren MP4-30") == 0) {

				// Si el coche es el F1 cambiamos la forma de mover los LED y añadimos el DRS
				/**
				*  McLaren MP4-30
				*
				*  - DRS Status can now be seen in telemetry. The variable has four possible values:
				*  - - 0 = Inactive (None LED)
				*  - - 1 = Available in Next Zone (First LED)
				*  - - 2 = Available and in a DRS Zone (Second LED)
				*  - - 3 = Active (Third and four LED)
				*/
				drs = g_drsStatus.getInt();

				if (showScreen == 2)
				{
					unsigned char trama[] = {

						0xCA,
						convertIntToDigit(int(lastTime) % 10),
						convertIntToDigit(int(lastTime / 10) % 10),
						convertIntToDigit(int(lastTime / 100) % 10),
						convertIntToDigitPoint(int(lastTime / 1000) % 10),
						convertIntToDigit(int(lastTime / 10000) % 10) ,
						convertIntToDigitPoint(int(lastTime / 100000) % 10) ,
						convertIntToDigit2(int(lastTime / 1000000) % 10) ,
						0xFF,

						convertRPMToLEDS3(rpmPercent, drs),
						convertRPMToLEDS4(rpmPercent),

						convertGearToDigit(gear)
					};

					serial.writeSerial(trama, 12);					
				}
				else if (showScreen == 3) {

					unsigned char trama[] = {

						0xCA,
						0xC7,
						convertIntToDigit(int(fuelLevel) % 10),
						convertIntToDigitPoint(int(fuelLevel / 10) % 10),
						convertIntToDigit(int(fuelLevel / 100) % 10),
						convertIntToDigit(int(fuelLevel / 1000) % 10),
						0xFF,
						0xFF,
						0xFF,

						convertRPMToLEDS1(rpmPercent),
						convertRPMToLEDS2(rpmPercent),

						convertGearToDigit(gear)
					};

					serial.writeSerial(trama, 12);

				}
				else {

					unsigned char trama[] = {

						0xCA,
						convertIntToDigit(int(delta / 10) % 10),
						convertIntToDigit(int(delta / 100) % 10),
						convertIntToDigitPoint(int(delta / 1000) % 10),
						deltaSign,
						0xFF,
						convertIntToDigit(speed % 10),
						convertIntToDigit(int(speed / 10) % 10),
						convertIntToDigit2(int(speed / 100) % 10),

						convertRPMToLEDS3(rpmPercent, drs),
						convertRPMToLEDS4(rpmPercent),

						convertGearToDigit(gear)
					};

					serial.writeSerial(trama, 12);
				}

			} else {

				if (showScreen == 2)
				{

					unsigned char trama[] = {

						0xCA,
						convertIntToDigit(int(fuelLevel) % 10),
						convertIntToDigit(int(fuelLevel / 10) % 10),
						convertIntToDigit(int(fuelLevel / 100) % 10),
						0xFF,
						0xFF,
						0xFF,
						0xFF,
						0xFF,

						convertRPMToLEDS1(rpmPercent),
						convertRPMToLEDS2(rpmPercent),

						convertGearToDigit(gear)
					};

					serial.writeSerial(trama, 12);

				}
				else if (showScreen == 3) {

					unsigned char trama[] = {

						0xCA,
						convertIntToDigit(int(lastTime) % 10),
						convertIntToDigit(int(lastTime / 10) % 10),
						convertIntToDigit(int(lastTime / 100) % 10),
						convertIntToDigitPoint(int(lastTime / 1000) % 10),
						convertIntToDigit(int(lastTime / 10000) % 10) ,
						convertIntToDigitPoint(int(lastTime / 100000) % 10) ,
						convertIntToDigit2(int(lastTime / 1000000) % 10) ,
						0xFF,

						convertRPMToLEDS1(rpmPercent),
						convertRPMToLEDS2(rpmPercent),

						convertGearToDigit(gear)
					};

					serial.writeSerial(trama, 12);
				}
				else {

					unsigned char trama[] = {

						0xCA,
						convertIntToDigit(int(delta / 10) % 10),
						convertIntToDigit(int(delta / 100) % 10),
						convertIntToDigitPoint(int(delta / 1000) % 10),
						deltaSign,
						0xFF,
						convertIntToDigit(speed % 10),
						convertIntToDigit(int(speed / 10) % 10),
						convertIntToDigit2(int(speed / 100) % 10),

						convertRPMToLEDS1(rpmPercent),
						convertRPMToLEDS2(rpmPercent),

						convertGearToDigit(gear)
					};

					serial.writeSerial(trama, 12);
				}
			}
		}
				
	} else {
		unsigned char trama[] = {

			0xCA,

			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0xFF,
			0xFF,

			0xFF,
			0xFF,

			0xFF
		};
	}

	// your normal process loop would go here
	monitorConnectionStatus();
}

int main(int argc, char *argv[])
{
	printf("ir2ad 1.0\n");
	printf(" send iracing data to arduino\n\n");
	
	short prev_screen = 0, curr_screen = 0;

	if(init())
	{
		while(1)
		{
			run();
			
			if (GetAsyncKeyState(0x36))
				curr_screen = 1;
			else
				curr_screen = 0;

			if (prev_screen != curr_screen)
			{
				if (curr_screen) {

					// Pusla boton
					if (showScreen < maxScreen)
						showScreen++;
					else
						showScreen = 1;
				}
				else {
					// Suelta boton
				}

				prev_screen = curr_screen;
			}


		}

		printf("Shutting down.\n\n");
		serial.closeSerial();
		timeEndPeriod(1);
	}
	else
		printf("init failed\n");

	return 0;
}
