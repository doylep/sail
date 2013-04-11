/*   Space Whales Team: Backbone Functions for Weather Balloon Ground Station
		           Last Updated March 11, 2013
		   Released under GNU GPL - version 2 or later
		              By The Space Whales                   	 */


#ifndef __WHALEBONE_H__
#define __WHALEBONE_H__

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

// Choose Appropriate Port Library
#ifdef _LOCALTEST_
#include "rsfile.h"
# else
#include "rs232.h"
#endif

using namespace std;


//////////////////////////////////////////////////////////////////////////////
/// Global Constants
//////////////////////////////////////////////////////////////////////////////

// Serial Buffer Size
const int MAXBUF = 1000;

// Maximum Number of Data Points
const int MAXPTS = 4000;

// Maximum GPS Data Point Size
const int MAXGPSDAT = 50;

// Maximum Command String Size
const int MAXCMD = 100;

// Program Termination Command
const string STOPSAIL = "Stop";

// Program Silent Mode Command
const string SLNTMODE = "Silent";

// Command Delimeter
const char CMDDLIM = '@';

// Data Field Delimeter
const char DATDLIM = '#';

// Sensor Data Delimeter
const char SENSDLIM = ',';

// Float Output Precision
const int PRECSN = 10;

// GPS Data Conversion Constant
const double DCONV = 0.016666666666667;

// Accelerometer Calibration
const double ACCALSUB[3] = {342, 342.5, 349};
const double ACCALMULT[3] = {0.014285714, 0.013986014, 0.014492754};


//////////////////////////////////////////////////////////////////////////////
/// Program Structure
//////////////////////////////////////////////////////////////////////////////

struct Param{
// OVERVIEW: Struct that contains all the parameters for the program instance

	ofstream datfile;	// stream that connects to the datafile
	string dfilenm;		// name of the datafile
	ifstream cmdfile;	// stream that connects to the commandfile
	string cfilenm;		// name of the commandfile
	int dlay;		// delay between each data read
	int mapdlay;		// delay between each map refresh
	int comnum;		// number of the com port
	int baud;		// baud rate for the com port
	string dfltcmd;		// default command to send to the balloon
};


//////////////////////////////////////////////////////////////////////////////
/// Base Function Declarations
//////////////////////////////////////////////////////////////////////////////

void castArray(const char array1[], unsigned char array2[], const int arr_size);
void castArray(const unsigned char array1[], char array2[], const int arr_size);
// REQUIRES: array1 and array 2 have at least arr_size elements
// MODIFIES: array2
// EFFECTS: Copies the first <arr_size> elements of array1 into array2 making
//		the appropriate static_cast

bool loadParam(Param &inst, const string &filenm);
// MODIFIES: inst if <filenm> can be opened
// EFFECTS: Tries to load program parameters from <filenm> into inst, returns
//		true if successful and false otherwise

void promptParam(Param &inst);
// MODIFIES: inst
// EFFECTS: Prompts for all program parameters and places them in inst

bool openDFile(ofstream &file, const string &filenm);
// MODIFIES: file, cout
// EFFECTS: Opens <file> at file <filenm> in append mode, prints an error and
//		returns false if the file does not open

bool openCFile(ifstream &file, const string &filenm);
// MODIFIES: file, cout
// EFFECTS: Opens <file> at file <filenm>, prints an error and returns false if
//		the file does not open

void openPort(int &comnum, int &baud);
// MODIFIES: Sets comnum to the port that is opened and baud to its baudrate,
//		modifies cin and cout
// EFFECTS: Prompts cin until <comnum> and <baud> successfully opens port
//		<comnum> with baudrate <baud>

bool sendCMD(Param &inst);
// REQUIRES: inst.cfilenm is the name of a valid command file, inst.comnum is the
//		serial port in use
// MODIFIES: cout
// EFFECTS: Returns 1 if the <STOPSAIL> is in <cfilenm>, Prints to the cout if
//		<SLNTMODE> is in <cfilenm>, otherwise sends a valid command in
//		<cfilenm> (see README) if it exists and sends a default command
//		if not

#endif // __WHALEBONE_H__
