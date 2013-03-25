/*     Space Whales Team: RS232 Read From File
            Last Updated February 29, 2013
	Released under GNU GPL - version 2 or later
		 By The Space Whales                 */

#ifndef rs232_INCLUDED
#define rs232_INCLUDED

int RS232_OpenComport(int, int);
int RS232_PollComport(int, unsigned char *, int);
int RS232_SendByte(int, unsigned char);
int RS232_SendBuf(int, unsigned char *, int);
void RS232_CloseComport(int);
void RS232_cputs(int, const char *);
int RS232_IsCTSEnabled(int);
int RS232_IsDSREnabled(int);
void RS232_enableDTR(int);
void RS232_disableDTR(int);
void RS232_enableRTS(int);
void RS232_disableRTS(int);

#endif


