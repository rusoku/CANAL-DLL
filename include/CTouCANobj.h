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

#include "windows.h"
#include <winusb.h>
#include <cstdio>
#include "CDllList.h"
#include "canal.h"

#define    TouCAN_MAX_FIFO	   500000

/////////////////////////////////////////////////////
// USB request types & mask defines

#define		USB_HOST_TO_DEVICE							0x00
#define		USB_DEVICE_TO_HOST							0x80	

#define		USB_REQ_TYPE_STANDARD                       0x00
#define		USB_REQ_TYPE_CLASS                          0x20
#define		USB_REQ_TYPE_VENDOR                         0x40
#define		USB_REQ_TYPE_MASK                           0x60

#define		USB_REQ_RECIPIENT_DEVICE                    0x00
#define		USB_REQ_RECIPIENT_INTERFACE                 0x01
#define		USB_REQ_RECIPIENT_ENDPOINT                  0x02
#define		USB_REQ_RECIPIENT_MASK                      0x03

/////////////////////////////////////////////////////
// TouCAN requests types (Command)

#define		TouCAN_RESET								0x00 // OK
#define		TouCAN_CAN_INTERFACE_INIT					0x01 // OK
#define		TouCAN_CAN_INTERFACE_DEINIT    				0x02 // OK
#define		TouCAN_CAN_INTERFACE_START					0x03 // OK
#define		TouCAN_CAN_INTERFACE_STOP					0x04 // OK

#define		TouCAN_FILTER_STD_ACCEPT_ALL				0x05
#define		TouCAN_FILTER_STD_REJECT_ALL				0x06

#define		TouCAN_FILTER_EXT_ACCEPT_ALL				0x07
#define		TouCAN_FILTER_EXT_REJECT_ALL			    0x08

#define		TouCAN_SET_FILTER_STD_LIST_MASK   			0x09
#define		TouCAN_SET_FILTER_EXT_LIST_MASK				0x0A
#define		TouCAN_GET_FILTER_STD_LIST_MASK   			0x0B
#define		TouCAN_GET_FILTER_EXT_LIST_MASK				0x0C

#define		TouCAN_GET_CAN_ERROR_STATUS          		0x0D  // OK   - CAN_Error_Status(&hcan1)
#define		TouCAN_CLEAR_CAN_ERROR_STATUS		        0x0D  // NOK  - CAN_Error_Status(&hcan1) // nenusistato
#define		TouCAN_GET_STATISTICS						0x0E  // OK (VSCP CAN state)
#define     TouCAN_CLEAR_STATISTICS						0x0F  // OK (VSCP CAN state)
#define		TouCAN_GET_HARDWARE_VERSION					0x10  // OK
#define		TouCAN_GET_FIRMWARE_VERSION					0x11  // OK
#define		TouCAN_GET_BOOTLOADER_VERSION				0x12  // OK
#define		TouCAN_GET_SERIAL_NUMBER					0x13  // OK
//#define		TouCAN_SET_SERIAL_NUMBER					0x14  
//#define		TouCAN_RESET_SERIAL_NUMBER					0x15  
#define		TouCAN_GET_VID_PID							0x16  // OK
#define		TouCAN_GET_DEVICE_ID						0x17  // OK
#define		TouCAN_GET_VENDOR  							0x18  // OK

#define		TouCAN_GET_LAST_ERROR_CODE					0x20  // HAL return error code	8bit // OK
#define		TouCAN_CLEAR_LAST_ERROR_CODE				0x21  // HAL return error code  8bit // OK
#define		TouCAN_GET_CAN_INTERFACE_STATE 				0x22  // HAL_CAN_GetState(&hcan1)		8bit   // OK
#define		TouCAN_CLEAR_CAN_INTERFACE_STATE 			0x23  // HAL_CAN_GetState(&hcan1); ----------------------------- NOK
#define		TouCAN_GET_CAN_INTERFACE_ERROR_CODE			0x24  // hcan->ErrorCode;	32bit    // OK  HAL_CAN_GetError(&hcan1);
#define		TouCAN_CLEAR_CAN_INTERFACE_ERROR_CODE	    0x25  // hcan->ErrorCode;	32bit    // OK HAL_CAN_GetError(&hcan1);

#define     TouCAN_SET_CAN_INTERFACE_DELAY              0x26  // OK
#define     TouCAN_GET_CAN_INTERFACE_DELAY              0x27  // OK

///////////////////////////////////////////////////////
// TouCAN  return error codes (HAL)   

