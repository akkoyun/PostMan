// Library include guard
#pragma once

// Define Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Hardware Functions
class Hardware {

	public:

		// GSM_PWMon 		- PJ2
		// GSM_RING 		- PJ3
		// GSM_SWReady		- PJ4

		// 3V8_EN			- PA0
		// GSM_COMM_ENABLE	- PA1
		// GSM_ON_OFF		- PA2
		// GSM_SDOWN		- PA3
		// GNSS_EN		    - PA4

		// Constractor
		Hardware(void) {


		}

		// Enable Communication Buffer.
		void Communication(const bool _State) {

			// Enable Communication 
			if (_State) PORTA &= 0b11111101;

			// Disable Communication
			if (!_State) PORTA |= 0b00000010;

		}

		// Power Switch
		void Power_Switch(const bool _State) {

			// Set GSM Power Enable
			if (_State) PORTA |= 0b00000001;

			// Set GSM Power Disable
			if (!_State) PORTA &= 0b11111110;
		
		}

		// On or Off Modem.
		void OnOff(const uint16_t _Time) {

			// Set On/Off Signal HIGH [PA2]
			PORTA |= 0b00000100;

			// Command Delay
			for (uint8_t i = 0; i < 36; i++) {

				// Calculate Delay (2000)
				uint8_t _Delay = _Time / 37;

				// Wait
				delay(_Delay); 

			}

			// Set On/Off Signal LOW [PA2]
			PORTA &= 0b11111011;

		}

		// ShutDown Modem
		void ShutDown(const uint16_t _Time) {

			// Set Shut Down Signal HIGH [PA3]
			PORTA |= 0b00001000;

			// Command Delay
			delay(_Time);

			// Set Shut Down Signal LOW [PA3]
			PORTA &= 0b11110111;

		}

		// Get Power Monitor
		bool PowerMonitor(void) {

			// Control for PWMon (PJ2)
			if ((PINJ & (1 << PINJ2)) == (1 << PINJ2)) {

				// End Function
				return (true);

			} else {

				// End Function
				return(false);

			}

		}

		// Get Software Ready
		bool SWReady(void) {

			// Control for SWReady (PJ4)
			if ((PINJ & (1 << PINJ4)) == (1 << PINJ4)) {

				// End Function
				return (true);

			} else {

				// End Function
				return(false);

			}

		}

		// Enable GNSS
		void GNSS_Enable(const bool _State) {

			// Enable GNSS 
			if (_State) PORTA |= 0b00010000;

			// Disable GNSS
			if (!_State) PORTA &= 0b11101111;

		}

		// Power ON Sequence of Modem
		bool ON(void) {

			// Enable GSM Modem Power Switch
			this->Power_Switch(true);  

			// Power On Delay
			delay(50);
			
			// Set Communication Signal LOW
			this->Communication(true);

			// Communication Delay
			delay(50);
			
			// Turn On Modem
			if (this->PowerMonitor()) {

				// End Function
				return (true);

			} else {

				// Send On Off Signal
				this->OnOff(1500);

				// Boot Delay
				delay(10000);

				// Control for PWMon (PJ3)
				if (this->PowerMonitor()) {

					// Boot Delay
					delay(10000);

					// Control for SWReady (PJ4)
					if (this->SWReady()) {

						// End Function
						return (true);

					} else {

						// Send On Off Signal
						this->OnOff(3000);

					}

				} else {

					// Send On Off Signal
					this->OnOff(3000);

				}

			}

			// End Function
			return (false);

		}

		// Power OFF Sequence of Modem
		bool OFF(void) {

			// Turn Off Modem
			if (this->PowerMonitor()) {

				// Turn Off Modem
				this->OnOff(2750);

				// Set Variable
				bool _Power = true;

				// Read Current Time
				const uint32_t _Current_Time = millis();

				// Control for Power Monitor
				while (_Power) {

					// Control for PowerMonitor
					if (!this->PowerMonitor()) {

						// Set Variable
						_Power = false;

						// Disable GSM Modem Voltage Translator
						this->Communication(false);

						// Disable GSM Modem Main Power Switch
						this->Power_Switch(false);  

					}

					// Handle for timeout
					if (millis() - _Current_Time >= 15000) break;;

				}
				
			} else {

				// Disable GSM Modem Voltage Translator
				this->Communication(false);

				// Disable GSM Modem Main Power Switch
				this->Power_Switch(false);  

			}

			// End Function
			return (true);

		}

};
