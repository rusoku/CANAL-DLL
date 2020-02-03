@echo off
rem Build using GCC on MSYS2 (https://www.msys2.org/ ).
rem This built most likely won't work on older systems like Windows XP, because it links to some
rem functions present only in the newer msvcrt.dll and kernel32.dll (e.g. GetTickCount64).
setlocal
set PATH=C:\msys64\mingw32\bin\;%PATH%
set CPP_FILES=CDllDrvObj.cpp CTouCANobjCmdMsg.cpp Threads.cpp stdafx.cpp CDllList.cpp CTouCANobjWinUSB.cpp canal_se.cpp CTouCANobj.cpp MyStrings.cpp dllmain.cpp

rem Normal build
g++ -m32 -Wall -g %CPP_FILES% canal32.def -shared -static -static-libgcc -static-libstdc++ -lwinusb -lcfgmgr32 -o canal32.dll

rem WinXP build (still not functioning)
rem The functions added to the newer msvcrt.dll were present in msvcr100.dll, so we link to it,
rem but it still will link to GetTickCount64, which didn't exist before Vista.

rem g++ -m32 -specs=winxp\msvcr100.specs -Wall %CPP_FILES% dllmain.cpp libkernel32.a canal32.def -shared -static -static-libgcc -static-libstdc++ -lwinusb -lcfgmgr32 -o canal32.dll
