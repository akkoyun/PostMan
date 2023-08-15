// Library include guard
#pragma once

// Define Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Hardware Functions
class Hardware {

	public:

		// Command Time Delay Definitions
		#define ON_Delay					5000
		#define OFF_Delay					3000
		#define BOOT_Delay					1500
		#define SWREADY_Wait				15000

		// Constractor
		Hardware(void) {

		}

		// Communication Buffer IC Control
		void Communication(const bool _State) {

			// Enable Communication 
			if (_State) PORT_GSM_COMM_EN &= ~(1 << PIN_GSM_COMM_EN);

			// Disable Communication
			if (!_State) PORT_GSM_COMM_EN |= (1 << PIN_GSM_COMM_EN);

			// Command Delay
			delay(50);

		}

		// Power Switch IC Control
		void Power_Switch(const bool _State) {

			// Set GSM Power Enable
			if (_State) PORT_GSM_POWER_EN |= (1 << PIN_GSM_POWER_EN);

			// Set GSM Power Disable
			if (!_State) PORT_GSM_POWER_EN &= ~(1 << PIN_GSM_POWER_EN);

			// Command Delay
			delay(50);

		}

		// On/Off Signal Control
		void OnOff(const uint16_t _Time) {

			// Set On/Off Signal HIGH [PA2]
			PORT_GSM_ONOFF |= (1 << PIN_GSM_ONOFF);

			// Command Delay
			delay(_Time);

			// Set On/Off Signal LOW [PA2]
			PORT_GSM_ONOFF &= ~(1 << PIN_GSM_ONOFF);

		}

		// ShutDown Signal Control
		void ShutDown(const uint16_t _Time) {

			// Set Shut Down Signal HIGH [PA3]
			PORT_GSM_SDOWN |= (1 << PIN_GSM_SDOWN);

			// Command Delay
			delay(_Time);

			// Set Shut Down Signal LOW [PA3]
			PORT_GSM_SDOWN &= ~(1 << PIN_GSM_SDOWN);

		}

		// Read Power Monitor Signal
		bool PowerMonitor(void) {

			// Read Power Monitor Signal
			return ((PIN_REGISTER_GSM_PWMon & (1 << PIN_GSM_PWMon)) ? true : false);
			
		}

		// Read SWReady Signal
		bool SWReady(void) {

			// Read SWReady Signal
			return ((PIN_REGISTER_GSM_SWReady & (1 << PIN_GSM_SWReady)) ? true : false);

		}

		// GNSS Control
		void GNSS(const bool _State) {

			// Enable GNSS 
			if (_State) PORT_GSM_GNSS_EN |= (1 << PIN_GSM_GNSS_EN);

			// Disable GNSS
			if (!_State) PORT_GSM_GNSS_EN &= ~(1 << PIN_GSM_GNSS_EN);

		}

		// Power ON Sequence of Modem
		bool ON(void) {

			// Enable GSM Modem Power Switch
			this->Power_Switch(true);  
			
			// Set Communication Signal LOW
			this->Communication(true);
			
			// Turn On Modem
			if (this->PowerMonitor()) {

				// End Function
				return (true);

			} else {

				// Send On Off Signal
				this->OnOff(ON_Delay);

				// Control for PWMon (PJ3)
				if (this->PowerMonitor()) {

					// Read Current Time
					const uint32_t Current_Time = millis();

					// Wait for SWReady (PJ4)
					while (!this->SWReady()) {

						// Wait Delay
						delay(5);

						// Handle for timeout
						if (millis() - Current_Time >= SWREADY_Wait) return (false);

					}

					// Boot Delay
					delay(BOOT_Delay);

					// End Function
					return (true);

				} else {

					// Send On Off Signal
					this->OnOff(OFF_Delay);

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
				this->OnOff(OFF_Delay);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Wait for SWReady (PJ4)
				while (this->PowerMonitor()) {

					// Wait Delay
					delay(5);

					// Handle for timeout
					if (millis() - Current_Time >= OFF_Delay) return (false);

				}

				// Disable GSM Modem Voltage Translator
				this->Communication(false);

				// Disable GSM Modem Main Power Switch
				this->Power_Switch(false);  

				// End Function
				return (true);
				
			} else {

				// Disable GSM Modem Voltage Translator
				this->Communication(false);

				// Disable GSM Modem Main Power Switch
				this->Power_Switch(false);  

				// End Function
				return (true);

			}

			// End Function
			return (false);

		}

};
