MIT License

Copyright (c) 2000-2023 Åke Hedman, Grodans Paradis AB
Copyright (c) 2005-2023 Gediminas Simanskis, Rusoku technologijos UAB (gediminas@rusoku.com)
Copyright (C) 2020 Alexander Sorokin (sorockin@yandex.ru)
Copyright (C) 2020 Uwe Vogt (uwe.vogt@uv-software.de)

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


CTouCANObj::CTouCANObj()
{
	deviceData.HandlesOpen = FALSE;
	noDevice = TRUE;

	// RX&TX list init
	pDllList = new CDllList;
	pDllList->Init(&m_receiveList, SORT_NONE);
	pDllList->Init(&m_transmitList, SORT_NONE);

	// open, closed
	m_bOpen = FALSE;
	// for treads
	m_bRun = FALSE;

	// Tasks
	m_bRunRxTask = FALSE;
	m_bRunTxTask = FALSE;

	// DEBUG counter
	TransmitCounter = 0;
	ReceiveCounter = 0;

	// receive list mutex
	m_receiveListMutex = CreateMutex(NULL, FALSE, NULL);
	m_transmitListMutex = CreateMutex(NULL, FALSE, NULL);

	// Events
	m_receiveDataEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_transmitDataPutEvent = CreateEvent(NULL, TRUE, TRUE, NULL);   // default: signaled
	m_transmitDataGetEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  // default: not signaled

#ifdef DEBUG_CANAL
	if (!AllocConsole())
	{
    	MessageBox(NULL, L"The console window was not created", NULL, MB_ICONEXCLAMATION);
	}
	freopen_s(&fp, "CONOUT$", "w", stdout); // <=-- DEBUG
	//fopen_s(&log,"log.txt", "w+");
#endif // DEBUG_CANAL
}


CTouCANObj::~CTouCANObj()
{

	//CloseDevice(&deviceData);
	m_bOpen = FALSE;
	m_bRun = FALSE;

	// Mutex
	//CloseHandle(m_receiveListMutex);
	//CloseHandle(m_transmitListMutex);

	// Events
	//CloseHandle(m_receiveDataEvent);
	//CloseHandle(m_transmitDataPutEvent);
	//CloseHandle(m_transmitDataGetEvent);

	//delete pDllList;

#ifdef DEBUG_CANAL
//	fclose(fp);
	//fclose(log);
	FreeConsole();
	//if (!FreeConsole())
	//	MessageBox(NULL, L"Failed to free the console!", NULL, MB_ICONEXCLAMATION);
#endif
}


//////////////////////////////////////////////////////////////////////
//  DLL API interface
//


//////////////////////////////////////////////////////////////////////
// Open
//

