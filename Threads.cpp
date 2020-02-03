/*
 * CANAL interface DLL for RUSOKU technologies for TouCAN, TouCAN Marine, TouCAN Duo USB to CAN bus converter
 *
 * Copyright (C) 2000-2008 Ake Hedman, eurosource, <akhe@eurosource.se>
 * Copyright (C) 2020 Gediminas Simanskis (gediminas@rusoku.com), www.rusoku.com
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

void workThreadTransmit(void *pObject);
void workThreadReceive(void *pObject);

///////////////////////////////////////////////////////////////////////////////
// workThreadTransmit
//
//

void workThreadTransmit(void *pObject)
{
	DWORD   errorCode = 0;
	ULONG	cnt;
	ULONG	timeout;
	canalMsg	msg;
	UINT8	TxDataBuf[100];
	ULONG	Transfered = 0;
	UINT8	index = 0;
	DWORD   dw;
	UINT32  ErrorCounter = 0;

	CTouCANObj * pobj = (CTouCANObj *)pObject;

	//wprintf(L"TX_Thread_Begin\n");

	timeout = 500;
	WinUsb_SetPipePolicy(pobj->deviceData.WinusbHandle, 0x01, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);
	WinUsb_SetPipePolicy(pobj->deviceData.WinusbHandle, 0x01, RAW_IO, 0, 0);

	while (pobj->m_bRunTxTask)
	{
		LOCK_MUTEX(pobj->m_transmitListMutex);
		//cnt = pobj->pDllList->GetNodeCount(&pobj->m_transmitList);
		cnt = pobj->m_transmitList.nCount;
		UNLOCK_MUTEX(pobj->m_transmitListMutex);

		//wprintf(L"TxThread=cnt%d\n", cnt);

		if (cnt == 0)
		{
			if (WaitForSingleObject(pobj->m_transmitDataGetEvent, 500) != WAIT_OBJECT_0) // default: NOT signaled
			{  
				//wprintf(L"TxThread: WaitForSingleObject\n");
				continue;
			}
		}

		// get NodeCount once again
		LOCK_MUTEX(pobj->m_transmitListMutex);
		//cnt = pobj->pDllList->GetNodeCount(&pobj->m_transmitList);
		cnt = pobj->m_transmitList.nCount;
		UNLOCK_MUTEX(pobj->m_transmitListMutex);

		//wprintf(L"TxThread=cnt%d\n", cnt);

		if (cnt == 0)
			continue;

		// max canmsg 3 frames in buffer		
		if (cnt > 3)
			cnt = 3;

		index = 0;

		for (UINT8 x = 0; x < cnt; x++)
		{
			// copy message from RxList to local variable
			memcpy_s(&msg, sizeof(canalMsg), pobj->m_transmitList.pHead->pObject, sizeof(canalMsg));

			TxDataBuf[index++] = (UINT8)(msg.flags & 3);

			TxDataBuf[index++] = (UINT8)(msg.id >> 24);
			TxDataBuf[index++] = (UINT8)(msg.id >> 16);
			TxDataBuf[index++] = (UINT8)(msg.id >> 8);
			TxDataBuf[index++] = (UINT8)msg.id;

			TxDataBuf[index++] = msg.sizeData;

			for (UINT y = 0; y < 8; y++)
			{
				TxDataBuf[index++] = msg.data[y];
			}

			TxDataBuf[index++] = (UINT8)(msg.timestamp >> 24);
			TxDataBuf[index++] = (UINT8)(msg.timestamp >> 16);
			TxDataBuf[index++] = (UINT8)(msg.timestamp >> 8);
			TxDataBuf[index++] = (UINT8)msg.timestamp;

			LOCK_MUTEX(pobj->m_transmitListMutex);
			pobj->pDllList->RemoveNode(&pobj->m_transmitList, pobj->m_transmitList.pHead);
			UNLOCK_MUTEX(pobj->m_transmitListMutex);
			//wprintf(L"TransmitCounter=%I64d\n", ++pobj->TransmitCounter);
			ResetEvent(pobj->m_transmitDataGetEvent);
			SetEvent(pobj->m_transmitDataPutEvent);
		}

		do {
			//wprintf(L"WinUsb_WritePipe=");
			if (WinUsb_WritePipe(pobj->deviceData.WinusbHandle, 0x01, &TxDataBuf[0], index, &Transfered, NULL) == TRUE)
			{
				//wprintf(L"OK\n");
				break;
			}
			else
			{
				dw = GetLastError();
				if (dw == ERROR_BAD_COMMAND)
				{
					ErrorCounter++;
					if (ErrorCounter == 100)
					{
						pobj->m_bRunTxTask = FALSE;
						//pobj->m_bOpen = FALSE;
						pobj->m_bRun = FALSE;
					}
				}
				if (dw == ERROR_SEM_TIMEOUT)
			 	{
					//wprintf(L"TIMEOUT\n");
				}
			}
		} while (pobj->m_bRunTxTask);		
	}

	//wprintf(L"TX_Thread_End\n");

	ExitThread(errorCode);
}

///////////////////////////////////////////////////////////////////////////////
// workThreadReceive
//
//

void workThreadReceive(void *pObject)
{
	DWORD   errorCode = 0;
	ULONG	timeout;
	UINT8	RxDataBuf[100];
	ULONG	Transfered;
	UINT8	FrameCounter = 0;
	UINT8	index = 0;
	canalMsg *pMsg = NULL;
	dllnode  *pNode = NULL;
	DWORD   dw;
	//HRESULT hr;
	UINT32  ErrorCounter = 0;

//	DWORD    dwWaitResult;

	//ULONGLONG	counter = 0;

	CTouCANObj * pobj = (CTouCANObj *)pObject;

	//wprintf(L"RX_Thread_Begin\n");

	timeout = 500;
	WinUsb_SetPipePolicy(pobj->deviceData.WinusbHandle, 0x81, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);
	WinUsb_SetPipePolicy(pobj->deviceData.WinusbHandle, 0x81, RAW_IO, 0, 0);

	while (pobj->m_bRunRxTask)
	{
		//wprintf(L"READ DATA from PIPE 0x81: ");
		if (WinUsb_ReadPipe(pobj->deviceData.WinusbHandle, 0x81, &RxDataBuf[0], 64, &Transfered, NULL) != TRUE)
		{
			dw = GetLastError();

			if (dw == ERROR_BAD_COMMAND)
			{
				ErrorCounter++;
				if (ErrorCounter == 100)
				{
				 pobj->m_bRunRxTask = FALSE;
				 pobj->m_bRun = FALSE;
				}
			}
			//if (dw == ERROR_SEM_TIMEOUT)
			//{
			//}

			continue;
		}

		// Check FrameCounter number from total USB length
		switch (Transfered)
		{
		case	18:
			FrameCounter = 1;
			break;
		case	36:
			FrameCounter = 2;
			break;
		case	54:
			FrameCounter = 3;
			break;
		default:
			FrameCounter = 0;
			break;
		}

		if (FrameCounter == 0)
			continue;		

		index = 0;

		for (UINT8 x=0; x<FrameCounter; x++)
		{					
			pMsg = new canalMsg;
			if (pMsg == NULL)
			{
			  break;
			}

			pNode = new dllnode;
			if (pNode == NULL)
			{
				//wprintf(L"dllnode create ERROR!\n");
				break;
			}

			 pMsg->flags = (((UINT32)RxDataBuf[index++]) & 0x000000ff);

			 pMsg->id  = (((UINT32)RxDataBuf[index++] << 24) & 0x1f000000);
			 pMsg->id |= (((UINT32)RxDataBuf[index++] << 16) & 0x00ff0000);
			 pMsg->id |= (((UINT32)RxDataBuf[index++] << 8) & 0x0000ff00);
			 pMsg->id |= (((UINT32)RxDataBuf[index++]) & 0x000000ff);

			 pMsg->sizeData = RxDataBuf[index++];
			 memcpy_s((UCHAR*)pMsg->data,8 ,(&RxDataBuf[index]), 8);
			 index += 8;

			 pMsg->timestamp  = (((UINT32)RxDataBuf[index++] << 24) & 0xff000000);
			 pMsg->timestamp |= (((UINT32)RxDataBuf[index++] << 16) & 0x00ff0000);
			 pMsg->timestamp |= (((UINT32)RxDataBuf[index++] << 8) & 0x0000ff00);
			 pMsg->timestamp |= (((UINT32)RxDataBuf[index++])  & 0x000000ff);

				 pNode->pObject = pMsg;

				 if (pobj->m_receiveList.nCount >= TouCAN_MAX_FIFO)
				 {
					 delete  pMsg;
					 delete  pNode;					 
					 break;
				 }

				 LOCK_MUTEX(pobj->m_receiveListMutex);
				 if( !pobj->pDllList->AddNode(&pobj->m_receiveList, pNode))
				 { 
					 delete  pMsg;
					 delete  pNode;
				 }

				 UNLOCK_MUTEX(pobj->m_receiveListMutex);
				 SetEvent(pobj->m_receiveDataEvent); // Signal frame in queue
		}
	}

	//wprintf(L"RX_Thread_End\n");

	ExitThread(errorCode);
}

