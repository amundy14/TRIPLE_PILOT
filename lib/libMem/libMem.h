#include <windows.h>

//#define SAFEFREE(var) if(var) 

#define SAFEHANDLEFREE(var) if(var != NULL && var != INVALID_HANDLE_VALUE) { CloseHandle(var); var = NULL; }