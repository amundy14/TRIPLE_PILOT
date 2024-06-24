#define DEBUG 1
#define LOGFILE "C:\\users\\redteam\\desktop\\log.log"

#ifdef DEBUG
#define DPRINT(format, ...) writeToLog(LOGFILE, format, ##__VA_ARGS__)
#else
#define DPRINT(...) do {} while (0)
#endif

#define CHECK_RETVAL(retval, funcName) if(retval) { \
	DPRINT("[-] %s failed with error: %d at %d\n", funcName, retval, __LINE__); \
	goto CLEANUP; }

#define CHECK_RETVAL_NV(retval, funcName, varToCheck, valToCheck, newRet) \
	if(varToCheck == valToCheck) { \
		retVal = newRet; \
		DPRINT("[-] %s failed with error: %d at %d\n", funcName, retval, __LINE__); \
	goto CLEANUP; }

#define CHECK_RETVAL_GLE(retval, funcName, varToCheck, valToCheck) \
	if (varToCheck == valToCheck) { \
		retVal = GetLastError(); \
		DPRINT("[-] %s failed with error: %d at %d\n", funcName, retval, __LINE__); \
	goto CLEANUP; }

#define WHEREAMI DPRINT("I'm in %s at line %d\n", __FILE__, __LINE__)

int writeToLog(const char* filename, const char* format, ...);
