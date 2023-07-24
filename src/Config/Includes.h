#ifndef __PostMan_Includes__
#define __PostMan_Includes__

	// Define Config Includes
	#ifndef __PostMan_Config__
		#include "Config/Config.h"
	#endif



	// Define JSON Handler
	#include <ArduinoJson.h>

	// Define RTC Library
	#ifndef __RV3028__
		#include <RV3028.h>
	#endif

	// SD Library
	#include <SPI.h>
	#include "SD.h"

	// Define Pack Types
	#ifndef PowerStat_Pack_Types
		#define Pack_Online						1
		#define Pack_Update						2
		#define Pack_Timed						3
		#define Pack_Interrupt					4
		#define Pack_Alarm						5
		#define Pack_Offline					6
		#define Pack_FOTA_Info					90
		#define Pack_FOTA_Download				91
		#define Pack_FOTA_Burn					99
	#endif

	// Define PowerStat Commands
	#ifndef PowerStat_Commands

		// Define Reset Command
		#define Command_OK					200
		#define Command_NOK					201

		// Define Pump Commands
		#define Command_Start				256
		#define Command_Stop				257

		// Define Device Commands
		#define Command_Update				262
		#define Command_Reset				999

		// Define Device Configuration Commands
		#define Command_Parameter			500

		// Define FOTA Commands
		#define Command_FOTA_Download		900
		#define Command_FOTA_Burn			901

	#endif
	
	// EEPROM Address Table
	#ifndef PowerStat_EEPROM
		#define EEPROM_Online_Interval		0x00
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
	#endif

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