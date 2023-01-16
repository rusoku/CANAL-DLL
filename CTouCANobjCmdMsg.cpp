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

#include "stdafx.h"


//////////////////////////////////////////////////////////////////////
// TouCAN init
//

bool CTouCANObj::TouCAN_init(void)
{
	UCHAR	data[64];
	ULONG	Transfered;
	UINT8	res;
	WINUSB_SETUP_PACKET SetupPacket;

	SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_CAN_INTERFACE_INIT;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 9;
	// tseg1
	data[0] = m_Tseg1;
	// tseg2
	data[1] = m_Tseg2;
	// sjw
	data[2] = m_Sjw;
	// Brp

	data[3] = (UINT8) ((m_Brp >> 8) & 0xFF);    
	data[4] = (UINT8) (m_Brp & 0xFF);          

	// flags
	data[5] = (UINT8) ((m_OptionFlag >> 24) & 0xFF);   
	data[6] = (UINT8) ((m_OptionFlag >> 16) & 0xFF);    
	data[7] = (UINT8) ((m_OptionFlag >> 8) & 0xFF);    
	data[8] = (UINT8)  (m_OptionFlag & 0xFF);          

	// TouCAN_CAN_INTERFACE_INIT
	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &data[0], 9, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN deinit
//

bool CTouCANObj::TouCAN_deinit(void)
{
	WINUSB_SETUP_PACKET SetupPacket;
	UINT8	res;

	SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_CAN_INTERFACE_DEINIT;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 0;

	// TouCAN_CAN_INTERFACE_DEINIT
	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, NULL, NULL, NULL, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return	TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN start
//

bool CTouCANObj::TouCAN_start(void)
{
	WINUSB_SETUP_PACKET SetupPacket;
	UINT8	res;

	SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_CAN_INTERFACE_START;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 0;

	// TouCAN_CAN_INTERFACE_START
	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, NULL, NULL, NULL, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN stop
//

bool CTouCANObj::TouCAN_stop(void)
{
	WINUSB_SETUP_PACKET SetupPacket;
	UINT8	res;

	SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_CAN_INTERFACE_STOP;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 0;

	// TouCAN_CAN_INTERFACE_START
	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, NULL, NULL, NULL, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return	TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN HAL get last error
//

bool	CTouCANObj::TouCAN_get_last_error_code(UINT8 *res)
{
	UINT8	LastErrorCode;
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	if (res == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_LAST_ERROR_CODE;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 1;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &LastErrorCode, 1, &Transfered, NULL) != TRUE)
		return	FALSE;

	if ((LastErrorCode != HAL_OK) & (Transfered != 1))
		return	FALSE;

	*res = LastErrorCode;

	return	TRUE;
}

///////////////////////////////////////////////////////
// TouCAN get CAN interface ERROR:  hcan->ErrorCode;
//

bool	CTouCANObj::TouCAN_get_interface_error_code(UINT32 *ErrorCode)
{
	WINUSB_SETUP_PACKET	SetupPacket;
	UINT8	buf[4];
	ULONG	Transfered;
	UINT8	res;
	UINT32  ErrorCodeTmp;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_CAN_INTERFACE_ERROR_CODE;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	ErrorCodeTmp  = (((UINT32) buf[0] << 24) & 0xFF000000);
	ErrorCodeTmp |= (((UINT32) buf[1] << 16) & 0x00FF0000);
	ErrorCodeTmp |= (((UINT32) buf[2] << 8)  & 0x0000FF00);
	ErrorCodeTmp |=  ((UINT32) buf[3]        & 0x000000FF);

	*ErrorCode = ErrorCodeTmp;
	return	TRUE;
}	

///////////////////////////////////////////////////////
// TouCAN TouCAN_CLEAR_CAN_INTERFACE_ERROR_CODE:  hcan->ErrorCode;
//

bool	CTouCANObj::TouCAN_clear_interface_error_code(void)
{
	WINUSB_SETUP_PACKET SetupPacket;
	UINT8	res;

	SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_CLEAR_CAN_INTERFACE_ERROR_CODE;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 0;

	// TouCAN_CAN_INTERFACE_START
	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, NULL, NULL, NULL, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return	TRUE;
}


//////////////////////////////////////////////////////////////////////
// TouCAN TouCAN_GET_CAN_INTERFACE_STATE   hcan->State;
//

bool	CTouCANObj::TouCAN_get_interface_state(UINT8 *state)
{
	//UINT8	LastErrorCode;
	UINT8	res;
	UINT8	StateTmp;
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	if (state == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_CAN_INTERFACE_STATE;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 1;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &StateTmp, 1, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	*state = StateTmp;
	return	TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN TouCAN_get_statistics VSCP
//

bool	CTouCANObj::TouCAN_get_statistics(PCANALSTATISTICS statistics)
{
	//UINT8	LastErrorCode;
	UINT8	buf[30];
	UINT8	res;
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	if (statistics == NULL)
		   return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_STATISTICS;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 28;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 28, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	m_statistics.cntReceiveFrames  = (((UINT32)buf[0] << 24) & 0xFF000000);
	m_statistics.cntReceiveFrames |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	m_statistics.cntReceiveFrames |= (((UINT32)buf[2] <<  8) & 0x0000FF00);
	m_statistics.cntReceiveFrames |=  ((UINT32)buf[3]        & 0x000000FF);

	m_statistics.cntTransmitFrames  = (((UINT32)buf[4] << 24) & 0xFF000000);
	m_statistics.cntTransmitFrames |= (((UINT32)buf[5] << 16) & 0x00FF0000);
	m_statistics.cntTransmitFrames |= (((UINT32)buf[6] <<  8) & 0x0000FF00);
	m_statistics.cntTransmitFrames |=  ((UINT32)buf[7]        & 0x000000FF);

	m_statistics.cntReceiveData  = (((UINT32)buf[8] << 24) & 0xFF000000);
	m_statistics.cntReceiveData |= (((UINT32)buf[9] << 16) & 0x00FF0000);
	m_statistics.cntReceiveData |= (((UINT32)buf[10] << 8) & 0x0000FF00);
	m_statistics.cntReceiveData |=  ((UINT32)buf[11]       & 0x000000FF);

	m_statistics.cntTransmitData  = (((UINT32)buf[12] << 24) & 0xFF000000);
	m_statistics.cntTransmitData |= (((UINT32)buf[13] << 16) & 0x00FF0000);
	m_statistics.cntTransmitData |= (((UINT32)buf[14] << 8) & 0x0000FF00);
	m_statistics.cntTransmitData |=  ((UINT32)buf[15]       & 0x000000FF);

	m_statistics.cntOverruns =  (((UINT32)buf[16] << 24) & 0xFF000000);
	m_statistics.cntOverruns |= (((UINT32)buf[17] << 16) & 0x00FF0000);
	m_statistics.cntOverruns |= (((UINT32)buf[18] << 8)  & 0x0000FF00);
	m_statistics.cntOverruns |=  ((UINT32)buf[19]        & 0x000000FF);

	m_statistics.cntBusWarnings  = (((UINT32)buf[20] << 24) & 0xFF000000);
	m_statistics.cntBusWarnings |= (((UINT32)buf[21] << 16) & 0x00FF0000);
	m_statistics.cntBusWarnings |= (((UINT32)buf[22] <<  8) & 0x0000FF00);
	m_statistics.cntBusWarnings |=  ((UINT32)buf[23]        & 0x000000FF);

	m_statistics.cntBusOff =  (((UINT32)buf[24] << 24) & 0xFF000000);
	m_statistics.cntBusOff |= (((UINT32)buf[25] << 16) & 0x00FF0000);
	m_statistics.cntBusOff |= (((UINT32)buf[26] <<  8) & 0x0000FF00);
	m_statistics.cntBusOff |=  ((UINT32)buf[27]        & 0x000000FF);

	//TouCAN_clear_statistics();

	return	TRUE;
}


//////////////////////////////////////////////////////////////////////
// TouCAN_clear_statistics VSCP
//

bool	CTouCANObj::TouCAN_clear_statistics(void)
{
	UINT8	res;
	WINUSB_SETUP_PACKET	SetupPacket;

	SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_CLEAR_STATISTICS;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 0;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, NULL, NULL, NULL, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return	TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN_get_status VSCP
//

bool	CTouCANObj::TouCAN_get_canal_status(canalStatus *status)
{
	UINT8	res;
	canalStatus  StatusTmp;
	UINT8	buf[4];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	if (status == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_CAN_ERROR_STATUS;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	StatusTmp.channel_status  = (((UINT32)buf[0] << 24) & 0xFF000000);
	StatusTmp.channel_status |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	StatusTmp.channel_status |= (((UINT32)buf[2] << 8)  & 0x0000FF00);
	StatusTmp.channel_status |= (((UINT32)buf[3])       & 0x000000FF);

	*status = StatusTmp;
	return	TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN_get_hardware_version
//
 
bool   CTouCANObj::TouCAN_get_hardware_version(UINT32 *ver)
{
	UINT8	res;
	UINT8	buf[4];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	UINT32	temp;

	if (ver == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_HARDWARE_VERSION;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	temp = (((UINT32)buf[0] << 24) & 0xFF000000);
	temp |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	temp |= (((UINT32)buf[2] << 8) & 0x0000FF00);
	temp |= ((UINT32) buf[3] & 0x000000FF);

	*ver = temp;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN_get_firmware_version
//
//
bool   CTouCANObj::TouCAN_get_firmware_version(UINT32 *ver)
{
	UINT8	res;
	UINT8	buf[4];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	UINT32	temp;

	if (ver == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_FIRMWARE_VERSION;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	temp = (((UINT32)buf[0] << 24) & 0xFF000000);
	temp |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	temp |= (((UINT32)buf[2] << 8) & 0x0000FF00);
	temp |= ((UINT32)buf[3] & 0x000000FF);

	*ver = temp;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// TouCAN_get_bootloader_version
//
//
bool   CTouCANObj::TouCAN_get_bootloader_version(UINT32 *ver)
{
	UINT8	res;
	UINT8	buf[4];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	UINT32	temp;

	if (ver == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_BOOTLOADER_VERSION;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	temp = (((UINT32)buf[0] << 24) & 0xFF000000);
	temp |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	temp |= (((UINT32)buf[2] << 8) & 0x0000FF00);
	temp |= ((UINT32)buf[3] & 0x000000FF);

	*ver = temp;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN_get_serial_number
//
// 
bool   CTouCANObj::TouCAN_get_serial_number(UINT32 *ver)
{
	UINT8	res;
	UINT8	buf[4];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	UINT32	temp;

	if (ver == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_SERIAL_NUMBER;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	temp =  (((UINT32)buf[0] << 24) & 0xFF000000);
	temp |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	temp |= (((UINT32)buf[2] <<  8) & 0x0000FF00);
	temp |=	((UINT32) buf[3]        & 0x000000FF);

	*ver = temp;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN_get_vid_pid
//
//
bool   CTouCANObj::TouCAN_get_vid_pid(UINT32 *ver)
{
	//UINT8	LastErrorCode;
	UINT8	res;
//	canalStatus  StatusTmp;
	UINT8	buf[4];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	UINT32	temp;

	if (ver == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_VID_PID;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	temp = (((UINT32)buf[0] << 24) & 0xFF000000);
	temp |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	temp |= (((UINT32)buf[2] << 8) & 0x0000FF00);
	temp |= ((UINT32)buf[3] & 0x000000FF);

	*ver = temp;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN_get_device_id
//
//
bool   CTouCANObj::TouCAN_get_device_id(UINT32 *ver)
{
	//UINT8	LastErrorCode;
	UINT8	res;
//	canalStatus  StatusTmp;
	UINT8	buf[4];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	UINT32	temp;

	if (ver == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_DEVICE_ID;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	temp = (((UINT32)buf[0] << 24) & 0xFF000000);
	temp |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	temp |= (((UINT32)buf[2] << 8) & 0x0000FF00);
	temp |= ((UINT32)buf[3] & 0x000000FF);

	*ver = temp;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TouCAN_get_vendor
//
//
bool   CTouCANObj::TouCAN_get_vendor(unsigned int size, CHAR *str)
{
	UINT8	res;
	UINT8	buf[32];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;

	if (str == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_VENDOR;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	//wprintf(L"TouCAN_GET_STATUS = ");
	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 32, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	strcpy_s(str, size,(CHAR*) buf);

	return TRUE;
}


bool CTouCANObj::TouCAN_get_interface_transmit_delay(UINT8 channel, UINT32 *delay)
{
	UINT8	res;
	UINT8	buf[4];
	ULONG	Transfered;
	WINUSB_SETUP_PACKET	SetupPacket;
	UINT32	temp;

	if (delay == NULL)
		return FALSE;

	SetupPacket.RequestType = USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_GET_CAN_INTERFACE_DELAY;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 4;

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	temp =  (((UINT32)buf[0] << 24) & 0xFF000000);
	temp |= (((UINT32)buf[1] << 16) & 0x00FF0000);
	temp |= (((UINT32)buf[2] << 8)  & 0x0000FF00);
	temp |= ((UINT32)buf[3] &         0x000000FF);

	*delay = temp;
	return TRUE;
}

bool  CTouCANObj::TouCAN_set_interface_transmit_delay(UINT8 channel, UINT32 *delay)
{
	UINT8	res;
	WINUSB_SETUP_PACKET	SetupPacket;
	UINT32  temp;
	UINT8	buf[4];
	ULONG	Transfered;

	SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	SetupPacket.Request = TouCAN_SET_CAN_INTERFACE_DELAY;
	SetupPacket.Value = 0;
	SetupPacket.Index = 0;
	SetupPacket.Length = 0;

	temp = *delay;

	buf[0] = (UINT8)((temp >> 24) & 0xFF);
	buf[1] = (UINT8)((temp >> 16) & 0xFF);
	buf[2] = (UINT8)((temp >> 8) & 0xFF);
	buf[3] = (UINT8)(temp & 0xFF);

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 4, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return	TRUE;
}

///////////////////////////////////////// LIST, MASK ////////////////////////////////////////////
bool CTouCANObj::TouCAN_set_filter_std_list_mask( Filter_Type_TypeDef type, UINT32 list, UINT32 mask)
{
	UINT8	res;
	WINUSB_SETUP_PACKET	SetupPacket;
	UINT8	buf[8];
	ULONG	Transfered;

	switch (type)
	{
	case	FILTER_ACCEPT_ALL:

		SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		SetupPacket.Request = TouCAN_FILTER_STD_ACCEPT_ALL;
		SetupPacket.Value = 0;
		SetupPacket.Index = 0;
		SetupPacket.Length = 0;

		break;
	case	FILTER_REJECT_ALL:

		SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		SetupPacket.Request = TouCAN_FILTER_STD_REJECT_ALL;
		SetupPacket.Value = 0;
		SetupPacket.Index = 0;
		SetupPacket.Length = 0;

		break;
	case	FILTER_VALUE:

		SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		SetupPacket.Request = TouCAN_SET_FILTER_STD_LIST_MASK;
		SetupPacket.Value = 0;
		SetupPacket.Index = 0;
		SetupPacket.Length = 0;

		break;
	default:
		break;
	}

	buf[0] = (UINT8)((list >> 24) & 0xFF);
	buf[1] = (UINT8)((list >> 16) & 0xFF);
	buf[2] = (UINT8)((list >> 8) & 0xFF);
	buf[3] = (UINT8) (list & 0xFF);

	buf[4] = (UINT8)((mask >> 24) & 0xFF);
	buf[5] = (UINT8)((mask >> 16) & 0xFF);
	buf[6] = (UINT8)((mask >> 8) & 0xFF);
	buf[7] = (UINT8) (mask & 0xFF);

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 8, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return	TRUE;
}

bool CTouCANObj::TouCAN_set_filter_ext_list_mask(Filter_Type_TypeDef type, UINT32 list, UINT32 mask)
{
	UINT8	res;
	WINUSB_SETUP_PACKET	SetupPacket;
//	UINT32  temp;
	UINT8	buf[8];
	ULONG	Transfered;

	switch (type)
	{
	case	FILTER_ACCEPT_ALL:

		SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		SetupPacket.Request = TouCAN_FILTER_EXT_ACCEPT_ALL;
		SetupPacket.Value = 0;
		SetupPacket.Index = 0;
		SetupPacket.Length = 0;

		break;
	case	FILTER_REJECT_ALL:

		SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		SetupPacket.Request = TouCAN_FILTER_EXT_REJECT_ALL;
		SetupPacket.Value = 0;
		SetupPacket.Index = 0;
		SetupPacket.Length = 0;

		break;
	case	FILTER_VALUE:

		SetupPacket.RequestType = USB_HOST_TO_DEVICE | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		SetupPacket.Request = TouCAN_SET_FILTER_EXT_LIST_MASK;
		SetupPacket.Value = 0;
		SetupPacket.Index = 0;
		SetupPacket.Length = 0;

		break;
	default:
		break;
	}

	buf[0] = (UINT8)((list >> 24) & 0xFF);
	buf[1] = (UINT8)((list >> 16) & 0xFF);
	buf[2] = (UINT8)((list >> 8) & 0xFF);
	buf[3] = (UINT8) (list & 0xFF);

	buf[4] = (UINT8)((mask >> 24) & 0xFF);
	buf[5] = (UINT8)((mask >> 16) & 0xFF);
	buf[6] = (UINT8)((mask >> 8) & 0xFF);
	buf[7] = (UINT8) (mask & 0xFF);

	if (WinUsb_ControlTransfer(deviceData.WinusbHandle, SetupPacket, &buf[0], 8, &Transfered, NULL) != TRUE)
		return	FALSE;

	if (TouCAN_get_last_error_code(&res) != TRUE)
		return FALSE;

	if (res != TouCAN_RETVAL_OK)
		return FALSE;

	return	TRUE;
}


