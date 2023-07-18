#ifndef __PostMan_Includes__
#define __PostMan_Includes__

	// Define Config Includes
	#ifndef __PostMan_Config__
		#include "Config/Config.h"
	#endif

	// Serial Communications Definations
	#define Serial_Terminal 			Serial

	// Define Console Library
	#ifdef GSM_Debug

		#ifndef __Console__
			#include <Console.h>
		#endif

		// Define Terminal Defaults
		#include "Terminal_Defaults.h"

		// Define Console Library
		Console Terminal_GSM(Serial_Terminal);

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
	#define Command_Update				262
	#define Command_Parameter			500

	#define Command_Interval			301
	#define Command_Current_Ratio		801
	#define Command_FOTA_Download		900
	#define Command_FOTA_Burn			901
	#define Command_Reset				999

	// EEPROM Address Table
	#define EEPROM_Online_Interval		0x00
	#define EEPROM_Offline_Interval		0x02
	#define EEPROM_Alarm_Duration		0x04
	#define EEPROM_V_Min				0x0A
	#define EEPROM_V_Max				0x0C
	#define EEPROM_I_Max				0x0E
	#define EEPROM_FQ_Min				0x10
	#define EEPROM_FQ_Max				0x12
	#define EEPROM_VIMB_Max				0x14
	#define EEPROM_IIMB_Max				0x16
	#define EEPROM_P_Regression			0x1E
	#define EEPROM_PMAX					0x20
	#define EEPROM_Current_Ratio		0x28
	#define EEPROM_Fault_Control		0x2A

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

	// Define Status Bits
	#define STATUS_PUMP					0
	#define STATUS_PHASE_R				1
	#define STATUS_PHASE_S				2
	#define STATUS_PHASE_T				3
	#define STATUS_FAULT_TH				5
	#define STATUS_FAULT_MP				6
	#define STATUS_FAULT_SA				7
	#define STATUS_INPUT				8
	#define STATUS_P_LOW				10
	#define STATUS_P_HIGH				11
	#define STATUS_P_DROP				12
	#define STATUS_P_RISE				13
	#define STATUS_V_LOW				16
	#define STATUS_V_HIGH				17
	#define STATUS_I_HIGH				18
	#define STATUS_FQ_LOW				19
	#define STATUS_FQ_HIGH				20
	#define STATUS_V_IMBALANCE			21
	#define STATUS_I_IMBALANCE			22
	#define STATUS_CT_R					26
	#define STATUS_CT_S					27
	#define STATUS_CT_T					28
	#define STATUS_P_SENSOR				29
	#define STATUS_SD					30
	#define STATUS_SYSTEM				31

#endif