#define     TouCAN_RETVAL_OK		                    0x00
#define     TouCAN_RETVAL_ERROR                         0x01
#define     TouCAN_RETVAL_BUSY				            0x02   
#define     TouCAN_RETVAL_TIMEOUT                       0x03

//////////////////////////////////////////////////////
//  TouCAN init string FLAGS (32 bit)

#define		TouCAN_ENABLE_SILENT_MODE					0x00000001 //	 1
#define		TouCAN_ENABLE_LOOPBACK_MODE					0x00000002 //	 2
#define		TouCAN_DISABLE_RETRANSMITION				0x00000004 //	 4
#define		TouCAN_ENABLE_AUTOMATIC_WAKEUP_MODE			0x00000008 //	 8
#define		TouCAN_ENABLE_AUTOMATIC_BUS_OFF				0x00000010 //	16
#define		TouCAN_ENABLE_TTM_MODE						0x00000020 //	32
#define		TouCAN_ENABLE_RX_FIFO_LOCKED_MODE			0x00000040 //	64
#define		TouCAN_ENABLE_TX_FIFO_PRIORITY		 		0x00000080 //  128

#define     TouCAN_ENABLE_STATUS_MESSAGES               0x00000100 //  256
#define     TouCAN_ENABLE_TIMESTAMP_DELAY               0x00000200 //  512

/////////////////////////////////////////////////////
// TouCAN HAL return error codes 

