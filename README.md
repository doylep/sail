sail
============

Reads serial port data from a weather balloon, saves it into a file, and creates a Google maps webpage tracking the balloon

------------

Usage
------------

Commandline Options: (-s and default.config)
The program attempts to open default.config before prompting you for input.  You can bypass this option by specifying '-s' with or without a corresponding file to open instead.  If you specify a different configuration file (or use a default one), the file must contain the parameters separated by line in the following order without any spaces:
[ Datafile | Commandfile | Program Delay | HTML Delay | Port Number | Baud Rate ]

Command Structure:
Commands in the command file must contain no spaces and be terminated with global constant "CMDDLIM" ('@' by default).  Only commands that follow this structure will be transmitted.

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
