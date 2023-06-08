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
#include "include/CDllDrvObj.h"
#include "include/canal_macro.h"

/////////////////////////////////////////////////////////////////////////////
// CDllDrvObj

////////////////////////////////////////////////////////////////////////////
// CDllDrvObj construction

CDllDrvObj::CDllDrvObj()
{
	m_instanceCounter = 0;

	m_objMutex = CreateMutex(nullptr, true, nullptr);

	// Init the driver array
	for (int i = 0; i<CANAL_TouCAN_DRIVER_MAX_OPEN; i++)
    {
		m_drvObjArray[i] = nullptr;
	}

	UNLOCK_MUTEX(m_objMutex);
}


CDllDrvObj::~CDllDrvObj()
{
	LOCK_MUTEX(m_objMutex);

	for (int i = 0; i<CANAL_TouCAN_DRIVER_MAX_OPEN; i++)
	{

		if (nullptr == m_drvObjArray[i])
		{

			CTouCANObj *pdrvObj = GetDriverObject(i);

			if (nullptr != pdrvObj)
			{
				pdrvObj->Close();
				delete m_drvObjArray[i];
				m_drvObjArray[i] = nullptr;
			}
		}
	}

	UNLOCK_MUTEX(m_objMutex);
	CloseHandle(m_objMutex);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllDrvObjgerdllApp object

//CDllDrvObj theApp;



///////////////////////////////////////////////////////////////////////////////
// addDriverObject
//

long CDllDrvObj::AddDriverObject(CTouCANObj *pdrvObj)
{
	long h = 0;

	LOCK_MUTEX(m_objMutex);

	for (int i = 0; i<CANAL_TouCAN_DRIVER_MAX_OPEN; i++)
	{
		if (m_drvObjArray[i] == nullptr)
		{
			m_drvObjArray[i] = pdrvObj;
			h = i + 1681;
			break;
		}
	}

	UNLOCK_MUTEX(m_objMutex);
	return h;
}


///////////////////////////////////////////////////////////////////////////////
// getDriverObject
//

CTouCANObj * CDllDrvObj::GetDriverObject(long h)
{
	long idx = h - 1681;

	// Check if valid handle
	if (idx < 0) return nullptr;
	if (idx >= CANAL_TouCAN_DRIVER_MAX_OPEN) return nullptr;
	return m_drvObjArray[idx];
}


///////////////////////////////////////////////////////////////////////////////
// removeDriverObject
//

void CDllDrvObj::RemoveDriverObject(long h)
{
	long idx = h - 1681;

	// Check if valid handle
	if (idx < 0) return;
	if (idx >= CANAL_TouCAN_DRIVER_MAX_OPEN) return;

	LOCK_MUTEX(m_objMutex);

	if (nullptr != m_drvObjArray[idx])
		delete m_drvObjArray[idx];

	m_drvObjArray[idx] = nullptr;

	UNLOCK_MUTEX(m_objMutex);
}

///////////////////////////////////////////////////////////////////////////////
// InitInstance

BOOL CDllDrvObj::InitInstance()
{
	m_instanceCounter++;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//    removeAllObjects
//

BOOL CDllDrvObj::RemoveAllObjects()
{
	long h = 0;

	LOCK_MUTEX(m_objMutex);

	for (int i = 0; i<CANAL_TouCAN_DRIVER_MAX_OPEN; i++)
	{

		if (nullptr == m_drvObjArray[i])
		{

			CTouCANObj *pdrvObj = GetDriverObject(i);

			if (nullptr != pdrvObj)
			{
				pdrvObj->Close();
				delete m_drvObjArray[i];
				m_drvObjArray[i] = nullptr;
			}
		}

	}

	UNLOCK_MUTEX(m_objMutex);
	return TRUE;
}

