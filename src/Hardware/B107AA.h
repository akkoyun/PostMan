#ifndef __PostMan_Hardware__
#define __PostMan_Hardware__

	// Define Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Hardware Functions
	class Hardware {

		public:

			// GSM_RING 		- PJ2
			// GSM_PWMon 		- PJ3
			// GSM_SWReady		- PJ4
			// GSM_COMM_ENABLE	- PJ5
			// GSM_ON_OFF		- PJ6
			// GSM_SDOWN		- PJ7
			// 3V8_EN			- PE2

			// Constractor
			Hardware(void) {

				/*  PORT D
					PD0 - SCL
					PD1 - SDA
					PD2 - RXD1
					PD3 - TXD1
					PD4 - Output / Pull Down [NC] 			- ICP1		- [85]
					PD5 - Output / Pull Down [MCU LED Blue]	- XCK1		- [84]
					PD6 - Output / Pull Down [MCU LED Green]- T1		- [83]
					PD7 - Output / Pull Down [MCU LED Red]	- T0		- [82]
				*/
				DDRD |= 0b11110000; PORTD &= 0b00001111;

			}

			// Enable Communication Buffer.
			void Communication(const bool _State) {

				// Enable Communication 
				if (_State) PORTJ &= 0b11011111;

				// Disable Communication
				if (!_State) PORTJ |= 0b00100000;

			}

			// Power Switch
			void Power_Switch(const bool _State) {

				// Set GSM Power Enable
				if (_State) PORTE |= 0b00000100;

				// Set GSM Power Disable
				if (!_State) PORTE &= 0b11111011;
			
			}

			// On or Off Modem.
			void OnOff(const uint16_t _Time) {

				// Set On/Off Signal HIGH [PJ6]
				PORTJ |= 0b01000000;

				// Command Delay
				for (uint8_t i = 0; i < 36; i++) {

					// Calculate Delay (2000)
					uint8_t _Delay = _Time / 37;

					// Wait
					delay(_Delay); 

				}

				// Set On/Off Signal LOW [PJ6]
				PORTJ &= 0b10111111;

			}

			// ShutDown Modem
			void ShutDown(const uint16_t _Time) {

				// Set Shut Down Signal HIGH [PJ5]
				PORTJ |= 0b10000000;

				// Command Delay
				delay(_Time);

				// Set Shut Down Signal LOW [PJ5]
				PORTJ &= 0b01111111;

			}

			// Get Power Monitor
			bool PowerMonitor(void) {

				// Control for PWMon (PJ3)
				if ((PINJ & (1 << PINJ3)) == (1 << PINJ3)) {

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

#endif /* defined(__PostMan_Hardware__) */