// Library include guard
#pragma once

// Define Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Hardware Functions
class Hardware {

	public:

		// GSM Power Switch Pin Definitions [PA0]
		#define DDR_GSM_Power				DDRA
		#define PORT_GSM_Power				PORTA
		#define PIN_GSM_Power				PA0

		// COMM_EN Pin Definitions [PA1]
		#define DDR_COMM_ENABLE				DDRA
		#define PORT_COMM_ENABLE			PORTA
		#define PIN_COMM_ENABLE				PA1

		// GSM On/Off Signal Pin Definitions [PA2]
		#define DDR_GSM_OnOff				DDRA
		#define PORT_GSM_OnOff				PORTA
		#define PIN_GSM_OnOff				PA2

		// GSM Shut Down Signal Pin Definitions [PA3]
		#define DDR_GSM_ShutDown			DDRA
		#define PORT_GSM_ShutDown			PORTA
		#define PIN_GSM_ShutDown			PA3

		// GNSS Enable Signal Pin Definitions [PA4]
		#define DDR_GNSS_Enable				DDRA
		#define PORT_GNSS_Enable			PORTA
		#define PIN_GNSS_Enable				PA4

		// GSM_PWMon Pin Definitions [PJ2]
		#define DDR_GSM_PWMon				DDRJ
		#define PORT_GSM_PWMon				PORTJ
		#define PIN_REGISTER_GSM_PWMon		PINJ
		#define PIN_GSM_PWMon				PJ2

		// GSM_RING Pin Definitions [PJ3]
		#define DDR_GSM_RING				DDRJ
		#define PORT_GSM_RING				PORTJ
		#define PIN_REGISTER_GSM_RING		PINJ
		#define PIN_GSM_RING				PJ3

		// GSM_SWReady Pin Definitions [PJ4]
		#define DDR_GSM_SWReady				DDRJ
		#define PORT_GSM_SWReady			PORTJ
		#define PIN_REGISTER_GSM_SWReady	PINJ
		#define PIN_GSM_SWReady				PJ4

		// Command Time Delay Definitions
		#define ON_Delay					5000
		#define OFF_Delay					3000
		#define BOOT_Delay					1500
		#define SWREADY_Wait				15000

		// Constractor
		Hardware(void) {

			// Set GSM_Power as Output with Pull-Down
			DDR_GSM_Power |= (1 << PIN_GSM_Power);
			PORT_GSM_Power &= ~(1 << PIN_GSM_Power);

			// Set GSM_COMM_ENABLE as Output with Pull-Up
			DDR_COMM_ENABLE |= (1 << PIN_COMM_ENABLE);
			PORT_COMM_ENABLE |= (1 << PIN_COMM_ENABLE);

			// Set GSM_OnOff as Output with Pull-Down
			DDR_GSM_OnOff |= (1 << PIN_GSM_OnOff);
			PORT_GSM_OnOff &= ~(1 << PIN_GSM_OnOff);

			// Set GSM_ShutDown as Output with Pull-Down
			DDR_GSM_ShutDown |= (1 << PIN_GSM_ShutDown);
			PORT_GSM_ShutDown &= ~(1 << PIN_GSM_ShutDown);

			// Set GNSS_Enable as Output with Pull-Down
			DDR_GNSS_Enable |= (1 << PIN_GNSS_Enable);
			PORT_GNSS_Enable &= ~(1 << PIN_GNSS_Enable);

			// Set GSM_PWMon as Input with Pull-Down
			DDR_GSM_PWMon &= ~(1 << PIN_GSM_PWMon);
			PORT_GSM_PWMon &= ~(1 << PIN_GSM_PWMon);

			// Set GSM_RING as Input with Pull-Up
			DDR_GSM_RING &= ~(1 << PIN_GSM_RING);
			PORT_GSM_RING |= (1 << PIN_GSM_RING);

			// Set GSM_SWReady as Input with Pull-Down
			DDR_GSM_SWReady &= ~(1 << PIN_GSM_SWReady);
			PORT_GSM_SWReady &= ~(1 << PIN_GSM_SWReady);

		}

		// Communication Buffer IC Control
		void Communication(const bool _State) {

			// Enable Communication 
			if (_State) PORT_COMM_ENABLE &= ~(1 << PIN_COMM_ENABLE);

			// Disable Communication
			if (!_State) PORT_COMM_ENABLE |= (1 << PIN_COMM_ENABLE);

			// Command Delay
			delay(50);

		}

		// Power Switch IC Control
		void Power_Switch(const bool _State) {

			// Set GSM Power Enable
			if (_State) PORT_GSM_Power |= (1 << PIN_GSM_Power);

			// Set GSM Power Disable
			if (!_State) PORT_GSM_Power &= ~(1 << PIN_GSM_Power);

			// Command Delay
			delay(50);

		}

		// On/Off Signal Control
		void OnOff(const uint16_t _Time) {

			// Set On/Off Signal HIGH [PA2]
			PORT_GSM_OnOff |= (1 << PIN_GSM_OnOff);

			// Command Delay
			delay(_Time);

			// Set On/Off Signal LOW [PA2]
			PORT_GSM_OnOff &= ~(1 << PIN_GSM_OnOff);

		}

		// ShutDown Signal Control
		void ShutDown(const uint16_t _Time) {

			// Set Shut Down Signal HIGH [PA3]
			PORT_GSM_ShutDown |= (1 << PIN_GSM_ShutDown);

			// Command Delay
			delay(_Time);

			// Set Shut Down Signal LOW [PA3]
			PORT_GSM_ShutDown &= ~(1 << PIN_GSM_ShutDown);

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
			if (_State) PORT_GNSS_Enable |= (1 << PIN_GNSS_Enable);

			// Disable GNSS
			if (!_State) PORT_GNSS_Enable &= ~(1 << PIN_GNSS_Enable);

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
