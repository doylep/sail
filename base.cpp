/*   Space Whales Team: Backbone Functions for Weather Balloon Ground Station
		           Last Updated March 11, 2013
		   Released under GNU GPL - version 2 or later
		              By The Space Whales                   	 */


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "packet.h"
#include "base.h"

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
		>> inst.comnum >> inst.baud >> inst.dfltcmd;

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

	// Prompt for Default Command
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

void sendCMD(Param &inst)
{
	// Open Command File
	openCFile(inst.cmdfile, inst.cfilenm);

	// Read Line into String
	string cmd;
	inst.cmdfile >> cmd;

	// Check for a Valid Command
	unsigned char mssg[MAXCMD + 1];
	int mssg_size = cmd.size();
	if (cmd[mssg_size - 1] == CMDDLIM) {
		cout << "Sending command \"" << cmd << "\" ..... \t";
		castArray(cmd.c_str(), mssg, mssg_size);

	// Use Default Command
	} else {
		cout << "Sending default command ..... \t";
		mssg_size = inst.dfltcmd.size();
		castArray(inst.dfltcmd.c_str(), mssg, mssg_size);
	}

	// Transmit Command
	if (RS232_SendBuf(inst.comnum, mssg, mssg_size) != mssg_size)
		cout << "Error sending command.\n";
	else
		cout << "Success.\n";

	// Close Command File
	inst.cmdfile.close();
}

