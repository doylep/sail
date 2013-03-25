/*     Space Whales Team: RS232 Read From File
            Last Updated February 29, 2013
	Released under GNU GPL - version 2 or later
		 By The Space Whales                 */


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace std;


// Global File Streams
ifstream inport;
string fileout;
ofstream outport;

int RS232_OpenComport(int t1, int t2)
{
	// Default File In
	string filein = "inport";
	inport.open(filein.c_str());

	// Format Stream
	cout << "\n";

	// Prompt for File In
	while (!inport.good()) {
		cout << "File to Use as Port In:\n";
		cin >> filein;
		inport.open(filein.c_str());
	}

	// Prompt for File Out
	cout << "File to Use as Port Out:\n";
	cin >> fileout;
	outport.open(fileout.c_str());

	// Prompt for File Out
	while (!outport.good()) {
		cout << "File to Use as Port Out:\n";
		cin >> fileout;
		outport.open(fileout.c_str());
	}

	// Close File Out
	outport.close();

	// Dummy Return
	return 0;
}

int RS232_PollComport(int t1, unsigned char *data, int data_size)
{
	// Read to String
	string tmp;
	inport >> tmp;

	// Write to Array
	int tmp_size = tmp.size();
	if (tmp_size > data_size)
		assert(0);
	for (int i = 0; i < tmp_size; ++i)
		data[i] = tmp[i];

	// Indicate Success
	return tmp_size;
}



int RS232_SendBuf(int t1, unsigned char *data, int data_size)
{
	// Write Data to File
	outport.open(fileout.c_str(), ios_base::app);
	if (outport.good()) {

		// Write Individual Bytes
		for (int i = 0; i < data_size; ++i) {
			outport << data[i];
		}

		// Save Data
		outport.close();

		// Indicate Success
		return data_size;

	// Indicate Failure
	} else
		return 0;
}

void RS232_CloseComport(int t1)
{
	// Close File In
	inport.close();
}


// Invalid Fuctions
int RS232_SendByte(int t1, unsigned char t2)
{
	assert(0);
}
void RS232_cputs(int t1, const char *t2)
{
	assert(0);
}
int RS232_IsCTSEnabled(int t1)
{
	assert(0);
}
int RS232_IsDSREnabled(int t1)
{
	assert(0);
}
void RS232_enableDTR(int t1)
{
	assert(0);
}
void RS232_disableDTR(int t1)

{
	assert(0);
}
void RS232_enableRTS(int t1)
{
	assert(0);
}
void RS232_disableRTS(int t1)
{
	assert(0);
}

