cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TWIN_PILOT)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Specify the cross-compilation tools
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Specify the target environment
set(CMAKE_C_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_CXX_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_LINKER x86_64-w64-mingw32-ld)
set(CMAKE_AR x86_64-w64-mingw32-ar)
set(CMAKE_RANLIB x86_64-w64-mingw32-ranlib)

# Specify the path to MinGW root directory
#set(MINGW_ROOT /usr/x86_64-w64-mingw32)

# Set the CMAKE_FIND_ROOT_PATH
#set(CMAKE_FIND_ROOT_PATH ${MINGW_ROOT})
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

include_directories(${MINGW_ROOT}/include)
link_directories(${MINGW_ROOT}/lib)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Set the output directory for build artifacts
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add lower level CMake files
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/crypts)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/debug)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/inject)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/proc)

# Include the headers
include_directories(${PROJECT_SOURCE_DIR}/lib/crypts)
include_directories(${PROJECT_SOURCE_DIR}/lib/debug)
include_directories(${PROJECT_SOURCE_DIR}/lib/inject)
include_directories(${PROJECT_SOURCE_DIR}/lib/proc)

# Source Files
include_directories(${PROJECT_SOURCE_DIR}/src)
set(SRC_FILES ${PROJECT_SOURCE_DIR}/src/main.c)

# Add the executable
add_executable(TWIN_PILOT ${SRC_FILES})

# Link libraries to the executable
target_link_libraries(TWIN_PILOT crypts debug cnoError inject proc)


# Enable unit tests
#enable_testing()
#file(GLOB_RECURSE TEST_FILES ${PROJECT_SOURCE_DIR}/test/*.c)
#add_executable(runUnitTests ${TEST_FILES})
#target_link_libraries(runUnitTests lib1 lib2)
#add_test(NAME runUnitTests COMMAND runUnitTests)
