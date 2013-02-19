/*     Space Whales Team: Data Packet to Google Maps
            Last Updated February 15, 2013
		 By The Space Whales                 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include "rs232.h"

// Choose Approprite System Library
#ifdef __GNUC__
#include <unistd.h>
#else
#include <Windows.h>
#endif

using namespace std;


//////////////////////////////////////////////////////////////////////////////
/// Global Constants
//////////////////////////////////////////////////////////////////////////////

// Serial Buffer Size
const int MAXBUF = 1000;

// Maximum Number of Data Points
const int MAXPTS = 1000;

// Maximum GPS Data Point Size
const int MAXGPSDAT = 50;

// Maximum Command String Size
const int MAXCMD = 100;

// Command Delimeter
const char CMDDLIM = '$';

// Default Command
const string DFLTCMD = "OKAY$";


//////////////////////////////////////////////////////////////////////////////
/// Internal Structures
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
	int pkts;		// number of data packets received
};


//////////////////////////////////////////////////////////////////////////////
/// Internal Functions
//////////////////////////////////////////////////////////////////////////////

template <typename T, typename R>
void castArray(const T array1[], R array2[], const int arr_size);
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
//		returns 0 if the file does not open

bool openCFile(ifstream &file, const string &filenm);
// MODIFIES: file, cout
// EFFECTS: Opens <file> at file <filenm>, prints an error and returns 0 if
//		the file does not open

void openPort(int &comnum, int &baud);
// MODIFIES: Sets comnum to the port that is opened and baud to its baudrate,
//		modifies cin and cout
// EFFECTS: Prompts cin until <comnum> and <baud> successfully opens port
//		<comnum> with baudrate <baud>

bool verifyPacket(const unsigned char *buff);
// EFFECTS: verifies that the data packet is complete

void parseData(char *data, int &data_size);
// MODIFIES: data, data_size
// EFFECTS: Parses NMEA strings in data, places them pack into data, and
//		changes the size of data_size to match the new length of data

void writeHTML(const string &dfilenm, const int mapdlay, const int pkts);
// MODIFIES: cout
// EFFECTS: Writes the data in <dfilenm> to a map in GPSmap.html assuming
//		<pkts> data packets with a delay of <mapdlay>

void writeHeader(ofstream &maphtml, const int mapdlay);
// REQUIRES: maphtml points to an open ofstream
// MODIFIES: maphtml
// EFFECTS: Prints the HTML header to the file for GPS mapping with refresh
//		delay <mapdlay>

void writePts(ofstream &maphtml, const string &dfilenm, const int pkts);
// REQUIRES: maphtml points to an open ofstream, dfilenm is the name of
//		valid datafile with no more than <pkts> datapoints
// MODIFIES: maphtml
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


//////////////////////////////////////////////////////////////////////////////
/// Main Function
//////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
	// Program Instance Information
	Param inst;

	// Program Header
	cout << "\nBalloon Data - by the Space Whale team\nVersion 0.2.9"
		<< "\n========================================\n";

	// Try to Load Parameters from File
	try {
		// Check for "-s"
		if (argc != 1 && argv[1][0] == '-' && argv[1][1] == 's') {

			// Check for No File Specification
			if (argc == 2)
				throw false;

			// Try to Load Specified File
			if (!loadParam(inst, argv[2]))
				throw false;

		// Try to Load Default
		} else {
			string dconfg = "default.config";
			if (!loadParam(inst, dconfg.c_str()))
				throw false;
		}
	// Catch Failure to Load Parameters
	} catch (...) {
		promptParam(inst);
	}

	// Read data from the port until the cycle is broken
	while (true) {

		// Pause For Dlay Seconds
		cout << "Waiting for " << inst.dlay << " seconds.\n";
		#ifdef __GNUC__
	/// Commented out for compatability with MinGW
			//sleep(inst.dlay);
		#endif
		#ifdef _WIN32
			_sleep(inst.dlay * 1000);
		#endif

		// Read data from the port
		cout << "Reading from port ..... \t";
		unsigned char buff[MAXBUF];
		int buff_size = RS232_PollComport(inst.comnum, buff, MAXBUF - 1);
		cout << "Success.\n";

		/// ADD TRY BLOCK !!!

		/// ADD CODE TO VERIFY DATA !!!
		// Verify Data Packet
		buff[buff_size] = '\0'; // null terminate the buffer
		cout << "Verifying data packet ..... \t";
		verifyPacket(buff);

		// Increment Number of Packets
		++inst.pkts;

		// Parse Packet Data
		char data[MAXBUF]; // create a char[] to hold final data
		int data_size = buff_size;
		castArray(buff, data, buff_size);
		cout << "Parsing data ..... \t\t";
		parseData(data, data_size);

		// Write Data to File
		cout << "Writing to file ..... \t\t";
		if (openDFile(inst.datfile, inst.dfilenm)) {

			// Write Individual Bytes
			for (int i = 0; i < data_size; ++i) {
				inst.datfile << data[i];
			}
			cout << "Success.\n";

			// Save Datafile
			inst.datfile.close();
		} else
			cout << "Error opening the datafile.\n";

		// Make HTML File
		writeHTML(inst.dfilenm, inst.mapdlay, inst.pkts);

		// Send Command to Balloon
		sendCMD(inst.cfilenm, inst.comnum);

	} // Close While Loop

	return 0;
}


//////////////////////////////////////////////////////////////////////////////
/// Internal Function Implementations
//////////////////////////////////////////////////////////////////////////////

template <typename T, typename R>
void castArray(const T array1[], R array2[], const int arr_size)
{
	// Copy <arr_size> Elements
	for (int i = 0; i < arr_size; ++i)
		array2[i] = static_cast<R>(array1[i]);
}



bool loadParam(Param &inst, const string &filenm)
{
	// Try to Open <filenm>
	cout << "Loading parameters from " << filenm << " ..... \t";
	ifstream file;
	file.open(filenm.c_str());
	if (!file.good()) {
		cout << "Error opening file.\n";
		return false;
	}

	// Read parameters
	file >> inst.dfilenm >> inst.cfilenm >> inst.dlay >> inst.mapdlay
		>> inst.comnum >> inst.baud;

	// Attempt to Open Port
	if (RS232_OpenComport(inst.comnum, inst.baud)) {
		cout << "Error opening port.\n";
		return false;
	}

	// Count Number of Packets
	inst.pkts = 0;

	// Indicate Success
	cout << "Success.\n";
	return true;
}


void promptParam(Param &inst)
{
	// Prompt for Datafile and Open
	cout << "File to store incoming data:\n";
	cin >> inst.dfilenm;
	while (!openDFile(inst.datfile, inst.dfilenm)) {
		cout << "File to store incoming data:\n";
		cin >> inst.dfilenm;
	}

	// Write File Header and Save
	inst.datfile << "\nBegin New Data\n========================================\n";
	inst.datfile.close();

	// Prompt for Command File
	cout << "File to send commands to balloon:\n";
	cin >> inst.cfilenm;
	while (!openCFile(inst.cmdfile, inst.cfilenm)){
		cout << "File to send commands to balloon:\n";
		cin >> inst.cfilenm;
	}

	// Prompt for Program Refresh Rate
	cout << "Delay between each data read (in seconds):\n";
	cin >> inst.dlay;

	// Prompt for HTML Refresh Rate
	cout << "Delay between each HTML refresh (in seconds):"
		<< "\n[Zero for never]\n";
	cin >> inst.mapdlay;

	// Prompt for and Open Serial Port
	openPort(inst.comnum, inst.baud);

	// Count Number of Packets
	inst.pkts = 0;
}

bool openDFile(ofstream &file, const string &filenm)
{
	// Open <filenm>
	file.open(filenm.c_str(), ios_base::app);
	if (!file.good()) {
		cout << "Could not open datafile.\n";
		return false;
	}

	// Indicate Success
	return true;
}

bool openCFile(ifstream &file, const string &filenm)
{
	// Open <filenm>
	file.open(filenm.c_str());
	if (!file.good()) {
		cout << "Could not open commandfile.\n";
		return false;
	}

	// Indicate Success
	return true;
}


void openPort(int &comnum, int &baud)
{
	// Prompt for Port
	cout << "Comport to open:\n[USB ports start at 16 for Linux]\n";
	cin >> comnum;

	// Prompt for Baudrate
	cout << "Baudrate for the comport:\n";
	cin >> baud;

	// Attempt to Open Port
	while (RS232_OpenComport(comnum, baud)) {

		// Ask for New Values
		cout << "Error opening the port. Enter new port number:\n";
		cin >> comnum;
		cout << "Baudrate for the comport:\n";
		cin >> baud;
	}
}

bool verifyPacket(const unsigned char *buff)
{
	/// Add code to verify data !!!
	cout << "Success.\n";
	return true;
}

void parseData(char *data, int &data_size)
{
	// Index to Track Location in Data
	int ind = 0;

	// Variables to Hold Lat, Lng, tmp data
	string tmp, lat, lng;

	// Place the Data in a String
	string raw = data;

	// Try to Parse Data
	try {

		// Check for "$GPGGA"
		if (raw.find("$GPGGA") == string::npos) {
			string error = "Unable to find $GPGGA.";
			throw error;
		}

		// Interate Through 2 Commas
		for (int i = 0; i < 2; ++i) {
			tmp = raw.substr(ind, raw.size());
			ind = ind + tmp.find(",") + 1;
		}

		// Check Latitude Format
		if (raw.substr(ind, 1) == ",") {
			string error = "Bad Latitude Format.";
			throw error;
		}

		// Extract Number and Shift Decimal Point
		raw.erase(ind + 4, 1);
		tmp = raw.substr(ind, 2) + "." + raw.substr(ind + 2, 6);
		string dir = raw.substr(ind + 9,1);

		// Correct North/South
		if (dir == "S")
			lat = "-" + tmp;
		else
			lat = tmp;

		// Check Longitude Format
		if (raw.substr(ind + 11, 1) == ",") {
			string error = "Bad Longitude Format.";
			throw error;
		}

		// Extract Number and Shift Decimal Point
		raw.erase(ind + 16, 1);
		tmp = raw.substr(ind + 11, 3) + "." + raw.substr(ind + 14, 6);
		dir = raw.substr(ind + 21,1);

		// Correct East/West
		if (dir == "W")
			lng = "-"+tmp;
		else
			lng = tmp;

		// Rewrite Data
		string result = lat + "," + lng + ",";
		data_size = lat.size() + lng.size() + 2;
		for (int i = 0; i < data_size; ++i)
			data[i] = result[i];
		data[data_size] = '\0'; // null terminate the data

		// Indicate Success
		cout << "Success.\n";

	// Catch Failed Parsing
	} catch (string error) {
		cout << "Error: " << error << endl;
	}
}

void writeHTML(const string &dfilenm, const int mapdlay, const int pkts)
{
	// Make HTML File and Write Header
	cout << "Writing data to HTML file .....\t";
	ofstream maphtml;
	maphtml.open("GPSmap.html", ios_base::trunc);
	if (!maphtml.good()) {
		cout << "Error creating html file.\n";
	}
	writeHeader(maphtml, mapdlay);

	// Read Data from File and Place on Map
	writePts(maphtml, dfilenm, pkts);

	// Write HTML File Ending and Close
	writeEnd(maphtml);
	maphtml.close();

	// Indicate Success.
	cout << "Success.\n";
}

void writeHeader(ofstream &maphtml, int mapdlay)
{
	// Write Beginning of HTML Header
	maphtml << "<!DOCTYPE html>\n<html>\n	<head>\n		<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />";

	// Swtich for HTML Refresh
	if (mapdlay != 0)
	maphtml << "<meta http-equiv=\"refresh\" content=\"" << mapdlay << "\" />";

	// Write Remaining HTML Header
	maphtml << "\n		<style type=\"text/css\">\n			html { height: 100% }\n			body { height: 100%; margin: 0; padding: 0 }"
		<< "\n			#map_canvas { height: 100% }\n		</style>\n		<script type=\"text/javascript\""
		<< "\n			src=\"https://maps.googleapis.com/maps/api/js?key=AIzaSyBlMSLG5cI9PteGfyoqCG8OhkTdLqv5lmE&sensor=false\">"
		<< "\n		</script>\n		<script type=\"text/javascript\">\n	  var map;\n      function initialize() {"
		<< "\n	    var initialLatlng = new google.maps.LatLng(42.1653, -83.4454);\n		var lats, lngs;\n"
		<< "\n        var mapOptions = {\n          center: initialLatlng,\n          zoom: 8,\n          mapTypeId: google.maps.MapTypeId.ROADMAP\n        }"
		<< "\n		map = new google.maps.Map(document.getElementById(\"map_canvas\"),\n            mapOptions);\n"
		<< "\n		lats = [";
}

void writePts(ofstream &maphtml, const string &dfilenm, int pkts)
{
	// Open Datafile
	ifstream gpsdat;
	gpsdat.open(dfilenm.c_str());
	if (!gpsdat.good()) {
		cout << "Could not open datafile.\n";
		return;
	}

	// Storage for Lat/Long Data
	double lat[pkts], lon[pkts];
	int numpts = 0;

	// Read Until File Ends
	while (gpsdat.good()) {

		// Read Latitude
		char tmp[MAXGPSDAT];
		gpsdat.getline(tmp, MAXGPSDAT, ',');
		lat[numpts] = atof(tmp);

		// Read Longitude
		gpsdat.getline(tmp, MAXGPSDAT, ',');
		lon[numpts] = atof(tmp);

		// Increment count
		++numpts;
	}

	// Write Latitude Data
	for (int i = 0; i < numpts; ++i)
		maphtml << lat[i] << ",";
	maphtml << "];\n		lngs = [";

	// Write Longitude Data
	for (int i = 0; i < numpts; ++i)
		maphtml << lon[i] << ",";

	// Close Datafile
	gpsdat.close();
}

void writeEnd(ofstream &maphtml)
{
	// Write File End
	maphtml << "];\n\n		var location;"
		<< "\n		for (var i=0; i<lats.length; i++) {\n			location = new google.maps.LatLng(lats[i], lngs[i]);"
		<< "\n			var marker = new google.maps.Marker({\n				position: location,"
		<< "\n				map: map,\n			});\n		}"
		<< "\n\n	    map.setCenter(location);\n	  }\n		</script>\n	</head>\n	<body onload=\"initialize()\">"
		<< "\n		<div id=\"map_canvas\" style=\"width:100%; height:100%\"></div>\n	</body>\n</html>";
}

void sendCMD(const string &cfilenm, const int comnum)
{
	// Open Command File
	ifstream cmdfile;
	openCFile(cmdfile, cfilenm);

	// Read Line into String
	string cmd;
	cmdfile >> cmd;

	// Check for a Valid Command
	unsigned char mssg[MAXCMD + 1];
	int mssg_size = cmd.size();
	if (cmd[mssg_size - 1] == CMDDLIM) {
		cout << "Sending command \"" << cmd << "\" ..... \t";
		castArray(cmd.c_str(), mssg, mssg_size);

	// Use Default Command
	} else {
		cout << "Sending default command ..... \t";
		mssg_size = DFLTCMD.size();
		castArray(DFLTCMD.c_str(), mssg, mssg_size);
	}

	// Transmit Command
	if (RS232_SendBuf(comnum, mssg, mssg_size) != mssg_size)
		cout << "Error sending command.\n";
	else
		cout << "Success.\n";

	// Close Command File
	cmdfile.close();
}

