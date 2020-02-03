@echo off
rem Build using Microsoft Visual Studio 2017
set MSVS_ROOT=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build
call "%MSVS_ROOT%\vcvars32.bat"
cl /LD /Fe:canal32 CDllDrvObj.cpp CTouCANobjCmdMsg.cpp Threads.cpp stdafx.cpp CDllList.cpp CTouCANobjWinUSB.cpp canal_se.cpp CTouCANobj.cpp MyStrings.cpp dllmain.cpp canal32.def winusb.lib cfgmgr32.lib
