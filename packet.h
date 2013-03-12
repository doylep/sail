/*   Space Whales Team: Packet Class for Weather Balloon Ground Station
		        Last Updated March 11, 2013
	              Released under GNU GPL - any version
		            By The Space Whales                   	*/


#ifndef __PACKET_H__
#define __PACKET_H__

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "whalebone.h"

using namespace std;


//////////////////////////////////////////////////////////////////////////////
/// Class Function Declarations
//////////////////////////////////////////////////////////////////////////////

class Packet{
// OVERVIEW: Class that handles the data transmitted in the weather
// balloon packet

	string echo;		// balloon echo
	double pres;		// pressure
	double humd;		// humidity
	double accel[3];	// acceleration data
	double temp[2]; 	// temperature data
	double lat;		// latitude
	double lng;		// longitude
	bool gps;		// flag to indicate good gps data
	bool sens;		// flag to indicate good sensor data

	void verifyPacket(const string &raw);
	// MODIFIES: cout
	// EFFECTS: Tries to verify the contents of raw, prints success to the
	//		terminal and throws an error string for failure

	void parseSens(const string &raw);
	// MODIFIES: this, cout
	// EFFECTS: Parses sensor data at the beginning of raw and places them
	//		into this, prints success to the terminal and throws
	//		an error string for failure

	void parseGPS(const string &raw);
	// MODIFIES: this, cout
	// EFFECTS: Parses NMEA strings in data and places them into this,
	//		prints success to the terminal and throws an error
	//		string for failure

	double convrtData(const string &raw);
	// REQUIRES: raw is valid NMEA GPGGA string
	// EFFECTS: Returns the string converted to decimal degrees

public:
	void parseData(const unsigned char *buff, const int buff_size);
	// REQUIRES: buff is NULL terminated
	// MODIFIES: this, cout
	// EFFECTS: Takes data and places it into this, prints progess and any
	//		errors to cout

	void writeData(Param &inst);
	// MODIFIES: inst.datfile, cout
	// EFFECTS: Tries to write pket data to <inst.dfilenm>, prints success to
	//		the terminal and throws an error string for failure

};

#endif // __PACKET_H__
