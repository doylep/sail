/*   Space Whales Team: Ground Station for Weather Balloons
		Last Updated March 11, 2013'
	     Released under GNU GPL - any version
		     By The Space Whales                    */


/// Testing Switch
//#define _LOCALTEST_

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "packet.h"
#include "rigging.h"

using namespace std;


//////////////////////////////////////////////////////////////////////////////
/// Main Function
//////////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
	// Set Program Precision
	Param inst;
	inst.datfile.precision(PRECSN);

	// Program Header
	cout << "Balloon Data, Version 0.4.1 beta"
		<< "\nDesigned by the Space Whale team"
		<< "\nCopyright (C) Patton Doyle and Molly Flynn"
		<< "\n\nReleased under GNU GPL v2 (see Licence)"
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

		// Pause For <dlay> Seconds
		cout << "Waiting for " << inst.dlay << " seconds.\n\n";
		#ifdef _WIN32
			_sleep(inst.dlay * 1000);
		#else
			sleep(inst.dlay);
		#endif

		// Read data from the port
		cout << "Reading from port ..... \t";
		unsigned char buff[MAXBUF];
		int buff_size = RS232_PollComport(inst.comnum, buff, MAXBUF - 1);
		buff[buff_size] = '\0'; // null terminate the buffer
		cout << "Success.\n";

		// Parse Packet Data
		Packet pket;
		pket.parseData(buff, buff_size);

		// Try to Write Data
		cout << "Writing to file ..... \t\t";
		try {
			// Write Data to File and Update HTML
			pket.writeData(inst);

		// Catch Bad File Write
		} catch (string error) {
			cout << "Error: " << error;
		}

		// Send Command to Balloon
		sendCMD(inst.cfilenm, inst.comnum);

	} // Close While Loop

	// Close the Port
	RS232_CloseComport(inst.comnum);

	// Exit
	return 0;
}
