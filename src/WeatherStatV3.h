// Library include guard
#pragma once

// Include Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Include Console Library
#ifdef DEBUG

	// Define Console Library
	#ifndef __Console__
		#include <Console.h>
	#endif

#endif

// Include WeatherStat V3 Hardware Functions
#include "Hardware/B108AA.h"

// Include AT Command Library
#if defined(LE910C1_EUX)

	// Include LE910C1-EU AT Command Library
	#include "AT_Command/LE910C1_EUX.h"

#else if defined(LE910S1_EAG)

	// Include LE910S1-EAG AT Command Library
	#include "AT_Command/LE910S1_EAG.h"

#endif

// Include Product Configurations
#if defined(B107AA)

	// Include PowerStat V4 Configurations
	#include "Config/PowerStat_Definitions.h"

#endif

// Cloud Functions
class Postman_WeatherStatV3 : private AT_Command_Set, private Console, private Hardware {

	// Private Functions
	private:

		// Define IoT Status Structure
		struct Struct_Status {
			bool		SIM_Inserted		= false;
			uint8_t		SIM_PIN				= 0;
			bool 		Initialize		 	= false;
			bool		Connection			= false;
			uint8_t		Socket_State		= 0;
		} IoT_Status;

		// Define IoT Module Structure
		struct Struct_Module {

			// Modem IMEI
			char 		IMEI[17];

			// GSM Serial ID
			uint32_t	Serial_ID 			= 0;

			// Manufacturer and Model
			uint8_t 	Manufacturer 		= 0;
			uint8_t 	Model 				= 0;

			// Modem Firmware Version
			char 		Firmware[15];

		} IoT_Module;

		// Define IoT Operator Structure
		struct Struct_Network {

			// SIM ICCID
			char 		ICCID[21];

			// Operator
			uint16_t 	MCC 				= 0;
			uint16_t	MNC					= 0;
			uint16_t	WDS					= 0;

			// Signal Level
			uint16_t	RSSI				= 0;
			uint8_t		Signal				= 0;

			// Location
			uint16_t	TAC					= 0;
			uint32_t	Cell_ID				= 0;

			// IP Address
			char 		IP_Address[16];

			// Connection Time
			uint8_t 	Connection_Time;

		} IoT_Operator;

		// Define Time Structure
		struct Struct_Time {
			uint16_t 	Year				= 0;
			uint16_t 	Month				= 0;
			uint16_t 	Day					= 0;
			uint16_t 	Hour				= 0;
			uint16_t 	Minute				= 0;
			uint16_t 	Second				= 0;
			uint16_t	Time_Zone			= 0;
		} Time;

		// Define Runtime Buffer Structure
		struct Struct_Buffer {

			// Connection Time
			uint32_t 	Connection_Time_Buffer	= 0;

			// Define JSON
			char 		JSON_Pack[Send_JSON_Size];

		} Buffer;

		// Define CallBack Functions
		void (*_Environment_CallBack)(float&, float&);
		void (*_Battery_CallBack)(float&, float&, float&, float&, uint16_t&, uint16_t&, uint8_t&);
		void (*_Send_Data_CallBack)(uint8_t);
		void (*_Send_Response_CallBack)(uint16_t, uint8_t);
		void (*_Command_CallBack)(uint16_t, char*);

