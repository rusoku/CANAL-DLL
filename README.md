# CANAL-DLL
VSCP CANAL DLL for TouCAN USB to CAN bus converter
====================================================

Version 1.0.5 based on CANAL VSCP version 1.0.8 specification

- Added function to get list serial numbers, USB VID/PID, type, etc.. of TouCAN adapters attached to a PC
- fixed a memory leaks
- bug fixes

Project prepared for LLVM Compiler and CMAKE cross platform tool.

Build Commands
--------------
download LLVM clang compiler from: https://releases.llvm.org/download.html

install LLVM to C:/LLVM

download Ninja from: https://github.com/ninja-build/ninja/releases

install Ninja to: C:/Ninja

download CMAKE from: https://cmake.org/download/

install CMAKE to anywhere

clone CANAL DLL source code from:

git clone https://github.com/rusoku/CANAL-DLL

mkdir build & cd build

cmake.exe -DCMAKE_RC_COMPILER=C:/LLVM/bin -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=C:/Ninja/ninja.exe "-DCMAKE_C_COMPILER=C:/LLVM/bin/clang.exe" "-DCMAKE_CXX_COMPILER=C:/LLVM/bin/clang++.exe" -G Ninja -S .. -B Release

cmake --build Release --target all -v -j8

