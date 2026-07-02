# Windows Cross-Compile Toolchain for MXE
# Use with: cmake -DCMAKE_TOOLCHAIN_FILE=deployment/windows-toolchain.cmake ..

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# MXE path - adjust based on your installation
set(MXE_PATH "/usr/lib/mxe" CACHE PATH "Path to MXE installation")

# Set compilers
set(CMAKE_C_COMPILER ${MXE_PATH}/usr/bin/x86_64-w64-mingw32.static-gcc)
set(CMAKE_CXX_COMPILER ${MXE_PATH}/usr/bin/x86_64-w64-mingw32.static-g++)
set(CMAKE_RC_COMPILER ${MXE_PATH}/usr/bin/x86_64-w64-mingw32.static-windres)

# Where to find libraries
set(CMAKE_FIND_ROOT_PATH ${MXE_PATH}/usr/x86_64-w64-mingw32.static)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Static linking flags
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

# Qt5 path for MXE
set(Qt5_DIR ${MXE_PATH}/usr/x86_64-w64-mingw32.static/qt5/lib/cmake/Qt5)
