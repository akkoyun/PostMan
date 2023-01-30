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

	// Define Time Structure
	typedef struct {
		uint8_t 	Year				= 0;
		uint8_t 	Month				= 0;
		uint8_t 	Day					= 0;
		uint8_t 	Hour				= 0;
		uint8_t 	Minute				= 0;
		uint8_t 	Second				= 0;
	} Struct_Time;

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
	#define Command_Interval			301
	#define Command_Current_Ratio		801
	#define Command_FOTA_Download		900
	#define Command_FOTA_Burn			901
	#define Command_Reset				999

	// EEPROM Address Table
	#define EEPROM_Current_Ratio		0x00
	#define EEPROM_Online_Interval		0x01
	#define EEPROM_Offline_Interval		0x02

#endif