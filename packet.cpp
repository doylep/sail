/*   Space Whales Team: Packet Class for Weather Balloon Ground Station
		        Last Updated March 11, 2013
	              Released under GNU GPL - any version
		            By The Space Whales                   	*/


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "packet.h"
#include "whalebone.h"

using namespace std;


//////////////////////////////////////////////////////////////////////////////
/// Class Function Implementations
//////////////////////////////////////////////////////////////////////////////

void Packet::verifyPacket(const string &raw)
{
	// Check for Any Data
	if (raw.size() == 0) {
		string error = "No Data Received.\n";
		throw error;
	}

	// Check for Valid Sensor Data
	int ind = raw.find(DATDLIM);
	if ((raw.find(DATDLIM, ind + 1) - ind) < 24) {

		// Look for Temperature Data in First Section
		if (ind < 24) {
			string error = "Invalid Sensor Data.\n";
			throw error;
		}

		// Indicate Missing Echo
		throw ind;
	}

	// Indicate Success
	cout << "Success.\n";
}

void Packet::parseSens(const string &raw)
{
	// Get pressure data
	int ind = 0;
	string var = raw.substr(ind, 3);

	// Attempt Extraction
	if (!(pres = atof(var.c_str()))) {
		string error = "Invalid Pressure.\n";
		throw error;
	}

	// Convert pressure
	pres /= (1024 * 0.00776);

	// Get Humidity Data
	ind += 4;
	var = raw.substr(ind, 3);
	if (!(humd = atof(var.c_str()))) {
		string error = "Invalid Humidity.\n";
		throw error;
	}

	// Convert Humidity
	humd = ((LOWV * humd / 1024) - 0.958) / 0.03068;

	// Get Accelerometer Data
	for (int i = 0; i < 3; ++i) {

		// Extract Data
		var = raw.substr(ind, 2);
		if (!(accel[i] = atof(var.c_str()))) {
			string error = "Invalid Acceration.\n";
			throw error;
		}

		// Covert Data
		accel[i] = ((LOWV * accel[i] / 1024) - 1.0725) * (-1000 / .22);

		// Move to Next Point
		ind = ind + 3;
	}

	// Get Temperature Data
	for (int i = 0; i < 2; ++i) {

		// Extract Data
		var = raw.substr(ind, 3);
		if (!(temp[i] = atof(var.c_str()))) {
			string error = "Invalid Temperature.\n";
			throw error;
		}

                // Convert Data
                temp[i] = (LOWV * temp[i] / 1024) / .01 - 32;

                // Move to Next Point
                ind = ind+4;
	}

	// Indicate Success
	cout << "Success.\n";
}

void Packet::parseGPS(const string &raw)
{
	// Local Variables
	int ind = 0;
	string tmp;

	// Check for "$GPGGA"
	if (raw.find("$GPGGA") == string::npos) {
		string error = "Unable to find $GPGGA.\n";
		throw error;
	}

	// Verify Length
	ind = raw.find("$GPGGA");
	if ((raw.size() - ind) < 42) {
		string error = "Truncated GPS Data.\n";
		throw error;
	}

	// Interate Through 2 Commas
	for (int i = 0; i < 2; ++i) {
		tmp = raw.substr(ind, raw.size());
		ind = ind + tmp.find(",") + 1;
	}

	// Check That There isn't a Comma
	if (raw.at(ind) == ',') {
		string error = "No Latitude Data.\n";
		throw error;
	}

	// Check Latitude Characters
	bool valid = true;
	for (int i = 0; i < 9; ++i) {

		// Don't Check the Period
		if (i != 4) {

			// Verify all are Numbers
			char num = raw.at(ind + i);
			if ((num > '9') || (num < '0')) {
				valid = false;
				break;
			}
		}
	}

	// Try to Process Latitude
	if (valid) {

		// Extract Number and Shift Decimal Point
		lat = convrtData(raw.substr(ind, 9));
		char dir = raw.at(ind + 10);

		// Correct North/South
		if (dir == 'S')
			lat = -lat;

	// Else Throw Error
	} else {
		string error = "Bad Latitude Format.\n";
		throw error;
	}

	// Check That There isn't a Comma
	if (raw.at(ind + 12) == ',') {
		string error = "No Longitude Data.\n";
		throw error;
	}

	// Check Longitude Characters
	for (int i = 0; i < 10; ++i) {

		// Don't Check the Period
		if (i != 5) {
			char num = raw.at(ind + i + 12);

			// Verify all are Numbers
			if ((num > '9') || (num < '0')) {
				valid = false;
				break;
			}
		}
	}

	// Try to Process Longitude
	if (valid) {

		// Extract Number and Shift Decimal Point
		lng = convrtData(raw.substr(ind + 12, 10));
		char dir = raw.at(ind + 23);

		// Correct East/West
		if (dir == 'W')
			lng = -lng;

	// Else Throw Error
	} else {
		string error = "Bad Longitude Format.\n";
		throw error;
	}

	// Indicate Success
	cout << "Success.\n";
}

