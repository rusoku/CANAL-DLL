/*
 * CANAL interface DLL for RUSOKU technologies for TouCAN, TouCAN Marine, TouCAN Duo USB to CAN bus converter
 *
 * Copyright (C) 2018 Gediminas Simanskis (gediminas@rusoku.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.0 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.
 *
 */

#include "stdafx.h"


///////////////////////////////////////////////////////////////
// WinUSB  RetrieveDevicePath

HRESULT
CTouCANObj::RetrieveDevicePath(
	_Out_bytecap_(BufLen) LPTSTR DevicePath,
	_In_                  ULONG  BufLen,
	_Out_opt_             PBOOL  FailureDeviceNotFound, // noDevice
	_In_				  LPTSTR DeviceSerialNumber // GS

)
{
	CONFIGRET  cr = CR_SUCCESS;
	HRESULT    hr = S_OK;
	LPTSTR     DeviceInterfaceList = NULL;
	LPTSTR     CurrentInterface = NULL;
	ULONG      DeviceInterfaceListLength = 0;

	//======== GS =====

	LPTSTR	TmpDeviceInterfaceList = 0;
	LPTSTR  str = 0;
	LPTSTR  token = 0;

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

		cr = CM_Get_Device_Interface_List_Size(&DeviceInterfaceListLength,
			(LPGUID)&GUID_DEVINTERFACE_WinUsbF4FS1,
			NULL,
			CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

		if (cr != CR_SUCCESS) {
			hr = HRESULT_FROM_WIN32(CM_MapCrToWin32Err(cr, ERROR_INVALID_DATA));
			break;
		}

		DeviceInterfaceList = (PTSTR)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			DeviceInterfaceListLength * sizeof(WCHAR));

		if (DeviceInterfaceList == NULL) {
			hr = E_OUTOFMEMORY;
			break;
		}

		cr = CM_Get_Device_Interface_List((LPGUID)&GUID_DEVINTERFACE_WinUsbF4FS1,
			NULL,
			DeviceInterfaceList,
			DeviceInterfaceListLength,
			CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

		if (cr != CR_SUCCESS) {
			HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);

			if (cr != CR_BUFFER_SMALL) {
				hr = HRESULT_FROM_WIN32(CM_MapCrToWin32Err(cr, ERROR_INVALID_DATA));
			}
		}

	} while (cr == CR_BUFFER_SMALL);

	if (FAILED(hr)) {
		return hr;
	}

	//
	// If the interface list is empty, no devices were found.
	//
	if (*DeviceInterfaceList == TEXT('\0'))
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

	TmpDeviceInterfaceList = (LPTSTR)HeapAlloc(GetProcessHeap(),
		HEAP_ZERO_MEMORY,
		MAX_PATH * sizeof(WCHAR));

	if (DeviceInterfaceList == NULL) {
		hr = E_OUTOFMEMORY;
	}

	/************ Ieskom savo interfeiso pagal 'SerialNumber' **************/

	for (CurrentInterface = DeviceInterfaceList;
		*CurrentInterface;
		CurrentInterface += wcslen(CurrentInterface) + 1) {

		StringCbCopy(TmpDeviceInterfaceList,
			wcslen(CurrentInterface),
			CurrentInterface);

		str = wcstok_s(TmpDeviceInterfaceList, L"#", &token);
		str = wcstok_s(NULL, L"#", &token);
		str = wcstok_s(NULL, L"#", &token);

		if (wcscmp(str, DeviceSerialNumber) == 0)
		{
			hr = StringCbCopy(deviceData.FoundSerialNumber,
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
	hr = StringCbCopy(DevicePath,
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
	_In_	               LPTSTR DeviceSerialNumber
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

	DeviceData->DeviceHandle = CreateFile(DeviceData->DevicePath,
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

