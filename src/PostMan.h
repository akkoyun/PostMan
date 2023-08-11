#ifndef __PostMan__
#define __PostMan__

	// Include Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Include Configurations
	#ifndef __PostMan_Config__
		#include "Config/Config.h"
	#endif

	// Include Console Library
	#ifdef DEBUG

		// Define Console Library
		#ifndef __Console__
			#include <Console.h>
		#endif

	#endif

	// Select Project
	#if defined(_PowerStat_)

		// Include Files
		#include "PowerStatV4.h"

		// Define Library
		typedef Postman_PowerStatV4 PostMan;

	#elif defined(_WeatherStat_)

		// Include Files
		#include "WeatherStatV3.h"

		// Define Library
		typedef Postman_WeatherStatV3 PostMan;

	#else

		#error "No Project Selected"

	#endif

#endif /* defined(__PostMan__) */