double Packet::convrtData(const string &raw)
{
	// Extract the Minutes
	string smin = raw.substr(raw.size() - 7, 7);
	double fixd = atof(smin.c_str());

	// Convert the Minutes
	fixd *= DCONV;

	// Add the Degrees
	string sdeg = raw.substr(0, raw.size() - 7);
	fixd += atof(sdeg.c_str());

	// Return Result
	return fixd;
}

void Packet::parseData(const unsigned char *buff, const int buff_size)
{
	// Repackage Packet Data
	char tmp[MAXBUF]; // create a char[] to transfer data
	castArray(buff, tmp, buff_size);
	string data = tmp;
	int ind = 0;
	echo = "";

	// Try to Verify Data Packet
	try {
		cout << "Verifying data packet ..... \t";
		verifyPacket(data);

		// Remove Received Command
		ind = data.find(DATDLIM);
		if (ind)
			echo = data.substr(0, ind);
		data = data.substr(ind + 1);

	// Catch Unusable Packet
	} catch (string error) {
		cout << "Error: " << error;
		gps = false;
		sens = false;
		return;

	// Catch Misplaced Sensor Data
	} catch (...) { }

	// Print Received Command
	cout << "Balloon received \"" << echo << "\".\n";

	// Try to Parse Sensor Data
	try {
		cout << "Parsing sensor data ..... \t";
		parseSens(data);
		sens = true;

	} catch (string error) {
		cout << "Error: " << error;
		sens = false;
	}

	// Try to Parse GPS Data
	ind = data.find(DATDLIM);
	data = data.substr(ind + 1);
	try {
		cout << "Parsing GPS data ..... \t\t";
		parseGPS(data);
		gps = true;

	// Catch Bad GPS Data
	} catch (string error) {
		cout << "Error: " << error;
		gps = false;
	}
}

void Packet::writeData(Param &inst)
{
	// Write Data to File
	cout << "Writing to file ..... \t\t";
	if (openDFile(inst.datfile, inst.dfilenm)) {

		// Write Sensor Data
		if (sens)
			inst.datfile << pres << "\t" << humd << "\t"
				<< accel[0] << "\t" << accel[1] << "\t"
				<< accel[2] << "\t" << temp[0] << "\t"
				<< temp[2] << "\t";
		else
			inst.datfile << "\t\t\t\t\t\t\t";

		// Write GPS Data
		if (gps)
			inst.datfile << lat << "\t" << lng << "\t\n";
		else
			inst.datfile << "\t\t\n";

		// Save Datafile
		inst.datfile.close();

		// Indicate Success
		cout << "Success.\n";

	// Throw Error
	} else {
		string error = "Error opening the datafile.\n";
		throw error;
	}
}

