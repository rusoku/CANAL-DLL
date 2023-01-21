/*
MIT License

Copyright (c) 2000-2023 Åke Hedman, Grodans Paradis AB
Copyright (c) 2005-2023 Gediminas Simanskis, Rusoku technologijos UAB (gediminas@rusoku.com)
Copyright (C) 2020 Alexander Sorokin (sorockin@yandex.ru)

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


///////////////////////////////////////////////////////////////
// WinUSB  RetrieveDevicePath

/* Workaround for some ancient systems (prior to Windows 7) that don't have
   CM_MapCrToWin32Err. */

static DWORD WINAPI CM_MapCrToWin32Err_stub(CONFIGRET CmReturnCode, WORD DefaultErr) {

	return DefaultErr;
}

static DWORD MyCM_MapCrToWin32Err(CONFIGRET CmReturnCode, DWORD DefaultErr) {
	typedef DWORD (WINAPI * CM_MapCrToWin32Err_type)
		(CONFIGRET CmReturnCode, WORD DefaultErr);
	static CM_MapCrToWin32Err_type CM_MapCrToWin32Err_impl = 0;

	if (!CM_MapCrToWin32Err_impl) {
		CM_MapCrToWin32Err_impl = (CM_MapCrToWin32Err_type)GetProcAddress(
			GetModuleHandle(L"cfgmgr32.dll"),
			"CM_MapCrToWin32Err");

		if (!CM_MapCrToWin32Err_impl)
			CM_MapCrToWin32Err_impl = (CM_MapCrToWin32Err_type)CM_MapCrToWin32Err_stub;
	}

	return CM_MapCrToWin32Err_impl(CmReturnCode, DefaultErr);
}


