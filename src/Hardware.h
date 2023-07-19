#ifndef __PostMan_Hardware__
#define __PostMan_Hardware__

	// Define Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Define Library Includes
	#ifndef __PostMan_Includes__
		#include "Config/Includes.h"
	#endif

	// Define Relay Inputs
	#define POWER_MON (((PINJ) >> (PINJ3)) & 0x01)
	#define COMMUNICATION(_State) (_State ? PORTJ &= 0b11101111 : PORTJ |= 0b00010000)
	#define POWER_SWITCH(_State) (_State ? PORTH |= 0b00000100 : PORTH &= 0b11111011)
	#define STAT_LED(_State) (_State ? PORTH &= 0b11101111 : PORTH |= 0b00010000)

	// Hardware Functions
	class Hardware {

		private:

			// On or Off Modem.
			void OnOff(const uint16_t _Time) {

				// Set On/Off Signal HIGH [PJ6]
				PORTJ |= 0b01000000;

				// Command Delay
				for (uint8_t i = 0; i < 36; i++) {

					// Calculate Delay (2000)
					uint8_t _Delay = _Time / 37;

					// Terminal Bar
					#ifdef DEBUG
						Terminal_GSM.Text(14, 4 + i, WHITE, F("▒"));
					#endif

					// Wait
					delay(_Delay); 

				}

				// Set On/Off Signal LOW [PJ6]
				PORTJ &= 0b10111111;

				// Clear Bar
				#ifdef DEBUG
					for (uint8_t i = 0; i < 36; i++) Terminal_GSM.Text(14, 4 + i, WHITE, F(" "));
				#endif

			}

			// ShutDown Modem
			void ShutDown(const uint16_t _Time) {

				// Set Shut Down Signal HIGH [PJ5]
				PORTJ |= 0b00100000;

				// Command Delay
				delay(_Time);

				// Set Shut Down Signal LOW [PJ5]
				PORTJ &= 0b11011111;

			}

			// Power ON Sequence of Modem
			bool ON(void) {

				// Enable GSM Modem Power Switch
				#ifdef GSM_Power_Switch
					POWER_SWITCH(true);  
				#endif
				
				// Enable GSM Modem LED Feed
				#ifdef GSM_LED_Switch
					STAT_LED(true);
				#endif

				// Set Communication Signal LOW
				#ifdef GSM_Comm_Switch
					COMMUNICATION(true);
				#endif
				
				// Boot Delay
				delay(2000);

				// Turn On Modem
				if (POWER_MON) {

					// Command Delay
					delay(300);

					// End Function
					return (true);

				} else {

					// Send On Off Signal
					this->OnOff(5000);

					// Control for PWMon (PH7)
					if (POWER_MON) {

						// End Function
						return(true);

					} else {

						// Send Shut Down Signal
						this->ShutDown(200);

					}

				}

				// End Function
				return (false);

			}

			// Power OFF Sequence of Modem
			bool OFF(void) {

				// Turn Off Modem
				if (POWER_MON) {

					// Turn Off Modem
					this->OnOff(3000);

					// Set Variable
					bool _Power = true;

					// Read Current Time
					const uint32_t _Current_Time = millis();

					// Control for Power Monitor
					while (_Power) {

						// Control for PowerMonitor
						if (!POWER_MON) {

							// Set Variable
							_Power = false;

							// Set Status Variable
							this->Status.Power = false;

							// Disable GSM LED Power
							#ifdef GSM_LED_Switch
								STAT_LED(false);
							#endif

							// Disable GSM Modem Voltage Translator
							#ifdef GSM_Comm_Switch
								COMMUNICATION(false);
							#endif

							// Disable GSM Modem Main Power Switch
							#ifdef GSM_Power_Switch
								POWER_SWITCH(false);  
							#endif

						}

						// Handle for timeout
						if (millis() - _Current_Time >= 15000) break;;

					}
					
				} else {

					// Set Status Variable
					this->Status.Power = false;

					// Disable GSM LED Power
					#ifdef GSM_LED_Switch
						STAT_LED(false);
					#endif

					// Disable GSM Modem Voltage Translator
					#ifdef GSM_Comm_Switch
						COMMUNICATION(false);
					#endif

					// Disable GSM Modem Main Power Switch
					#ifdef GSM_Power_Switch
						POWER_SWITCH(false);  
					#endif

				}

				// End Function
				return (true);

			}

		public:

			// Define Modem Hardware Status Structure
			struct Struct_Status {
				bool 		Power 				= false;
			} Status;

			// Constractor
			Hardware(void) {

			}

			// GSM Modem Power Sequence
			bool Power(const bool _State) {

				// Power Process
				if (_State) {

					// Power ON Sequence
					return(this->ON());

				} else {

					// Power OFF Sequence
					return(this->OFF());

				}

			}

		};

#endif /* defined(__PostMan_Hardware__) */