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

#include    <windows.h>
#include    "CTouCANobj.h"

#define DLL_MAIN_VERSION					1
#define DLL_MINOR_VERSION					0
#define DLL_SUB_VERSION				        5

// This is the vendor string - Change to your own value
#define CANAL_DLL_VENDOR "Rusoku Technologijos UAB, Lithuania, http://www.rusoku.com"


// Max number of open connections
#define CANAL_TouCAN_DRIVER_MAX_OPEN	8

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

	HANDLE m_objMutex;

	/// Counter for users of the interface
	unsigned long m_instanceCounter;

public:
	BOOL InitInstance();

};