HRESULT
CTouCANObj::RetrieveDevicePath(
	_Out_bytecap_(BufLen) LPWSTR DevicePath,
	_In_                  ULONG  BufLen,
	_Out_opt_             PBOOL  FailureDeviceNotFound, // noDevice
	_In_				  LPWSTR DeviceSerialNumber // GS

)
{
	CONFIGRET  cr = CR_SUCCESS;
	HRESULT    hr = S_OK;
	LPWSTR     DeviceInterfaceList = NULL;
	LPWSTR     CurrentInterface = NULL;
	ULONG      DeviceInterfaceListLength = 0;

	//======== GS =====

	LPWSTR	TmpDeviceInterfaceList = 0;
	LPWSTR  str = 0;
	LPWSTR  token = 0;

	if (NULL != FailureDeviceNotFound) {

		*FailureDeviceNotFound = FALSE;
	}

	if (NULL != DevicePath) {

		*FailureDeviceNotFound = FALSE;
	}

	//
	// Enumerate all devices exposing the interface. Do this in a loop
	// in case a new interface is discovered while this code is executing,
	// causing CM_Get_Device_Interface_List to return CR_BUFFER_SMALL.
	//

	do {

		cr = CM_Get_Device_Interface_List_SizeW(&DeviceInterfaceListLength,
			(LPGUID)&GUID_DEVINTERFACE_WinUsbF4FS1,
			NULL,
			CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

		if (cr != CR_SUCCESS) {
			hr = HRESULT_FROM_WIN32(MyCM_MapCrToWin32Err(cr, ERROR_INVALID_DATA));
			break;
		}

		DeviceInterfaceList = (LPWSTR)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			DeviceInterfaceListLength * sizeof(WCHAR));

		if (DeviceInterfaceList == NULL) {
			hr = E_OUTOFMEMORY;
			break;
		}

		cr = CM_Get_Device_Interface_ListW((LPGUID)&GUID_DEVINTERFACE_WinUsbF4FS1,
			NULL,
			DeviceInterfaceList,
			DeviceInterfaceListLength,
			CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

		if (cr != CR_SUCCESS) {
			HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);

			if (cr != CR_BUFFER_SMALL) {
				hr = HRESULT_FROM_WIN32(MyCM_MapCrToWin32Err(cr, ERROR_INVALID_DATA));
			}
		}

	} while (cr == CR_BUFFER_SMALL);

	if (FAILED(hr)) {
		return hr;
	}

	//
	// If the interface list is empty, no devices were found.
	//
	if (*DeviceInterfaceList == L'\0')
	{
		if (NULL != FailureDeviceNotFound)
		{
			*FailureDeviceNotFound = TRUE;
		}

		hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
		HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);
		return hr;
	}

	//Isskiriam laikina string atminti s/n paieskai

	TmpDeviceInterfaceList = (LPWSTR)HeapAlloc(GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		MAX_PATH * sizeof(WCHAR));

	if (DeviceInterfaceList == NULL) {
		hr = E_OUTOFMEMORY;
	}

	/************ Ieskom savo interfeiso pagal 'SerialNumber' **************/

	for (CurrentInterface = DeviceInterfaceList;
		*CurrentInterface;
		CurrentInterface += wcslen(CurrentInterface) + 1) {

		wcscpy_s(TmpDeviceInterfaceList,
			MAX_PATH,
			CurrentInterface);

		str = wcstok_s(TmpDeviceInterfaceList, L"#", &token);
		str = wcstok_s(NULL, L"#", &token);
		str = wcstok_s(NULL, L"#", &token);

		//if (wcscmp(str, DeviceSerialNumber) == 0)
		if ((wcscmp(str, DeviceSerialNumber) == 0) || (wcscmp(L"00000000", DeviceSerialNumber) == 0) || (wcscmp(L"ED000200", DeviceSerialNumber) == 0))
		{
			hr = wcscpy_s(deviceData.FoundSerialNumber,
							  sizeof(deviceData.FoundSerialNumber),
				              str);
			break;
		}
	}

	// Atlaisvinam laikina atminti
	HeapFree(GetProcessHeap(), 0, TmpDeviceInterfaceList);

	//
	// If the interface list is empty, no devices were found.
	//
	if (*CurrentInterface == TEXT('\0'))
	{
		if (NULL != FailureDeviceNotFound)
		{
			*FailureDeviceNotFound = TRUE;
		}

		hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
		HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);
		return hr;
	}

	//
	// Give path of the first found device interface instance to the caller. CM_Get_Device_Interface_List ensured
	// the instance is NULL-terminated.
	//
	hr = wcscpy_s(DevicePath,
		BufLen,
		CurrentInterface);

	HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);

	return hr;
}

///////////////////////////////////////////////////////////////
// WinUSB  OpenDevice

HRESULT
CTouCANObj::OpenDevice(
	_Out_     PDEVICE_DATA DeviceData,
	_Out_opt_ PBOOL        FailureDeviceNotFound,
	_In_	               LPWSTR DeviceSerialNumber
)
{
	HRESULT hr = S_OK;
	BOOL    bResult;

	DeviceData->HandlesOpen = FALSE;

	hr = RetrieveDevicePath(DeviceData->DevicePath,
		sizeof(DeviceData->DevicePath),
		FailureDeviceNotFound,
		DeviceSerialNumber
	);

	if (FAILED(hr))
	{
		return hr;
	}

	DeviceData->DeviceHandle = CreateFileW(DeviceData->DevicePath,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);

	if (INVALID_HANDLE_VALUE == DeviceData->DeviceHandle) {

		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	bResult = WinUsb_Initialize(DeviceData->DeviceHandle,
		&DeviceData->WinusbHandle);

	if (FALSE == bResult) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		CloseHandle(DeviceData->DeviceHandle);
		return hr;
	}

	DeviceData->HandlesOpen = TRUE;
	return hr;
}


///////////////////////////////////////////////////////////////
// WinUSB  CloseDevice

VOID
CTouCANObj::CloseDevice(
	_Inout_ PDEVICE_DATA DeviceData
)
{
	if (FALSE == DeviceData->HandlesOpen) {
		return;
	}

	WinUsb_Free(DeviceData->WinusbHandle);
	CloseHandle(DeviceData->DeviceHandle);
	DeviceData->HandlesOpen = FALSE;

	return;
}

