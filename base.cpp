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
/// Base Function Implementations
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
	cout << "File to store incoming data: ";
	cin >> inst.dfilenm;
	while (!openDFile(inst.datfile, inst.dfilenm)) {
		cout << "File to store incoming data: ";
		cin >> inst.dfilenm;
	}

	// Close Datafile
	inst.datfile.close();

	// Prompt for Command File
	cout << "File to send commands to balloon: ";
	cin >> inst.cfilenm;
	while (!openCFile(inst.cmdfile, inst.cfilenm)){
		cout << "File to send commands to balloon: ";
		cin >> inst.cfilenm;
	}

	// Prompt for Program Refresh Rate
	cout << "Delay between each data read (in seconds): ";
	cin >> inst.dlay;

	// Prompt for HTML Refresh Rate
	cout << "Delay between each HTML refresh (in seconds) "
		<< "[zero for never]: ";
	cin >> inst.mapdlay;

	// Prompt for and Open Serial Port
	openPort(inst.comnum, inst.baud);

	// Prompt for Default Command
	cout << "Default command to send to the balloon: ";
	cin >> inst.dfltcmd;
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
	cout << "USB ports start at 0 for Windows and 16 for Linux.\n"
		<< "Entering -1 will use the first available port.\n"
		<< "Comport to open: ";
	cin >> comnum;

	// Prompt for Baudrate
	cout << "Baudrate for the comport: ";
	cin >> baud;

	// Search for Available Ports
	if (comnum == -1) {

		cout << "Trying to use first available port.\n";
		comnum = 0;

		// Cycle Though Ports
		while (RS232_OpenComport(comnum, baud)) {

			// Advance comnum
			++comnum;

			// Maximum Port Number
			if (comnum > 29) {
				cout << "No ports available.\n";
				openPort(comnum, baud);
				break;
			}
		}

	// Success;
	return;
	}

	// Attempt to Open Port
	while (RS232_OpenComport(comnum, baud)) {

		// Ask for New Values
		cout << "Error opening the port.\nEnter new port number: ";
		cin >> comnum;
		cout << "Baudrate for the comport: ";
		cin >> baud;
	}
}

bool sendCMD(Param &inst)
{
	// Open Command File
	openCFile(inst.cmdfile, inst.cfilenm);

	// Read Line into String
	string cmd;
	inst.cmdfile >> cmd;

	// Check for Program Termination
	if (cmd == STOPSAIL) {
		inst.cmdfile.close();
		return true;
	}

	// Check for Silent Mode
	if (cmd == SLNTMODE) {
		cout << "Operating in silent mode.\n";
		inst.cmdfile.close();
		return false;
	}

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

	// Indicate Normal Command
	return false;
}