bool CTouCANObj::Open(const char * szFileName, unsigned long flags, bool start) {

	HRESULT   hr;
	WCHAR	szDrvParams[256];
	LPWSTR	szDrvParamsToken;
	LPWSTR  token;
	LPWSTR	pEnd; // wcstol()

	// if open we have noting to do
	if (m_bOpen == TRUE)
		return FALSE;

	if (NULL == szFileName)
		return FALSE;

	// Intin string not more 100 symbols
	if (strlen(szFileName) > 100)
		return FALSE;

	// convert configuration string to UNICODE
	MultiByteToWideChar(CP_UTF8, 0, szFileName, -1, szDrvParams, 256);

	// clean whitespace characters
	MyStrTrim(szDrvParams);

	//------------------ Device ID --------------------------

	//Looking for "Device ID"
	//szDrvParamsToken = wcstok_s(NULL, L";", &token);
	szDrvParamsToken = wcstok_s(szDrvParams, L";", &token);
	//wprintf(L"String Speed=%s\n", szDrvParamsToken);

	// wrong init string format "Device ID"
	if (szDrvParamsToken == NULL)
		return FALSE;

	// Convert to integer "Device ID"
	m_DeviceId = (INT16)wcstol(szDrvParamsToken, &pEnd, 10);
	//	wprintf(L"m_DeviceId=%d\n", m_DeviceId);

	//------------------ serial number ------------------

		// Looking for "serial number"
		//szDrvParamsToken = wcstok_s(szDrvParams, L";", &token);
	szDrvParamsToken = wcstok_s(NULL, L";", &token);
	//wprintf(L"SerialNumber=%s\n", szDrvParamsToken);

	// wrong init string format "serial number"
	if (szDrvParamsToken == NULL)
		return FALSE;

	// "serial number" length must be 8 symbols
	if (wcslen(szDrvParamsToken) != 8)
		return FALSE;

	// Coping "serial number"
	if ((wcscpy_s(m_SerialNumber, sizeof(m_SerialNumber), szDrvParamsToken)) != 0)
		return FALSE;

	//------------------ speed --------------------------

		//Looking for "speed"
	szDrvParamsToken = wcstok_s(NULL, L";", &token);
	//wprintf(L"String Speed=%s\n", szDrvParamsToken);

	// wrong init string format "speed"
	if (szDrvParamsToken == NULL)
		return FALSE;

	// Convert to integer "speed"
	m_Speed = (INT16)wcstol(szDrvParamsToken, &pEnd, 10);
	//wprintf(L"Converted Speed=%d\n", m_Speed);

//------------------ ext parameters if(speed == 0) ------------------
	if (m_Speed == 0) {
		//------------------------------ Tseg1 -------------------------------

				//Looking for "tseg1"
		szDrvParamsToken = wcstok_s(NULL, L";", &token);
		//wprintf(L"String tseg1=%s\n", szDrvParamsToken);

		// wrong init string format "Tseg1"
		if (szDrvParamsToken == NULL)
			return FALSE;

		// Convert to integer "Tseg1"
		m_Tseg1 = (INT8)wcstol(szDrvParamsToken, &pEnd, 10);
		//wprintf(L"Converted Tseg1=%d\n", m_Tseg1);

//---------------------------- Tseg2 ---------------------------------

		//Looking for "tseg2"
		szDrvParamsToken = wcstok_s(NULL, L";", &token);
		//wprintf(L"String Tseg2=%s\n", szDrvParamsToken);

		// wrong init string format "Tseg2"
		if (szDrvParamsToken == NULL)
			return FALSE;

		// Convert to integer "Tseg2"
		m_Tseg2 = (INT8)wcstol(szDrvParamsToken, &pEnd, 10);
		//wprintf(L"Converted Tseg2=%d\n", m_Tseg2);

//--------------------------- Sjw -------------------------------------

		//Looking for "Sjw"
		szDrvParamsToken = wcstok_s(NULL, L";", &token);
		//wprintf(L"String Sjw=%s\n", szDrvParamsToken);

		// wrong init string format "Sjw"
		if (szDrvParamsToken == NULL)
			return FALSE;

		// Convert to integer "Sjw"
		m_Sjw = (INT8)wcstol(szDrvParamsToken, &pEnd, 10);
		//wprintf(L"Converted Sjw=%d\n", m_Sjw);

//--------------------------- Brp -------------------------------------

		//Looking for "Brp"
		szDrvParamsToken = wcstok_s(NULL, L";", &token);
		//wprintf(L"String Brp=%s\n", szDrvParamsToken);

		// wrong init string format "Brp"
		if (szDrvParamsToken == NULL)
			return FALSE;

		// Convert to integer "Brp"
		m_Brp = (INT16)wcstol(szDrvParamsToken, &pEnd, 10);
		//wprintf(L"Converted Brp=%d\n", m_Brp);
	}
	else {  //if (m_Speed == 0)

		// CAN base clock = 50 MHz

		switch (m_Speed)
		{
		case	1000:
			// 75%
			m_Brp = 5;
			m_Tseg1 = 7;
			m_Tseg2 = 2;
			m_Sjw = 2;
			break;
		case	500:
			// 75%
			m_Brp = 5;
			m_Tseg1 = 14;
			m_Tseg2 = 5;
			m_Sjw = 4;
			break;
		case	250:
			// 75%
			m_Brp = 10;
			m_Tseg1 = 14;
			m_Tseg2 = 5;
			m_Sjw = 4;
			break;
		case	125:
			// 75%
			m_Brp = 20;
			m_Tseg1 = 14;
			m_Tseg2 = 5;
			m_Sjw = 4;
			break;
		case	100:
			// 75%
			m_Brp = 25;
			m_Tseg1 = 14;
			m_Tseg2 = 5;
			m_Sjw = 4;
			break;
		case	50:
			// 75%
			m_Brp = 50;
			m_Tseg1 = 14;
			m_Tseg2 = 5;
			m_Sjw = 4;
			break;
		case	20:
			// 75%
			m_Brp = 125;
			m_Tseg1 = 14;
			m_Tseg2 = 5;
			m_Sjw = 4;
			break;
		case	10:
			// 75%
			m_Brp = 250;
			m_Tseg1 = 14;
			m_Tseg2 = 5;
			m_Sjw = 4;
			break;
		default:
			return	FALSE;
		}
	}

	if ((m_Tseg1 < 1) || (m_Tseg1 > 16))
		return FALSE;

	if ((m_Tseg2 < 1) || (m_Tseg2 > 8))
		return FALSE;

	if ((m_Sjw < 1) || (m_Sjw > 4))
		return FALSE;

	if (m_Brp > 1024)
		return FALSE;

	m_OptionFlag = flags;

	//wprintf(L"\n==================================\n");
	//wprintf(L"Tseg1=%d\n", m_Tseg1);
	//wprintf(L"Tseg2=%d\n", m_Tseg2);
	//wprintf(L"Sjw=%d\n", m_Sjw);
	//wprintf(L"Brp=%d\n", m_Brp);
	//wprintf(L"flags=%d\n", m_OptionFlag);

//------------------------ Find WinUSB TouCAN device ----------------------------
	hr = OpenDevice(&deviceData, &noDevice, m_SerialNumber);

	if (FAILED(hr))
		if (noDevice) {
			//wprintf(L"Device not connected or driver not installed\n");
			return FALSE;
		}
		else {
			//wprintf(L"Failed looking for device, HRESULT 0x%x\n", hr);
			return FALSE;
		}

	// check is it our DeviceId or not
	UINT32   dev_id;
	TouCAN_get_device_id(&dev_id);

	if (dev_id != m_DeviceId)
	{
		//wprintf(L"TouCAN_DEV_ID_ERROR\n");
		CloseDevice(&deviceData);
		m_bOpen = FALSE;
		return FALSE;
	}

	m_bOpen = TRUE;
	m_bRun = TRUE;

	// Start RX thread
	m_bRunRxTask = TRUE;
	m_bRunTxTask = TRUE;

	if (NULL == (m_hTreadReceive = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)workThreadReceive, this, 0, NULL)))
	{
		return FALSE;
	}

	if (NULL == (m_hTreadTransmit = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)workThreadTransmit, this, 0, NULL)))
	{
		return FALSE;
	}

	SetThreadPriority(m_hTreadReceive, THREAD_PRIORITY_TIME_CRITICAL);
	SetThreadPriority(m_hTreadTransmit, THREAD_PRIORITY_TIME_CRITICAL);

	Sleep(100);

	pDllList->RemoveAllNodes(&m_receiveList);
	pDllList->RemoveAllNodes(&m_transmitList);

	UINT8 state;
	TouCAN_get_interface_state(&state);

	if (state == HAL_CAN_STATE_READY)
	{
		//wprintf(L"state:HAL_CAN_STATE_READY\n");
		TouCAN_stop();
		TouCAN_deinit();
	}

	if (state == HAL_CAN_STATE_LISTENING)
	{
		//wprintf(L"state:HAL_CAN_STATE_LISTENING\n");
		TouCAN_stop();
		TouCAN_deinit();
	}

	//  TouCAN initialisation
	if (TouCAN_init() == FALSE)
	{
		//wprintf(L"TouCAN_init ERROR\n");
		TerminateThreads();
		CloseDevice(&deviceData);
		m_bOpen = FALSE;
		return FALSE;
	}

	//  TouCAN CAN interface start
	if (start == TRUE)
	{
		if (TouCAN_start() == FALSE)
		{
			//wprintf(L"TouCAN_start ERROR\n");
			TerminateThreads();
			CloseDevice(&deviceData);
			m_bOpen = FALSE;
			return FALSE;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// TerminateThreads
//
void CTouCANObj::TerminateThreads(void) {
	DWORD rv = ERROR_INVALID_PARAMETER;

	// Terminate Rx Thread
	m_bRunRxTask = FALSE;
	// Terminate Rx Thread
	m_bRunTxTask = FALSE;

	while (true)
	{
		GetExitCodeThread(m_hTreadReceive, &rv);
		if (rv != STILL_ACTIVE)
			break;
	}
	//wprintf(L"GetExitCodeReceiveThread=%X\n", rv);

	while (true)
	{
		GetExitCodeThread(m_hTreadTransmit, &rv);
		if (rv != STILL_ACTIVE)
			break;
	}
	//wprintf(L"GetExitCodeTransmitThread=%X\n", rv);
}

//////////////////////////////////////////////////////////////////////
// Close
//

bool CTouCANObj::Close(void){

	DWORD rv = ERROR_INVALID_PARAMETER;

	//  TouCAN CAN interface stop
	TouCAN_stop();
	//  TouCAN initialisation
	TouCAN_deinit();

	Sleep(100);

	m_bOpen = FALSE;
	m_bRun = FALSE;

	TerminateThreads();

	CloseDevice(&deviceData);

	pDllList->RemoveAllNodes(&m_receiveList);
	pDllList->RemoveAllNodes(&m_transmitList);

	return TRUE;
}

bool CTouCANObj::Start(void)
{
	// Must be open
	if (m_bOpen == FALSE)
		return FALSE;

	return TouCAN_start();
}

bool CTouCANObj::Stop(void)
{
	// Must be open
	if (m_bOpen == FALSE)
		return FALSE;

	return TouCAN_stop();
}

//////////////////////////////////////////////////////////////////////
// SetFilter
//

bool CTouCANObj::SetFilter(unsigned long filter)
{
	//m_filter = filter;

	// Must be open
	if (m_bOpen == FALSE)
		return FALSE;

	if (m_bRun == FALSE)
		return FALSE;

	return TRUE;
}

bool CTouCANObj::SetMask(unsigned long mask)
{
	//m_mask = mask;

	 //Must be open
	  if (m_bOpen == FALSE)
	    	return FALSE;

	  if (m_bRun == FALSE)
		  return FALSE;

	return TRUE;
}

int CTouCANObj::SetFilter11bit(Filter_Type_TypeDef type, unsigned long list, unsigned long mask)
{
	//Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_NOT_OPEN;

	if (TouCAN_set_filter_std_list_mask( type, list, mask) != TRUE)
	{
		return	CANAL_ERROR_COMMUNICATION;
	}
	return CANAL_ERROR_SUCCESS;
}

int CTouCANObj::SetFilter29bit(Filter_Type_TypeDef type, unsigned long list, unsigned long mask)
{
	//Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_NOT_OPEN;

	if (TouCAN_set_filter_ext_list_mask(type, list, mask) != TRUE)
	{
		return	CANAL_ERROR_COMMUNICATION;
	}
	return CANAL_ERROR_SUCCESS;

}


///////////////////////////////////////////////////////////////////////////////
//	getStatistics
//

int CTouCANObj::GetStatistics(PCANALSTATISTICS pCanalStatistics)
{
	//UINT32   ver = 0;

	// Must be open
	if (m_bOpen != TRUE)
		return CANAL_ERROR_NOT_OPEN;

	if (m_bRun != TRUE)
		return CANAL_ERROR_HARDWARE;

	// Must be a message pointer
	if (NULL == pCanalStatistics)
		return CANAL_ERROR_PARAMETER;

	ReceiveCounter = 0;
	TransmitCounter = 0;

	if (TouCAN_get_statistics(&m_statistics) != TRUE)
	{
		return	CANAL_ERROR_COMMUNICATION;
	}

	*pCanalStatistics = m_statistics;

	//wprintf(L"Bus off=%08X\n", m_statistics.cntBusOff);

	return CANAL_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//	dataAvailable
//

int CTouCANObj::DataAvailable(void)
{
    //Must be open
	if (!m_bOpen) return 0;
	if (!m_bRun) return 0;

	return  m_receiveList.nCount;
}

///////////////////////////////////////////////////////////////////////////////
//	getStatus
//

int CTouCANObj::GetStatus(PCANALSTATUS pCanalStatus)
{
	canalStatus   status;

	// Must be a message pointer
	if (NULL == pCanalStatus)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_get_canal_status(&status) != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	*pCanalStatus = status;

	return CANAL_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// readMsg
// Non blocking

int CTouCANObj::ReadMsg(canalMsg *pMsg)
{
	// Must be a message pointer
	if (NULL == pMsg)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (m_receiveList.nCount == 0)
		return  CANAL_ERROR_FIFO_EMPTY;

	if (m_bRunRxTask != TRUE)
		return CANAL_ERROR_HARDWARE;

	LOCK_MUTEX(m_receiveListMutex);
	memcpy_s(pMsg, sizeof(canalMsg), m_receiveList.pHead->pObject, sizeof(canalMsg));
	pDllList->RemoveNode(&m_receiveList, m_receiveList.pHead);
	UNLOCK_MUTEX(m_receiveListMutex);

	ResetEvent(m_receiveDataEvent);

	//wprintf(L"Counter=%d\n", ++counter);

	return CANAL_ERROR_SUCCESS;
}



//////////////////////////////////////////////////////////////////////
// readMsg
// Blocking

int CTouCANObj::ReadMsgBlocking(canalMsg *pMsg, ULONG	 Timeout)
{
	//int rv = CANAL_ERROR_SUCCESS;
	//DWORD res;

	// Must be a message pointer
	if (NULL == pMsg)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	// Must be a message pointer
	if (NULL == pMsg)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (m_bRunRxTask != TRUE)
		return	CANAL_ERROR_HARDWARE;

	// Yes we block if inqueue is empty
	if (m_receiveList.nCount == 0)
	{
		if (WaitForSingleObject(m_receiveDataEvent, Timeout) != WAIT_OBJECT_0)
			return  CANAL_ERROR_TIMEOUT;
		//
		//if( res != )
/*
		if (res == WAIT_TIMEOUT)
			return CANAL_ERROR_TIMEOUT;
		else if (res == WAIT_ABANDONED)
			return CANAL_ERROR_GENERIC;
*/
	}

	if (m_receiveList.nCount > 0)
	{
		LOCK_MUTEX(m_receiveListMutex);
		memcpy_s(pMsg, sizeof(canalMsg), m_receiveList.pHead->pObject, sizeof(canalMsg));
		pDllList->RemoveNode(&m_receiveList, m_receiveList.pHead);
        UNLOCK_MUTEX(m_receiveListMutex);

		ResetEvent(m_receiveDataEvent);
		//wprintf(L"ReceiveCounter=%I64d\n", ++ReceiveCounter);
		//wprintf(L"============== Received Message ===============\n");
	}
	else
		return  CANAL_ERROR_FIFO_EMPTY;

	return CANAL_ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////
// writeMsg
//

int CTouCANObj::WriteMsg(canalMsg *pMsg)
{
	ULONG  cnt;
	DWORD res;

	// Must be a message pointer
	if (NULL == pMsg)
		return CANAL_ERROR_PARAMETER;

	//Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	LOCK_MUTEX(m_transmitListMutex);
	//cnt = pDllList->GetNodeCount(&m_transmitList);
	cnt = m_transmitList.nCount;
	UNLOCK_MUTEX(m_transmitListMutex);

	if (cnt >= TouCAN_MAX_FIFO)
	{
	//wprintf(L"FIFO FULL\n");
	return CANAL_ERROR_FIFO_FULL;
    }

	 dllnode *pNode = new dllnode;
	 if (NULL == pNode)
		 return CANAL_ERROR_MEMORY;

	 canalMsg *pcanalMsg = new canalMsg;
	 if (NULL == pcanalMsg)
	 {
		 delete pNode;
		 return CANAL_ERROR_MEMORY;
	 }

	 pNode->pObject = pcanalMsg;
	 pNode->pKey = NULL;
	 pNode->pstrKey = NULL;

	 memcpy_s(pcanalMsg, sizeof(canalMsg), pMsg, sizeof(canalMsg));

	 LOCK_MUTEX(m_transmitListMutex);
	 res = pDllList->AddNode(&m_transmitList, pNode);
	 UNLOCK_MUTEX(m_transmitListMutex);

	 //wprintf(L"ADDED TO LIST\n");

	 if (res != TRUE)
	 {
		 delete pNode;
		 delete pcanalMsg;
		 return CANAL_ERROR_MEMORY;
	 }

	 SetEvent(m_transmitDataGetEvent);

	return CANAL_ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// writeMsg blocking
//

int CTouCANObj::WriteMsgBlocking(canalMsg *pMsg, ULONG Timeout)
{
	DWORD res;
	ULONG cnt;

	// Must be a message pointer
	if (NULL == pMsg)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	// Timeout max 500ms
	if (Timeout > 500)
		     Timeout = 500;

	LOCK_MUTEX(m_transmitListMutex);
	//cnt = pDllList->GetNodeCount(&m_transmitList); //too slow
	cnt = m_transmitList.nCount;
	UNLOCK_MUTEX(m_transmitListMutex);

	//wprintf(L"FIFO CNT=%d\n", cnt);

	if(cnt >= TouCAN_MAX_FIFO)
	{
		ResetEvent(m_transmitDataPutEvent);
		res = WaitForSingleObject(m_transmitDataPutEvent, Timeout); // default: signaled

		if (res == WAIT_TIMEOUT)
		{
			//wprintf(L"WAIT TIMEOUT\n");
			return CANAL_ERROR_TIMEOUT;
		}
		else if (res == WAIT_ABANDONED)
		{
			//wprintf(L"CANAL_ERROR_GENERIC\n");
			return CANAL_ERROR_GENERIC;
		}
	}

	dllnode *pNode = new dllnode;
	if (NULL == pNode)
		return CANAL_ERROR_MEMORY;

	canalMsg *pcanalMsg = new canalMsg;
	if (NULL == pcanalMsg)
	{
		delete pNode;
		return CANAL_ERROR_MEMORY;
	}

	pNode->pObject = pcanalMsg;
	pNode->pKey = NULL;
	pNode->pstrKey = NULL;
	memcpy_s(pcanalMsg, sizeof(canalMsg), pMsg, sizeof(canalMsg));

	// get NodeCount once again
	LOCK_MUTEX(m_transmitListMutex);
	//cnt = pDllList->GetNodeCount(&m_transmitList); //too slow
	cnt = m_transmitList.nCount;
	UNLOCK_MUTEX(m_transmitListMutex);

	if (cnt >= TouCAN_MAX_FIFO)
	{
		delete pNode;
		delete pcanalMsg;
		return  CANAL_ERROR_FIFO_FULL;
	}


	LOCK_MUTEX(m_transmitListMutex);
	res = pDllList->AddNode(&m_transmitList, pNode);
	UNLOCK_MUTEX(m_transmitListMutex);

	//wprintf(L"ADDED TO LIST\n");

	if (res != TRUE)
	{
	 delete pNode;
	 delete pcanalMsg;
	 return CANAL_ERROR_MEMORY;
	}

	SetEvent(m_transmitDataGetEvent);

	return CANAL_ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////
// writeMsg blocking
//

char* CTouCANObj::GetVendorString(void)
{
	CHAR	 Vendor[256];
	UINT32	 HardwareVersion;
	CHAR	 HardwareVersionStr[256];
	UINT32   FirmwareVersion;
	CHAR     FirmwareVersionStr[256];
	static CHAR	 str[256];

	memset(str, 0, 255);

	// Must be open
	if (!m_bOpen)
		return str;

	TouCAN_get_hardware_version(&HardwareVersion);
	//wprintf(L"Hardware version= %08X\n", HardwareVersion);

	TouCAN_get_firmware_version(&FirmwareVersion);
	//wprintf(L"Firmware version= %08X\n", FirmwareVersion);

	TouCAN_get_vendor(256, Vendor);
	//wprintf(L"VENDOR ID= %S\n", Vendor);

	//sprintf_s(HardwareVersionStr, sizeof(str), "%d",(UINT8) HardwareVersion);
	sprintf_s(HardwareVersionStr, sizeof(str), "%d.%d.%d", (UINT8)(HardwareVersion >> 24), (UINT8)(HardwareVersion >> 16), (UINT8)(HardwareVersion >> 8));
	sprintf_s(FirmwareVersionStr, sizeof(str), "%d.%d.%d", (UINT8)(FirmwareVersion >> 24), (UINT8)(FirmwareVersion >> 16), (UINT8)(FirmwareVersion >> 8));

	strcat_s(str, HardwareVersionStr);
	strcat_s(str, ";");
	strcat_s(str, FirmwareVersionStr);
	strcat_s(str, ";");
	strcat_s(str, Vendor);

	return	str;
}

////////////////////////////////////////////////////////////
// Get Bootloder version
//
int CTouCANObj::GetBootloaderVersion(unsigned long *version)
{
	UINT32 ver = 0;

	// Must be a message pointer
	if (NULL == version)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_get_bootloader_version(&ver) != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	*version = ver;
	//wprintf(L"Bootloader version= %08X\n", ver);

	return CANAL_ERROR_SUCCESS;
}


////////////////////////////////////////////////////////////
// Get Hardware version
//
int CTouCANObj::GetHardwareVersion(unsigned long * version)
{
	UINT32 ver = 0;

	// Must be a message pointer
	if (NULL == version)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_get_hardware_version(&ver) != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	*version = ver;

	return CANAL_ERROR_SUCCESS;
}



int CTouCANObj::GetFirmwareVersion(unsigned long * version)
{
	UINT32 ver = 0;

	// Must be a message pointer
	if (NULL == version)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_get_firmware_version(&ver) != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	*version = ver;

	return CANAL_ERROR_SUCCESS;
}


int CTouCANObj::GetSerialNumber(unsigned long * serial)
{
	UINT32 ver = 0;

	// Must be a message pointer
	if (NULL == serial)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_get_serial_number(&ver) != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	*serial = ver;

	return CANAL_ERROR_SUCCESS;
}

int CTouCANObj::GetVidPid(unsigned long * vidpid)
{
	UINT32 ver = 0;

	// Must be a message pointer
	if (NULL == vidpid)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_get_vid_pid(&ver) != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	*vidpid = ver;

	return CANAL_ERROR_SUCCESS;
}

int CTouCANObj::GetDeviceId(unsigned long * devid)
{
	UINT32 ver = 0;

	// Must be a message pointer
	if (NULL == devid)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_get_device_id(&ver) != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	*devid = ver;

	return CANAL_ERROR_SUCCESS;
}

int CTouCANObj::GetVendor(unsigned int size, char * vendor)
{
	CHAR	 local_vendor[256];

	// Must be a message pointer
	if (NULL == vendor)
		return CANAL_ERROR_PARAMETER;

	// Must be a message pointer
	if (size == 0)
		return CANAL_ERROR_PARAMETER;

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_get_vendor(size, local_vendor) != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	strcpy_s(vendor, size, local_vendor);

	return CANAL_ERROR_SUCCESS;
}

//int InterfaceStart(void);

int CTouCANObj::InterfaceStart(void)
{

	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_start() != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

	return CANAL_ERROR_SUCCESS;
}


int CTouCANObj::InterfaceStop(void)
{
	// Must be open
	if (!m_bOpen)
		return CANAL_ERROR_NOT_OPEN;

	if (!m_bRun)
		return CANAL_ERROR_HARDWARE;

	if (TouCAN_stop() != TRUE)
		return	CANAL_ERROR_COMMUNICATION;

    // signal blocking reception event
    if (m_receiveDataEvent != NULL)
        (void)SetEvent(m_receiveDataEvent);
    
    return CANAL_ERROR_SUCCESS;
}

