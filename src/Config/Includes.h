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

	// PostOffice Pack Types
	enum Pack_Types {
		Online		= 1,
		Timed		= 2,
		Interrupt	= 3,
		Alarm		= 4,
		Offline		= 5,
		FOTA		= 99
	};

#endif