#include <wchar.h>

#include <windows.h>
#include <tlhelp32.h>

#include "libProc.h"
#include "libDebug.h"
#include "libMem.h"
#include "libString.h"

#include <stdio.h>

int GetPidByName(const wchar_t* nameOfProcess, int* pid){
	int retVal = NO_ERROR;
	HANDLE snapShot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32W proc = { };
	int compareResult = 1;
	int _pid = 0;

	proc.dwSize = sizeof(PROCESSENTRY32W);

	snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	CHECK_RETVAL_GLE(retVal, "CreateToolhelp32Snapshot", snapShot, INVALID_HANDLE_VALUE);

	retVal = Process32FirstW(snapShot, &proc);
	CHECK_RETVAL_GLE(retVal, "Process32FirstW", retVal, 0);

	// compareResult will be 0 on match
	// retVal will be 0 on error, including having no more processes
	while (compareResult && retVal) {
		compareResult = wcsicmp(nameOfProcess, proc.szExeFile);
		_pid = proc.th32ProcessID;
		retVal = Process32NextW(snapShot, &proc);
	}
	// If compareResult is 0, we really don't care if Process32NextW errored.
	CHECK_RETVAL_GLE(retVal, "Process32NextW", compareResult, 1);

	// At this point, there should have been a match.
	// Otherwise, the CHECK_RETVAL macro would have jumped to CLEANUP
	
	// Set my out params
	*pid = _pid;
	retVal = 0; // reset retval to 0

CLEANUP:
	SAFEHANDLEFREE(snapShot);
	return retVal;
}