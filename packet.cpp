/*   Space Whales Team: Packet Class for Weather Balloon Ground Station
		        Last Updated March 11, 2013
		Released under GNU GPL - version 2 or later
		            By The Space Whales                   	*/


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "packet.h"
#include "base.h"

using namespace std;


//////////////////////////////////////////////////////////////////////////////
/// Class Function Implementations
//////////////////////////////////////////////////////////////////////////////

double Packet::extractSens(string &raw)
{
	// Find Data Boundaries
	unsigned int indx = 0, endx = 0;
	endx = raw.find(SENSDLIM, indx + 1);

	// Check For Valid Length
	if ((endx == string::npos) || (endx - indx == 1)) {
		string error = "Invalid";
		throw error;
	}

	// Try to Extract Data
	string var = raw.substr(indx, endx - indx);

	// Attempt Extraction
	double reslt;
	if (!(reslt = atof(var.c_str()))) {
		string error = "Invalid";
		throw error;
	}

	// Advance String
	raw = raw.substr(endx + 1);

	// Return Value
	return reslt;
}

void Packet::parseSens(string &raw)
{
	// Try to Extract Data
	string dattyp;

	try {
		// Get Pressure
		dattyp = "pressure data";
		pres = extractSens(raw);

		// Convert Pressure
		pres /= (1024 * 0.00776);

		// Get Humidity
		dattyp = "humidity data";
		humd = extractSens(raw);

		// Convert Humidity
		humd = ((HIGHV * humd / 1024) - 0.958) / 0.03068;

		// Get Acceleration
		dattyp = "acceleration data";
		for (int i = 0; i < 3; ++i) {
			accel[i] = extractSens(raw);

			// Covert Data
			accel[i] = ((LOWV * accel[i] / 1024) - ACCAL[i]) * (4545.454545);
		}

		// Get Temperature
		dattyp = "temperature data";
		for (int i = 0; i < 2; ++i) {
			temp[i] = extractSens(raw);

			// Convert Data
			temp[i] = (LOWV * temp[i] / 1024) * 100 - 32;
		}

	} catch (string error) {
		error = error + " " + dattyp + ".\n";
		throw error;
	}

	// Indicate Success
	cout << "Success.\n";
}

void Packet::parseGPS(const string &raw)
{
	// Local Variables
	int indx = 0;
	string tmp;

	// Check for "$GPGGA"
	if (raw.find("$GPGGA") == string::npos) {
		string error = "Unable to find $GPGGA.\n";
		throw error;
	}

	// Verify Length
	indx = raw.find("$GPGGA");
	if ((raw.size() - indx) < 72) {
		string error = "Truncated GPS data.\n";
		throw error;
	}

	// Interate Through 2 Commas
	for (int i = 0; i < 2; ++i) {
		tmp = raw.substr(indx);
		indx = indx + tmp.find(',') + 1;
	}

	// Check That There isn't a Comma
	if (raw.at(indx) == ',') {
		string error = "No latitude data.\n";
		throw error;
	}

	// Check Latitude Characters
	bool valid = true;
	for (int i = 0; i < 9; ++i) {

		// Don't Check the Period
		if (i != 4) {

			// Verify all are Numbers
			char num = raw.at(indx + i);
			if ((num > '9') || (num < '0')) {
				valid = false;
				break;
			}
		}
	}

	// Try to Process Latitude
	if (!valid) {
		string error = "Bad latitude format.\n";
		throw error;
	}

	// Extract Number and Change to Decimal
	lat = convrtGPS(raw.substr(indx, 9));
	char dir = raw.at(indx + 10);

	// Correct North/South
	if (dir == 'S')
		lat = -lat;

	// Check That There isn't a Comma
	if (raw.at(indx + 12) == ',') {
		string error = "No longitude data.\n";
		throw error;
	}

	// Check Longitude Characters
	for (int i = 0; i < 10; ++i) {

		// Don't Check the Period
		if (i != 5) {
			char num = raw.at(indx + i + 12);

			// Verify all are Numbers
			if ((num > '9') || (num < '0')) {
				valid = false;
				break;
			}
		}
	}

	// Try to Process Longitude
	if (!valid) {
		string error = "Bad longitude format.\n";
		throw error;
	}

	// Extract Number and Change to Decimal
	lng = convrtGPS(raw.substr(indx + 12, 10));
	dir = raw.at(indx + 23);

	// Correct East/West
	if (dir == 'W')
		lng = -lng;

	// Interate Through 5 Commas
	tmp = raw.substr(indx + 25);
	indx = 0;
	for (int i = 0; i < 3; ++i) {
		indx = tmp.find(',') + 1;
		tmp = tmp.substr(indx);
	}

	// Check for Value
	indx = tmp.find(',');
	if (indx < 5 || indx > 8)
		valid = false;

	// Try to Process Altitude
	if (!valid) {
		string error = "Bad altitude format.\n";
		throw error;
	}

	// Extract Number
	tmp = tmp.substr(0, indx);
	alt = atof(tmp.c_str());

	// Indicate Success
	cout << "Success.\n";
}

double Packet::convrtGPS(const string &raw)
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

void Packet::writeHeader(ofstream &maphtml, int mapdlay)
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

