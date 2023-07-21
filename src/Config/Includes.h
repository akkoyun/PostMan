#ifndef __PostMan_Includes__
#define __PostMan_Includes__

	// Define Config Includes
	#ifndef __PostMan_Config__
		#include "Config/Config.h"
	#endif



	// Define JSON Handler
	#include <ArduinoJson.h>

	// Define Environment Library
	#ifndef __Environment__
		#include <Environment.h>
	#endif

	// Define RTC Library
	#ifndef __RV3028__
		#include <RV3028.h>
	#endif

	// Define Battery Charger Library
	#ifndef __BQ24298__
		#include <BQ24298.h>
	#endif

	// Define Battery Gauge Library
	#ifndef __MAX17055__
		#include <MAX17055.h>
	#endif

	// SD Library
	#include <SPI.h>
	#include "SD.h"


	// PostOffice Pack Types
	enum Pack_Types {
		Online			= 1,
		Update			= 2,
		Timed			= 3,
		Interrupt		= 4,
		Alarm			= 5,
		Offline			= 6,
		FOTA_Info		= 90,
		FOTA_Download	= 91,
		FOTA_Burn		= 99
	};

	// Define Commands
	#define Command_OK					200
	#define Command_NOK					201
	#define Command_Start				256
	#define Command_Stop				257
	#define Command_Update				262
	#define Command_Parameter			500

	#define Command_Interval			301
	#define Command_Current_Ratio		801
	#define Command_FOTA_Download		900
	#define Command_FOTA_Burn			901
	#define Command_Reset				999

	// EEPROM Address Table
//	#define EEPROM_Online_Interval		0x00
	#define EEPROM_Offline_Interval		0x01
	#define EEPROM_V_Min				0x06
	#define EEPROM_V_Max				0x08
	#define EEPROM_I_Max				0x0A
	#define EEPROM_FQ_Min				0x0C
	#define EEPROM_FQ_Max				0x0D
	#define EEPROM_VIMB_Max				0x10
	#define EEPROM_IIMB_Max				0x11
	#define EEPROM_PMIN					0x1A
	#define EEPROM_PMAX					0x1C
	#define EEPROM_P_Regression			0x1E
	#define EEPROM_STOP_MASK			0x20
	#define EEPROM_PUBLISH_MASK			0x24


	// Define FOTA Status
	#define FOTA_Download_OK			1
	#define FOTA_FTP_Open_Error			10
	#define FOTA_FTP_Type_Error			11
	#define FOTA_FTP_Folder_Error		12
	#define FOTA_FTP_File_Size_Error	13
	#define FOTA_FTP_Get_Error			14
	#define FOTA_FTP_Close_Error		15
	#define FOTA_Download_TimeOut		20
	#define FOTA_Download_Size_Error	21
	#define FOTA_Download_Not_Save		21
	#define FOTA_SD_Error				30

	// Define HTTP Headers
	#define HTTP_OK						200
	#define HTTP_Accepted				202
	#define HTTP_NoContent				204
	#define HTTP_BadRequest				400
	#define HTTP_NotFound				404
	#define HTTP_MethodNotAllowed		405
	#define HTTP_NotAcceptable			406


#endif