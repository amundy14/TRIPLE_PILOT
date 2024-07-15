#include <stdio.h>

#include <windows.h>

#include "libFile.h"
#include "libDebug.h"
#include "libError.h"

// TODO: This could be made more efficient. It technically reads the file twice.
int readFile(char* pathToFile, char** fileBuffer, int* bufferLen){
	DPRINT("getPayload\n");
	int retVal = 0;

	FILE* file = NULL;
	file = fopen(pathToFile, "rb");
	CHECK_RETVAL_NV(retVal, "fopen", file, NULL, FILE_ERROR_OPEN);

	fseek(file, 0L, SEEK_END); //find the end of the file
	*bufferLen = ftell(file); // see what position we're at
	CHECK_RETVAL_NV(retVal, "fseek/ftell", *bufferLen, 0, FILE_ERROR_READ);
	fseek(file, 0L, SEEK_SET); // go back to the beginning of the file

	*fileBuffer = VirtualAlloc(NULL, *bufferLen, 0x3000, PAGE_READWRITE);
	CHECK_RETVAL_GLE(retVal, "VirtualAlloc", *fileBuffer, NULL);

	retVal = fread(*fileBuffer, sizeof(char), *bufferLen, file);
	CHECK_RETVAL_NV_NE(retVal, "fread", retVal, *bufferLen, FILE_ERROR_READ);

	retVal = NO_ERROR; // clear the result from fread out

CLEANUP:
	if(file) { fclose(file); file = NULL; }
	return retVal;
}