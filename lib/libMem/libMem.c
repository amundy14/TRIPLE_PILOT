#include <handleapi.h>
#include <memoryapi.h>
#include <minwindef.h>
#include <stdio.h>
#include <string.h>
#include <synchapi.h>
#include <wchar.h>

#include <windows.h>
#include <winerror.h>
#include <winnt.h>

#include "libMem.h"
#include "libDebug.h"

// Creates a named file mapping
// If that file mapping already exists, it will return the existing one **with the existing size**.
int openNamedMemory(wchar_t* name, unsigned long buf_size, struct namedMemory* mem){
    DPRINT("openNamedMemory\n");

    int retVal = NO_ERROR;
    //HANDLE mapFile = NULL;
    wchar_t* event_name = NULL;
    wchar_t* mutex_name = NULL;
    unsigned long long name_len = 0;

    // Minimal error checking
    if (name == NULL || mem == NULL) {
        SET_RETVAL(retVal, "openNamedMemory", BAD_PARAMETER);
    }

    name_len = wcslen(name);
    if(name_len == 0){
        SET_RETVAL(retVal, "wcslen", BAD_PARAMETER);
    }

    mem->mapFile = CreateFileMappingW(
        INVALID_HANDLE_VALUE, 
        NULL,
        PAGE_READWRITE, 
        0, 
        buf_size, 
        name);
    CHECK_RETVAL_GLE(retVal, "CreateFileMappingW", mem->mapFile, NULL);

    mem->buf_size = buf_size;

    mem->buf = MapViewOfFile(mem->mapFile, FILE_MAP_ALL_ACCESS, 0, 0, mem->buf_size);
    CHECK_RETVAL_GLE(retVal, "MapViewOfFile", mem->buf, NULL);

    event_name = calloc(name_len + 2, sizeof(wchar_t));
    if(event_name == NULL){
        SET_RETVAL(retVal, "calloc", UNKNOWN_ERROR);
    }

    event_name = wcscpy(event_name, name);
    event_name[name_len] = L'e';

    mem->writeEvent = CreateEventW(NULL, FALSE, FALSE, event_name);
    CHECK_RETVAL_GLE(retVal, "CreateEventW", mem->writeEvent, NULL);

    mutex_name = calloc(name_len + 2, sizeof(wchar_t));
    if(mutex_name == NULL){
        SET_RETVAL(retVal, "calloc", UNKNOWN_ERROR);
    }

    mutex_name = wcscpy(mutex_name, name);
    mutex_name[name_len] = L'm';

    mem->mutex = CreateMutexW(NULL, FALSE, mutex_name);
    CHECK_RETVAL_GLE(retVal, "CreateMutexW", mem->mutex, NULL);

    // clear out the last value of retVal if we get here
    retVal = NO_ERROR;

CLEANUP:
    SAFE_FREE(event_name);
    SAFE_FREE(mutex_name);
    //SAFE_CLOSEHANDLE(mapFile);

    return retVal;
}

// Ensures that all handles are closed
int closeNamedMemory(struct namedMemory* mem){
    DPRINT("closeNamedMemory\n");

    int retVal = NO_ERROR;

    if (mem == NULL) goto CLEANUP;

    retVal = ReleaseMutex(mem->mutex);
    if(retVal == 0){
        retVal = GetLastError();
        // 288 is returned when the mutex isn't owned, which is OK in this case.
        // Likewise with 6, which is invalid handle.
        if (retVal != 288 && retVal != 6){
		    DPRINT("[-] %s failed with error: %u at %d in %s\n", "ReleaseMutex", retVal, __LINE__, __FILE__);
            goto CLEANUP;
        }
    }

    SAFE_CLOSEHANDLE(mem->mutex);

    SAFE_CLOSEHANDLE(mem->writeEvent);

    mem->name = NULL;

    mem->buf_size = 0;

    retVal = UnmapViewOfFile(mem->buf);
    CHECK_RETVAL_GLE(retVal, "UnmapViewOfFile", retVal, 0);

    SAFE_CLOSEHANDLE(mem->mapFile);

    mem->buf = NULL;

    mem = NULL;

    // clear out the last value of retVal if we get here
    retVal = NO_ERROR;

CLEANUP:
    return retVal;
}

