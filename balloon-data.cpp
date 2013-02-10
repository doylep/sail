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

// Serial buffer size
const int MAXBUF = 1000;

// Maximum number of data points
const int MAXPTS = 1000;

// Maximum GPS data point size
const int MAXGPSDAT = 50;


//////////////////////////////////////////////////////////////////////////////
/// Internal Functions
//////////////////////////////////////////////////////////////////////////////

void openPort(int &comnum, int &baud);
// MODIFIES: Sets comnum to the port that is opened and baud to its baudrate,
//		modifies cin and cout
// EFFECTS: Prompts cin until <comnum> and <baud> successfully opens port
//		<comnum> with baudrate <baud>

bool verifyPacket(const unsigned char *buff);
// EFFECTS: verifies that the data packet is complete

void writeHeader(ofstream &maphtml, const int mapdlay);
// REQUIRES: maphtml points to an open ofstream
// MODIFIES: maphtml
// EFFECTS: Prints the HTML header to the file for GPS mapping with refresh
//		delay <mapdlay>

void writePts(ofstream &maphtml, const string filenm, const int pkts);
// REQUIRES: maphtml points to an open ofstream, filenm is the name of
//		valid datafile with no more than <pkts> datapoints
// MODIFIES: maphtml
// EFFECTS: Write the the lat and lon points in the file <filenm> to the
//		HTML of maphtml

void writeEnd(ofstream &maphtml);
// REQUIRES: maphtml points to an open ofstream
// MODIFIES: maphtml
// EFFECTS: Prints the HTML end to the file for the GPS mapping


//////////////////////////////////////////////////////////////////////////////
/// Main Function
//////////////////////////////////////////////////////////////////////////////

int main ()
{
	// Prompt for Datafile and Open
	ofstream datfile;
	string filenm;
	cout << "\nFile to store incoming data:\n";
	cin >> filenm;
	datfile.open(filenm.c_str(), ios::app);
	while (!datfile.good()) {
		cout << "\nCould not open data file."
			<< "\nFile to store incoming data:\n";
	}

	// Prompt for Program Refresh Rate
	int dlay;
	cout << "\nDelay between each data read (in seconds):\n";
	cin >> dlay;

	// Prompt for HTML Refresh Rate
	int mapdlay;
	cout << "\nDelay between each HTML refresh (in seconds):"
		<< "\n[Zero for never]\n";
	cin >>mapdlay;

	// Prompt for and open serial port
	int comnum, baud;
	openPort(comnum, baud);

	// Count Number of Packets
	int pkts = 0;

	// Read data from the port until the cycle is broken
	while (true) {

		// Read data from the port
		cout << "\nReading from port ...\n";
		unsigned char buff[MAXBUF];
		int datnum = RS232_PollComport(comnum, buff, MAXBUF - 1);

		/// ADD CODE TO VERIFY DATA !!!
		// Verify Data Packet
		verifyPacket(buff);

		// Increment Number of Packets
		++pkts;

		// Write Data to File
		cout << "\nWriting to file ...\n";
		for (int i = 0; i < datnum; ++i) {
			datfile << buff[i];
		}

		// Make HTML File
		ofstream maphtml;
		maphtml.open("GPSmap.html", ios::trunc);
		if (!maphtml.good()) {
			cout << "\nError creating html file.\n";
		}
		writeHeader(maphtml, mapdlay);

		// Read Data from File and Place on Map
		writePts(maphtml, filenm, pkts);

		// Write HTML File End and Close
		writeEnd(maphtml);
		maphtml.close();

		// Pause For Dlay Seconds
		cout << "Waiting ... \n\n";

		#ifdef __GNUC__
	/// Commented out for compatability with MinGW
			// sleep(dlay);
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

void openPort(int &comnum, int &baud)
{
	// Prompt for Port
	cout << "\nComport to open:\n[USB ports start at 16 for Linux]\n";
	cin >> comnum;

	// Prompt for Baudrate
	cout << "\nBaudrate for the comport:\n";
	cin >> baud;

	// Attempt to Open Port
	while (RS232_OpenComport(comnum, baud)) {

		// Ask for New Values
		cout << "\nError opening the port. Enter new port number:\n";
		cin >> comnum;
		cout << "\nBaudrate for the comport:\n";
		cin >> baud;
	}
}

bool verifyPacket(const unsigned char *buff)
{
	cout << "\nData packet is complete.\n";
	return true;
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

void writePts(ofstream &maphtml, const string filenm, int pkts)
{
	// Open Datafile
	ifstream gpsdat;
	gpsdat.open(filenm.c_str());
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
