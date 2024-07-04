# Specify the cross-compilation tools
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

# Specify the target environment
set(CMAKE_C_COMPILER_TARGET x86_64-w64-mingw32)
set(CMAKE_CXX_COMPILER_TARGET x86_64-w64-mingw32)

# Mingw error commands
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Os
    #-pedantic
    #-fstack-protector-strong
    #-fsanitize=address
    #-fsanitize=undefined
    #-fsanitize=leak
    -D_FORTIFY_SOURCE=2
    -funroll-loops
)