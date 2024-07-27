#include <stdio.h>
#include <stdarg.h>

#include <windows.h>

#include "libDebug.h"

int writeToLog(const char* filename, const char* format, ...){
	FILE* logfile = NULL;
	va_list args; //no idea how to initialize this
	int retVal = NO_ERROR;
	unsigned long PID = 0;
	unsigned long TID = 0;

	PID = GetCurrentProcessId();
	TID = GetCurrentThreadId();
	// These APIs does not fail

	logfile = fopen(filename, "a+");
	if(!logfile){
		retVal = FILE_ERROR_OPEN;
		goto CLEANUP;
	}

	va_start(args, format);

	retVal = fprintf(logfile, "%u:%u  --  ", PID, TID);
	if(retVal < 0){
		retVal = FILE_ERROR_WRITE;
		goto CLEANUP;
	}

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
