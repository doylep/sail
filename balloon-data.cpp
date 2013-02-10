/*     Space Whales Team: Data Packet to Google Maps
            Last Updated January 29, 2013
                    By The Space Whales                 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

// Choose Approprite System Library
#ifdef __GNUC__
#include <unistd.h>
#endif

using namespace std;


//////////////////////////////////////////////////////////////////////////////
/// Global Constants
//////////////////////////////////////////////////////////////////////////////

// Maximum number of data points
const int MAXPTS = 1000;

// Maximum data read size (Should be very large)
const int MAXSTREAM = 100;

// Maximum data character size
const int MAXDAT = 25;


//////////////////////////////////////////////////////////////////////////////
/// Main Function
//////////////////////////////////////////////////////////////////////////////

int main ()
{
	// Prompt for Filename
	string filnme;
	cout << "File containing the GPS data:\n";
	cin >> filnme;

	// Prompt for Program Refresh Rate
	int dlay;
	cout << "\nDelay between each map refresh (in seconds):\n";
	cin >> dlay;

	// Prompt for HTML Refresh Rate
	int hldlay;
	cout << "\nDelay between each HTML refresh (in seconds):\n[Zero for never]\n";
	cin >> hldlay;

	while(true) {

		// Notify of Refresh
		cout << "\nRefreshing map.\n";

		// Open GPS Data File
		ifstream gpsdat;
		gpsdat.open(filnme.c_str());
		if (!gpsdat.good()) {
			cout << "Could not open data file.";
			exit(1);
		}

		// Make HTML File
		ofstream maphtml;
		maphtml.open("GPS_map.html", ios_base::trunc);
		if (!maphtml.good()) {
			cout << "Error creating html file.";
			exit(1);
		}

		// Write Beginning of HTML Header
		maphtml << "<!DOCTYPE html>\n<html>\n	<head>\n		<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no\" />";

		// Swtich for HTML Refresh
		if (hldlay != 0)
			maphtml << "<meta http-equiv=\"refresh\" content=\"1\" />";

		// Write Remaining HTML Header
		maphtml << "\n		<style type=\"text/css\">\n			html { height: 100% }\n			body { height: 100%; margin: 0; padding: 0 }"
			<< "\n			#map_canvas { height: 100% }\n		</style>\n		<script type=\"text/javascript\""
			<< "\n			src=\"https://maps.googleapis.com/maps/api/js?key=AIzaSyBlMSLG5cI9PteGfyoqCG8OhkTdLqv5lmE&sensor=false\">"
			<< "\n		</script>\n		<script type=\"text/javascript\">\n	  var map;\n      function initialize() {"
			<< "\n	    var initialLatlng = new google.maps.LatLng(42.1653, -83.4454);\n		var lats, lngs;\n"
			<< "\n        var mapOptions = {\n          center: initialLatlng,\n          zoom: 8,\n          mapTypeId: google.maps.MapTypeId.ROADMAP\n        }"
			<< "\n		map = new google.maps.Map(document.getElementById(\"map_canvas\"),\n            mapOptions);\n"
			<< "\n		lats = [";

		// Read Lat/Long Data
		double lat[MAXPTS], lon[MAXPTS];
		int numpts = 0;

		// Read Until File Ends
		while (gpsdat.good()) {

			// Read Latitude
			char tmp[MAXDAT];
			gpsdat.getline(tmp, MAXSTREAM, ',');
			lat[numpts] = atof(tmp);

			// Read Longitude
			gpsdat.getline(tmp, MAXSTREAM, ',');
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

		// Write File End
		maphtml << "];\n\n		var location;"
			<< "\n		for (var i=0; i<lats.length; i++) {\n			location = new google.maps.LatLng(lats[i], lngs[i]);"
			<< "\n			var marker = new google.maps.Marker({\n				position: location,"
			<< "\n				map: map,\n			});\n		}"
			<< "\n\n	    map.setCenter(location);\n	  }\n		</script>\n	</head>\n	<body onload=\"initialize()\">"
			<< "\n		<div id=\"map_canvas\" style=\"width:100%; height:100%\"></div>\n	</body>\n</html>";

		// Close Files
		gpsdat.close();
		maphtml.close();

		// Pause For Dlay Seconds
		cout << "Waiting ... \n\n";

		#ifdef __GNUC__
/// Commented out for compatability with MinGW (cross platform compiler)
			// sleep(dlay);
		#endif
		#ifdef _WIN32
			_sleep(dlay * 1000);
		#endif

	} // Close While Loop

	return 0;
}
