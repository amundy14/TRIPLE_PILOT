#define DEBUG 1

#ifndef NO_ERROR
#define NO_ERROR 0 // Comes from Windows.h usually
#endif

#define PAYLOAD_NOT_FOUND 1000001
#define ALLOC_ERROR 1000002
#define FILE_ERROR_OPEN 1000003
#define FILE_ERROR_READ 1000004
#define FILE_ERROR_WRITE 1000005
#define EXEC_ERROR_RUN 1000006
#define BAD_PARAMETER 1000007
#define INSUFFICIENT_SPACE 1000008

#define UNKNOWN_ERROR 9999999

#define LOGFILE "C:\\log.log"

#ifdef DEBUG
#define DPRINT(format, ...) writeToLog(LOGFILE, format, ##__VA_ARGS__)
#else
#define DPRINT(...) do {} while (0)
#endif

#define SET_RETVAL(retval, funcName, value) retval = value; \
	DPRINT("[-] %s failed with error: %u at %d in %s\n", funcName, retval, __LINE__, __FILE__); \
	goto CLEANUP;

#define CHECK_RETVAL(retval, funcName) if(retval) { \
	DPRINT("[-] %s failed with error: %u at %d in %s\n", funcName, retval, __LINE__, __FILE__); \
	goto CLEANUP; }

#define CHECK_RETVAL_NV(retval, funcName, varToCheck, valToCheck, newRet) \
	if(varToCheck == valToCheck) { \
		retVal = newRet; \
		DPRINT("[-] %s failed with error: %u at %d in %s\n", funcName, retval, __LINE__, __FILE__); \
	goto CLEANUP; }

#define CHECK_RETVAL_NV_NE(retval, funcName, varToCheck, valToCheck, newRet) \
	if(varToCheck != valToCheck) { \
		retVal = newRet; \
		DPRINT("[-] %s failed with error: %u at %d in %s\n", funcName, retval, __LINE__, __FILE__); \
	goto CLEANUP; }

#define CHECK_RETVAL_GLE(retval, funcName, varToCheck, valToCheck) \
	if (varToCheck == valToCheck) { \
		retVal = GetLastError(); \
		DPRINT("[-] %s failed with error: %u at %d in %s\n", funcName, retval, __LINE__, __FILE__); \
	goto CLEANUP; }

#define WHEREAMI DPRINT("I'm in %s at line %d\n", __FILE__, __LINE__)

int writeToLog(const char* filename, const char* format, ...);
