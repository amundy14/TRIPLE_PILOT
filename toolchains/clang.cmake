# Disables the C compiler test. Required for cross compilation in clang
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Specify the cross-compilation tools
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Specify the target environment
set(CMAKE_C_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_CXX_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_LINKER x86_64-w64-mingw32-ld)
set(CMAKE_AR x86_64-w64-mingw32-ar)
set(CMAKE_RANLIB x86_64-w64-mingw32-ranlib)

add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Oz
    #-pedantic
    #-fstack-protector-strong
    #-fsanitize=address
    #-fsanitize=undefined
    #-fsanitize=leak
    -D_FORTIFY_SOURCE=2
    -funroll-loops
    #-flto
)

# Specify the path to MinGW root directory
set(MINGW_ROOT /usr/x86_64-w64-mingw32)

# Set the CMAKE_FIND_ROOT_PATH
set(CMAKE_FIND_ROOT_PATH ${MINGW_ROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Make clang include and link the mingw runtime
include_directories(${MINGW_ROOT}/include)
link_directories(/usr/lib/gcc/x86_64-w64-mingw32/10-win32)
link_directories(${MINGW_ROOT}/lib)