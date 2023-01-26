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

	// Define Battery Structure
	typedef struct {

		// Device ID
		char * 		Device_ID;

		// Environment
		float		Temperature			= 0;
		float		Humidity			= 0;

		// Battery
		float		IV					= 0;
		float		AC					= 0;
		float		SOC					= 0;
		uint8_t		Charge				= 0;
		float		Charge_Temp			= 0;
		uint16_t	Full_Cap			= 0;
		uint16_t	Instant_Cap			= 0;

	} Struct_Device;

	// PostOffice Pack Types
	enum Pack_Types {
		Online		= 1,
		Timed		= 2,
		Interrupt	= 3,
		Alarm		= 4,
		Offline		= 5,
		FOTA_Info	= 99
	};

#endif