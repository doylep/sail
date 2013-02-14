/*     Space Whales Team: Data Packet to Google Maps
            Last Updated January 29, 2013
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
/// !!!


//////////////////////////////////////////////////////////////////////////////
/// Internal Functions
//////////////////////////////////////////////////////////////////////////////

bool openDFile(ofstream &file, const string &filenm);
// MODIFIES: file, cout
// EFFECTS: Opens <file> at file <filenm> in append mode, prints an error and
//		returns 0 if the file does not open

bool openCFile(ifstream &file, const string &filenm);
// MODIFIES: file, cout
// EFFECTS: Opens <file> at file <filenm>, prints an error and returns 0 if
//		the file does not open

void promptParam(int &dlay, int &mapdlay);
// MODIFIES: dlay, mapdlay
// EFFECTS: Prompts for program refresh delay and map refresh delay and places
//		 the values in dlay and mapdlay, respectively

void openPort(int &comnum, int &baud);
// MODIFIES: Sets comnum to the port that is opened and baud to its baudrate,
//		modifies cin and cout
// EFFECTS: Prompts cin until <comnum> and <baud> successfully opens port
//		<comnum> with baudrate <baud>

bool verifyPacket(const unsigned char *buff);
// EFFECTS: verifies that the data packet is complete

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

int main ()
{
	// Program Header
	cout << "\nBalloon Data - by the Space Whale team\nVersion 0.2.1"
		<< "\n========================================\n";

	// Prompt for Datafile and Open
	ofstream datfile;
	string dfilenm;
	cout << "File to store incoming data:\n";
	cin >> dfilenm;
	while (!openDFile(datfile, dfilenm)) {
		cout << "File to store incoming data:\n";
		cin >> dfilenm;
	}

	// Write File Header and Save
	datfile << "Begin New Data\n========================================\n";
	datfile.close();

	// Prompt for Command File
	ifstream cmdfile;
	string cfilenm;
	cout << "File to send commands to balloon:\n";
	cin >> cfilenm;
	while (!openCFile(cmdfile, cfilenm)){
		cout << "File to send commands to balloon:\n";
		cin >> cfilenm;
	}

	// Prompt for Other Parameters
	int dlay, mapdlay;
	promptParam(dlay, mapdlay);

	// Prompt for and Open Serial Port
	int comnum, baud;
	openPort(comnum, baud);

	// Count Number of Packets
	int pkts = 0;

	// Read data from the port until the cycle is broken
	while (true) {

		// Read data from the port
		cout << "Reading from port ..... ";
		unsigned char buff[MAXBUF];
		int datnum = RS232_PollComport(comnum, buff, MAXBUF - 1);
		cout << "Success.\n";

		/// ADD CODE TO VERIFY DATA !!!
		// Verify Data Packet
		cout << "Verifying data packet ..... ";
		verifyPacket(buff);

		// Increment Number of Packets
		++pkts;

		// Write Data to File
		cout << "Writing to file ..... ";
		if (openDFile(datfile, dfilenm)) {

			// Write Individual Bytes
			for (int i = 0; i < datnum; ++i) {
				datfile << buff[i];
			}
			cout << "Success.\n";

			// Save Datafile
			datfile.close();
		}

		// Make HTML File
		writeHTML(dfilenm, mapdlay, pkts);

		// Send Command to Balloon
		//sendCMD();

		// Pause For Dlay Seconds
		cout << "Waiting ...\n";
		#ifdef __GNUC__
	/// Commented out for compatability with MinGW
			sleep(dlay);
		#endif
		#ifdef _WIN32
			_sleep(dlay * 1000);
		#endif

	} // Close While Loop

	return 0;
}


//////////////////////////////////////////////////////////////////////////////
/// Internal Function Implementations
//////////////////////////////////////////////////////////////////////////////

bool openDFile(ofstream &file, const string &filenm)
{
	// Open filenm
	file.open(filenm.c_str(), ios_base::app);
	if (!file.good()) {
		cout << "Could not open data file.\n";
		return false;
	}

	// Indicate Success
	return true;
}

bool openCFile(ifstream &file, const string &filenm)
{
	// Open filenm
	file.open(filenm.c_str());
	if (!file.good()) {
		cout << "Could not open command file.\n";
		return false;
	}

	// Indicate Success
	return true;
}

void promptParam(int &dlay, int &mapdlay)
{
	// Prompt for Program Refresh Rate
	cout << "Delay between each data read (in seconds):\n";
	cin >> dlay;

	// Prompt for HTML Refresh Rate
	cout << "Delay between each HTML refresh (in seconds):"
		<< "\n[Zero for never]\n";
	cin >>mapdlay;
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
	cout << "Success.\n";
	return true;
}

void writeHTML(const string &dfilenm, const int mapdlay, const int pkts)
{
	// Make HTML File and Write Header
	ofstream maphtml;
	maphtml.open("GPSmap.html", ios_base::trunc);
	if (!maphtml.good()) {
		cout << "\nError creating html file.\n";
	}
	writeHeader(maphtml, mapdlay);

	// Read Data from File and Place on Map
	writePts(maphtml, dfilenm, pkts);

	// Write HTML File Ending and Close
	writeEnd(maphtml);
	maphtml.close();
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
		cout << "\nCould not open data file.\n";
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
	cmdfile.getline(cmd, MAXCMD);

	// Send Command if Valid
	const int cmd_size = cmd.size();
	if (cmd[cmd_size - 1] == CMDDLIM) {

		// Transmit Command
		cout << "Sending command \"" << cmd << "\" ..... ";
		if (RS232_SendBuf(comnum, cmd.c_str(), cmd_size) != cmd_size)
			cout << "Error sending command.\n";
		else
			cout << "Success.\n";

	// Default Command
	} else {

		/// Default Command !!!
	}

	// Close Command File
	cmdfile.close();
}

