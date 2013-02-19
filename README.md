balloon-data
============

Reads serial port data from a weather balloon, saves it into a file, and creates a Google maps webpage tracking the balloon

Copyright Patton Doyle and Molly Flynn
Using the RS-232 library by Teunis van Beelen http://www.teuniz.net/RS-232/
and code from https://github.com/doylep/gps_data-map.git by Patton Doyle and Molly Flynn

Released under GNU GPL v2 (see licence)

------------

Usage
------------

Commandline Options: (-s and default.config)
The program attempts to open default.config before prompting you for input.  You can bypass this option by specifying '-s' with or without a corresponding file to open instead.  If you specify a different configuration file (or use a default one), the file must contain the parameters separated by line in the following order without any spaces:
[ Datafile | Commandfile | Program Delay | HTML Delay | Port Number | Baud Rate ]

Command Structure:
Commands in the command file must contain no spaces and be terminated with global constant "CMDDLIM" ('$' by default).  Only commands that follow this structure will be transmitted.
