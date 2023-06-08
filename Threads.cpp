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

#include "windows.h"
#include "include/canal.h"
#include "include/canal_macro.h"
#include "include/CTouCANobj.h"
#include "include/debug.h"

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

	auto pobj = (CTouCANObj *)pObject;

	timeout = 500;
	WinUsb_SetPipePolicy(pobj->deviceData.WinusbHandle, 0x01, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);
	WinUsb_SetPipePolicy(pobj->deviceData.WinusbHandle, 0x01, RAW_IO, 0, nullptr);

	while (pobj->m_bRunTxTask)
	{
		LOCK_MUTEX(pobj->m_transmitListMutex);
		//cnt = pobj->pDllList->GetNodeCount(&pobj->m_transmitList);
		cnt = pobj->m_transmitList.nCount;
		UNLOCK_MUTEX(pobj->m_transmitListMutex);

		if (cnt == 0)
		{
			if (WaitForSingleObject(pobj->m_transmitDataGetEvent, 500) != WAIT_OBJECT_0) // default: NOT signaled
			{
				continue;
			}
		}

		// get NodeCount once again
		LOCK_MUTEX(pobj->m_transmitListMutex);
		//cnt = pobj->pDllList->GetNodeCount(&pobj->m_transmitList);
		cnt = pobj->m_transmitList.nCount;
		UNLOCK_MUTEX(pobj->m_transmitListMutex);

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

			ResetEvent(pobj->m_transmitDataGetEvent);
			SetEvent(pobj->m_transmitDataPutEvent);
		}

		do {
			if (WinUsb_WritePipe(pobj->deviceData.WinusbHandle, 0x01, &TxDataBuf[0], index, &Transfered, nullptr) == TRUE)
			{
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
			}
		} while (pobj->m_bRunTxTask);		
	}

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
	canalMsg *pMsg = nullptr;
	dllnode  *pNode = nullptr;
	DWORD   dw;
	UINT32  ErrorCounter = 0;

	auto pobj = (CTouCANObj *)pObject;

	timeout = 500;
	WinUsb_SetPipePolicy(pobj->deviceData.WinusbHandle, 0x81, PIPE_TRANSFER_TIMEOUT, sizeof(ULONG), &timeout);
	WinUsb_SetPipePolicy(pobj->deviceData.WinusbHandle, 0x81, RAW_IO, 0, nullptr);

	while (pobj->m_bRunRxTask)
	{
		if (WinUsb_ReadPipe(pobj->deviceData.WinusbHandle, 0x81, &RxDataBuf[0], 64, &Transfered, nullptr) != TRUE)
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
			pNode = new dllnode;

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

	ExitThread(errorCode);
}

