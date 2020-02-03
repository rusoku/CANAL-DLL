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


#ifndef ___CANAL_A_H___
#define ___CANAL_A_H___


#ifndef EXPORT
#define EXPORT
#endif



#ifdef WIN32
#ifndef WINAPI
#define WINAPI __stdcall
#endif
#else
 
#endif

/* FILTER req type */
typedef enum {
  FILTER_ACCEPT_ALL   = 0,
  FILTER_REJECT_ALL,
  FILTER_VALUE,
}Filter_Type_TypeDef;


#ifdef __cplusplus
extern "C" {
#endif

/*!
	Set the 11bit filter  ID, Mask for a CANAL channel

	@param handle Handle to open physical CANAL channel.
	@return zero on success or error-code on failure.
*/
#ifdef WIN32
int WINAPI EXPORT CanalSetFilter11bit( long handle, Filter_Type_TypeDef type,  unsigned long list, unsigned long mask );
#else
int CanalSetFilter11bit( long handle, Filter_Type_TypeDef type, unsigned long id, unsigned long mask );
#endif

/*!
	Set the 29bit filter  ID, Mask for a CANAL channel

	@param handle Handle to open physical CANAL channel.
	@return zero on success or error-code on failure.
*/
#ifdef WIN32
int WINAPI EXPORT CanalSetFilter29bit( long handle, Filter_Type_TypeDef type,  unsigned long list, unsigned long mask );
#else
int CanalSetFilter29bit( long handle, Filter_Type_TypeDef type,  unsigned long id, unsigned long mask );
#endif

/*!
	Get bootloader ver

	@param handle Handle to open physical CANAL channel.
	@return zero on success or error-code on failure.
*/
#ifdef WIN32
int WINAPI EXPORT CanalGetBootloaderVersion(long handle, unsigned long *bootloader_version);
#else
int CanalGetBootloaderVersion(long handle, unsigned long *bootloader_version);
#endif


#ifdef WIN32
int WINAPI EXPORT CanalGetHardwareVersion(long handle, unsigned long *hardware_version);
#else
int CanalGetHardwareVersion(long handle, unsigned long *hardware_version);
#endif


#ifdef WIN32
int WINAPI EXPORT CanalGetFirmwareVersion(long handle, unsigned long *firmware_version);
#else
int CanalGetFirmwareVersion(long handle, unsigned long *firmware_version);
#endif

#ifdef WIN32
int WINAPI EXPORT CanalGetSerialNumber(long handle, unsigned long *serial);
#else
int CanalGetSerialNumber(long handle, unsigned long *serial);
#endif

#ifdef WIN32
int WINAPI EXPORT CanalGetVidPid(long handle, unsigned long *vidpid);
#else
int CanalGetVidPid(long handle, unsigned long *vidpid);
#endif

#ifdef WIN32
int WINAPI EXPORT CanalGetDeviceId(long handle, unsigned long *deviceid);
#else
int CanalGetDeviceId(long handle, unsigned long *deviceid);
#endif

#ifdef WIN32
int WINAPI EXPORT CanalGetVendor(long handle, unsigned int size, char *vendor);
#else
int CanalGetVendor(long handle, unsigned long *bootloader_version);
#endif

#ifdef WIN32
int WINAPI EXPORT CanalInterfaceStart(long handle);
#else
int CanalInterfaceStart(long handle);
#endif

#ifdef WIN32
int WINAPI EXPORT CanalInterfaceStop(long handle);
#else
int CanalInterfaceStop(long handle);
#endif

#ifdef __cplusplus
}
#endif

#endif //___CANAL_A_H___