typedef enum
{
	HAL_OK =	  0x00U,
	HAL_ERROR =   0x01U,
	HAL_BUSY =	  0x02U,
	HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;


/////////////////////////////////////////////////////
// TouCAN CAN interface state 

typedef enum
{
	HAL_CAN_STATE_RESET =			0x00U,  /*!< CAN not yet initialized or disabled */
	HAL_CAN_STATE_READY =			0x01U,  /*!< CAN initialized and ready for use   */
	HAL_CAN_STATE_LISTENING =		0x02U,  /*!< CAN receive process is ongoing      */
	HAL_CAN_STATE_SLEEP_PENDING =	0x03U,  /*!< CAN sleep request is pending        */
	HAL_CAN_STATE_SLEEP_ACTIVE =	0x04U,  /*!< CAN sleep mode is active            */
	HAL_CAN_STATE_ERROR =		    0x05U   /*!< CAN error state                     */

} HAL_CAN_StateTypeDef;


////////////////////////////////////////////////////
//  TouCAN CAN interface ERROR codes

#define HAL_CAN_ERROR_NONE            (0x00000000U)  /*!< No error                                             */
#define HAL_CAN_ERROR_EWG             (0x00000001U)  /*!< Protocol Error Warning                               */
#define HAL_CAN_ERROR_EPV             (0x00000002U)  /*!< Error Passive                                        */
#define HAL_CAN_ERROR_BOF             (0x00000004U)  /*!< Bus-off error                                        */
#define HAL_CAN_ERROR_STF             (0x00000008U)  /*!< Stuff error                                          */
#define HAL_CAN_ERROR_FOR             (0x00000010U)  /*!< Form error                                           */
#define HAL_CAN_ERROR_ACK             (0x00000020U)  /*!< Acknowledgment error                                 */
#define HAL_CAN_ERROR_BR              (0x00000040U)  /*!< Bit recessive error                                  */
#define HAL_CAN_ERROR_BD              (0x00000080U)  /*!< Bit dominant error                                   */
#define HAL_CAN_ERROR_CRC             (0x00000100U)  /*!< CRC error                                            */
#define HAL_CAN_ERROR_RX_FOV0         (0x00000200U)  /*!< Rx FIFO0 overrun error                               */
#define HAL_CAN_ERROR_RX_FOV1         (0x00000400U)  /*!< Rx FIFO1 overrun error                               */
#define HAL_CAN_ERROR_TX_ALST0        (0x00000800U)  /*!< TxMailbox 0 transmit failure due to arbitration lost */
#define HAL_CAN_ERROR_TX_TERR0        (0x00001000U)  /*!< TxMailbox 1 transmit failure due to tranmit error    */
#define HAL_CAN_ERROR_TX_ALST1        (0x00002000U)  /*!< TxMailbox 0 transmit failure due to arbitration lost */
#define HAL_CAN_ERROR_TX_TERR1        (0x00004000U)  /*!< TxMailbox 1 transmit failure due to tranmit error    */
#define HAL_CAN_ERROR_TX_ALST2        (0x00008000U)  /*!< TxMailbox 0 transmit failure due to arbitration lost */
#define HAL_CAN_ERROR_TX_TERR2        (0x00010000U)  /*!< TxMailbox 1 transmit failure due to tranmit error    */
#define HAL_CAN_ERROR_TIMEOUT         (0x00020000U)  /*!< Timeout error                                        */
#define HAL_CAN_ERROR_NOT_INITIALIZED (0x00040000U)  /*!< Peripheral not initialized                           */
#define HAL_CAN_ERROR_NOT_READY       (0x00080000U)  /*!< Peripheral not ready                                 */
#define HAL_CAN_ERROR_NOT_STARTED     (0x00100000U)  /*!< Peripheral not started                               */
#define HAL_CAN_ERROR_PARAM           (0x00200000U)  /*!< Parameter error                                      */

/////////////////////////////////////////////////////
// WinUSB device

//
// Device Interface GUID.
// Used by all WinUsb devices that this application talks to.
// Must match "DeviceInterfaceGUIDs" registry value specified in the INF file.
//

//DEFINE_GUID(GUID_DEVINTERFACE_WinUsbF4FS1,
//	0xFD361109, 0x858D, 0x4F6F, 0x81, 0xEE, 0xAA, 0xB5, 0xD6, 0xCB, 0xF0, 0x6B);


typedef struct _DEVICE_DATA {

	BOOL                    HandlesOpen;
	WINUSB_INTERFACE_HANDLE WinusbHandle;
	HANDLE                  DeviceHandle;
	TCHAR                   DevicePath[MAX_PATH];
	TCHAR				    FoundSerialNumber[MAX_PATH];
} DEVICE_DATA, *PDEVICE_DATA;

typedef struct {
	UINT8	direction;
	UINT8	channel;
	UINT8	command;
	UINT8	opt0;
	UINT8	opt1;
	UINT8	data[64];
}CommandMsg_Typedef;

class CTouCANObj
{

private:

	//////////////////////////////////////////
	//    TouCAN hardware

	bool	TouCAN_init (void);
	bool	TouCAN_deinit (void);
	bool	TouCAN_start (void);
	bool	TouCAN_stop (void);

	bool	TouCAN_get_last_error_code(UINT8 *res);				// HAL error code
	bool	TouCAN_get_interface_error_code(UINT32	*ErrorCode);	// hcan->ErrorCode;
	bool	TouCAN_clear_interface_error_code(void);				// hcan->ErrorCode;
	bool	TouCAN_get_interface_state(UINT8 *state);				// hcan->State;
	bool	TouCAN_get_statistics(PCANALSTATISTICS statistics);    // VSCP get statistics
	bool	TouCAN_clear_statistics(void);							// VSCP clear statistics	
	bool	TouCAN_get_canal_status(canalStatus *status);

	bool	TouCAN_get_hardware_version(UINT32 *ver);
	bool	TouCAN_get_firmware_version(UINT32 *ver);
	bool	TouCAN_get_bootloader_version(UINT32 *ver);
	bool	TouCAN_get_serial_number(UINT32 *ver);
	bool	TouCAN_get_vid_pid(UINT32 *ver);
	bool	TouCAN_get_device_id(UINT32 *ver);
	bool	TouCAN_get_vendor(unsigned int size, CHAR *str);
	bool    TouCAN_get_interface_transmit_delay(UINT8 channel,UINT32 *delay);
	bool    TouCAN_set_interface_transmit_delay(UINT8 channel,UINT32 *delay);

	bool	TouCAN_set_filter_std_list_mask(Filter_Type_TypeDef type, UINT32 list, UINT32 mask);
	bool	TouCAN_set_filter_ext_list_mask(Filter_Type_TypeDef type, UINT32 list, UINT32 mask);



	//TouCAN_SET_FILTER_STD_LIST

//	bool	TouCAN_reset (void);

	// Run flag
	//bool m_bRun;

	// Open flag
	//bool m_bOpen;

	// Threads handles
	HANDLE m_hTreadReceive;
	HANDLE m_hTreadTransmit;

	// TouCAN configuraton string parameters
	/////////////////////////////////////////////

	TCHAR		m_SerialNumber[12];
	UINT32		m_DeviceId;
	UINT16		m_Speed;
	UINT8		m_Tseg1;
	UINT8		m_Tseg2;
	UINT8		m_Sjw;
	UINT16		m_Brp;
	UINT32		m_OptionFlag;

	// Statistics
	canalStatistics		m_statistics;    // VSCP statistics

	// Filter, Mask
	UINT32		m_filter;
	UINT32		m_mask;

////////////////////////////////////////////
// WinUSB

	//DEVICE_DATA           deviceData;
	USB_DEVICE_DESCRIPTOR deviceDesc;
	BOOL                  noDevice;

#ifdef DEBUG_CANAL
	FILE* fp;
	FILE* log;
#endif // DEBUG_CANAL

/*------------------------------------------------------------*/
	HRESULT
	RetrieveDevicePath(
				_Out_bytecap_(BufLen) LPTSTR,
				_In_                  ULONG,
				_Out_opt_             PBOOL,
				_In_				  LPTSTR
		);
/*------------------------------------------------------------*/
	HRESULT
		OpenDevice(
			_Out_     PDEVICE_DATA,
			_Out_opt_ PBOOL,
			_In_      LPTSTR
		);
/*------------------------------------------------------------*/
	VOID
		CloseDevice(
			_Inout_ PDEVICE_DATA
		);


public:

	// Run tasks
	bool m_bRunRxTask;
	bool m_bRunTxTask;
	// Open flag
	bool m_bOpen;
	// Hardware error
	bool m_bRun;

	//WinUsb handle
	DEVICE_DATA deviceData;

	// Lists
	CDllList  *pDllList;
	DoubleLinkedList m_receiveList;
	DoubleLinkedList m_transmitList;

	/*!
		Mutex for receive queue.
     */	
	HANDLE  m_receiveListMutex;

	/*!
		Mutex for transmit queue.
	*/	
	HANDLE  m_transmitListMutex;

	// Events
	HANDLE	m_receiveDataEvent;
	HANDLE	m_transmitDataPutEvent;
	HANDLE	m_transmitDataGetEvent;

	// DEBUG
	ULONGLONG	TransmitCounter;
	ULONGLONG	ReceiveCounter;


	/*!
	Open device
	*/
	bool Open (const char *, unsigned long, bool start);

	/*!
	Close device
	*/
	bool Close ();

	/*!
	Start interface
	*/
	bool Start ();

	/*!
	Stop interface
	*/
	bool Stop ();

	/*!
	Write a message out to the file
	@param pcanalMsg Pointer to CAN message
	@return True on success.
	*/
	int WriteMsg (canalMsg*);

	/*
	Send blocking
	*/
	int WriteMsgBlocking (canalMsg *, ULONG);

	/*
	Non blocking
	*/
	int ReadMsg (canalMsg *);

	/*!
	Read a message fro the device
	@param pcanalMsg Pointer to CAN message
	@return True on success.
	*/
	int ReadMsgBlocking (canalMsg * ,ULONG);

	/*!
	Get Interface statistics
	@param pCanalStatistics Pointer to CANAL statistics structure
	@return True on success.
	*/
	int GetStatistics (PCANALSTATISTICS);

	/*!
	Check for data availability
	@return Number of packages in the queue
	*/
	int DataAvailable();

	/*!
	Get device status
	@param pCanalStatus Pointer to CANAL status structure
	@return True on success.
	*/
	int GetStatus (PCANALSTATUS);

	/*!
	Get device status
	@param pCanalStatus Pointer to CANAL status structure
	@return True on success.
	*/
	char* GetVendorString();


	/*!
	Set Filter
	*/
	bool SetFilter(unsigned long);


	/*!
	Set Mask
	*/
	bool SetMask(unsigned long);

	// FILTER 11bit
	int SetFilter11bit( Filter_Type_TypeDef type, unsigned long list, unsigned long mask);

	// FILTER 29bit
	int SetFilter29bit( Filter_Type_TypeDef type, unsigned long list, unsigned long mask);

	// Get Bootloader version
	int GetBootloaderVersion(unsigned long *version);

	// Get hardware version
	int GetHardwareVersion(unsigned long *version);

	// Get firmware version
	int GetFirmwareVersion(unsigned long *version);

	// Get serial number version
	int GetSerialNumber(unsigned long *serial);

	// Get vid pid version
	int GetVidPid(unsigned long *vidpid);

	// Get vid pid version
	int GetDeviceId(unsigned long *devid);

	// Get vid pid version
	int GetVendor(unsigned int size, char *vendor);

	// Interface Start
	int InterfaceStart();

	// Interface Stop
	int InterfaceStop();

	CTouCANObj();
	~CTouCANObj();
};
