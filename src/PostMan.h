#ifndef __PostMan__
#define __PostMan__

	// Include Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Include Configurations
	#include "Config/Config.h"

	// Select Project
	#if defined(_PowerStat_)

		// Include Project Files
		#include "PowerStatV4.h"

		// Define Library Name
		typedef Postman_PowerStatV4 PostMan;

	#elif defined(_WeatherStat_)

		// Include Project Files
		#include "WeatherStatV3.h"

		// Define Library Name
		typedef Postman_WeatherStatV3 PostMan;

	#else

		// Error
		#error "No Project Selected"

	#endif

#endif /* defined(__PostMan__) */