## Triple Pilot
This tool is based off an idea I had while talking with a co-worker one day. Essentially, what happens when the different essential steps to performing malicious actions are split across multiple processes?

#### Process Orchestration
The idea we came up with was "Process Orchestration". Borrowing loosely from the idea of container orchestration, what happens if a red teamer uses many different processes to each perform one small component of an attack? If one process allocates memory and reads in shellcode, another one changes the memory permissions, and a final one then executes the shellcode, do EDRs detect it?

#### Soft Persistence
Since I already had multiple processes on hand, this provided good opportunity to set up an aggressive soft persistence. Each process holds a handle to each other process and watches it. If any one process dies, the other processes should start a new one. This makes the tool more difficult to resilient and may help it survive an EDR catching onto one process.

#### Compilation
You will need to adjust the defines at the top of src/main.c to point at where you intend to put the payload on disk and where your shellcode is before compiling.  
The build script will run CMake to compile all the libraries I've included and then build the main program. It will then package up the compiled artifacts into an .iso, which is convenient for testing in VMs.  
In the build script, select which compiler you want to use by uncommenting the appropriate CMake command.

#### Usage
In it's current form, usage is very easy: drop it to disk and run it.

#### Dependencies
The main dependencies (that aren't typically installed) are:
 - cmake
 - make
 - mingw-w64
 - genisoimage

In addition, clang can be used as an alternate compiler. In this configuration, it relies on the headers from mingw, so don't skip installing that.

#### TODO
Some features I hope to add to this:
 - Shellcode decryption
 - Store shellcode in a resource section
 - Remote Injection
 - PPID spoof for the other processes
