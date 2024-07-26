#include <windows.h>

#define SAFE_VIRTFREE(var) if(var != NULL) { VirtualFree(var, 0, MEM_RELEASE); var = NULL; }
#define SAFE_CLOSEHANDLE(var) if(var != NULL && var != INVALID_HANDLE_VALUE) { CloseHandle(var); var = NULL; }
#define SAFE_FREE(var) if(var != NULL) {free(var);}

struct namedMemory{
    HANDLE mutex;
    HANDLE writeEvent;
    wchar_t* name;
    unsigned long buf_size;
    void* buf;
};

int openNamedMemory(wchar_t* name, unsigned long bufferSize, struct namedMemory* mem);

int closeNamedMemory(struct namedMemory* mem);

int writeNamedMemory(void* buf, unsigned long buf_size, struct namedMemory* mem);

int readNamedMemory(struct namedMemory* mem, void** buf, unsigned long* buf_size);

int readNamedMemoryOnEvent(unsigned long wait_time, struct namedMemory* mem, void** buf, unsigned long* buf_size);