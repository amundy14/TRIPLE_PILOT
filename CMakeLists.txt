cmake_minimum_required(VERSION 3.10)

# Set the project name
project(TRIPLE_PILOT)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Set the output directory for build artifacts
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add lower level CMake files
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libError)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libCrypt)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libDebug)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libInject)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libProc)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libMem)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libString)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libFile)

# Include the headers
include_directories(${PROJECT_SOURCE_DIR}/lib/libError)
include_directories(${PROJECT_SOURCE_DIR}/lib/libCrypt)
include_directories(${PROJECT_SOURCE_DIR}/lib/libDebug)
include_directories(${PROJECT_SOURCE_DIR}/lib/libInject)
include_directories(${PROJECT_SOURCE_DIR}/lib/libProc)
include_directories(${PROJECT_SOURCE_DIR}/lib/libMem)
include_directories(${PROJECT_SOURCE_DIR}/lib/libString)
include_directories(${PROJECT_SOURCE_DIR}/lib/libFile)

# Source Files
include_directories(${PROJECT_SOURCE_DIR}/src)
set(SRC_FILES ${PROJECT_SOURCE_DIR}/src/main.c)

# Add the executable
add_executable(TRIPLE_PILOT ${SRC_FILES})

# Link libraries to the executable
target_link_libraries(TRIPLE_PILOT libCrypt)
target_link_libraries(TRIPLE_PILOT libDebug)
target_link_libraries(TRIPLE_PILOT libError)
target_link_libraries(TRIPLE_PILOT libInject)
target_link_libraries(TRIPLE_PILOT libProc)
target_link_libraries(TRIPLE_PILOT libMem)
target_link_libraries(TRIPLE_PILOT libString)
target_link_libraries(TRIPLE_PILOT libFile)


# Enable unit tests
#enable_testing()
#file(GLOB_RECURSE TEST_FILES ${PROJECT_SOURCE_DIR}/test/*.c)
#add_executable(runUnitTests ${TEST_FILES})
#target_link_libraries(runUnitTests lib1 lib2)
#add_test(NAME runUnitTests COMMAND runUnitTests)