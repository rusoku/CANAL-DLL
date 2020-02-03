@echo off
rem Build using Microsoft Visual Studio 10 and WinDDK 7.1.0
rem (https://www.microsoft.com/en-us/download/details.aspx?id=11800 )
rem This is the only build guaranteed to work on WinXP 32-bit.

set MSVS_ROOT="C:\Program Files\Microsoft Visual Studio 10.0"
set MSVS_LIB=%MSVS_ROOT%\VC\lib
set WINDDK_ROOT=C:\WinDDK\7600.16385.1
set WINDDK_INC=%WINDDK_ROOT%\inc
set WINDDK_LIB=%WINDDK_ROOT%\lib

call %MSVS_ROOT%\VC\bin\vcvars32.bat

set CPP_FILES=CDllDrvObj.cpp CTouCANobjCmdMsg.cpp Threads.cpp stdafx.cpp CDllList.cpp CTouCANobjWinUSB.cpp canal_se.cpp CTouCANobj.cpp MyStrings.cpp dllmain.cpp

rem The system include path must go before C:\WinDDK\7600.16385.1\inc\{api,ddk}
rem so that sal.h is properly included.

set INC=/I%MSVS_ROOT%\VC\include /I%WINDDK_INC%\api /I%WINDDK_INC%\ddk

rem I don't know why but the compiler was able to find some system libraries like libcmt.lib etc
rem only if their full paths are specified explicitly .

set LIB=%WINDDK_LIB%\wxp\i386\winusb.lib %WINDDK_LIB%\wxp\i386\cfgmgr32.lib %WINDDK_LIB%\wxp\i386\uuid.lib %MSVS_LIB%\libcmt.lib %MSVS_LIB%\oldnames.lib %WINDDK_LIB%\wxp\i386\kernel32.lib

cl /LD /Ox /Fecanal32 %INC% %CPP_FILES% canal32.def %LIB%

del *.obj
