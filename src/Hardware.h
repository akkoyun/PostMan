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

	// Define LED Colors
	#ifndef __WHITE__
		#define	__WHITE__							(uint8_t) 0
	#endif
	#ifndef __RED__
		#define	__RED__								(uint8_t) 1
	#endif
	#ifndef __GREEN__
		#define	__GREEN__							(uint8_t) 2
	#endif
	#ifndef __BLUE__
		#define	__BLUE__							(uint8_t) 3
	#endif
	#ifndef __PURPLE__
		#define	__PURPLE__							(uint8_t) 4
	#endif

	// Hardware Functions
	class Hardware {

		private:

			// Enable Communication Buffer.
			void Communication(const bool _State) {

				// Enable Communication 
				if (_State) PORTJ &= 0b11101111;

				// Disable Communication
				if (!_State) PORTJ |= 0b00010000;

			}

			// Power Switch
			void Power_Switch(const bool _State) {

				// Set GSM Power Enable
				if (_State) PORTH |= 0b00000100;

				// Set GSM Power Disable
				if (!_State) PORTH &= 0b11111011;
			
			}

			// LED Power Enable
			void LED(const bool _State) {

				// Set GSM LED Power Enable
				if (_State) PORTH &= 0b11101111;

				// Set GSM LED Power Disable
				if (!_State) PORTH |= 0b00010000;

			}

			// On or Off Modem.
			void OnOff(const uint16_t _Time) {

				// Set On/Off Signal HIGH [PJ6]
				PORTJ |= 0b01000000;

				// Command Delay
				for (uint8_t i = 0; i < 36; i++) {

					// Calculate Delay (2000)
					uint8_t _Delay = _Time / 37;

					// Terminal Bar
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 4 + i, WHITE, F("▒"));
					#endif

					// Wait
					delay(_Delay); 

				}

				// Set On/Off Signal LOW [PJ6]
				PORTJ &= 0b10111111;

				// Clear Bar
				#ifdef GSM_Debug
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
					this->Power_Switch(true);  
				#endif
				
				// Enable GSM Modem LED Feed
				#ifdef GSM_LED_Switch
					this->LED(true);
				#endif

				// Set Communication Signal LOW
				#ifdef GSM_Comm_Switch
					this->Communication(true);
				#endif
				
				// Boot Delay
				delay(2000);

				// Turn On Modem
				if (this->PowerMonitor()) {

					// Command Delay
					delay(300);

					// End Function
					return (true);

				} else {

					// Send On Off Signal
					this->OnOff(5000);

					// Control for PWMon (PH7)
					if (this->PowerMonitor()) {

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
				if (this->PowerMonitor()) {

					// Turn Off Modem
					this->OnOff(3000);

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

							// Set Status Variable
							this->Status.Power = false;

							// Disable GSM LED Power
							#ifdef GSM_LED_Switch
								this->LED(false);
							#endif

							// Disable GSM Modem Voltage Translator
							#ifdef GSM_Comm_Switch
								this->Communication(false);
							#endif

							// Disable GSM Modem Main Power Switch
							#ifdef GSM_Power_Switch
								this->Power_Switch(false);  
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
						this->LED(false);
					#endif

					// Disable GSM Modem Voltage Translator
					#ifdef GSM_Comm_Switch
						this->Communication(false);
					#endif

					// Disable GSM Modem Main Power Switch
					#ifdef GSM_Power_Switch
						this->Power_Switch(false);  
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

			// GSM Modem Power Sequence
			bool Power(const bool _State) {

				// Open Modem
				if (_State) this->ON();

				// ShutDown Modem
				if (!_State) this->OFF();

				// End Function
				return(this->PowerMonitor());

			}

			// LED Functions
			void MCU_LED(const uint8_t _Color, const uint8_t _Blink, const uint16_t _Interval) {

				// Red Color
				if (_Color == __RED__) {

					// Blink
					for (size_t i = 0; i < _Blink; i++) {

						// Turn ON Red LED
						PORTD |= 0b10000000;

						// Delay
						delay(_Interval);

						// Turn OFF Red LED
						PORTD &= 0b01111111;

						// Delay
						delay(_Interval);

					}

				}
				
				// Green Color
				if (_Color == __GREEN__) {

					// Blink
					for (size_t i = 0; i < _Blink; i++) {

						// Turn ON Green LED
						PORTD |= 0b01000000;

						// Delay
						delay(_Interval);

						// Turn OFF Green LED
						PORTD &= 0b10111111;

						// Delay
						delay(_Interval);

					}

				}

				// Green Color
				if (_Color == __BLUE__) {

					// Blink
					for (size_t i = 0; i < _Blink; i++) {

						// Turn ON Blue LED
						PORTD |= 0b00100000;

						// Delay
						delay(_Interval);

						// Turn OFF Blue LED
						PORTD &= 0b11011111;

						// Delay
						delay(_Interval);

					}

				}

				// Purple Color
				if (_Color == __PURPLE__) {

					// Blink
					for (size_t i = 0; i < _Blink; i++) {

						// Turn ON Purple LED
						PORTD |= 0b10100000;

						// Delay
						delay(_Interval);

						// Turn OFF Purple LED
						PORTD &= 0b01011111;

						// Delay
						delay(_Interval);

					}

				}

				// White Color
				if (_Color == __WHITE__) {

					// Blink
					for (size_t i = 0; i < _Blink; i++) {

						// Turn ON White LED
						PORTD |= 0b11100000;

						// Delay
						delay(_Interval);

						// Turn OFF White LED
						PORTD &= 0b01111111;

						// Delay
						delay(_Interval);

					}

				}

				// Turn OFF all LED
				PORTD &= 0b00011111;

			}

		};

#endif /* defined(__PostMan_Hardware__) */