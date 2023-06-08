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

#pragma once

#include <windows.h>

#define TOTAL_DEVICES_AVAILABLE 8

struct CAN_DEV_INFO{
    TCHAR   DeviceType[64];
    TCHAR   uuid[64];
    UINT16  vid;
    UINT16  pid;
    TCHAR   SerialNumber[64];
};

struct CAN_DEV_LIST{
    struct CAN_DEV_INFO canDevInfo[TOTAL_DEVICES_AVAILABLE];
    UINT8   canDevCount;
};


/*
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
*/

//DEFINE_GUID(GUID_INTERFACE_WinUsbF4FS1,
//            0xFD361109, 0x858D, 0x4F6F, 0x81, 0xEE, 0xAA, 0xB5, 0xD6, 0xCB, 0xF0, 0x6B);

#ifdef __cplusplus
extern "C"
{
#endif

DWORD CAN_interface_list(struct CAN_DEV_LIST* canDeviceList);

#ifdef __cplusplus
}
#endif

