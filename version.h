#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Software Status
	static const char STATUS[] = "Beta";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 4;
	static const long BUILD = 5;
	static const long REVISION = 16;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 445;
	#define RC_FILEVERSION 0,4,5,16
	#define RC_FILEVERSION_STRING "0, 4, 5, 16\0"
	static const char FULLVERSION_STRING[] = "0.4.5.16";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 0;
	

}
#endif //VERSION_H
