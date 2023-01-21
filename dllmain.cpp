/*
MIT License

Copyright (c) 2000-2023 Åke Hedman, Grodans Paradis AB
Copyright (c) 2005-2023 Gediminas Simanskis, Rusoku technologijos UAB (gediminas@rusoku.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "stdafx.h"

static HANDLE hThisInstDll = NULL;
static CDllDrvObj *theApp = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		hThisInstDll = hModule;
		theApp = new CDllDrvObj();
		theApp->InitInstance();
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//                             C A N A L -  A P I
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//  CanalOpen
//

#ifdef WIN32
extern "C" long WINAPI EXPORT CanalOpen(const char *pDevice, unsigned long flags)
#else
extern "C" long CanalOpen(const char *pDevice, unsigned long flags)
#endif
{
	long h = 0;
	CTouCANObj *pDrvObj = NULL;

	pDrvObj = new CTouCANObj();

	if (pDrvObj != NULL)
	{

		if ((h = theApp->AddDriverObject(pDrvObj)) <= 0)
		{
			h = 0;
			delete  pDrvObj;
		}
		else
		{
			if (pDrvObj->Open(pDevice, flags, TRUE) == FALSE)
			{
				theApp->RemoveDriverObject(h);
				h = 0;
			}
		}
	}

	return h;
}

///////////////////////////////////////////////////////////////////////////////
//  CanalClose
//

#ifdef WIN32
extern "C" int  WINAPI EXPORT CanalClose(long handle)
#else
extern "C" int CanalClose(long handle)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);

	if (NULL == pDrvObj)
		return CANAL_ERROR_MEMORY;

	pDrvObj->Close();
	theApp->RemoveDriverObject(handle);

	return CANAL_ERROR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//  CanalGetLevel
//

#ifdef WIN32
extern "C" unsigned long WINAPI EXPORT CanalGetLevel(long handle)
#else
extern "C" unsigned long CanalGetLevel(long handle)
#endif
{
	return CANAL_LEVEL_STANDARD;
}



///////////////////////////////////////////////////////////////////////////////
// CanalSend
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalSend(long handle, PCANALMSG pCanalMsg)
#else
extern "C" int CanalSend(long handle, PCANALMSG pCanalMsg)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->WriteMsg(pCanalMsg));
}


///////////////////////////////////////////////////////////////////////////////
// CanalSend blocking
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalBlockingSend(long handle, PCANALMSG pCanalMsg, unsigned long timeout)
#else
extern "C" int CanalBlockingSend(long handle, PCANALMSG pCanalMsg)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->WriteMsgBlocking(pCanalMsg, timeout));
}


///////////////////////////////////////////////////////////////////////////////
// CanalReceive
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalReceive(long handle, PCANALMSG pCanalMsg)
#else
extern "C" int CanalReceive(long handle, PCANALMSG pCanalMsg)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->ReadMsg(pCanalMsg));
}

///////////////////////////////////////////////////////////////////////////////
// CanalReceive blocking
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalBlockingReceive(long handle, PCANALMSG pCanalMsg, unsigned long timeout)
#else
extern "C" int CanalBlockingReceive(long handle, PCANALMSG pCanalMsg, unsigned long timeout)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->ReadMsgBlocking(pCanalMsg, timeout));
}

///////////////////////////////////////////////////////////////////////////////
// CanalDataAvailable
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalDataAvailable(long handle)
#else
extern "C" int CanalDataAvailable(long handle)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return pDrvObj->DataAvailable();
}

///////////////////////////////////////////////////////////////////////////////
// CanalGetStatus
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalGetStatus(long handle, PCANALSTATUS pCanalStatus)
#else
extern "C" int CanalGetStatus(long handle, PCANALSTATUS pCanalStatus)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);

	if (NULL == pDrvObj)
		return CANAL_ERROR_MEMORY;

	return (pDrvObj->GetStatus(pCanalStatus));
}

///////////////////////////////////////////////////////////////////////////////
// CanalGetStatistics
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalGetStatistics(long handle, PCANALSTATISTICS pCanalStatistics)
#else
extern "C" int CanalGetStatistics(long handle, PCANALSTATISTICS pCanalStatistics)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);

	if (NULL == pDrvObj)
		return CANAL_ERROR_MEMORY;

	return (pDrvObj->GetStatistics(pCanalStatistics));
}

///////////////////////////////////////////////////////////////////////////////
// CanalSetFilter
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalSetFilter(long handle, unsigned long filter)
#else
extern "C" int CanalSetFilter(long handle, unsigned long filter)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->SetFilter(filter) ? CANAL_ERROR_SUCCESS : CANAL_ERROR_GENERIC);
}

///////////////////////////////////////////////////////////////////////////////
// CanalSetMask
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalSetMask(long handle, unsigned long mask)
#else
extern "C" int CanalSetMask(long handle, unsigned long mask)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->SetMask(mask) ? CANAL_ERROR_SUCCESS : CANAL_ERROR_GENERIC);
}

///////////////////////////////////////////////////////////////////////////////
// CanalSetBaudrate
//

#ifdef WIN32
extern "C" int WINAPI EXPORT CanalSetBaudrate(long handle, unsigned long baudrate)
#else
extern "C" int CanalSetBaudrate(long handle, unsigned long baudrate)
#endif
{
	// Not supported in this DLL
	return CANAL_ERROR_NOT_SUPPORTED;
}

///////////////////////////////////////////////////////////////////////////////
// CanalGetVersion
//

#ifdef WIN32
extern "C" unsigned long WINAPI EXPORT CanalGetVersion(void)
#else
extern "C" unsigned long CanalGetVersion(void)
#endif
{
	unsigned long version;
	unsigned char *p = (unsigned char *)&version;

	*p = CANAL_MAIN_VERSION;
	*(p + 1) = CANAL_MINOR_VERSION;
	*(p + 2) = CANAL_SUB_VERSION;
	*(p + 3) = 0;
	return version;
}

///////////////////////////////////////////////////////////////////////////////
// CanalGetDllVersion
//

#ifdef WIN32
extern "C" unsigned long WINAPI EXPORT CanalGetDllVersion(void)
#else
extern "C" unsigned long CanalGetDllVersion(void)
#endif
{

	unsigned long version;
	unsigned char *p = (unsigned char *)&version;

	*p = DLL_MAIN_VERSION;
	*(p + 1) = DLL_MINOR_VERSION;
	*(p + 2) = DLL_SUB_VERSION;
	*(p + 3) = 0;

	return version;
}

///////////////////////////////////////////////////////////////////////////////
// CanalGetVendorString
//

#ifdef WIN32
extern "C" const char * WINAPI EXPORT CanalGetVendorString(void)
#else
extern "C" const char * CanalGetVendorString(void)
#endif
{
	static char r_str[256];
//	char  tmp_str[256];
	//char* r_str;

	CTouCANObj *pDrvObj = theApp->GetDriverObject(1681);

	if (NULL == pDrvObj)
		return NULL;

   strcpy_s(r_str, 256, pDrvObj->GetVendorString());

	return r_str;
}


/*!
Get CANAL driver properties
*/
#ifdef WIN32
const char * WINAPI EXPORT CanalGetDriverInfo(void)
#else
const char * CanalGetDriverInfo(void);
#endif
{
	return "\n";
}

