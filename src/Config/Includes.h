#ifndef __PostOffice_Includes__
#define __PostOffice_Includes__

	// Define Config Includes
	#ifndef __PostOffice_Config__
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












	// Define Pack Type
	#define Pack_Online 				(uint8_t)1
	#define Pack_Timed 					(uint8_t)2
	#define Pack_Interrupt 				(uint8_t)3
	#define Pack_Alarm 					(uint8_t)4
	#define Pack_Offline 				(uint8_t)5
	#define FOTA_Download 				(uint8_t)99

#endif