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

	double extractSens(string &raw);
	// MODIFIES: raw, cout
	// EFFECTS: Removes any data up to and including SENSDLIM from raw,
	//		returns it as a double if possible, throws string
	//		"Invalid" if not

	void parseSens(string &raw);
	// MODIFIES: this, raw, cout
	// EFFECTS: Extracts sensor data at the beginning of raw and places it
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

public:
	void parseData(const unsigned char *buff, const int buff_size);
	// REQUIRES: buff is NULL terminated
	// MODIFIES: this, cout
	// EFFECTS: Takes data and places it into this, prints progess and any
	//		errors to cout

	void writeData(Param &inst);
	// MODIFIES: inst.datfile, cout, GPSmap.html
	// EFFECTS: Tries to write pket data to <inst.dfilenm>, if successful
	//		writes the data in <dfilenm> to a map in GPSmap.html with
	//		a delay of <mapdlay>, prints success to the terminal and
	//		throws an error string for failure

	void writeHTML(const string &dfilenm, const int mapdlay);
	// MODIFIES: cout
	// EFFECTS: Writes the data in <dfilenm> to a map in GPSmap.html with a
	//		delay of <mapdlay>, prints success to the terminal and
	//		throws an error string for failure or no data

};

#endif // __PACKET_H__
