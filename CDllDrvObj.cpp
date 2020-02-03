/*
 * CANAL interface DLL for RUSOKU technologies for TouCAN, TouCAN Marine, TouCAN Duo USB to CAN bus converter
 *
 * Copyright (C) 2000-2008 Ake Hedman, eurosource, <akhe@eurosource.se>
 * Copyright (C) 2020 Gediminas Simanskis (gediminas@rusoku.com)
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
#include "CDllDrvObj.h"


#ifdef WIN32

#else

void _init() __attribute__((constructor));
void _fini() __attribute__((destructor));

void _init() { printf("initializing\n"); }
void _fini() { printf("finishing\n"); }

#endif

/////////////////////////////////////////////////////////////////////////////
// CDllDrvObj

////////////////////////////////////////////////////////////////////////////
// CDllDrvObj construction

CDllDrvObj::CDllDrvObj()
{
	m_instanceCounter = 0;
#ifdef WIN32
	m_objMutex = CreateMutex(NULL, true, NULL /*L"__CANAL_IXXATVCI_MUTEX__"*/);
#else
	pthread_mutex_init(&m_objMutex, NULL);
#endif

	// Init the driver array
	for (int i = 0; i<CANAL_TouCAN_DRIVER_MAX_OPEN; i++) {
		m_drvObjArray[i] = NULL;
	}

	UNLOCK_MUTEX(m_objMutex);
}


CDllDrvObj::~CDllDrvObj()
{
	LOCK_MUTEX(m_objMutex);

	for (int i = 0; i<CANAL_TouCAN_DRIVER_MAX_OPEN; i++)
	{

		if (NULL == m_drvObjArray[i])
		{

			CTouCANObj *pdrvObj = GetDriverObject(i);

			if (NULL != pdrvObj)
			{
				pdrvObj->Close();
				delete m_drvObjArray[i];
				m_drvObjArray[i] = NULL;
			}
		}
	}

	UNLOCK_MUTEX(m_objMutex);

#ifdef WIN32
	CloseHandle(m_objMutex);
#else	
	pthread_mutex_destroy(&m_objMutex);
#endif
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
		if (m_drvObjArray[i] == NULL)
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
	if (idx < 0) return NULL;
	if (idx >= CANAL_TouCAN_DRIVER_MAX_OPEN) return NULL;
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

	if (NULL != m_drvObjArray[idx])
		delete m_drvObjArray[idx];

	m_drvObjArray[idx] = NULL;

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

		if (NULL == m_drvObjArray[i])
		{

			CTouCANObj *pdrvObj = GetDriverObject(i);

			if (NULL != pdrvObj)
			{
				pdrvObj->Close();
				delete m_drvObjArray[i];
				m_drvObjArray[i] = NULL;
			}
		}

	}

	UNLOCK_MUTEX(m_objMutex);

	return TRUE;
}