//---------------------------- A type CANAL API list ------------------------------

/*!
	Set the 11bit filter  ID, Mask for a CANAL channel

	@param handle Handle to open physical CANAL channel.
	@return zero on success or error-code on failure.
*/
#ifdef WIN32
int WINAPI EXPORT CanalSetFilter11bit(long handle, Filter_Type_TypeDef type, unsigned long list, unsigned long mask)
#else
int CanalSetFilter11bit(long handle, Filter_Type_TypeDef type, unsigned char opt, unsigned long id, unsigned long mask)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->SetFilter11bit(type, list, mask));
}

/*!
	Set the 29bit filter  ID, Mask for a CANAL channel

	@param handle Handle to open physical CANAL channel.
	@return zero on success or error-code on failure.
*/
#ifdef WIN32
int WINAPI EXPORT CanalSetFilter29bit(long handle, Filter_Type_TypeDef type, unsigned long list, unsigned long mask)
#else
int CanalSetFilter29bit(long handle, Filter_Type_TypeDef type, unsigned char opt, unsigned long id, unsigned long mask)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->SetFilter29bit(type, list, mask)); // ? CANAL_ERROR_SUCCESS : CANAL_ERROR_GENERIC);
}

/*!
	Get bootloader ver

	@param handle Handle to open physical CANAL channel.
	@return zero on success or error-code on failure.
*/
#ifdef WIN32
int WINAPI EXPORT CanalGetBootloaderVersion(long handle, unsigned long *bootloader_version)
#else
int CanalGetBootloaderVersion(long handle, unsigned long *bootloader_version)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->GetBootloaderVersion(bootloader_version));
}

#ifdef WIN32
int WINAPI EXPORT CanalGetHardwareVersion(long handle, unsigned long *hardware_version)
#else
int CanalGetHardwareVersion(long handle, unsigned long *hardware_version)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->GetHardwareVersion(hardware_version));
}


#ifdef WIN32
int WINAPI EXPORT CanalGetFirmwareVersion(long handle, unsigned long *firmware_version)
#else
int CanalGetFirmwareVersion(long handle, unsigned long *firmware_version)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->GetFirmwareVersion(firmware_version));
}


#ifdef WIN32
int WINAPI EXPORT CanalGetSerialNumber(long handle, unsigned long *serial)
#else
int CanalGetSerialNumber(long handle, unsigned long *serial)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->GetSerialNumber(serial));
}


#ifdef WIN32
int WINAPI EXPORT CanalGetVidPid(long handle, unsigned long *vidpid)
#else
int CanalGetVidPid(long handle, unsigned long *vidpid)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->GetVidPid(vidpid));
}


#ifdef WIN32
int WINAPI EXPORT CanalGetDeviceId(long handle, unsigned long *deviceid)
#else
int CanalGetDeviceId(long handle, unsigned long *deviceid)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return 0;
	return (pDrvObj->GetDeviceId(deviceid));
}


#ifdef WIN32
int WINAPI EXPORT CanalGetVendor(long handle, unsigned int size, char *vendor)
#else
int CanalGetVendor(long handle, unsigned long *bootloader_version)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->GetVendor(size, vendor));
}


#ifdef WIN32
int WINAPI EXPORT CanalInterfaceStart(long handle)
#else
int CanalInterfaceStart(long handle)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->InterfaceStart());
}


#ifdef WIN32
int WINAPI EXPORT CanalInterfaceStop(long handle)
#else
int CanalInterfaceStop(long handle)
#endif
{
	CTouCANObj *pDrvObj = theApp->GetDriverObject(handle);
	if (NULL == pDrvObj) return CANAL_ERROR_MEMORY;
	return (pDrvObj->InterfaceStop());
}

