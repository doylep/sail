/*   Space Whales Team: Backbone Functions for Weather Balloon Ground Station
		           Last Updated March 11, 2013
	              Released under GNU GPL - any version
		              By The Space Whales                   	 */


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "packet.h"
#include "whalebone.h"

using namespace std;


//////////////////////////////////////////////////////////////////////////////
/// Backbone Function Implementations
//////////////////////////////////////////////////////////////////////////////

void castArray(const char array1[], unsigned char array2[], const int arr_size)
{
	// Copy <arr_size> Elements
	for (int i = 0; i < arr_size; ++i)
		array2[i] = static_cast<unsigned char>(array1[i]);
}

void castArray(const unsigned char array1[], char array2[], const int arr_size)
{
	// Copy <arr_size> Elements
	for (int i = 0; i < arr_size; ++i)
		array2[i] = static_cast<char>(array1[i]);
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

	// Close Datafile
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
	cout << "Comport to open:\n[Start at 0 for Windows and 16 for Linux]\n";
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

void writeHTML(const string &dfilenm, const int mapdlay)
{
	// Make HTML File and Write Header
	cout << "Writing data to HTML file .....\t";
	ofstream maphtml;
	maphtml.open("GPSmap.html", ios_base::trunc);
	if (!maphtml.good()) {
		string error = "Error creating html file.\n";
		throw error;
	}
	writeHeader(maphtml, mapdlay);

	// Read Data from File and Place on Map
	writePts(maphtml, dfilenm);

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

void writePts(ofstream &maphtml, const string &dfilenm)
{
	// Open Datafile
	ifstream gpsdat;
	gpsdat.open(dfilenm.c_str());
	if (!gpsdat.good()) {
		cout << "Could not open datafile.\n";
		return;
	}

	// Storage for Lat/Long Data
	double lat[MAXPTS], lon[MAXPTS];
	int numpts = 0, pnt = 0;

	// Read Until File Ends
	while (gpsdat.good()) {

		// Advance Past Sensor Data
		string tmp;
		gpsdat >> tmp;
		int ind = 0;
		for (int i = 0; i < 7; ++i) {
			ind = tmp.find('\t', ind + 1);
		}

		// Read Lattitude
		int lnth = tmp.find('\t', ind + 1) - ind - 1;
		if (lnth)
			lat[pnt] = atof((tmp.substr(ind + 1, lnth)).c_str());
		else
			lat[pnt] = 0;

		// Read Longitude
		ind += lnth + 1;
		lnth = tmp.find('\t', ind + 1) - ind - 1;
		if (lnth)
			lon[pnt] = atof((tmp.substr(ind + 1, lnth)).c_str());
		else
			lon[pnt] = 0;

		// Increment count
		pnt = (pnt + 1) % MAXPTS;
		if (numpts < MAXPTS)
			++numpts;
	}

	// Don't Use End-of-File
	--numpts;

	// Write Latitude Data
	for (int i = 0; i < numpts; ++i)
		maphtml << lat[i] << ",";
	maphtml << "];\n\t\tlngs = [";

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