void Packet::writePts(ofstream &maphtml, const string &dfilenm)
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
		getline(gpsdat, tmp);
		int indx = 0;
		for (int i = 0; i < 10; ++i) {
			indx = tmp.find('\t', indx + 1);
		}

		// Read Lattitude
		int lnth = tmp.find('\t', indx + 1) - indx - 1;
		if (lnth)
			lat[pnt] = atof((tmp.substr(indx + 1, lnth)).c_str());
		else
			lat[pnt] = 0;

		// Read Longitude
		indx += lnth + 1;
		lnth = tmp.find('\t', indx + 1) - indx - 1;
		if (lnth)
			lon[pnt] = atof((tmp.substr(indx + 1, lnth)).c_str());
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

void Packet::writeEnd(ofstream &maphtml)
{
	// Write File End
	maphtml << "];\n\n		var location;"
		<< "\n		for (var i=0; i<lats.length; i++) {\n			location = new google.maps.LatLng(lats[i], lngs[i]);"
		<< "\n			var marker = new google.maps.Marker({\n				position: location,"
		<< "\n				map: map,\n			});\n		}"
		<< "\n\n	    map.setCenter(location);\n	  }\n		</script>\n	</head>\n	<body onload=\"initialize()\">"
		<< "\n		<div id=\"map_canvas\" style=\"width:100%; height:100%\"></div>\n	</body>\n</html>";
}


//////////////////////////////////////////////////////////////////////////////
/// Public Function Implementations
//////////////////////////////////////////////////////////////////////////////

void Packet::parseData(const unsigned char *buff, const int buff_size)
{
	// String for Packet Data
	string data;
	echo = "";
	int indx = 0;

	// Try to Process Data Packet
	cout << "Verifying data packet ..... \t";
	try {
		// Check for Empty Packet
		if (buff_size == 0) {
			string error = "No Data Received.\n";
			throw error;
		}

		// Indicate Success
		cout << "Success.\n";

	// Catch Unusable Packet
	} catch (string error) {
		cout << "Error: " << error;
		gps = false;
		sens = false;
		return;
	}


	// Repackage Packet Data
	char tmp[MAXBUF]; // create a char[] to transfer data
	castArray(buff, tmp, buff_size);
	data = tmp;

	// Remove Received Command
	indx = data.find(DATDLIM);
	if (indx)
		echo = data.substr(0, indx);
	data = data.substr(indx + 1);

	// Remove Flight Time
	indx = data.find(DATDLIM);
	if (indx) {
		string tmp1 = data.substr(0, indx);
		int tmp2 = atoi(tmp1.c_str());
		fltme.tm_sec = tmp2 % 60;
		fltme.tm_min = (tmp2 / 60) % 60;
		fltme.tm_hour = tmp2 / 3600;
	}
	data = data.substr(indx + 1);

	// Print Flight Time
	char flttime[20];
	strftime(flttime, 20, "%T", &fltme);
	cout << "Flight Time: " << flttime << "\t\t";

	// Print Received Command
	cout << "Balloon received \"" << echo << "\"\n";

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
	indx = data.find(DATDLIM);
	data = data.substr(indx + 1);
	try {
		cout << "Parsing GPS data ..... \t\t";
		parseGPS(data);
		gps = true;

		// Print Altitude
		cout << "Current Altitude: " << alt << " meters.\n";

	// Catch Bad GPS Data
	} catch (string error) {
		cout << "Error: " << error;
		gps = false;
	}
}

void Packet::writeData(Param &inst)
{
	// Write Data to File
	if (openDFile(inst.datfile, inst.dfilenm)) {

		// If All Data is Bad, Don't Write
		if (!sens && !gps) {
			cout << "No data to write.\n";
			inst.datfile.close();
			return;
		}

		// Write Timestamp
		time_t now;
		char systime[20];
		time(&now);
		strftime(systime, 20, "%T", localtime(&now));
		inst.datfile << systime << "\t";

		// Write Time of Flight
		char flttime[20];
		strftime(flttime, 20, "%T", &fltme);
		inst.datfile << flttime << "\t";

		// Write Echo
		inst.datfile << echo << "\t";

		// Write Sensor Data
		if (sens)
			inst.datfile << pres << "\t" << humd << "\t"
				<< accel[0] << "\t" << accel[1] << "\t"
				<< accel[2] << "\t" << temp[0] << "\t"
				<< temp[1] << "\t";
		else
			inst.datfile << "\t\t\t\t\t\t\t";

		// Write GPS Data
		if (gps) {
			inst.datfile << lat << "\t" << lng << "\t" << alt << "\t\n";
		} else
			inst.datfile << "\t\t\t\n";

		// Save Datafile
		inst.datfile.close();

		// Indicate Success
		cout << "Success.\n";

		// Refresh HTML Map
		if (gps) {
			cout << "Writing new HTML file .....\t";
			try {
				writeHTML(inst.dfilenm, inst.mapdlay);
			} catch (string error) {
				cout << "Error: " << error;
			}
		}

	// Throw Error
	} else {
		string error = "Error opening the datafile.\n";
		throw error;
	}
}

void Packet::writeHTML(const string &dfilenm, const int mapdlay)
{
	// Make HTML File and Write Header
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