int writeNamedMemory(void* buf, unsigned long buf_size, struct namedMemory* mem){
    DPRINT("writeNamedMemory\n");

    int retVal = NO_ERROR;
    unsigned long waitResult = 0;

    // Minimal error checking
    if (buf == NULL || mem == NULL) {
        SET_RETVAL(retVal, "writeNamedMemory", BAD_PARAMETER);
    }
    if (buf_size > mem->buf_size) {
        SET_RETVAL(retVal, "writeNamedMemory", INSUFFICIENT_SPACE);
    }

    // Get a lock on the mutex
    waitResult = WaitForSingleObject(mem->mutex, INFINITE);
    switch (waitResult) {// case fall-throughs are intentional
		case WAIT_ABANDONED:
		case WAIT_OBJECT_0:
			break;
		default:
			CHECK_RETVAL_GLE(retVal, "WaitForSingleObject", waitResult, WAIT_FAILED);
			break;
	}

    memcpy(mem->buf, buf, buf_size);

    retVal = SetEvent(mem->writeEvent);
    CHECK_RETVAL_GLE(retVal, "SetEvent", retVal, 0);

    retVal = ReleaseMutex(mem->mutex);
    CHECK_RETVAL_GLE(retVal, "ReleaseMutex", retVal, 0);

    // clear out the last value of retVal if we get here
    retVal = NO_ERROR;

CLEANUP:
    return retVal;
}

// Copies named memory to a local buf, allocating the buf if need be
// buf_size should be passed in as 0 if buf is unallocated
int readNamedMemory(struct namedMemory* mem, void** buf, unsigned long* buf_size){
    DPRINT("readNamedMemory\n");

    int retVal = NO_ERROR;
    unsigned long waitResult = 0;

    // Minimal error checking
    if (buf == NULL || mem == NULL) {
        SET_RETVAL(retVal, "readNamedMemory", BAD_PARAMETER);
    }
    if (*buf == NULL && *buf_size && (mem->buf_size != *buf_size)) {
        SET_RETVAL(retVal, "readNamedMemory", BAD_PARAMETER);
    }

    // Get a lock on the mutex
    waitResult = WaitForSingleObject(mem->mutex, INFINITE);
    switch (waitResult) {// case fall-throughs are intentional
		case WAIT_ABANDONED:
		case WAIT_OBJECT_0:
			break;
		default:
			CHECK_RETVAL_GLE(retVal, "WaitForSingleObject", waitResult, WAIT_FAILED);
			break;
	}

    // Allocate an out buf if it is not already set
    if (*buf == NULL) {
        *buf = VirtualAlloc(NULL, mem->buf_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        CHECK_RETVAL_GLE(retVal, "VirtualAlloc", *buf, NULL);
        *buf_size = mem->buf_size;
    }

    memcpy(*buf, mem->buf, mem->buf_size);

    retVal = ReleaseMutex(mem->mutex);
    CHECK_RETVAL_GLE(retVal, "ReleaseMutex", retVal, 0);

    // clear out the last value of retVal if we get here
    retVal = NO_ERROR;

CLEANUP:
    return retVal;
}

int readNamedMemoryOnEvent(unsigned long wait_time, struct namedMemory* mem, void** buf, unsigned long* buf_size){
    DPRINT("readNamedMemoryOnEvent\n");

    int retVal = NO_ERROR;
    unsigned long waitResult = 0;
    
    if (mem == NULL) {
        SET_RETVAL(retVal, "readNamedMemoryOnEvent", BAD_PARAMETER);
    }

    waitResult = WaitForSingleObject(mem->writeEvent, wait_time);
    switch (waitResult) {// case fall-throughs are intentional
		case WAIT_ABANDONED:
		case WAIT_OBJECT_0:
			break;
		default:
			CHECK_RETVAL_GLE(retVal, "WaitForSingleObject", waitResult, WAIT_FAILED);
			break;
	}

    retVal = readNamedMemory(mem, buf, buf_size);
    CHECK_RETVAL(retVal, "readNamedMemory");

    // clear out the last value of retVal if we get here
    retVal = NO_ERROR;

CLEANUP:
    return retVal;
}