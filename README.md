sail
============

This is a terminal program designed for serial communication with a weather balloon.  Sensor and GPS data is received from the balloon and commands delivered back to the balloon at a regular interval.  Data from the balloon is saved it into a file and a Google Maps webpage is created tracking the current balloon location.

This program is in beta status.  It's fit for most purposes and tested thoroughly on our setup, but you may see problems and even the occasional crash.  The program stores all data externally, so you shouldn't experience data loss even in the event of a crash.


------------

Limitations
------------

The best programs always start by telling you what they can't do, so here are some critical limitations:
sail only works with a single packet format.  If you can't comform your data to that packet style, you'll have to make some code modifications and recompile to fit your needs.  We'd love it if you're willing to fork the project and share your version with us.
sail doesn't have a GUI, and sending commands to the balloon is correspondingly ugly.
To effectively use sail, you'll also need to open a browser and a spreadsheet program.  Only data interpretation and storage is built into sail.
It only works for flights under 24 hours (because of a time storage decision).  If you're running flights longer than that, give me a call because you deserve an award.  Or just 


------------

Setup
------------

*Parameters to the Program*
Datafile - file to store data about the flight in tab separated form
Commandfile - file to send commands to the balloon
Program Delay - delay between each read/write cycle (in seconds)
HTML Delay - time between each refresh for the Google Maps webpage (in seconds).  A value of zero means the webpage will never refresh.
Port Number - port number to use to communicate with the balloon.  On Windows, USB ports begin at 1.  On Linux, serial ports begin at 0 and USB ports begin at 16.
Baud Rate - baud rate for communication
Default Command - command sent the balloon every cycle before

*Initialization*
At startup, the program attempts to open default.config.  If it is unable, it prompts you to input the parameters to the program.  You can bypass default.config by specifying '-s' in the terminal with or without a corresponding file to open instead.  If you specify a different configuration file (or use a default one), the file must contain the parameters separated by line in the following order and without any spaces:
[ Datafile | Commandfile | Program Delay | HTML Delay | Port Number | Baud Rate | Default Command ]

------------

Runtime
------------

Once the program cycle has begun, the program performs a series of tasks before repeating and reports them to the screen.  In order, the program
- reads incoming data from the balloon
- attempts to interpret the data packet (see packet formatting below)
- writes any valid data to the datafile
- rewrites the HTML file if there is new GPS data
- sends a command to the balloon
Descriptions of these steps are below.

*Reading Data from the Port*
This process is straightforward.  If the port is no longer accessible, an error will print to the screen.

*Interpreting the Data*
The program is designed to interpret data in the following form:
<echo>#<tof>#<pressure>,<humidity>,<accel x>,<accel y>,<accel z>,<temp 1>,<temp 2>,#<GPS string>
where echo is an echo of the command received by the balloon and tof is the time of flight (in seconds).  The calibration of the sensors will largely be application specific.  Contact Patton Doyle (doylep@umich.edu) for assitance.

*Writing Data to a File*
The validity of sensor and GPS data is determined separately.  If one, the other, or both are valid, they are appended to the datafile in tab-delimited form along with other instance information.  The order of data is <timestamp><tof><echo><converted sensor data><latitude><longitude>.

*Rewriting the Map*
If valid GPS data was received in the packet, the program reads all GPS data from the datafile and composes it into a Google Map webpage named GPSmap.html.

*Sending Commands*
Commands are sent to the balloon from a file (appropriately named the command file).  To be sent, command in the file must contain no spaces and be terminated with global constant "CMDDLIM" ('@' by default).  Only commands that follow this structure will be transmitted.

After sending commands, the program will pause for the given delay and start the cycle again.

-----------

License
-----------

Copyright (C) 2013 Patton Doyle and Molly Flynn
Using the RS-232 library by Teunis van Beelen http://www.teuniz.net/RS-232/
and code from https://github.com/doylep/gps_data-map.git by Patton Doyle and Molly Flynn

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.

Portions of this code are also released under the terms of the GNU General Public License version 3, or (at your option) any later version.  The header at the beginning of each file indicates its license.

Want to contribute?  Fork the repo and have at it!  Isn't open source awesome?!

If you have questions, comments, or would just like to rant about weather balloons and software in general, you can find me at doylep@umich.edu.
