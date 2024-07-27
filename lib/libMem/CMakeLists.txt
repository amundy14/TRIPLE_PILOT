add_library(libMem STATIC libMem.c)

target_include_directories(libMem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(libMem PRIVATE libDebug)