		// Initialize GSM Modem
		bool Initialize(void) {

			// Declare WatchDog Variable
			uint8_t _WD = 0;

			// Initialize Modem Parameters
			while (!this->IoT_Status.Initialize) {

				// Control for Power Monitor
				if (Hardware::PowerMonitor()) {

					// Print Batch Description
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, CYAN, F("Initializing Modem"));
						Console::Text(14, 34, WHITE, F("[    ]"));
					#endif

					// Set Control Variable
					this->IoT_Status.Initialize = true;

					// AT Command
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT"));
						#endif

						// Send Command
						if (!AT_Command_Set::AT()) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// ATE Command (Echo Off)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("ATE0"));
						#endif

						// Send Command
						if (!AT_Command_Set::ATE(false)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SIMDET Command (SIM Card Detect)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+SIMDET?"));
						#endif

						// Send Command
						if (!AT_Command_Set::SIMDET(GET, 0, this->IoT_Status.SIM_Inserted)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Initialize, 14, 35);

							// Print SIMDET State
							if (this->IoT_Status.SIM_Inserted) Console::Text(14, 44, GREEN, F("SIM Card Detected    "));
							if (!this->IoT_Status.SIM_Inserted) Console::Text(14, 44, RED, F("SIM Card Not Detected"));

						#endif

						// No SIM Card
						if (!this->IoT_Status.SIM_Inserted) return(false);

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// CFUN Command (Full Functionality)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+CFUN=1,0"));
						#endif

						// Send Command
						if (!AT_Command_Set::CFUN(1)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// CMEE Command (Error Messages)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+CMEE=1"));
						#endif

						// Send Command
						if (!AT_Command_Set::CMEE(1)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// FCLASS Command (Connection Mode)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+FCLASS=0"));
						#endif

						// Send Command
						if (!AT_Command_Set::FCLASS(0)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// ATK Command (No Flow Control)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT&K0"));
						#endif

						// Send Command
						if (!AT_Command_Set::K(0)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// CPIN Command (SIM PIN Control)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+CPIN?"));
						#endif

						// Send Command
						if (!AT_Command_Set::CPIN(this->IoT_Status.SIM_PIN)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// End Function
						if (this->IoT_Status.SIM_PIN != 1) break;

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// CGSN Command (Get IMEI)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+CGSN"));
						#endif

						// Send Command
						if (!AT_Command_Set::CGSN(this->IoT_Module.IMEI)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Initialize, 14, 35);
							
							// Print IMEI
							Console::Text(20, 24, CYAN, String(this->IoT_Module.IMEI));

						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// ICCID Command (Get SIM Card ID)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+CCID"));
						#endif

						// Send Command
						if (!AT_Command_Set::CCID(this->IoT_Operator.ICCID)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Initialize, 14, 35);
							
							// Print ICCID
							Console::Text(22, 20, CYAN, String(this->IoT_Operator.ICCID));

						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// CGMI Command (Get Manufacturer)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+GMI"));
						#endif

						// Send Command
						if (!AT_Command_Set::CGMI(this->IoT_Module.Manufacturer)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Initialize, 14,35);
							
							// Print Manufacturer
							Console::Text(17, 38, CYAN, String(this->IoT_Module.Manufacturer));

						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// CGMM Command (Get Model)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+GMM"));
						#endif

						// Send Command
						if (!AT_Command_Set::CGMM(this->IoT_Module.Model)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Initialize, 14, 35);
							
							// Print Model
							Console::Text(18, 38, CYAN, String(this->IoT_Module.Model));

						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// CGMR Command (Get Firmware Version)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+GMR"));
						#endif

						// Send Command
						if (!AT_Command_Set::SWPKGV(this->IoT_Module.Firmware)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Initialize, 14, 35);

							// Print Firmware Version
							Console::Text(19, 30, CYAN, String(this->IoT_Module.Firmware));

						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// E2RI Command (Set RING Indicator)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#E2RI=50,50"));
						#endif

						// Send Command
						if (!AT_Command_Set::E2SLRI(50)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Initialize, 14, 35);

						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// GPIO Command (Set Status LED)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#GPIO=1,0,2"));
						#endif

						// Send Command
						AT_Command_Set::GPIO(SET, 1, 0, 2);

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SLED Command (Set Status LED)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SLED=2"));
						#endif

						// Send Command
						if (!AT_Command_Set::SLED(2)) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SLEDSAV Command (Save Status LED)
					if (this->IoT_Status.Initialize) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SLEDSAV"));
						#endif

						// Send Command
						if (!AT_Command_Set::SLEDSAV()) this->IoT_Status.Initialize = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Initialize, 14, 35);
						#endif

						// Print Connection Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// Initialize Complete
					return(true);

				} else {

					// Print Modem Not ON
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, CYAN, F("Power ON Modem"));
					#endif

					// Power ON Modem
					Hardware::ON();

				}

				// Handle WatchDog
				if (_WD > 2) {

					// Print Initializing Failed
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, RED, F("Initializing Failed"));
					#endif

					// Clear States
					this->IoT_Status.Initialize = false;
					this->IoT_Status.SIM_Inserted = false;
					this->IoT_Status.SIM_PIN = 0;

					// Not Initialized
					return(false);

				} else {

					// Print Initializing Failed
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, YELLOW, F("Retry Initializing"));
					#endif

					// Set WatchDog Variable
					_WD++;

				}

			}

			// Not Initialized
			return(false);
			
		}

		// Connect GSM Modem
		bool Connect(void) {

			// Declare WatchDog Variable
			uint8_t _WD = 0;

			// Connect to Internet
			while (!this->IoT_Status.Connection) {

				// Control for Initialize
				if (this->IoT_Status.Initialize) {

					// Print Batch Description
					#ifdef DEBUG

						// Print Description
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, CYAN, F("Connecting"));
						Console::Text(14, 34, WHITE, F("[    ]"));

					#endif

					// Set Control Variable
					this->IoT_Status.Connection = true;

					// Get CREG Command (Get Network Registration Mode)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+CREG?"));
						#endif

						// Set Connection WatchDog
						bool _Conn_WD = false;
						uint8_t _Conn_WD_Counter = 0;

						// Wait for Connection
						while (!_Conn_WD) {

							// Declare Variable
							uint8_t _CREG_Connection_Mode = 99;
							uint8_t _CREG_Connection_Stat = 99;

							// Get CREG Status
							AT_Command_Set::CREG(GET, _CREG_Connection_Mode, _CREG_Connection_Stat);

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 35, CYAN, F("    "));
								Console::Text(14, 36, RED, String(_CREG_Connection_Stat));
							#endif

							// Control for Connection
							if (_CREG_Connection_Stat == 0) {

								// Set Variable
								this->IoT_Status.Connection = false;

								// Declare Response Status
								_Conn_WD = false;

								// Connection Wait Delay
								for (uint8_t i = 0; i < 5; i++) {

									// Print Command State
									#ifdef DEBUG

										// Set Connection Time
										this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
					
										// Print Connection Time
										Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));

									#endif

									// Connection Wait Delay
									delay(1000);

								}

							} else if (_CREG_Connection_Stat == 1) {

								// Set Variable
								this->IoT_Status.Connection = true;

								// Declare Response Status
								_Conn_WD = true;

							} else if (_CREG_Connection_Stat == 2) {

								// Set Variable
								this->IoT_Status.Connection = false;

								// Declare Response Status
								_Conn_WD = false;

								// Connection Wait Delay
								for (uint8_t i = 0; i < 5; i++) {

									// Print Command State
									#ifdef DEBUG

										// Set Connection Time
										this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
					
										// Print Connection Time
										Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));

									#endif

									// Connection Wait Delay
									delay(1000);

								}

							} else if (_CREG_Connection_Stat == 3) {

								// Set Variable
								this->IoT_Status.Connection = false;

								// Declare Response Status
								_Conn_WD = false;

								// Connection Wait Delay
								for (uint8_t i = 0; i < 3; i++) {

									// Print Command State
									#ifdef DEBUG

										// Set Connection Time
										this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
					
										// Print Connection Time
										Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));

									#endif

									// Connection Wait Delay
									delay(1000);

								}

							} else if (_CREG_Connection_Stat == 4) {

								// Set Variable
								this->IoT_Status.Connection = false;

								// Declare Response Status
								_Conn_WD = false;

								// Connection Wait Delay
								for (uint8_t i = 0; i < 5; i++) {

									// Print Command State
									#ifdef DEBUG

										// Set Connection Time
										this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
					
										// Print Connection Time
										Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));

									#endif

									// Connection Wait Delay
									delay(1000);

								}

							} else if (_CREG_Connection_Stat == 5) {

								// Set Variable
								this->IoT_Status.Connection = false;

								// Declare Response Status
								_Conn_WD = true;

							}

							// Print Command State
							#ifdef DEBUG
								Console::Text(19, 75, CYAN, String(_CREG_Connection_Stat));
							#endif

							// Print Connection Time
							#ifdef DEBUG

								// Set Connection Time
								this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
			
								// Print Connection Time
								Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));

							#endif

							// Set WD Variable
							_Conn_WD_Counter++;

							// Control for WD
							if (_Conn_WD_Counter > 300) _Conn_WD = true;

						}

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// CGDCONT Command (Set PDP Context)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+CGDCONT=1,IP,mgbs"));
						#endif

						// Send Command
						if (!AT_Command_Set::CGDCONT(1, "IP", _PDP_APN_)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SGACT Command (Activate PDP Context)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SGACT=1,1"));
						#endif

						// Send Command
						if (!AT_Command_Set::SGACT(1, 1, this->IoT_Operator.IP_Address)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Connection, 14, 35);
							
							// Print IP Address
							Console::Text(20, 64, CYAN, String(this->IoT_Operator.IP_Address));

						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// WS46 Command (Network Type)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT+WS46?"));
						#endif

						// Send Command
						if (!AT_Command_Set::WS46(GET, this->IoT_Operator.WDS)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Connection, 14, 35);
							
							// Print RED WDS Type
							Console::Text(47, 3, RED, F("2G"));
							Console::Text(47, 6, RED, F("3G"));
							Console::Text(47, 9, RED, F("LTE"));

							// Print WDS Type
							if (this->IoT_Operator.WDS == 12) {

								// Print 2G WDS Type
								Console::Text(47, 3, GREEN, F("2G"));

							} else if (this->IoT_Operator.WDS == 22) {

								// Print 3G WDS Type
								Console::Text(47, 6, GREEN, F("3G"));

							} else if (this->IoT_Operator.WDS == 25) {

								// Print 2G WDS Type
								Console::Text(47, 3, GREEN, F("2G"));

								// Print 3G WDS Type
								Console::Text(47, 6, GREEN, F("3G"));

								// Print LTE WDS Type
								Console::Text(47, 9, GREEN, F("LTE"));

							} else if (this->IoT_Operator.WDS == 28) {

								// Print LTE WDS Type
								Console::Text(47, 9, GREEN, F("LTE"));

							} else if (this->IoT_Operator.WDS == 29) {

								// Print 2G WDS Type
								Console::Text(47, 3, GREEN, F("2G"));

								// Print 3G WDS Type
								Console::Text(47, 6, GREEN, F("3G"));

							} else if (this->IoT_Operator.WDS == 30) {

								// Print 2G WDS Type
								Console::Text(47, 3, GREEN, F("2G"));

								// Print LTE WDS Type
								Console::Text(47, 9, GREEN, F("LTE"));

							} else if (this->IoT_Operator.WDS == 31) {

								// Print 3G WDS Type
								Console::Text(47, 6, GREEN, F("3G"));

								// Print LTE WDS Type
								Console::Text(47, 9, GREEN, F("LTE"));

							} else {

								// Print RED WDS Type
								Console::Text(47, 3, RED, F("2G"));
								Console::Text(47, 6, RED, F("3G"));
								Console::Text(47, 9, RED, F("LTE"));

							}

						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// RFSTS Command (Network Status)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#RFSTS"));
						#endif

						// Send Command
						AT_Command_Set::RFSTS(this->IoT_Operator.MCC, this->IoT_Operator.MNC, this->IoT_Operator.RSSI, this->IoT_Operator.Signal, this->IoT_Operator.Cell_ID, this->IoT_Operator.TAC);

						// Print Command State
						#ifdef DEBUG

							// Print Command State
							Console::OK(this->IoT_Status.Connection, 14, 35);

						#endif

						// Print Command State
						#ifdef DEBUG

							// Print Signal Level Value
							Console::Text(18, 65, WHITE, F("[-   ]"));
							Console::Text(18, 67, CYAN, String(this->IoT_Operator.RSSI));

							// Print Signal Level Bar
							Console::Text(18, 74, GRAY, F("_____"));
							for (uint8_t i = 1; i <= this->IoT_Operator.Signal; i++) Console::Text(18, 73 + i, CYAN, F("X"));

							// Print Operator Value
							Console::Text(19, 74, CYAN, String(this->IoT_Operator.MCC));
							Console::Text(19, 77, CYAN, F("0"));
							Console::Text(19, 78, CYAN, String(this->IoT_Operator.MNC));

							// Print Modem LAC Value
							Console::Text(21, 75, CYAN, uint64ToString(this->IoT_Operator.TAC));

							// Print Modem Cell ID Value
							Console::Text(22, 70, CYAN, String(this->IoT_Operator.Cell_ID));

						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SCFG (Send Port) Command (Send Data Port Configuration)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SCFG=3,1,1500,90,1200,0"));
						#endif

						// Send Command
						if (!AT_Command_Set::SCFG(Socket_Outgoing, 1, 1500, 90, 1200, 0)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SCFGEXT (Send Port) Command (Send Data Port Extended Configuration)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SCFGEXT=3,1,0,0,0,0"));
						#endif

						// Send Command
						if (!AT_Command_Set::SCFGEXT(Socket_Outgoing, 1, 0, 0, 0, 0)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SCFGEXT2 (Send Port) Command (Send Data Port Extended 2 Configuration)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SCFGEXT2=3,1,0,0,0,0"));
						#endif

						// Send Command
						if (!AT_Command_Set::SCFGEXT2(Socket_Outgoing, 1, 0, 0, 0, 0)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SCFG (In Port) Command (In Port Configuration)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SCFG=2,1,1500,90,300,50"));
						#endif

						// Send Command
						if (!AT_Command_Set::SCFG(Socket_Incomming, 1, 1500, 90, 300, 50)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SCFGEXT (In Port) Command (In Port Extended Configuration)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SCFGEXT=2,1,0,1,0,0"));
						#endif

						// Send Command
						if (!AT_Command_Set::SCFGEXT(Socket_Incomming, 2, 0, 0, 0, 0)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// SCFGEXT2 (In Port) Command (Send Data Port Extended 2 Configuration)
					if (this->IoT_Status.Connection) {

						// Print Command State
						#ifdef DEBUG
							Console::AT_Command(14, 4, F("AT#SCFGEXT2=3,1,0,0,0,0"));
						#endif

						// Send Command
						if (!AT_Command_Set::SCFGEXT2(Socket_Incomming, 1, 0, 0, 0, 0)) this->IoT_Status.Connection = false;

						// Print Command State
						#ifdef DEBUG
							Console::OK(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Connecion Time
						#ifdef DEBUG
							this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

					} else break;

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 4, CYAN, F("                                    "));
						Console::Text(14, 4, CYAN, F("PostOffice Connected"));
					#endif

					// Connection Success
					return(true);

				} else {

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 4, CYAN, F("                                    "));
						Console::Text(14, 4, CYAN, F("Modem Not Ready"));
					#endif

					// Initialize Modem
					this->Initialize();

				}

				// Handle WatchDog
				if (_WD > 4) {

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 4, CYAN, F("                                    "));
						Console::Text(14, 4, RED, F("Connection Failed"));
					#endif

					// Clear States
					this->IoT_Status.SIM_Inserted = false;
					this->IoT_Status.SIM_PIN = 0;
					this->IoT_Status.Initialize = false;
					this->IoT_Status.Connection = false;

					// Turn Off Modem
					this->OFF();

					// Connection Failed
					return(false);

				} else {

					// Set WatchDog Variable
					_WD++;

				}

			}
							
			// Connection Failed
			return(false);

		}

		// Set Clock Configuration
		bool Clock(void) {

			// Control for Connection
			if (this->IoT_Status.Connection) {

				// Print Batch Description
				#ifdef DEBUG
					Console::Text(14, 44, CYAN, F("                                    "));
					Console::Text(14, 44, CYAN, F("Updating RTC"));
				#endif

				// Print Command State
				#ifdef DEBUG
					Console::AT_Command(14, 4, F("AT+CCLK"));
				#endif

				// CCLK Command (Real Time Clock Configuration)
				bool _Clock_State = AT_Command_Set::CCLK(this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second, this->Time.Time_Zone);

				// Print Command State
				#ifdef DEBUG
					Console::OK(_Clock_State, 14, 35);
				#endif

				// Print Connection Time
				#ifdef DEBUG

					// Set Connection Time
					this->IoT_Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer) / 1000);

					// Print Connection Time
					Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));

				#endif

				// Update RTC

				// Detect RTC
				I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);

				// RTC Object Definitions	
				RV3028 RTC(true, 1);

				// Update Time
				RTC.Set_Time(this->Time.Second, this->Time.Minute, this->Time.Hour, this->Time.Day, this->Time.Month, this->Time.Year);

				// Declare TimeStamp Variable
				char _TimeStamp[26];

				// Clear TimeStamp Variable
				memset(_TimeStamp, '\0', 26);

				// Handle TimeStamp
				sprintf(_TimeStamp, "20%02hhu-%02hhu-%02hhu %02hhu:%02hhu:%02hhu", this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second);

				// Print Batch Description
				#ifdef DEBUG
					Console::Text(14, 44, CYAN, F("                                    "));
					Console::Text(14, 44, CYAN, F("RTC Updated : "));
					Console::Text(14, 58, CYAN, String(_TimeStamp));
				#endif

				// Connection Success
				return(true);

			} else {

				// Print Command State
				#ifdef DEBUG
					Console::Text(14, 4, CYAN, F("                                    "));
					Console::Text(14, 4, RED, F("Time Functions Failed"));
				#endif

			}

			// Time Configuration Failed
			return(false);

		}

		// Parse JSON Pack
		uint16_t Parse_JSON(uint8_t _Pack_Type) {

			// JSON Document Segments
			#define JSON_Segment_Info
			#define JSON_Segment_Battery
			#define JSON_Segment_GSM
			#define JSON_Segment_Payload

			// Clear Pack
			memset(this->Buffer.JSON_Pack, '\0', Send_JSON_Size);

			// Define JSON
			StaticJsonDocument<Send_JSON_Size> JSON;

			// Set Device ID Variable
			if (_Pack_Type == Pack_Online) {

				// Set Command
				JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Online"));

			} else if (_Pack_Type == Pack_Update) {

				// Set Command
				JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Update"));

			} else if (_Pack_Type == Pack_Timed) {

				// Set Command
				JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Timed"));

			} else if (_Pack_Type == Pack_Interrupt) {

				// Set Command
				JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Interrupt"));

				// Get Data
			} else if (_Pack_Type == Pack_Alarm) {

				// Set Command
				JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Alarm"));

			} else if (_Pack_Type == Pack_Offline) {

				// Set Command
				JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Offline"));

			} else if (_Pack_Type == Pack_FOTA_Info) {

				// Set Command
				JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("FOTA_Download"));

			} else {

				// Set Command
				JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("UnKnown"));

			}

			// Define Device Section
			JsonObject JSON_Device = JSON.createNestedObject(F("Device"));

			// Parse Info Segment
			#ifdef JSON_Segment_Info

				// Define Device Section
				JsonObject JSON_Info = JSON_Device.createNestedObject(F("Info"));

				// Set Device ID Variable
				JSON_Info[F("ID")] = this->PowerStat.Device_ID;
				
				// Set Device Hardware Version Variable
				if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update) JSON_Info[F("Hardware")] = F(__Hardware__);

				// Set Device Firmware Version Variable
				if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update) JSON_Info[F("Firmware")] = F(__Firmware__);

			#endif

			// Parse Battery Segment
			#ifdef JSON_Segment_Battery

				// Define Power Section
				JsonObject JSON_Battery = JSON_Device["Power"].createNestedObject("Battery");

				// Declare Variables
				float _IV = 0.0;
				float _AC = 0.0;
				float _SOC = 0.0;
				float _Temp = 0.0;
				uint16_t _DCAP = 0;
				uint16_t _ICAP = 0;
				uint8_t _State = 0;

				// Get Battery Variables
				_Battery_CallBack(_IV, _AC, _SOC, _Temp, _DCAP, _ICAP, _State);

				// Set Battery Variables
				JSON_Battery[F("IV")] = _IV;
				JSON_Battery[F("AC")] = _AC;
				JSON_Battery[F("SOC")] = _SOC;
				if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update) JSON_Battery[F("T")] = _Temp;
				if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update) JSON_Battery[F("FB")] = _DCAP;
				if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update) JSON_Battery[F("IB")] = _ICAP;
				JSON_Battery[F("Charge")] = _State;

			#endif

			// Parse Battery Segment
			#ifdef JSON_Segment_GSM

				// WS46 Command (Network Type)
				AT_Command_Set::WS46(GET, this->IoT_Operator.WDS);

				// RFSTS Command (Network Status)
				AT_Command_Set::RFSTS(this->IoT_Operator.MCC, this->IoT_Operator.MNC, this->IoT_Operator.RSSI, this->IoT_Operator.Signal, this->IoT_Operator.Cell_ID, this->IoT_Operator.TAC);

				#ifdef DEBUG
					
					// Print RED WDS Type
					Console::Text(47, 3, RED, F("2G"));
					Console::Text(47, 6, RED, F("3G"));
					Console::Text(47, 9, RED, F("LTE"));

					// Print WDS Type
					if (this->IoT_Operator.WDS == 12) {

						// Print 2G WDS Type
						Console::Text(47, 3, GREEN, F("2G"));

					} else if (this->IoT_Operator.WDS == 22) {

						// Print 3G WDS Type
						Console::Text(47, 6, GREEN, F("3G"));

					} else if (this->IoT_Operator.WDS == 25) {

						// Print 2G WDS Type
						Console::Text(47, 3, GREEN, F("2G"));

						// Print 3G WDS Type
						Console::Text(47, 6, GREEN, F("3G"));

						// Print LTE WDS Type
						Console::Text(47, 9, GREEN, F("LTE"));

					} else if (this->IoT_Operator.WDS == 28) {

						// Print LTE WDS Type
						Console::Text(47, 9, GREEN, F("LTE"));

					} else if (this->IoT_Operator.WDS == 29) {

						// Print 2G WDS Type
						Console::Text(47, 3, GREEN, F("2G"));

						// Print 3G WDS Type
						Console::Text(47, 6, GREEN, F("3G"));

					} else if (this->IoT_Operator.WDS == 30) {

						// Print 2G WDS Type
						Console::Text(47, 3, GREEN, F("2G"));

						// Print LTE WDS Type
						Console::Text(47, 9, GREEN, F("LTE"));

					} else if (this->IoT_Operator.WDS == 31) {

						// Print 3G WDS Type
						Console::Text(47, 6, GREEN, F("3G"));

						// Print LTE WDS Type
						Console::Text(47, 9, GREEN, F("LTE"));

					} else {

						// Print RED WDS Type
						Console::Text(47, 3, RED, F("2G"));
						Console::Text(47, 6, RED, F("3G"));
						Console::Text(47, 9, RED, F("LTE"));

					}

				#endif

				// Print Command State
				#ifdef DEBUG

					// Print Signal Level Value
					Console::Text(18, 65, WHITE, F("[-   ]"));
					Console::Text(18, 67, CYAN, String(this->IoT_Operator.RSSI));

					// Print Signal Level Bar
					Console::Text(18, 74, GRAY, F("_____"));
					for (uint8_t i = 1; i <= this->IoT_Operator.Signal; i++) Console::Text(18, 73 + i, CYAN, F("X"));

					// Print Operator Value
					Console::Text(19, 74, CYAN, String(this->IoT_Operator.MCC));
					Console::Text(19, 77, CYAN, F("0"));
					Console::Text(19, 78, CYAN, String(this->IoT_Operator.MNC));

					// Print Modem LAC Value
					Console::Text(21, 75, CYAN, uint64ToString(this->IoT_Operator.TAC));

					// Print Modem Cell ID Value
					Console::Text(22, 70, CYAN, String(this->IoT_Operator.Cell_ID));

				#endif

				// Define GSM Section
				JsonObject JSON_GSM = JSON_Device["IoT"].createNestedObject(F("GSM"));

				// Get GSM Parameters
				if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update) {

					// Define IoT Module
					JsonObject JSON_Module = JSON_GSM.createNestedObject(F("Module"));

					// Set IoT Parameters
					JSON_Module[F("Manufacturer")] = this->IoT_Module.Manufacturer;
					JSON_Module[F("Model")] = this->IoT_Module.Model;
					JSON_Module[F("Firmware")] = this->IoT_Module.Firmware;
					JSON_Module[F("Serial")] = this->IoT_Module.Serial_ID;
					JSON_Module[F("IMEI")] = this->IoT_Module.IMEI;

				}

				// Define GSM Operator Section
				JsonObject JSON_Operator = JSON_GSM.createNestedObject(F("Operator"));

				// Set Device GSM Connection Detail Section
				if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update) JSON_Operator[F("SIM_Type")] = 1;
				if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update)JSON_Operator[F("ICCID")] = this->IoT_Operator.ICCID;
				JSON_Operator[F("Country")] = this->IoT_Operator.MCC;
				JSON_Operator[F("Operator")] = this->IoT_Operator.MNC;
				JSON_Operator[F("RSSI")] = this->IoT_Operator.RSSI;
				JSON_Operator[F("TAC")] = uint64ToString(this->IoT_Operator.TAC);
				JSON_Operator[F("Cell_ID")] = uint64ToString(this->IoT_Operator.Cell_ID);

			#endif

			// Parse Payload Segment
			#ifdef JSON_Segment_Payload

				// Detect RTC
				I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);

				// RTC Object Definitions	
				RV3028 RTC(true, 1);

				// Declare TimeStamp Variable
				char _TimeStamp[26];
				memset(_TimeStamp, '\0', 26);

				// Handle TimeStamp
				sprintf(_TimeStamp, "20%02hhu-%02hhu-%02hhu %02hhu:%02hhu:%02hhu", RTC.Get_Year(), RTC.Get_Month(), RTC.Get_Date(), RTC.Get_Hour(), RTC.Get_Minute(), RTC.Get_Second());

				// Define Data Section
				JsonObject JSON_Payload = JSON.createNestedObject(F("Payload"));

				// Set Device Time Variable
				JSON_Payload[F("TimeStamp")] = _TimeStamp;

				// Set Device Status Variable
				JSON_Payload[F("Status")] = uint64ToString((uint64_t)this->PowerStat.Status.Status_Register);

				// Get Data
				_Send_Data_CallBack(_Pack_Type);

				// Set Device ID Variable
				if (_Pack_Type == Pack_Online) {

					// Declare Variables
					float _Temperature = 0.0;
					float _Humidity = 0.0;

					// Get Environment
					_Environment_CallBack(_Temperature, _Humidity);

					// Set Device Environment Variable
					JSON_Payload[F("PCBT")] = _Temperature;
					JSON_Payload[F("PCBH")] = _Humidity;

				} else if (_Pack_Type == Pack_Update) {

					// Declare Variables
					float _Temperature = 0.0;
					float _Humidity = 0.0;

					// Get Environment
					_Environment_CallBack(_Temperature, _Humidity);

					// Set Device Environment Variable
					JSON_Payload[F("PCBT")] = _Temperature;
					JSON_Payload[F("PCBH")] = _Humidity;

				} else if (_Pack_Type == Pack_Timed) {

					// Declare Pressure Variables
					float _Instant = 0.0;
					float _Min = 0.0;
					float _Max = 0.0;
					float _Avg = 0.0;
					float _Deviation = 0.0;
					float _Slope = 0.0;
					uint16_t _Count = 0;

					// Get Environment
					_Pressure_CallBack(_Instant, _Min, _Max, _Avg, _Deviation, _Slope, _Count);

					// Set Presure Variable
					JsonArray JSON_Pressure = JSON_Payload.createNestedArray(F("Pressure"));

					// Set Presure Variable
					JSON_Pressure.add(_Instant);
					JSON_Pressure.add(_Min);
					JSON_Pressure.add(_Max);
					JSON_Pressure.add(_Avg);
					JSON_Pressure.add(_Deviation);
					JSON_Pressure.add(_Slope);
					JSON_Pressure.add(_Count);

				} else if (_Pack_Type == Pack_Interrupt) {


				} else if (_Pack_Type == Pack_Alarm) {


				} else if (_Pack_Type == Pack_Offline) {


				} else if (_Pack_Type == Pack_FOTA_Info) {

					// Set Device Environment Variable
					JSON_Payload[F("File_ID")] = this->IoT_FOTA.File_ID;
					JSON_Payload[F("Status")] = this->IoT_FOTA.Download_Status;
					JSON_Payload[F("SD_Size")] = this->IoT_FOTA.SD_File_Size;
					JSON_Payload[F("Download_Time")] = this->IoT_FOTA.Download_Time;

				} else {


				}

			#endif

			// Clear Unused Data
			JSON.garbageCollect();

			// Serialize JSON	
			uint16_t _JSON_Size = serializeJson(JSON, this->Buffer.JSON_Pack);

			// Print Command State
			#ifdef DEBUG
				Console::Text(25, 4, WHITE,String(this->Buffer.JSON_Pack).substring(0, 75));
				Console::Text(26, 4, WHITE,String(this->Buffer.JSON_Pack).substring(75, 150));
				Console::Text(27, 4, WHITE,String(this->Buffer.JSON_Pack).substring(150, 225));
				Console::Text(28, 4, WHITE,String(this->Buffer.JSON_Pack).substring(225, 300));
				Console::Text(29, 4, WHITE,String(this->Buffer.JSON_Pack).substring(300, 375));
				Console::Text(30, 4, WHITE,String(this->Buffer.JSON_Pack).substring(375, 450));
				Console::Text(31, 4, WHITE,String(this->Buffer.JSON_Pack).substring(450, 525));
			#endif

			// End Function
			return(_JSON_Size);

		}

		// Reset Function
		void(* Reset) (void) = 0;

	// Public Functions
	public:

		// Define Interrupt Structure
		struct Struct_Interrupt {

			// Send Interrupt
			bool 		Timed				= false;

		} Interrupt;

		// PostMan Constructor
		Postman_WeatherStatV3(Stream &_Serial) : AT_Command_Set(_Serial), Console(Serial_Terminal), Hardware() {

			// Clear All Interrupts
			this->Interrupt.Timed = false;

		}

		// ************************************************************

		// CallBack Definitions
		void Environment_CallBack(void (*_Environment_CallBack)(float&, float&)) {

			// Set CallBack Functions
			this->_Environment_CallBack = _Environment_CallBack;

		}
		void Battery_CallBack(void (*_Battery_CallBack)(float&, float&, float&, float&, uint16_t&, uint16_t&, uint8_t&)) {

			// Set CallBack Functions
			this->_Battery_CallBack = _Battery_CallBack;

		}
		void Event_PackData(void (*_Send_Data_CallBack)(uint8_t)) {

			// Set CallBack Functions
			this->_Send_Data_CallBack = _Send_Data_CallBack;

		}
		void Event_PackSend_Response(void (*_Send_Response_CallBack)(uint16_t, uint8_t)) {

			// Set CallBack Functions
			this->_Send_Response_CallBack = _Send_Response_CallBack;

		}
		void Event_Request(void (*_Command_CallBack)(uint16_t, char*)) {

			// Set CallBack Functions
			this->_Command_CallBack = _Command_CallBack;

		}

		// ************************************************************

		// Connect GSM Modem
		void Online(void) {

			// Set Connection Start Time
			this->Buffer.Connection_Time_Buffer = millis();

			// GSM Connection Squence
			if (Hardware::OFF()) {

				// GSM Initialize Sequence
				if (this->Initialize()) {

					// GSM Connect Sequence
					if (this->Connect()) {

						// Set Clock
						this->Clock();

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
						#endif

					} else {
						
						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, RED, F("GSM Connect Error"));
						#endif

						// Power Down GSM
						Hardware::OFF();

					}

				}

			} 

		}

		// ************************************************************

		// Send Data Batch Function
		bool Publish(const uint8_t _Pack_Type) {

			// Control for Connection
			if (this->IoT_Status.Connection) {

				#ifdef DEBUG

					// Get Time
					uint32_t _Time = millis();

				#endif

				// Set Buffer Variable
				this->PowerStat.Status.Buffer_Register = this->PowerStat.Status.Status_Register;

				// Print Command State
				#ifdef DEBUG
					Console::Text(14, 44, CYAN, F("                                    "));
					Console::Text(14, 44, GREEN, F("Connecting to Server..."));
				#endif

				// Open Connection
				if (AT_Command_Set::ATSD(Socket_Outgoing, 0, 80, 255, 88, 1, _BackEnd_Server_)) {

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, GREEN, F("Parsing JSON Pack..."));
					#endif

					// Parse JSON
					this->Parse_JSON(_Pack_Type);

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, GREEN, F("Sending Pack..."));
					#endif

					// Sending Data
					if (AT_Command_Set::SSEND(Socket_Outgoing, 2, 0, _BackEnd_Server_, _BackEnd_EndPoint_, this->Buffer.JSON_Pack)) {

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, GREEN, F("Waiting Response..."));
						#endif

						// Declare Ring Length
						uint16_t _Length;

						// Get Ring Port
						if (AT_Command_Set::SRING(_Length)) {

							// Declare Response Variable
							char _Response[Response_JSON_Size];

							// Clear Response
							memset(_Response, '\0', Response_JSON_Size);

							// Command Delay
							delay(50);

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 44, CYAN, F("                                    "));
								Console::Text(14, 44, GREEN, F("Getting Response..."));
							#endif

							// Handle Length
							if (_Length > Response_JSON_Size) _Length = Response_JSON_Size;

							// Get Request Data
							if (AT_Command_Set::SRECV(Socket_Outgoing, _Length, _Response)) {

								// Define JSON Object
								StaticJsonDocument<Response_JSON_Size> Incoming_JSON;

								// Deserialize the JSON document
								deserializeJson(Incoming_JSON, _Response);
								
								// Get Response Command
								uint16_t _Response_Command = Incoming_JSON["Event"];

								// Print Command State
								#ifdef DEBUG
									Console::Text(14, 44, CYAN, F("                                    "));
									Console::Text(14, 44, GREEN, F("Response --> [   ]"));
									Console::Text(14, 58, YELLOW, String(_Response_Command));
								#endif

								// Print Command State
								#ifdef DEBUG

									// Calculate Send Time
									uint32_t _Send_Duration = millis() - _Time;

									// Print Command State
									Console::Text(2, 116, CYAN, F("     "));
									Console::Text(2, 116, YELLOW, String(_Send_Duration));

								#endif

								// Command Delay
								delay(50);

								// Print Command State
								#ifdef DEBUG
									Console::Text(14, 44, CYAN, F("                                    "));
									Console::Text(14, 44, GREEN, F("Closing Socket..."));
								#endif

								// Closing Socket
								if (AT_Command_Set::SH(Socket_Outgoing)) {

									// Control for Incoming Call
									this->Listen(true);

									// Send Data CallBack
									_Send_Response_CallBack(_Response_Command, 0);

									// Clear JSON Print
									#ifdef DEBUG
										for (size_t i = 3; i < 79; i++) {
											Console::Text(25, i, CYAN, F(" "));
											Console::Text(26, i, CYAN, F(" "));
											Console::Text(27, i, CYAN, F(" "));
											Console::Text(28, i, CYAN, F(" "));
											Console::Text(29, i, CYAN, F(" "));
											Console::Text(30, i, CYAN, F(" "));
											Console::Text(31, i, CYAN, F(" "));
										}
									#endif

									// Clear Interrupt
									switch (_Pack_Type) {

										// Clear Online Interrupt
										case Pack_Online: {

											// Clear Interrupt
											this->Interrupt.Online = false;

											// End Case
											break;

										}

										// Clear Timed Interrupt
										case Pack_Timed: {

											// Clear Interrupt
											this->Interrupt.Timed = false;

											// End Case
											break;

										}

										// Clear Update Interrupt
										case Pack_Update: {

											// Clear Interrupt
											this->Interrupt.Update = false;

											// End Case
											break;

										}

										// Clear Interrupt Interrupt
										case Pack_Interrupt: {

											// Clear Interrupt
											this->Interrupt.Interrupt = false;

											// End Case
											break;

										}

										// Clear Alarm Interrupt
										case Pack_Alarm: {

											// Clear Interrupt
											this->Interrupt.Alarm = false;

											// End Case
											break;

										}

										// Clear Offline Interrupt
										case Pack_Offline: {

											// Clear Interrupt
											this->Interrupt.Offline = false;

											// End Case
											break;

										}

									}

									// Handle Response
									if (_Response_Command == 200) {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, CYAN, F("                                    "));
											Console::Text(14, 44, GREEN, F("Pack Saved"));
										#endif

									} else if (_Response_Command == 201) {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, CYAN, F("                                    "));
											Console::Text(14, 44, RED, F("Wrong Data Format"));
										#endif

									}

									// End Function
									return(true);

								} else {

									// Send Data CallBack Error
									_Send_Response_CallBack(0, 1);

									// Clear JSON Print
									#ifdef DEBUG
										for (size_t i = 3; i < 79; i++) {
											Console::Text(25, i, CYAN, F(" "));
											Console::Text(26, i, CYAN, F(" "));
											Console::Text(27, i, CYAN, F(" "));
											Console::Text(28, i, CYAN, F(" "));
											Console::Text(29, i, CYAN, F(" "));
											Console::Text(30, i, CYAN, F(" "));
											Console::Text(31, i, CYAN, F(" "));
										}
									#endif

								}

							} else {

								// Send Data CallBack Error
								_Send_Response_CallBack(0, 2);
					
							}
							
						} else {

							// Send Data CallBack Error
							_Send_Response_CallBack(0, 3);

						}

					} else {

						// Send Data CallBack Error
						_Send_Response_CallBack(0, 4);

					}

				} else {

					// Send Data CallBack Error
					_Send_Response_CallBack(0, 5);

				}

			} else {

				// LOG JSON Data
				this->JSON_LOG();

				// Send Data CallBack Error
				_Send_Response_CallBack(0, 6);

			}

			// Port Control
			this->Listen(true);

			// End Function
			return(false);

		}

		// ************************************************************

		// uint64 to String Converter Function
		String uint64ToString(uint64_t input) {
			
			String result = "";
			uint8_t base = 16;

			do {
				
				char c = input % base;
				input /= base;

				if (c < 10)
					c +='0';
				else
					c += 'A' - 10;
			
				result = c + result;

			} while (input);

			return result;

		}

};
