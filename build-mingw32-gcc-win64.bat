@echo off
rem Same as build-mingw32-gcc-win32.bat, but 64 bit.
setlocal
set PATH=C:\msys64\mingw64\bin\;%PATH%
g++ -Wall CDllDrvObj.cpp CTouCANobjCmdMsg.cpp Threads.cpp stdafx.cpp CDllList.cpp CTouCANobjWinUSB.cpp canal_se.cpp CTouCANobj.cpp MyStrings.cpp dllmain.cpp canal64.def -shared -static -static-libgcc -static-libstdc++ -lwinusb -lcfgmgr32 -o canal64.dll
