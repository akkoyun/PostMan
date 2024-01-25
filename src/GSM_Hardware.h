// Define Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Include Configurations
#include "Config/PinOut.h"

// Define Configurations
#define ENABLE  true
#define DISABLE false

// Hardware Functions
class GSM_Hardware {

    // Public Context
	public:

		// Constractor
		GSM_Hardware(void) {


		}

		// SD Multiplexer Function
		void SD_Multiplexer(const bool _State) {

			// Control for SD Sense
			if (_State) {

				// Set SD_EN
				PORT_SD_EN |= (1 << PIN_SD_EN);

			} else {

				// Clear SD_EN
				PORT_SD_EN &= ~(1 << PIN_SD_EN);

			}

			// SD Wait Delay
			delay(200);

		}

		// Power Switch
		void Power_Switch(const bool _State = false) {

			// Control for _State
			if (_State) {

				// Set PIN_EN_3V8 pin HIGH
				PORT_EN_3V8 |= (1 << PIN_EN_3V8);


			} else {

				// Set PIN_EN_3V8 pin LOW
				PORT_EN_3V8 &= ~(1 << PIN_EN_3V8);

			}

		}

		// Enable Communication Buffer.
		void Communication(const bool _State = false) {

			// Control for _State
			if (_State) {

				// Set GSM_COMM_EN pin LOW
				PORT_GSM_COMM_EN &= ~(1 << PIN_GSM_COMM_EN);

			} else {

				// Set GSM_COMM_EN pin HIGH
				PORT_GSM_COMM_EN |= (1 << PIN_GSM_COMM_EN);

			}

		}

		// On or Off Modem.
		void OnOff(const uint16_t _Time) {

			// Set PIN_GSM_ONOFF Signal HIGH
			PORT_GSM_ONOFF |= (1 << PIN_GSM_ONOFF);

			// Command Delay
			for (uint8_t i = 0; i < 36; i++) {

				// Calculate Delay (2000)
				uint8_t _Delay = _Time / 37;

				// Wait
				delay(_Delay); 

			}

			// Set PIN_GSM_ONOFF Signal LOW
			PORT_GSM_ONOFF &= ~(1 << PIN_GSM_ONOFF);

		}

		// ShutDown Modem
		void ShutDown(const uint16_t _Time) {

			// Set PIN_GSM_SDOWN Signal HIGH
			PORT_GSM_SDOWN |= (1 << PIN_GSM_SDOWN);

			// Command Delay
			delay(_Time);

			// Set PIN_GSM_SDOWN Signal LOW
			PORT_GSM_SDOWN &= ~(1 << PIN_GSM_SDOWN);

		}

		// Get Power Monitor
		bool PowerMonitor(void) {

			// Control for PIN_GSM_PMON pin
			if ((PIN_REGISTER_GSM_PMON & (1 << PIN_GSM_PMON)) == (1 << PIN_GSM_PMON)) {

				// End Function
				return (true);

			} else {

				// End Function
				return(false);

			}

		}

		// Get Software Ready
		bool SWReady(void) {

			// Control for PIN_GSM_SWREADY pin
			if ((PIN_REGISTER_GSM_SWREADY & (1 << PIN_GSM_SWREADY)) == (1 << PIN_GSM_SWREADY)) {

				// End Function
				return (true);

			} else {

				// End Function
				return(false);

			}

		}

		// Power ON Sequence of Modem
		bool ON(void) {

			// Get Start Time
			uint32_t _Start_Time = millis();

			// Enable GSM Modem Power Switch
			this->Power_Switch(true);  

			// Power On Delay
			delay(10);

			// Set Communication Signal LOW
			this->Communication(true);

			// Communication Delay
			delay(10);

			// Turn On Modem
			if (this->PowerMonitor()) {

				// End Function
				return (true);

			} else {

				// Send On Off Signal
				this->OnOff(1500);

				// Wait for Power Monitor
				while (millis() - _Start_Time < 15000) {

					// Control for PWMon (PJ3)
					if (this->PowerMonitor()) {

						// Wait for Software Ready
						while (millis() - _Start_Time < 30000) {

							// Control for SWReady (PJ4)
							if (this->SWReady()) return (true);

							// Wait Delay
							delay(10);

						}

					}

					// Wait Delay
					delay(10);

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

		// Heartbeat Function
		void WD_Heartbeat(void) {

			// Turn ON HeartBeat
			PORT_HEARTBEAT |= (1 << PIN_HEARTBEAT);

			// Turn OFF HeartBeat
			PORT_HEARTBEAT &= ~(1 << PIN_HEARTBEAT);

		}
};
