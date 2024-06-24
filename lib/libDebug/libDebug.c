#include <stdio.h>
#include <stdarg.h>

#include "libError.h"

int writeToLog(const char* filename, const char* format, ...){
	FILE* logfile = NULL;
	va_list args; //no idea how to initialize this
	int retVal = NO_ERROR;

	logfile = fopen(filename, "a+");
	if(!logfile){
		retVal = FILE_ERROR_OPEN;
		goto CLEANUP;
	}

	va_start(args, format);

	retVal = vfprintf(logfile, format, args);
	if(retVal < 0){
		retVal = FILE_ERROR_WRITE;
		goto CLEANUP;
	}

	fflush(logfile);

CLEANUP:
	if(logfile) fclose(logfile);
	va_end(args);
	return retVal;
}
