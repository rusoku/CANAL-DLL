// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//#define WIN32
//#define DEBUG_CANAL
// Windows Header Files:
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <initguid.h>
#include <tchar.h>
#include <winusb.h>
#include <usb.h>
#include <cfgmgr32.h>
/* looks like we don't need these ones here
   but they can raise problems with some compilers (GCC). */
//#include <strsafe.h>
//#include <shlwapi.h>

#ifndef _Out_bytecap_
#   define _Out_bytecap_(x)
#endif

#include "CDllList.h"
#include "CDllDrvObj.h"
#include "CTouCANobj.h"
#include "vscp_common/canal_macro.h"
#include "vscp_common/canal.h"
#include "vscp_common/canal_a.h"
#include "MyStrings.h"
#include "Threads.h"

