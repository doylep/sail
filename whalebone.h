/*   Space Whales Team: Backbone Functions for Weather Balloon Ground Station
		           Last Updated March 11, 2013
	              Released under GNU GPL - any version
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

// Command Delimeter
const char CMDDLIM = '@';

// Default Command
const string DFLTCMD = "OKAY@";

// Data Delimeter
const char DATDLIM = '#';

// Conversion Constant
const double DCONV = 0.016666666666667;

// Float Output Precision
const int PRECSN = 10;

// Low Voltage
const double LOWV = 3.3;


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
};


//////////////////////////////////////////////////////////////////////////////
/// Backbone Functions
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

void writeHTML(const string &dfilenm, const int mapdlay);
// MODIFIES: cout
// EFFECTS: Writes the data in <dfilenm> to a map in GPSmap.html with a
//		delay of <mapdlay>, throws an err

void writeHeader(ofstream &maphtml, const int mapdlay);
// REQUIRES: maphtml points to an open ofstream
// MODIFIES: maphtml
// EFFECTS: Prints the HTML header to the file for GPS mapping with refresh
//		delay <mapdlay>, prints success to the terminal and throws
//		an error string for failure

void writePts(ofstream &maphtml, const string &dfilenm);
// REQUIRES: maphtml points to an open ofstream, dfilenm is the name of
//		valid datafile
// MODIFIES: maphtml, cout
// EFFECTS: Write the the lat and lon points in the file <dfilenm> to the
//		HTML of maphtml

void writeEnd(ofstream &maphtml);
// REQUIRES: maphtml points to an open ofstream
// MODIFIES: maphtml
// EFFECTS: Prints the HTML end to the file for the GPS mapping

void sendCMD(const string &cfilenm, const int comnum);
// REQUIRES: cfilenm is the name of a valid command file, comnum is the
//		serial port in use
// MODIFIES: cout
// EFFECTS: Sends a valid command in <cfilenm> (see README) if it exists
//		and sends a default command if not

#endif // __WHALEBONE_H__
