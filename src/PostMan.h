#ifndef __PostMan__
#define __PostMan__

	// Include Arduino Library
	#ifndef Arduino_h
		#include <Arduino.h>
	#endif

	// Include Configurations
	#include "Config.h"

	// Include Definitions
	#include "Definitions.h"

	// Include AT Commands
	#ifndef __AT_Command__
		#include <AT_Command.h>
	#endif

	// Include MD5 Hashing
	#ifndef _MD5_
		#include <md5.h>
	#endif

	// Cloud Functions
	class Postman : private LE910C1_EUX {

		// Private Context
		private:

			// Define Pointers
			B107AA* Hardware;
			Variable<float>* Payload;
			PowerStat_Console* Terminal;

			// Define Variables
			uint8_t Socket_Status = _SOCKET_CLOSED_;

			// Define Operator Structure
			struct Struct_Operator {
				uint8_t 	SIM_PIN = _SIM_UNKNOWN_;
				char 		ICCID[21];
				uint16_t 	MCC;
				uint16_t 	MNC;
				uint8_t 	IP_Address[4];
				uint8_t		WDS = _CONNECTION_UNKNOWN_;
				uint16_t	RSSI;
				uint8_t		Signal;
				float 		Connection_Time;
				uint32_t 	Connection_Start;
			} Operator;

			// Define Module Structure
			struct Struct_Module {
				uint8_t 	Manufacturer = _MODEM_MANUFACTURER_UNKNOWN_;
				uint8_t 	Model = _MODEM_MODEL_UNKNOWN_;
				char 		IMEI[17];
				char 		Firmware[15];
			} Module;

			// Define Time Structure
			struct Struct_Time {
				uint8_t 	Year				= 0;
				uint8_t 	Month				= 0;
				uint8_t 	Day					= 0;
				uint8_t 	Hour				= 0;
				uint8_t 	Minute				= 0;
				uint8_t 	Second				= 0;
				uint8_t		Time_Zone			= 0;
			} Time;

			// Clear Variables
			void Clear_Variables(void) {

				// Initialize Status Variables
				bitClear(this->Status, PostMan_Status_Initialize);
				bitClear(this->Status, PostMan_Status_SIM_Inserted);
				bitClear(this->Status, PostMan_Status_Connection);

				// Initialize Module Variables
				this->Module.Manufacturer = 0;
				this->Module.Model = 0;
				memset(this->Module.IMEI, 0, sizeof(this->Module.IMEI));
				memset(this->Module.Firmware, 0, sizeof(this->Module.Firmware));

				// Initialize Operator Variables
				this->Operator.SIM_PIN = _SIM_UNKNOWN_;
				memset(this->Operator.ICCID, 0, sizeof(this->Operator.ICCID));
				this->Operator.MCC = 0;
				this->Operator.MNC = 0;
				memset(this->Operator.IP_Address, 0, sizeof(this->Operator.IP_Address));
				this->Operator.WDS = _CONNECTION_UNKNOWN_;
				this->Operator.RSSI = 0;
				this->Operator.Signal = 0;
				this->Operator.Connection_Start = 0;
				this->Operator.Connection_Time = 0;

				// Initialize Socket Variables
				this->Socket_Status = _SOCKET_CLOSED_;

			}

		// Protected Context
		protected:

			// Initalize GSM Module
			bool Initialize(void) {

				// Declare WatchDog Variable
				uint8_t _WD = 0;

				// Initialize Modem Parameters
				while (!bitRead(this->Status, PostMan_Status_Initialize)) {

					// Control for Modem Power Monitor
					if (Hardware->PowerMonitor()) {

						// Set Control Variable
						bitSet(this->Status, PostMan_Status_Initialize);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT"));

						// Send Command
						if (!LE910C1_EUX::AT()) bitClear(this->Status, PostMan_Status_Initialize);

						// Calculate Connection Time
						this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) {

							// Print Message
							Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

							// Print Connection Time
							Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						}

						// ATE Command (Echo Off)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT0"));

							// Send Command
							if (!LE910C1_EUX::ATE(false)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SIMDET Command (SIM Detect)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+SIMDET?"));

							// Declare Buffer Variable
							bool _SIMDET = false;

							// Send Command
							if (!LE910C1_EUX::SIMDET(GET, 0, _SIMDET)) bitClear(this->Status, PostMan_Status_Initialize);

							// Handle Variable
							if (_SIMDET) {bitSet(this->Status, PostMan_Status_SIM_Inserted);} else {bitClear(this->Status, PostMan_Status_SIM_Inserted);}

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

							// Control for SIM State
							if (!bitRead(this->Status, PostMan_Status_SIM_Inserted)) {

								// Clear Variables
								bitClear(this->Status, PostMan_Status_Initialize);

								// End Function
								return(false);

							}

						} else break;

						// CFUN Command (Set Full Functionality)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CFUN=1,0"));

							// Send Command
							if (!LE910C1_EUX::CFUN(1)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// CMEE Command (Set Error Format)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CMEE=1"));

							// Send Command
							if (!LE910C1_EUX::CMEE(1)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// FCLASS Command (Set Internet Mode)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+FCLASS=0"));

							// Send Command
							if (!LE910C1_EUX::FCLASS(0)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// ATK0 Command (Set Data Mode)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT&K0"));

							// Send Command
							if (!LE910C1_EUX::K(0)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// CPIN Command (Check SIM Pin)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CPIN?"));

							// Send Command
							if (!LE910C1_EUX::CPIN(this->Operator.SIM_PIN)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print SIM Status
								if (bitRead(this->Status, PostMan_Status_SIM_Inserted) && this->Operator.SIM_PIN == _SIM_READY_) {

									// Print SIM Status
									Terminal->Text(17, 29, _Console_GRAY_, F("  SIM OK  "));

								} else {

									// Print SIM Status
									Terminal->Text(17, 29, _Console_GRAY_, F("SIM ERROR "));

								}

							}

							// Control for SIM State
							if (this->Operator.SIM_PIN != _SIM_READY_) {

								// Clear Variables
								bitClear(this->Status, PostMan_Status_Initialize);

								// End Function
								return(false);

							}

						} else break;

						// CGSN Command (Get IMEI Number)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CGSN"));

							// Send Command
							if (!LE910C1_EUX::CGSN(this->Module.IMEI)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print IMEI Number
								Terminal->Text(16, 24, _Console_GRAY_, this->Module.IMEI);

							}

						} else break;

						// ICCID Command (Get SIM ICCID Number)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CCID"));

							// Send Command
							if (!LE910C1_EUX::CCID(this->Operator.ICCID)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print ICCID Number
								Terminal->Text(18, 20, _Console_GRAY_, this->Operator.ICCID);

							}

						} else break;

						// CGMI Command (Get Manufacturer)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CGMI"));

							// Send Command
							if (!LE910C1_EUX::CGMI(this->Module.Manufacturer)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Manufacturer
								if (this->Module.Manufacturer < 9) {
									Terminal->Text(13, 37, _Console_GRAY_, F("0"));
									Terminal->Text(13, 38, _Console_GRAY_, this->Module.Manufacturer);
								} else {
									Terminal->Text(13, 37, _Console_GRAY_, this->Module.Manufacturer);
								}

							}

						} else break;

						// CGMM Command (Get Model)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+GMM"));

							// Send Command
							if (!LE910C1_EUX::CGMM(this->Module.Model)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Model
								Terminal->Text(14, 37, _Console_GRAY_, this->Module.Model);

							}

						} else break;

						// CGMR Command (Get Firmware)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SWPKGV"));

							// Send Command
							if (!LE910C1_EUX::SWPKGV(this->Module.Firmware)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Firmware
								Terminal->Text(15, 30, _Console_GRAY_, this->Module.Firmware);

							}

						} else break;

						// E2RI Command (Set RING Indicator)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#E2RI=50,50"));

							// Send Command
							if (!LE910C1_EUX::E2SLRI(50)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// GPIO Command (Set GPIO)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#GPIO=1,0,2"));

							// Send Command
							if (!LE910C1_EUX::GPIO(SET, 1, 0, 2)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SLED Command (Set LED)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SLED=2"));

							// Send Command
							if (!LE910C1_EUX::SLED(2)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SLEDSAV Command (Set LED Save)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SLEDSAV"));

							// Send Command
							if (!LE910C1_EUX::SLEDSAV()) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(21, 84, _Console_WHITE_, F("                                    "));

						// Initialize Complete
						return(true);

					} else {

						// Power ON GSM Module
						Hardware->ON();

					}

					// Handle WatchDog
					if (_WD > 2) {

						// Initialize Variables
						this->Clear_Variables();

						// End Function
						return(false);

					} else {

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) {

							// Print Message
							Terminal->Show_Message(_Console_YELLOW_, F("Retry Initializing..."));

						}

						// Set WatchDog Variable
						_WD++;

					}

				}

				// Not Initialized
				return(false);

			}

			// Connect GSM Module
			bool Connect(void) {

				// Declare WatchDog Variable
				uint8_t _WD = 0;

				// Connect Modem Parameters
				while (!bitRead(this->Status, PostMan_Status_Connection)) {

					// Control for Modem Power Monitor
					if (bitRead(this->Status, PostMan_Status_Initialize)) {

						// Set Control Variable
						bitSet(this->Status, PostMan_Status_Connection);

						// CREG Command (Get Network Registration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CREG?"));

							// Set Connection WatchDog
							bool _Conn_WD = false;
							uint8_t _Conn_WD_Counter = 0;

							// Wait for Connection
							while (!_Conn_WD) {

								// Declare Variable
								uint8_t _CREG_Connection_Mode = 99;
								uint8_t _CREG_Connection_Stat = 99;

								// Get CREG Status
								LE910C1_EUX::CREG(GET, _CREG_Connection_Mode, _CREG_Connection_Stat);

								// Control for Connection
								if (_CREG_Connection_Stat == 0) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5000; i++) {

										// Calculate Connection Time
										this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

										// Control for Terminal State
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Print Connection Time
											Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

										}

										// Connection Wait Delay
										delay(1);

									}

								} else if (_CREG_Connection_Stat == 1) {

									// Set Variable
									bitSet(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = true;

								} else if (_CREG_Connection_Stat == 2) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5000; i++) {

										// Calculate Connection Time
										this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

										// Control for Terminal State
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Print Connection Time
											Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

										}

										// Connection Wait Delay
										delay(1);

									}

								} else if (_CREG_Connection_Stat == 3) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 3000; i++) {

										// Calculate Connection Time
										this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

										// Control for Terminal State
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Print Connection Time
											Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

										}

										// Connection Wait Delay
										delay(1);

									}

								} else if (_CREG_Connection_Stat == 4) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5000; i++) {

										// Calculate Connection Time
										this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

										// Control for Terminal State
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Print Connection Time
											Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

										}

										// Connection Wait Delay
										delay(1);

									}

								} else if (_CREG_Connection_Stat == 5) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = true;

								}

								// Calculate Connection Time
								this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

								// Control for Terminal State
								if (bitRead(this->Status, PostMan_Status_Terminal)) {

									// Print Connection Time
									Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								}

								// Set WD Variable
								_Conn_WD_Counter++;

								// Control for WD
								if (_Conn_WD_Counter > 300) _Conn_WD = true;

							}

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// CGDCONT Command (Set PDP Context)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CGDCONT=1,IP,mgbs"));

							// Send Command
							if (!LE910C1_EUX::CGDCONT(1, "IP", _PostMan_APN_)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SGACT Command (Activate PDP Context)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SGACT=1,1"));

							// Send Command
							if (!LE910C1_EUX::SGACT(1, 1, this->Operator.IP_Address)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Define IP char Array
								char _IP_Char[16];

								// Set IP char Array
								sprintf(_IP_Char, "%03d.%03d.%03d.%03d", this->Operator.IP_Address[0], this->Operator.IP_Address[1], this->Operator.IP_Address[2], this->Operator.IP_Address[3]);

								// Print IP Address
								Terminal->Text(16, 64, _Console_GRAY_, _IP_Char);

							}

						} else break;

						// WS46 Command (Connection Status)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+WS46?"));

							// Send Command
							if (!LE910C1_EUX::WS46(GET, this->Operator.WDS)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Connection Status
								Terminal->Show_Connection_Type(2, 116, this->Operator.WDS);

							}

						} else break;

						// RFSTS Command (Get Signal Quality)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+WS46?"));

							// Send Command
							if (!LE910C1_EUX::RFSTS(this->Operator.MCC, this->Operator.MNC, this->Operator.RSSI, this->Operator.Signal)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Operator Value
								char _Operator[6];
								sprintf(_Operator, "%03d%02d", this->Operator.MCC, this->Operator.MNC);
								Terminal->Text(15, 74, _Console_GRAY_, String(_Operator));

								// Print Signal Level Value
								Terminal->Text(14, 75, _Console_GRAY_, this->Operator.RSSI);

								// Print Signal Quality
								Terminal->Show_Signal_Quality(2, 110, this->Operator.Signal);

							}

						} else break;

						// SCFG (Send Port) Command (Send Data Port Configuration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SCFG=2,1,1500,90,1200,0"));

							// Send Command
							if (!LE910C1_EUX::SCFG(_PostMan_Outgoing_Socket_, 1, 1500, 90, 1200, 0)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SCFGEXT (Send Port) Command (Send Data Port Extended Configuration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SCFGEXT=3,1,0,0,0,0"));

							// Send Command
							if (!LE910C1_EUX::SCFGEXT(_PostMan_Outgoing_Socket_, 1, 0, 0, 0, 0)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SCFGEXT2 (Send Port) Command (Send Data Port Extended 2 Configuration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SCFGEXT2=3,1,0,0,0,0"));

							// Send Command
							if (!LE910C1_EUX::SCFGEXT2(_PostMan_Outgoing_Socket_, 1, 0, 0)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SCFG (In Port) Command (In Port Configuration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SCFG=2,1,1500,90,300,50"));

							// Send Command
							if (!LE910C1_EUX::SCFG(_PostMan_Incomming_Socket_, 1, 1500, 90, 1200, 50)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SCFGEXT (In Port) Command (In Port Extended Configuration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SCFGEXT=2,1,0,1,0,0"));

							// Send Command
							if (!LE910C1_EUX::SCFG(_PostMan_Incomming_Socket_, 1, 1500, 90, 1200, 50)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							} else break;

						} else break;

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(21, 84, _Console_WHITE_, F("                                    "));

						// Connection Success
						return(true);

					} else {

						// Power ON GSM Module
						this->Initialize();

					}

					// Handle WatchDog
					if (_WD > 2) {

						// Initialize Variables
						this->Clear_Variables();

						// End Function
						return(false);

					} else {

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) {

							// Print Message
							Terminal->Show_Message(_Console_YELLOW_, F("Retry Connection..."));

						}

						// Set WatchDog Variable
						_WD++;

					}

				}

				// Not Initialized
				return(false);

			}

			// Set Firewall Configuration
			bool Firewall(void) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Declare Variable
					bool _Firewall = false;

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#FRWL=2"));

					// FRWL Clear (Firewall Configuration)
					_Firewall = LE910C1_EUX::FRWL(SET, 2, _PostMan_Firewall_1_);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// AT Command
						Terminal->AT_Command(F("AT#FRWL=1,***"));

					}

					// FRWL Command 1 (Firewall Configuration)
					_Firewall = LE910C1_EUX::FRWL(SET, 1, _PostMan_Firewall_1_);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// AT Command
						Terminal->AT_Command(F("AT#FRWL=1,***"));

					}

					// FRWL Command 2 (Firewall Configuration)
					_Firewall = LE910C1_EUX::FRWL(SET, 1, _PostMan_Firewall_2_);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// AT Command
						Terminal->AT_Command(F("AT#FRWL=1,***"));

					}

					// FRWL Command 3 (Firewall Configuration)
					_Firewall = LE910C1_EUX::FRWL(SET, 1, _PostMan_Firewall_3_);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// AT Command
						Terminal->AT_Command(F("AT#FRWL=1,***"));

					}

					// ICMP Command (Ping Configuration)
					_Firewall = LE910C1_EUX::ICMP(1);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// Clear Message
						Terminal->Text(21, 84, _Console_WHITE_, F("                                    "));

					}

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Set Clock Configuration
			bool Syncronize_Time(void) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CCLK"));

					// CCLK Command (Real Time Clock Configuration)
					bool _Clock_State = LE910C1_EUX::CCLK(this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second, this->Time.Time_Zone);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_Clock_State);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Syncronizing Time..."));

					// Set RTC Clock
					Hardware->Set_Time(this->Time.Second, this->Time.Minute, this->Time.Hour, this->Time.Day, this->Time.Month, this->Time.Year);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Time Syncronized"));

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Configure Socket
			bool Listen(const bool _State) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Reset Socket Variable
					this->Socket_Status = _SOCKET_CLOSED_;

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SS=2"));

					// Send Command
					bool _Socket_State = LE910C1_EUX::SS(_PostMan_Incomming_Socket_, this->Socket_Status);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_Socket_State);

					// Activate Socket
					if (_State and this->Socket_Status != _SOCKET_LISTENING_) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SL=*,1,80,255"));

						// Activate Socket for Listen
						bool _SL_Command = LE910C1_EUX::SL(_PostMan_Incomming_Socket_, 1, 80, 255);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_SL_Command);

						// Command Delay
						delay(50);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SS=*"));

						// Get Socket Status
						bool _SS_Command = LE910C1_EUX::SS(_PostMan_Incomming_Socket_, this->Socket_Status);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_SS_Command);

						// Print Socket State
						if (this->Socket_Status == _SOCKET_CLOSED_) Terminal->Text(18, 64, _Console_RED_, F("    Closed     "));
						else if (this->Socket_Status == _SOCKET_ACTIVE_) Terminal->Text(18, 64, _Console_GRAY_, F("Active Transfer"));
						else if (this->Socket_Status == _SOCKET_SUSPENDED_) Terminal->Text(18, 64, _Console_GRAY_, F("   Suspended   "));
						else if (this->Socket_Status == _SOCKET_PENDING_) Terminal->Text(18, 64, _Console_GRAY_, F("  Pending Data "));
						else if (this->Socket_Status == _SOCKET_LISTENING_) Terminal->Text(18, 64, _Console_GRAY_, F("   Listening   "));

						// Control Socket
						if (this->Socket_Status != _SOCKET_LISTENING_) return(false);

						// End Function
						return(true);

					}

					// DeActivate Socket
					if (!_State and this->Socket_Status != _SOCKET_CLOSED_) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SL=*,0,80,255"));

						// Activate Socket for Listen
						bool _SL_Command = LE910C1_EUX::SL(_PostMan_Incomming_Socket_, 0, 80, 255);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_SL_Command);

						// Command Delay
						delay(50);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SS=*"));

						// Get Socket Status
						bool _SS_Command = LE910C1_EUX::SS(_PostMan_Incomming_Socket_, this->Socket_Status);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_SS_Command);

						// Print Socket State
						if (this->Socket_Status == _SOCKET_CLOSED_) Terminal->Text(18, 64, _Console_RED_, F("    Closed     "));
						else if (this->Socket_Status == _SOCKET_ACTIVE_) Terminal->Text(18, 64, _Console_GRAY_, F("Active Transfer"));
						else if (this->Socket_Status == _SOCKET_SUSPENDED_) Terminal->Text(18, 64, _Console_GRAY_, F("   Suspended   "));
						else if (this->Socket_Status == _SOCKET_PENDING_) Terminal->Text(18, 64, _Console_GRAY_, F("  Pending Data "));
						else if (this->Socket_Status == _SOCKET_LISTENING_) Terminal->Text(18, 64, _Console_GRAY_, F("   Listening   "));

						// Control Socket
						if (this->Socket_Status != _SOCKET_CLOSED_) return(false);

						// End Function
						return(true);

					}

				}

				// End Function
				return(false);

			}

			// Send Request Response Function
			bool Response(const uint16_t _Response_Code) {

				// Declare Buffer Array
				char _Buffer[17];

				// {"Response":200}

				// Generate Response
				sprintf(_Buffer, "{\"Response\":%03u}", _Response_Code);

				// Print Message
				if (bitRead(this->Status, PostMan_Status_Terminal)) {

					// Print Message
					Terminal->Show_Message(_Console_BLUE_, F("Sending Response..."));

					// Clear Line
					Terminal->Space(_Console_AT_Status_X_, _Console_AT_Status_Y_, sizeof(_Buffer));

					// Print Command
					Terminal->Text(_Console_AT_Status_X_, _Console_AT_Status_Y_, _Console_WHITE_, _Buffer);

				}

				// Send Answer
				if (LE910C1_EUX::SSEND(_PostMan_Incomming_Socket_, HTTP_RESPONSE, _PostMan_Server_, _PostMan_EndPoint_, _Buffer)) {

					// Command Delay
					delay(10);

					// Closing Socket
					if (LE910C1_EUX::SH(_PostMan_Incomming_Socket_)) {

						// Command Delay
						delay(10);

						// Control for Incoming Call
						this->Listen(true);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Space(21, 84, 36);

						// End Function
						return(true);

					} else {

						// End Function
						return(false);

					}

				}

				// End Function
				return(false);

			}

			// Download Firmware Function
			bool Download(const uint16_t _Firmware_ID) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Set LED Blue
					PORT_MCU_LED_BLUE |= (1 << PIN_MCU_LED_BLUE);

					// Enable SD Multiplexer
					Hardware->SD_Multiplexer(true);

					// Declare Variables
					uint16_t _FOTA_Download_Time = 0;
					uint32_t _FOTA_Download_Size = 0;
					uint32_t _FOTA_File_Size = 0;
					uint8_t _FOTA_Download_Status = 0;

					// Define ETag Variable
					char _FOTA_Server_ETag[33];
					memset(_FOTA_Server_ETag, '\0', sizeof(_FOTA_Server_ETag));

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Connecting FOTA Server..."));

					// Declare SD File
					File FOTA_File;

					// Declare MD5 Object
					MD5 MD5_Hash;

					// Delay
					delay(100);

					// Control for Existing File
					if (Hardware->exists(_PostMan_Firmware_Name_)) {

						// Delete Existing File
						Hardware->remove(_PostMan_Firmware_Name_);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Deleting Existing Firmware File..."));

						// Command Delay
						delay(100);

					} else {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Creating Firmware File..."));

					}

					// Open File for Write
					FOTA_File = Hardware->open(_PostMan_Firmware_Name_, O_WRITE | O_CREAT);

					// Command Delay
					delay(100);

					// Control for File Open
					if (FOTA_File) {

						// Set Download Start Time
						uint32_t _FOTA_Download_Start_Time = millis();

						// Set Socket Configuration for Download
						LE910C1_EUX::SCFGEXT(_PostMan_Outgoing_Socket_, 0, 0, 0, 0, 0);

						// Open Socket for Download
						if (LE910C1_EUX::ATSD(_PostMan_Outgoing_Socket_, TCP, _PostMan_Server_, _PostMan_Port_, _CONNECTION_MANUAL_CLOSE_, 88, _CONNECTION_COMMAND_)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Firmware Downloading..."));

							// Set Download Path
							char _Download_Path[16];
							sprintf(_Download_Path, "/Firmware/%d", _Firmware_ID);

							// Send Download Request
							if (LE910C1_EUX::SSEND(_PostMan_Outgoing_Socket_, HTTP_GET, _PostMan_Server_, _Download_Path, "")) {

								// Get Ring
								if (LE910C1_EUX::SRING()) {

									// Define Response Buffer Size
									const uint16_t _Response_Buffer_Size = 550;

									// Declare Buffer Variable
									char Buffer_Variable[_Response_Buffer_Size];
									char Data_Variable[_Response_Buffer_Size];

									// Clear Buffer Variable
									memset(Buffer_Variable, '\0', _Response_Buffer_Size);
									memset(Data_Variable, '\0', _Response_Buffer_Size);

									// Get Request Data Head
									bool _SRECV = LE910C1_EUX::SRECV(_PostMan_Outgoing_Socket_, (_Response_Buffer_Size - 150), Buffer_Variable);

									// Handle Response
									if (_SRECV) {

										// \r\n#SRECV: 2,400\r\n
										// HTTP/1.1 200 OK\r\n
										// Server: nginx/1.25.3\r\n
										// Date: Thu, 18 Jan 2024 12:55:03 GMT\r\n
										// Content-Type: application/octet-stream\r\n
										// Content-Length: 321838\r\n
										// Connection: keep-alive\r\n
										// content-disposition: attachment; filename="PS_041023.hex"\r\n
										// last-modified: Fri, 15 Dec 2023 11:46:38 GMT\r\n
										// etag: 43720fe3f56006394f3414b9ca9164f9\r\n\r\n
										// :100000000C947E0A0C94AF0A0C94AF0A0C94AF0ABD\r\n
										// :100010000C94AF0A0C94AF0A0C94AF0A0C94AF0A7C\r
										// \r\n\r\nOK\r\n
										// \r\nSRING: 2\r\n

										// Declare Start Position Variable
										uint16_t _Start_Position = 0;
										uint16_t _End_Position = 0;

										// Find Start Position
										for (uint16_t i = 5; i < _Response_Buffer_Size; i++) {

											// Find Start Position
											if (Buffer_Variable[i-4] == '\r' and Buffer_Variable[i-3] == '\n' and Buffer_Variable[i-2] == '\r' and Buffer_Variable[i-1] == '\n' and Buffer_Variable[i] == ':') {

												// Set Start Position
												_Start_Position = i;

											}

										}

										// Find End Position
										for (uint16_t i = _Start_Position; i < _Response_Buffer_Size; i++) {

											// Control for i
											if (i > 7) {

												// Find End Position
												if (
													Buffer_Variable[i-7] == '\r' and 
													Buffer_Variable[i-6] == '\n' and 
													Buffer_Variable[i-5] == '\r' and 
													Buffer_Variable[i-4] == '\n' and 
													Buffer_Variable[i-3] == 'O' and 
													Buffer_Variable[i-2] == 'K' and 
													Buffer_Variable[i-1] == '\r' and 
													Buffer_Variable[i-0] == '\n'
												) {

													// Set End Position
													_End_Position = i - 7;

												}

											}

										}

										// Parse Data
										for (uint16_t i = _Start_Position; i < _End_Position; i++) {

											// Parse Data
											Data_Variable[i - _Start_Position] = Buffer_Variable[i];

											// Control for HEX Char
											if (Data_Variable[i - _Start_Position] == 'A' || Data_Variable[i - _Start_Position] == 'B' || Data_Variable[i - _Start_Position] == 'C' || Data_Variable[i - _Start_Position] == 'D' || Data_Variable[i - _Start_Position] == 'E' || Data_Variable[i - _Start_Position] == 'F' || Data_Variable[i - _Start_Position] == '0' || Data_Variable[i - _Start_Position] == '1' || Data_Variable[i - _Start_Position] == '2' || Data_Variable[i - _Start_Position] == '3' || Data_Variable[i - _Start_Position] == '4' || Data_Variable[i - _Start_Position] == '5' || Data_Variable[i - _Start_Position] == '6' || Data_Variable[i - _Start_Position] == '7' || Data_Variable[i - _Start_Position] == '8' || Data_Variable[i - _Start_Position] == '9' || Data_Variable[i - _Start_Position] == '\r' || Data_Variable[i - _Start_Position] == '\n' || Data_Variable[i - _Start_Position] == ':') {

												// Calculate Size
												_FOTA_Download_Size += 1;

											}

										}

										// Declare Variables
										uint16_t _FOTA_GET_Status = HTTP_Unknown;

										// Get First Line
										char* _Response_Line = strtok(Buffer_Variable, "\r\n");

										// Control for Response Line
										while (_Response_Line != NULL) {

											// Control for Response
											if (strstr(_Response_Line, "Content-Length:") != NULL) {

												// Content-Length: 321838\r\n

												// Get Content Length
												sscanf(_Response_Line, "Content-Length: %lu", &_FOTA_File_Size);
							
											} else if (strstr(_Response_Line, "etag:") != NULL) {

												// etag: 43720fe3f56006394f3414b9ca9164f9\r\n\r\n

												// Get etag
												sscanf(_Response_Line, "etag: %32[^\r\n]", _FOTA_Server_ETag);

											} else if (strstr(_Response_Line, "HTTP/1.1") != NULL) {

												// HTTP/1.1 200 OK\r\n

												// Get Last Modified
												sscanf(_Response_Line, "HTTP/1.1 %u OK\r\n", &_FOTA_GET_Status);

											} else if (strstr(_Response_Line, "content-disposition:") != NULL) {

												// content-disposition: attachment; filename="PS_041023.hex"\r\n

												// Declare File Name Variable
												char _FOTA_File_Name[14];

												// Get File Name
												sscanf(_Response_Line, "content-disposition: attachment; filename=\"%13[^\"]", _FOTA_File_Name);

												// Print File Name
												if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(13, 106, _Console_GRAY_, _FOTA_File_Name);

											}

											// Get Line
											_Response_Line = strtok(NULL, "\r\n");

										}

										// Control for GET Status
										if (_FOTA_GET_Status == HTTP_OK) {

											// Print Message
											if (bitRead(this->Status, PostMan_Status_Terminal)) {

												// Declare File Size Char Buffer
												char _File_Size_Buffer[8];

												// Set Buffer
												sprintf(_File_Size_Buffer, "%07lu", _FOTA_File_Size);

												// Print File Size
												Terminal->Text(15, 112, _Console_CYAN_, _File_Size_Buffer);

											}

											// Write Data
											if (_End_Position - _Start_Position > 0) FOTA_File.write(Data_Variable, (_End_Position - _Start_Position));

											// MD5 Checksum
											MD5_Hash.Update(Data_Variable, (_End_Position - _Start_Position));

											// SD Print Delay
											delay(100);

											// Print Message
											if (bitRead(this->Status, PostMan_Status_Terminal)) {

												// Declare Download Size Char Buffer
												char _Download_Size_Buffer[8];

												// Set Buffer
												sprintf(_Download_Size_Buffer, "%07lu", _FOTA_Download_Size);

												// Print Download Size
												Terminal->Text(16, 112, _Console_CYAN_, _Download_Size_Buffer);

												// Declare Percent Char Buffer
												char _Download_Time_Buffer[10];

												// Calculate Time
												_FOTA_Download_Time = (millis() - _FOTA_Download_Start_Time) / 1000;

												// Set Buffer
												sprintf(_Download_Time_Buffer, "%04u", _FOTA_Download_Time);

												// Print Download Time
												Terminal->Text(18, 111, _Console_CYAN_, String(_Download_Time_Buffer));

											}

										} else {

											// Print Message
											if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Server Error..."));

											// End Function
											return(false);

										}
	
									}

									// Declare WD Variable
									uint8_t _WD = 0;

									// Get Data
									while (_FOTA_Download_Size < _FOTA_File_Size) {

										// Reset Variables
										memset(Buffer_Variable, '\0', sizeof(Buffer_Variable));
										memset(Data_Variable, '\0', sizeof(Data_Variable));

										// Handle Response
										if (LE910C1_EUX::SRECV(_PostMan_Outgoing_Socket_, (_Response_Buffer_Size - 100), Buffer_Variable)) {

											// \r\n#SRECV: 2,400\r\n
											// \n
											// :100020000C94AF0A0C94FC5F0C9434600C944E60FA\r\n
											// :100030000C94AF0A0C9415C70C94AF0A0C94AF0A39\r\n
											// :100040000C94AF0A0C94AF0A0C94AF0A0C94AF0A4C\r\n
											// :100050000C94AF0A0C94AF0A0C94AF0A0C943BCAF0\r\n
											// :100060000C94AF0A0C94A9BF0C94DFBF0C94AF0A98\r\n
											// :100070000C94AF0A0C94AF0A0C94AF0A0C94AF0A1C\r\n
											// :100080000C94AF0A0C94AF0A0C94AF0A0C94AF0A0C\r\n
											// :100090000C9440C00C9476C00C94AF0A0C9426BD0E\r\n
											// :1000A0000C94AF0A0C94AF0A0C94AF0A0C94AF
											// \r\n\r\nOK\r\n
											// \r\nSRING: 2\r\n

											// Declare Start Position Variable
											uint16_t _Start_Position = 0;
											uint16_t _End_Position = 0;

											// Find Start Position
											if (Buffer_Variable[0] == '\r' and Buffer_Variable[1] == '\n' and Buffer_Variable[15] == '\r' and Buffer_Variable[16] == '\n') {
												
												// Set Start Position
												_Start_Position = 17;

											} else if (Buffer_Variable[0] == '\r' and Buffer_Variable[1] == '\n' and Buffer_Variable[14] == '\r' and Buffer_Variable[15] == '\n') {

												// Set Start Position
												_Start_Position = 16;

											} else if (Buffer_Variable[0] == '\r' and Buffer_Variable[1] == '\n' and Buffer_Variable[13] == '\r' and Buffer_Variable[14] == '\n') {

												// Set Start Position
												_Start_Position = 15;

											}

											// Find End Position
											for (uint16_t i = _Start_Position; i < _Response_Buffer_Size; i++) {

												// Control for i
												if (i > 7) {

													// Find End Position
													if (
														Buffer_Variable[i-7] == '\r' and 
														Buffer_Variable[i-6] == '\n' and 
														Buffer_Variable[i-5] == '\r' and 
														Buffer_Variable[i-4] == '\n' and 
														Buffer_Variable[i-3] == 'O' and 
														Buffer_Variable[i-2] == 'K' and 
														Buffer_Variable[i-1] == '\r' and 
														Buffer_Variable[i-0] == '\n'
													) {

														// Set End Position
														_End_Position = i - 7;

													}

												}

											}

											// Parse Data
											for (uint16_t i = _Start_Position; i < _End_Position; i++) {

												// Parse Data
												Data_Variable[i - _Start_Position] = Buffer_Variable[i];

												// Control for HEX Char
												if (Data_Variable[i - _Start_Position] == 'A' || Data_Variable[i - _Start_Position] == 'B' || Data_Variable[i - _Start_Position] == 'C' || Data_Variable[i - _Start_Position] == 'D' || Data_Variable[i - _Start_Position] == 'E' || Data_Variable[i - _Start_Position] == 'F' || Data_Variable[i - _Start_Position] == '0' || Data_Variable[i - _Start_Position] == '1' || Data_Variable[i - _Start_Position] == '2' || Data_Variable[i - _Start_Position] == '3' || Data_Variable[i - _Start_Position] == '4' || Data_Variable[i - _Start_Position] == '5' || Data_Variable[i - _Start_Position] == '6' || Data_Variable[i - _Start_Position] == '7' || Data_Variable[i - _Start_Position] == '8' || Data_Variable[i - _Start_Position] == '9' || Data_Variable[i - _Start_Position] == '\r' || Data_Variable[i - _Start_Position] == '\n' || Data_Variable[i - _Start_Position] == ':') {

													// Calculate Size
													_FOTA_Download_Size += 1;

												}

											}

											// Write Data
											if (_End_Position - _Start_Position > 0) FOTA_File.write(Data_Variable, (_End_Position - _Start_Position));

											// MD5 Checksum
											MD5_Hash.Update(Data_Variable, (_End_Position - _Start_Position));

										} 

										// Control for CME ERROR 4
										// \r\n+CME ERROR: 4\r\n
										if (strstr(Buffer_Variable, "+CME ERROR: 4") != NULL) {

											// Increase WD
											_WD++;

										} else if (strstr(Buffer_Variable, "CME ERROR: 551") != NULL) {

											// \r\n+CME ERROR: 551\r\n

											// Print Message
											if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Firmware Download Error..."));

											// Set Status
											_FOTA_Download_Status = FOTA_Download_CME_Error;

											// End Function
											return(false);

										}

										// Print Message
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Declare Download Size Char Buffer
											char _Download_Size_Buffer[8];

											// Set Buffer
											sprintf(_Download_Size_Buffer, "%07lu", _FOTA_Download_Size);

											// Print Download Size
											Terminal->Text(16, 112, _Console_CYAN_, _Download_Size_Buffer);

											// Declare Percent Char Buffer
											char _Download_Percent_Buffer[8];

											// Calculate Percent
											uint8_t _Percent = (uint8_t)((_FOTA_Download_Size * 100) / _FOTA_File_Size);

											// Set Buffer for Percent (2 Digit Float)
											sprintf(_Download_Percent_Buffer, "%hu", _Percent);

											// Print Download Percent
											Terminal->Text(17, 114, _Console_CYAN_, _Download_Percent_Buffer);

											// Declare Percent Char Buffer
											char _Download_Time_Buffer[10];

											// Calculate Time
											_FOTA_Download_Time = (millis() - _FOTA_Download_Start_Time) / 1000;

											// Set Buffer
											sprintf(_Download_Time_Buffer, "%04u", _FOTA_Download_Time);

											// Print Download Time
											Terminal->Text(18, 111, _Console_CYAN_, String(_Download_Time_Buffer));

										}

										// Control for WD
										if (_WD > 100) {

											// Print Message
											if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("Firmware Download TimeOut..."));

											// Set Status
											_FOTA_Download_Status = FOTA_Download_TimeOut;

											// End While	
											break;

										}

										// Heart Beat
										Hardware->Heartbeat();

									}

								} else {

									// Set Status
									_FOTA_Download_Status = FOTA_Server_Error;

									// Print Message
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Server Error..."));

									// End Function
									return(false);

								}

							}

							// Closing Socket
							if (LE910C1_EUX::SH(_PostMan_Outgoing_Socket_)) this->Listen(true);

						} else {

							// Set Status
							_FOTA_Download_Status = FOTA_Server_Error;

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Server Error..."));

							// End Function
							return(false);

						}

						// Set Download Duration
						_FOTA_Download_Time = (millis() - _FOTA_Download_Start_Time) / 1000;

						// Set Socket Configuration for Stream
						LE910C1_EUX::SCFGEXT(_PostMan_Outgoing_Socket_, 1, 0, 0, 0, 0);

						// Work Delay
						delay(8);

						// Close SD File
						FOTA_File.close();

						// Disable LED Blue
						PORT_MCU_LED_BLUE &= ~(1 << PIN_MCU_LED_BLUE);

						// Disable SD Multiplexer
						Hardware->SD_Multiplexer(false);

						// Calculate MD5
						uint8_t _Hash[16];
						MD5_Hash.Finalize(_Hash);

						// MD5 to Char Array
						char _Etag[33];
						MD5::Digest(_Hash, _Etag);

						// Compare MD5 With ETag
						if (strcmp(_Etag, _FOTA_Server_ETag) == 0) {

							// Set Status
							_FOTA_Download_Status = FOTA_Download_OK;

							// Add Variable
							this->Payload->Add(1, _Firmware_ID);
							this->Payload->Add(2, _FOTA_Download_Status);

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Firmware Download Success!!"));

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(46, 4, _Console_GRAY_, _Etag);

							// End Function
							return(true);

						} else {

							// Set Status
							_FOTA_Download_Status = FOTA_Download_MD5_Error;

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Firmware MD5 Error!!"));

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(46, 4, _Console_GRAY_, _Etag);

							// End Function
							return(false);

						}

						// End Function
						return(false);

					} else {

						// Set Status
						_FOTA_Download_Status = FOTA_SD_Error;

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("Firmware File Open Failed!!"));

						// Disable SD Multiplexer
						Hardware->SD_Multiplexer(false);

						// End Function
						return(false);

					}

				}

				// Disable SD Multiplexer
				Hardware->SD_Multiplexer(false);

				// End Function
				return(false);

			}

			// Control Firmware File Function
			bool Calculate_MD5_Hash(char * _Hash, const char * _FileName) {

				// Set LED Blue
				PORT_MCU_LED_BLUE |= (1 << PIN_MCU_LED_BLUE);

				// Enable SD Multiplexer
				Hardware->SD_Multiplexer(true);

				// Declare MD5 Object
				MD5 MD5_Hash;

				// Declare SD File
				File Hash_File;

				// Control for Existing File
				if (Hardware->exists(_FileName)) {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Opening Firmware File..."));

					// Open File for Read
					Hash_File = Hardware->open(_PostMan_Firmware_Name_, O_READ);

					// Command Delay
					delay(100);

					// Control for File Open
					if (Hash_File) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Calculating MD5 Hash..."));

						// Read File
						while (Hash_File.available() > 0) {


							Terminal->Beep();

							// Read 10 Byte Data
							char _Data[10];
							Hash_File.readBytes(_Data, 9);

							// MD5 Checksum
							MD5_Hash.Update(_Data, 9);

						}

						// Close File
						Hash_File.close();

						// Calculate MD5
						uint8_t _Calculated_Hash[16];
						MD5_Hash.Finalize(_Calculated_Hash);

						// MD5 to Char Array
						MD5_Hash.Digest(_Calculated_Hash, _Hash);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, _Hash);

						// Set LED Off
						PORT_MCU_LED_BLUE &= ~(1 << PIN_MCU_LED_BLUE);

						// Disable SD Multiplexer
						Hardware->SD_Multiplexer(false);

						// End Function
						return(true);

					} else {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Can not Open Firmware File..."));

					}

				} else {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Firmware File Not Found..."));

				}

				// Set LED Off
				PORT_MCU_LED_BLUE &= ~(1 << PIN_MCU_LED_BLUE);

				// Disable SD Multiplexer
				Hardware->SD_Multiplexer(false);

				// End Function
				return(false);

			}

		// Public Context
		public:

			// Define Instance
			static Postman* Instance;

			// Define Variables
			uint8_t Status = 0x00;

			// PCMSK1 Mask Handler Function
			static void Interrupt_Handler_Static(void) {

				// Set Interrupt Handler
				if (Instance) Instance->Interrupt_Handler();

			}
			void Interrupt_Handler(void) {

				// Set Interrupt Handler
				bitSet(this->Status, PostMan_Status_Ring);

			}

			// PostMan Constructor
			Postman(Stream &_Serial, B107AA* _B107AA, Variable<float>* _Variable, PowerStat_Console* _Terminal) : LE910C1_EUX(_Serial), Hardware(_B107AA), Payload(_Variable), Terminal(_Terminal) {

				// Control for Terminal
				if (Terminal != nullptr) {bitSet(this->Status, PostMan_Status_Terminal);} else {bitClear(this->Status, PostMan_Status_Terminal);}

				// Set Instance
				Instance = this;

				// Initialize Variables
				this->Clear_Variables();

			}
			Postman(Stream &_Serial, B107AA* _B107AA, Variable<float>* _Variable) : LE910C1_EUX(_Serial), Hardware(_B107AA), Payload(_Variable), Terminal(nullptr) {

				// Control for Terminal
				if (Terminal != nullptr) {bitSet(this->Status, PostMan_Status_Terminal);} else {bitClear(this->Status, PostMan_Status_Terminal);}

				// Set Instance
				Instance = this;

				// Initialize Variables
				this->Clear_Variables();

			}

			// Cloud Connection Function
			bool Cloud(void) {

				// Heart Beat
				Hardware->Heartbeat();

				// Set Connection Start Time
				this->Operator.Connection_Start = millis();

				// Print Message
				if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Initializing GSM Module..."));

				// Initialize GSM Module
				if (this->Initialize()) {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Connecting GSM Module..."));

					// Connect GSM Module
					if (this->Connect()) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Connection Success!!"));

						// Set Firewall Configuration
						if (this->Firewall()) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Firewall Configuration Success!!"));

						}

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Syncronizing Time..."));

						// Update Time
						if (this->Syncronize_Time()) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Time Syncronized!!"));

						}

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Socket Listen..."));

						// Set Listen Configuration
						if (this->Listen(true)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Socket Listen Success!!"));

						}

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(21, 84, _Console_WHITE_, F("                                    "));

						// Disable Interrupts
						cli();

						// Enable GSM Interrupt
						Hardware->Set_GSM_Interrupt();

						// Enable Interrupts
						sei();

						// End Function
						return(true);

					} else {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("GSM Connection Failed!!"));

					}

				} else {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("GSM Initializing Failed!!"));

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// Get Server Command Function
			void Get(void) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Control for RING
					if (bitRead(this->Status, PostMan_Status_Ring)) {

						// Clear Ring Status
						bitClear(this->Status, PostMan_Status_Ring);

						// Wait for SRING
						if (LE910C1_EUX::SRING()) {

							// Declare JSON Variable
							char _JSON_Data[_PostMan_Recieve_JSON_Size_];

							// Clear JSON Data
							memset(_JSON_Data, '\0', _PostMan_Recieve_JSON_Size_);

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Recieving Message..."));

							// Declare Request Pack Length
							uint16_t _Length = 0;

							// Answer Socket
							LE910C1_EUX::SA(_PostMan_Incomming_Socket_, 1, _Length);

							// Handle Length
							if (_Length == 0 or _Length > _PostMan_Recieve_JSON_Size_) _Length = _PostMan_Recieve_JSON_Size_;

							// Get Request Data
							LE910C1_EUX::SRECV(_PostMan_Incomming_Socket_, _Length, _JSON_Data);

							// Declare Handle Variable
							bool Data_Handle = false;

							// Declare JSON Variable
							char _Data[_Length];

							// Clear JSON Data
							memset(_Data, '\0', _Length);

							// Declare Data Order
							uint16_t _Data_Order = 0;

							// Control for Buffer
							for (uint16_t i = 2; i < _PostMan_Recieve_JSON_Size_; i++) {

								// Handle JSON Data
								if (_JSON_Data[i] == '{') Data_Handle = true;

								// Get Data
								if (Data_Handle) {

									// Handle for Space
									if (_JSON_Data[i] != ' ' and _JSON_Data[i] != '\n' and _JSON_Data[i] != '\r') {

										// Set Data
										_Data[_Data_Order] = _JSON_Data[i];
										
										// Increase Data Order
										_Data_Order += 1;

									}

								}

								// Handle JSON Data
								if (_JSON_Data[i-2] == '}' and _JSON_Data[i-1] == '\r' and _JSON_Data[i] == '\n') Data_Handle = false;

							}

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, _Data);




							// TODO: Düzenlenecek




							// Send Response
							this->Response(HTTP_OK);


							this->Download(1);
							//this->Calculate_MD5_Hash(_Data, _PostMan_Firmware_Name_);




							// Port Control
							this->Listen(true);


						}

					}

				}

			}

	};

	// Set Static Instance
	Postman* Postman::Instance = nullptr;

	// Interrupt Routine PCMSK1 [PCINT11]
	ISR(PCINT1_vect) {

		// PCMSK1 Handler
		Postman::Interrupt_Handler_Static();

	}

#endif /* defined(__PostMan__) */