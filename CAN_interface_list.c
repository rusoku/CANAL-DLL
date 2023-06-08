/*
MIT License

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

#include <windows.h>
#include <strsafe.h>
#include <string.h>
#include <initguid.h>
#include <cfgmgr32.h>
#include <heapapi.h>
#include "include/CAN_interface_list.h"
#include "include/canal.h"
#include "include/guid.h"

CONFIGRET   cr = CR_SUCCESS;
ULONG   device_interface_list_length = 0;
TCHAR*  device_interface_list = NULL;
HANDLE  file_hd = NULL;
HRESULT hr = ERROR_SUCCESS;
size_t  DeviceStrLen = 0;
UINT16  CurrentDeviceIndex = 0;
TCHAR*  pCurrentIterfaceList;

/***
 *
 * @return cr
 */
DWORD CAN_interface_list(struct CAN_DEV_LIST* canDeviceList) {

    TCHAR   *token1 = NULL;
    TCHAR   *next_token1 = NULL;
    TCHAR   *token2 = NULL;
    TCHAR   *next_token2 = NULL;
    TCHAR   tmp_string[64];

    if (canDeviceList == NULL)
        return  CR_INVALID_POINTER;

    cr = CM_Get_Device_Interface_List_Size(&device_interface_list_length,
                                           (LPGUID)&GUID_DEVINTERFACE_WinUsbF4FS1,
                                           NULL,
                                           CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

    if (cr != CR_SUCCESS){
        hr = (HRESULT)CM_MapCrToWin32Err(cr, CR_DEFAULT);
        goto clean0;
    }

    if (device_interface_list_length <= 1)
        goto clean0;

    device_interface_list = (TCHAR*)HeapAlloc(GetProcessHeap(),
                                              HEAP_ZERO_MEMORY,
                                              device_interface_list_length * sizeof(TCHAR));

    if (device_interface_list == NULL) {
        hr = (HRESULT)ERROR_OUTOFMEMORY;
        goto clean0;
    }

    cr = CM_Get_Device_Interface_List((LPGUID)&GUID_DEVINTERFACE_WinUsbF4FS1,
                                      NULL,
                                      device_interface_list,
                                      device_interface_list_length,
                                      CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

    if (cr != CR_SUCCESS){
        hr = (HRESULT)CM_MapCrToWin32Err(cr, CR_DEFAULT);
        goto clean0;
    }

    pCurrentIterfaceList = device_interface_list;
    CurrentDeviceIndex = 0;

    for (UINT16 x = 0; x < TOTAL_DEVICES_AVAILABLE; x++)
    {
        hr = StringCchLengthA(pCurrentIterfaceList, STRSAFE_MAX_LENGTH, &DeviceStrLen);
        if (FAILED(hr) || DeviceStrLen == 0)
            break;

        file_hd = CreateFile(pCurrentIterfaceList,
                             GENERIC_WRITE | GENERIC_READ,
                             FILE_SHARE_WRITE | FILE_SHARE_READ,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                             NULL);

        if (file_hd != INVALID_HANDLE_VALUE)
        {
            /* Device ID */
            token1 = strtok_s(pCurrentIterfaceList,"#", &next_token1);
            StringCbCopyA(canDeviceList->canDevInfo[CurrentDeviceIndex].DeviceType,
                          sizeof(canDeviceList->canDevInfo[CurrentDeviceIndex].DeviceType)/sizeof(canDeviceList->canDevInfo[CurrentDeviceIndex].DeviceType[0]),
                          token1);

            if(token1 == NULL)
                goto next_device;

            /* VID, PID */
            token1 = strtok_s(NULL,"#", &next_token1);
            lstrcpynA(tmp_string, token1, sizeof(tmp_string)/sizeof(tmp_string[0]));
            token2 = strtok_s(tmp_string,"&", &next_token2);

            canDeviceList->canDevInfo[CurrentDeviceIndex].vid = (UINT16) strtoul(token2+4, NULL, 16);

            if(token2 == NULL)
                goto next_device;;

            token2 = strtok_s(NULL,"&", &next_token2);

            canDeviceList->canDevInfo[CurrentDeviceIndex].pid = (UINT16) strtoul(token2+4, NULL, 16);

            if(token1 == NULL)
                goto next_device;;

            /* Serial */
            token1 = strtok_s(NULL,"#", &next_token1);
            StringCbCopyA(canDeviceList->canDevInfo[CurrentDeviceIndex].SerialNumber,
                          sizeof(canDeviceList->canDevInfo[CurrentDeviceIndex].SerialNumber)/sizeof(canDeviceList->canDevInfo[CurrentDeviceIndex].SerialNumber[0]),
                          token1);

            if(token1 == NULL)
                goto next_device;;

            /* UUID */
            token1 = strtok_s(NULL,"#", &next_token1);

            StringCbCopyA(canDeviceList->canDevInfo[CurrentDeviceIndex].uuid,
                          sizeof(canDeviceList->canDevInfo[CurrentDeviceIndex].uuid)/sizeof(canDeviceList->canDevInfo[CurrentDeviceIndex].uuid[0]),
                          token1);

            CurrentDeviceIndex++;
        }

        next_device:
        CloseHandle(file_hd);
        pCurrentIterfaceList = DeviceStrLen + pCurrentIterfaceList + sizeof(TCHAR);
    }

    clean0:
    canDeviceList->canDevCount = CurrentDeviceIndex;
    HeapFree(GetProcessHeap(), 0, device_interface_list);

    return (HRESULT)hr;
}
