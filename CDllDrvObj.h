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

#pragma once
#include "CTouCANobj.h"

#define DLL_MAIN_VERSION					1
#define DLL_MINOR_VERSION					0
#define DLL_SUB_VERSION				        1

// This is the vendor string - Change to your own value
#define CANAL_DLL_VENDOR "Rusoku Technologijos UAB, Lithuania, http://www.rusoku.com"


// Max number of open connections
#define CANAL_TouCAN_DRIVER_MAX_OPEN	128

/////////////////////////////////////////////////////////////////////////////
// CDllDrvObj
// See loggerdll.cpp for the implementation of this class
//

class CDllDrvObj
{

public:

	/// Constructor
	CDllDrvObj();

	/// Destructor
	~CDllDrvObj();

	/*!
	Add a driver object

	@parm plog Object to add
	@return handle or 0 for error
	*/
	long AddDriverObject(CTouCANObj *pObj);

	/*!
	Get a driver object from its handle

	@param handle for object
	@return pointer to object or NULL if invalid
	handle.
	*/
	CTouCANObj *GetDriverObject(long h);

	/*!
	Remove a driver object

	@parm handle for object.
	*/
	void RemoveDriverObject(long h);


	/*
	GS
	*/
	BOOL RemoveAllObjects();

	/*!
	The log file object
	This is the array with driver objects (max 256 objects
	*/
	CTouCANObj *m_drvObjArray[CANAL_TouCAN_DRIVER_MAX_OPEN];

	/// Mutex for open/close
#ifdef WIN32	
	HANDLE m_objMutex;
#else
	pthread_mutex_t m_objMutex;
#endif

	/// Counter for users of the interface
	unsigned long m_instanceCounter;

public:
	BOOL InitInstance();

};


