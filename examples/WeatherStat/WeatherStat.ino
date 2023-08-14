// Include Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Include Libraries
#include "Includes.h"

// Define B107AA Object
AVR_Functions Module;

// Set PostOffice Cloud API
PostMan Postman(Serial_GSM);

// LED Functions
#ifdef LIGHTS
	
	void Module_LED(const uint8_t _Color, const uint8_t _Blink, const uint16_t _Interval) {

		switch (_Color)	{

			// Red Color
			case RED: {

				// Blink
				for (size_t i = 0; i < _Blink; i++) {

					// Turn ON Red LED
					PIN_WRITE_RED_LED(true);

					// Delay
					delay(_Interval);

					// Turn OFF Red LED
					PIN_WRITE_RED_LED(false);

					// Delay
					delay(_Interval);

				}

				// End Case
				break;

			}

			// Green Color
			case GREEN: {

				// Blink
				for (size_t i = 0; i < _Blink; i++) {

					// Turn ON Green LED
					PIN_WRITE_GREEN_LED(true);

					// Delay
					delay(_Interval);

					// Turn OFF Green LED
					PIN_WRITE_GREEN_LED(false);

					// Delay
					delay(_Interval);

				}

				// End Case
				break;

			}

			// Blue Color
			case BLUE: {

				// Blink
				for (size_t i = 0; i < _Blink; i++) {

					// Turn ON Blue LED
					PIN_WRITE_BLUE_LED(true);

					// Delay
					delay(_Interval);

					// Turn OFF Blue LED
					PIN_WRITE_BLUE_LED(false);

					// Delay
					delay(_Interval);

				}

				// End Case
				break;

			}

			// White Color
			case WHITE: {

				// Blink
				for (size_t i = 0; i < _Blink; i++) {

					// Turn ON White LED
					PIN_WRITE_RED_LED(true);
					PIN_WRITE_GREEN_LED(true);
					PIN_WRITE_BLUE_LED(true);

					// Delay
					delay(_Interval);

					// Turn OFF White LED
					PIN_WRITE_RED_LED(false);
					PIN_WRITE_GREEN_LED(false);
					PIN_WRITE_BLUE_LED(false);

					// Delay
					delay(_Interval);

				}

				// End Case
				break;

			}

			default: {

				// Turn OFF all LED
				PIN_WRITE_RED_LED(false);
				PIN_WRITE_GREEN_LED(false);
				PIN_WRITE_BLUE_LED(false);

				// End Case
				break;

			}

		}

	}

#endif

// Setup
void setup() {

	// PinOut Definition
	Module.PinOut();

	// Boot LED Sequence
	Module_LED(GREEN, 1, 1000);

	// Start GSM Serial Communication
	Serial_GSM.begin(115200);

	// Connect PostMan
	Postman.Online();

}

// Loop
void loop() {

	// Sleep Mode LED Sequence
	Module_LED(RED, 1, 100);

	// Sleep Mode
	SLEEP();

	// Signal Delay
	delay(100);

}
