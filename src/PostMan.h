#ifndef __PostMan__
#define __PostMan__

	// Define Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Define Library Includes
	#ifndef __PostMan_Includes__
		#include "Config/Includes.h"
	#endif

	// Define Library Includes
	#ifndef __Telit_AT_Command_Set__
		#include "AT_Command_Set.h"
	#endif

	// Define Console Library
	#ifdef DEBUG

		// Define Console Library
		#ifndef __Console__
			#include <Console.h>
		#endif

	#endif

	// Define PIN Macros
	#define POWER_MON (((PINJ) >> (PINJ3)) & 0x01)
	#define COMMUNICATION(_State) (_State ? PORTJ &= 0b11101111 : PORTJ |= 0b00010000)
	#define POWER_SWITCH(_State) (_State ? PORTH |= 0b00000100 : PORTH &= 0b11111011)
	#define STAT_LED(_State) (_State ? PORTH &= 0b11101111 : PORTH |= 0b00010000)
	#define ONOFF_SIGNAL(_State) (_State ? PORTJ |= 0b01000000 : PORTJ &= 0b10111111)
	#define SHUTDOWN_SIGNAL(_State) (_State ? PORTJ |= 0b00100000 : PORTJ &= 0b11011111)

	// Cloud Functions
	class PostMan : private AT_Command_Set, private Console {

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
				char 		Firmware[10];

			} IoT_Module;

			// Define IoT Operator Structure
			struct Struct_Network {

				// SIM ICCID
				char 		ICCID[21];

				// Operator
				uint16_t 	Code 				= 0;

				// Location
				uint16_t	LAC					= 0;
				uint16_t	Cell_ID				= 0;

				// Signal Level
				uint8_t 	dBm					= 0;
				uint8_t		Signal				= 0;

				// IP Address
				char 		IP_Address[16];

				// Connection Time
				uint8_t 	Connection_Time;

			} IoT_Operator;

			// Define IoT FOTA Structure
			struct Struct_FOTA {
				uint32_t		File_ID				= 0;
				uint32_t		Download_Time		= 0;
				uint32_t		File_Size			= 0;
				uint32_t		SD_File_Size		= 0;
				uint8_t 		Download_Status		= 0;
			} IoT_FOTA;

			// Define CallBack Functions
			void (*_Environment_CallBack)(float&, float&);
			void (*_Battery_CallBack)(float&, float&, float&, float&, uint16_t&, uint16_t&, uint8_t&);
			void (*_Start_Pump_CallBack)(void);
			void (*_Stop_Pump_CallBack)(void);
			void (*_Pressure_CallBack)(float&, float&, float&, float&, float&, float&, uint16_t&);


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
					if (POWER_MON) {

						// Print Batch Description
						#ifdef DEBUG

							// Print Description
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
								Console::GSM_Command(14, 4, F("AT"));
							#endif

							// Send Command
							if (!AT_Command_Set::AT()) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// ATE Command (Echo Off)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("ATE0"));
							#endif

							// Send Command
							if (!AT_Command_Set::ATE(false)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// SIMDET Command (SIM Card Detect)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+SIMDET?"));
							#endif

							// Send Command
							if (!AT_Command_Set::SIMDET(GET, 0, this->IoT_Status.SIM_Inserted)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);

								// Print SIMDET State
								if (this->IoT_Status.SIM_Inserted) Console::Text(14, 44, GREEN, F("SIM Card Detected"));
								if (!this->IoT_Status.SIM_Inserted) Console::Text(14, 44, RED, F("SIM Card Not Detected"));

							#endif

							// No SIM Card
							if (!this->IoT_Status.SIM_Inserted) return(false);

						} else break;

						// SEARCHLIM Command (Search Network)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SEARCHLIM=100,100"));
							#endif

							// Send Command
							if (!AT_Command_Set::SEARCHLIM(100, 100)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// CFUN Command (Full Functionality)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CFUN=1,0"));
							#endif

							// Send Command
							if (!AT_Command_Set::CFUN(1)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// CMEE Command (Error Messages)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CMEE=1"));
							#endif

							// Send Command
							if (!AT_Command_Set::CMEE(1)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// FCLASS Command (Connection Mode)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+FCLASS=0"));
							#endif

							// Send Command
							if (!AT_Command_Set::FCLASS(0)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// ATK Command (No Flow Control)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT&K0"));
							#endif

							// Send Command
							if (!AT_Command_Set::K(0)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// CPIN Command (SIM PIN Control)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CPIN?"));
							#endif

							// Send Command
							if (!AT_Command_Set::CPIN(this->IoT_Status.SIM_PIN)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

							// End Function
							if (this->IoT_Status.SIM_PIN != 1) break;

						} else break;

						// CGSN Command (Get IMEI)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CGSN"));
							#endif

							// Send Command
							if (!AT_Command_Set::CGSN(this->IoT_Module.IMEI)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
								
								// Print IMEI
								Console::Text(20, 24, CYAN, String(this->IoT_Module.IMEI));

							#endif

						} else break;

						// GSN Command (Get Serial Number)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+GSN"));
							#endif

							// Send Command
							if (!AT_Command_Set::GSN(this->IoT_Module.Serial_ID)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
								
								// Print Serial ID
								Console::Text(21, 29, CYAN, String(this->IoT_Module.Serial_ID));
							
							#endif

						} else break;

						// ICCID Command (Get SIM Card ID)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CCID"));
							#endif

							// Send Command
							if (!AT_Command_Set::CCID(this->IoT_Operator.ICCID)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
								
								// Print ICCID
								Console::Text(22, 20, CYAN, String(this->IoT_Operator.ICCID));

							#endif

						} else break;

						// GMI Command (Get Manufacturer)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+GMI"));
							#endif

							// Send Command
							if (!AT_Command_Set::GMI(this->IoT_Module.Manufacturer)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Console::OK_Decide(this->IoT_Status.Initialize, 14,35);
								
								// Print Manufacturer
								Console::Text(17, 38, CYAN, String(this->IoT_Module.Manufacturer));

							#endif

						} else break;

						// GMM Command (Get Model)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+GMM"));
							#endif

							// Send Command
							if (!AT_Command_Set::GMM(this->IoT_Module.Model)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
								
								// Print Model
								Console::Text(18, 38, CYAN, String(this->IoT_Module.Model));

							#endif

						} else break;

						// GMR Command (Get Firmware Version)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+GMR"));
							#endif

							// Send Command
							if (!AT_Command_Set::GMR(this->IoT_Module.Firmware)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);

								// Print Firmware Version
								Console::Text(19, 30, CYAN, String(this->IoT_Module.Firmware));

							#endif

						} else break;

						// SLED Command (Set Status LED)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SLED=2"));
							#endif

							// Send Command
							if (!AT_Command_Set::SLED(2)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// SLEDSAV Command (Save Status LED)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SLEDSAV"));
							#endif

							// Send Command
							if (!AT_Command_Set::SLEDSAV()) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// E2RI Command (Set RING Indicator)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#E2RI=50,50"));
							#endif

							// Send Command
							if (!AT_Command_Set::E2RI(50, 50)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// TXMONMODE Command (Set TX Monitor Mode)
						if (this->IoT_Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#TXMONMODE=1"));
							#endif

							// Send Command
							if (!AT_Command_Set::TXMONMODE(1)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// Initialize Complete
						return(true);

					} else {

						// Print Modem Not ON
						#ifdef DEBUG

							// Print Description
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, CYAN, F("Power ON Modem"));

						#endif

						// Power ON Modem
						this->ON();

					}

					// Handle WatchDog
					if (_WD > 2) {

						// Print Initializing Failed
						#ifdef DEBUG

							// Print Description
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

							// Print Description
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

						// COPS Command (Set Operator Selection Mode)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+COPS=0"));
							#endif

							// Send Command
							if (!AT_Command_Set::COPS(0)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// AUTOBND Command (Set Band Selection Mode)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#AUTOBND=2"));
							#endif

							// Send Command
							if (!AT_Command_Set::AUTOBND(2)) this->IoT_Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Initialize, 14, 35);
							#endif

						} else break;

						// Get Time
						uint32_t _Connection_Start_Time = millis();

						// Set CREG Command (Set Network Registration Mode)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CREG=1"));
							#endif

							// Declare Local Variables
							uint8_t _Mode = 1;
							uint8_t _Stat = 0;

							// Send Command
							if (!AT_Command_Set::CREG(true, _Mode, _Stat)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// Connection Wait Delay
						for (uint8_t i = 0; i < 5; i++) {

							// Print Connection Time
							#ifdef DEBUG
								Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
							#endif

							// Connection Wait Delay
							delay(1000);

						}

						// Get CREG Command (Get Network Registration Mode)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CREG?"));
							#endif

							// Set Connection WatchDog
							bool _Conn_WD = false;
							uint8_t _Conn_WD_Counter = 0;

							// Wait for Connection
							while (!_Conn_WD) {

								// Declare Variable
								uint8_t _CREG_Connection_Mode = 0;
								uint8_t _CREG_Connection_Stat = 0;

								// Get CREG Status
								AT_Command_Set::CREG(false, _CREG_Connection_Mode, _CREG_Connection_Stat);

								// Print Command State
								#ifdef DEBUG
									Console::Text(14, 35, CYAN, F("    "));
									Console::Text(14, 36, RED, String(_CREG_Connection_Stat));
								#endif

								// Control for Connection
								if (_CREG_Connection_Stat == 0) {

									// Set Variable
									this->IoT_Status.Connection = false;

									// End Function
									return(false);

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

										// Print Connection Time
										#ifdef DEBUG
											Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
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

										// Print Connection Time
										#ifdef DEBUG
											Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
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

										// Print Connection Time
										#ifdef DEBUG
											Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
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
									Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
								#endif

								// Set WD Variable
								_Conn_WD_Counter++;

								// Control for WD
								if (_Conn_WD_Counter > 300) _Conn_WD = true;

							}

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// Set CGREG Command (Set Network Registration Mode)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CGREG=1"));
							#endif

							// Declare Local Variables
							uint8_t _Mode = 1;
							uint8_t _Stat = 0;

							// Send Command
							if (!AT_Command_Set::CGREG(true, _Mode, _Stat)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// Connection Wait Delay
						for (uint8_t i = 0; i < 5; i++) {

							// Print Connection Time
							#ifdef DEBUG
								Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
							#endif

							// Connection Wait Delay
							delay(1000);

						}

						// Get CGREG Command (Get Network Registration Mode)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CGREG?"));
							#endif

							// Set Connection WatchDog
							bool _Conn_WD = false;
							uint8_t _Conn_WD_Counter = 0;

							// Wait for Connection
							while (!_Conn_WD) {

								// Declare Variable
								uint8_t _CGREG_Connection_Mode = 0;
								uint8_t _CGREG_Connection_Stat = 0;

								// Get CREG Status
								AT_Command_Set::CGREG(false, _CGREG_Connection_Mode, _CGREG_Connection_Stat);

								// Print Command State
								#ifdef DEBUG
									Console::Text(14, 35, CYAN, F("    "));
									Console::Text(14, 36, RED, String(_CGREG_Connection_Stat));
								#endif

								// Control for Connection
								if (_CGREG_Connection_Stat == 0) {

									// Set Variable
									this->IoT_Status.Connection = false;

									// End Function
									return(false);

								} else if (_CGREG_Connection_Stat == 1) {

									// Set Variable
									this->IoT_Status.Connection = true;

									// Declare Response Status
									_Conn_WD = true;

								} else if (_CGREG_Connection_Stat == 2) {

									// Set Variable
									this->IoT_Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CGREG_Connection_Stat == 3) {

									// Set Variable
									this->IoT_Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 3; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CGREG_Connection_Stat == 4) {

									// Set Variable
									this->IoT_Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CGREG_Connection_Stat == 5) {

									// Set Variable
									this->IoT_Status.Connection = false;

									// Declare Response Status
									_Conn_WD = true;

								}

								// Print Command State
								#ifdef DEBUG
									Console::Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
								#endif

								// Set WD Variable
								_Conn_WD_Counter++;

								// Control for WD
								if (_Conn_WD_Counter > 300) _Conn_WD = true;

							}

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// CGDCONT Command (Set PDP Context)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CGDCONT=1,IP,mgbs"));
							#endif

							// Send Command
							if (!AT_Command_Set::CGDCONT(1, "IP", _PDP_APN_)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// SGACT Command (Activate PDP Context)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SGACT=1,1"));
							#endif

							// Send Command
							if (!AT_Command_Set::SGACT(1, 1, this->IoT_Operator.IP_Address)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
								
								// Print IP Address
								Console::Text(20, 64, CYAN, String(this->IoT_Operator.IP_Address));

							#endif

						} else break;

						// Set Connection Time
						this->IoT_Operator.Connection_Time = ((millis() - _Connection_Start_Time) / 1000);

						// SCFG (Send Port) Command (Send Data Port Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SCFG=3,1,1500,90,1200,0"));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFG(3, 1, 1500, 90, 1200, 0)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT (Send Port) Command (Send Data Port Extended Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SCFGEXT=3,1,0,0,0,0"));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT(3, 1, 0, 0, 0, 0)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT2 (Send Port) Command (Send Data Port Extended 2 Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SCFGEXT2=3,1,0,0,0,0"));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT2(3, 1, 0, 0, 0, 0)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFG (In Port) Command (In Port Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SCFG=2,1,1500,90,300,50"));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFG(2, 1, 1500, 90, 300, 50)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT (In Port) Command (In Port Extended Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#SCFGEXT=2,1,0,1,0,0"));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT(2, 1, 0, 1, 0, 0)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, CYAN, F("Setting Firewall"));
						#endif

						// FRWL Command 1 (Firewall Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#FRWL=1,***"));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_1_IP_)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// FRWL Command 2 (Firewall Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#FRWL=1,***"));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_2_IP_)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// FRWL Command 3 (Firewall Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#FRWL=1,***"));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_3_IP_)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;

						// ICMP Command (Ping Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#ICMP=2"));
							#endif

							// Send Command
							if (!AT_Command_Set::ICMP(2)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;
			
						// MONI Command (Monitor Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT#MONIZIP"));
							#endif

							// Send Command
							if (!AT_Command_Set::MONIZIP(this->IoT_Operator.Code, this->IoT_Operator.LAC, this->IoT_Operator.Cell_ID, this->IoT_Operator.dBm, this->IoT_Operator.Signal)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG

								// Print Signal Level Value
								Console::Text(18, 65, WHITE, F("[-   ]"));
								Console::Text(18, 67, CYAN, String(this->IoT_Operator.dBm));

								// Print Signal Level Bar
								Console::Text(18, 74, GRAY, F("_____"));
								for (uint8_t i = 1; i <= this->IoT_Operator.Signal; i++) Console::Text(18, 73 + i, CYAN, F("X"));

								// Print Operator Value
								Console::Text(19, 74, CYAN, String(this->IoT_Operator.Code));

								// Print Modem LAC Value
								Console::Text(21, 75, CYAN, String(uint64ToString(this->IoT_Operator.LAC)));

								// Print Modem Cell ID Value
								Console::Text(22, 75, CYAN, String(uint64ToString(this->IoT_Operator.Cell_ID)));

								// Command Status
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);

							#endif

						} else break;

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, CYAN, F("Updating RTC"));
						#endif

						// CCLK Command (Real Time Clock Configuration)
						if (this->IoT_Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Console::GSM_Command(14, 4, F("AT+CCLK"));
							#endif

							// Send Command
							if (!AT_Command_Set::CCLK(this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second)) this->IoT_Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
							#endif

						} else break;
		
						// Print Command State
						#ifdef DEBUG
							Console::Text(17, 75, CYAN, String(this->IoT_Operator.Connection_Time));
						#endif

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

			// Configure Socket for Listen
			bool Listen(const bool _State) {

				// Check Connection Status
				if (this->IoT_Status.Connection) {

					// Clear Socket Status
					this->IoT_Status.Socket_State = 0;

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 34, WHITE, F("[    ]"));
					#endif

					// Print Command State
					#ifdef DEBUG
						Console::GSM_Command(14, 4, F("AT#SS=2"));
					#endif

					// Send Command
					AT_Command_Set::SS(2, this->IoT_Status.Socket_State);

					// Print Command State
					#ifdef DEBUG
						Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
					#endif

					// Activate Socket
					if (_State and this->IoT_Status.Socket_State != 4) {

						// Print Command State
						#ifdef DEBUG
							Console::GSM_Command(14, 4, F("AT#SL=2,1,80,255"));
						#endif

						// Print Command State
						#ifdef DEBUG

							// Activate Socket for Listen
							bool _SL_Command = AT_Command_Set::SL(2, 1, 80, 255);

							// Print Command State
							Console::OK_Decide(_SL_Command, 14, 35);

						#else

							// Activate Socket for Listen
							AT_Command_Set::SL(2, 1, 80, 255);

						#endif

						// Command Delay
						delay(50);

						// Print Command State
						#ifdef DEBUG
							Console::GSM_Command(14, 4, F("AT#SS=2"));
						#endif

						// Print Command State
						#ifdef DEBUG

							// Send Command
							bool _SS_Command = AT_Command_Set::SS(2, this->IoT_Status.Socket_State);

							// Print Command State
							Console::OK_Decide(_SS_Command, 14, 35);

						#else

							// Send Command
							AT_Command_Set::SS(2, this->IoT_Status.Socket_State);

						#endif

						// Print Command State
						#ifdef DEBUG
							
							// Print Socket State
							Console::Text(14, 44, CYAN, F("Socket [2] :                        "));
							
							// Print Socket State
							if (this->IoT_Status.Socket_State == 0) Console::Text(14, 57, RED, F("Closed"));
							else if (this->IoT_Status.Socket_State == 1) Console::Text(14, 57, GREEN, F("Active Transfer"));
							else if (this->IoT_Status.Socket_State == 2) Console::Text(14, 57, GREEN, F("Suspended"));
							else if (this->IoT_Status.Socket_State == 3) Console::Text(14, 57, GREEN, F("Pending Data"));
							else if (this->IoT_Status.Socket_State == 4) Console::Text(14, 57, GREEN, F("Listening"));

						#endif

						// Control Socket
						if (this->IoT_Status.Socket_State != 4) return(false);

						// End Function
						return(true);

					}

					// DeActivate Socket
					if (!_State and this->IoT_Status.Socket_State != 0) {

						// Print Command State
						#ifdef DEBUG
							Console::GSM_Command(14, 4, F("AT#SL=2,0,80,255"));
						#endif

						// Print Command State
						#ifdef DEBUG

							// DeActivate Socket for Listen
							bool _SL_Command = AT_Command_Set::SL(2, 0, 80, 255);

							// Print Command State
							Console::OK_Decide(_SL_Command, 14, 35);

						#else

							// DeActivate Socket for Listen
							AT_Command_Set::SL(2, 0, 80, 255);

						#endif

						// Command Delay
						delay(50);

						// Print Command State
						#ifdef DEBUG
							Console::GSM_Command(14, 4, F("AT#SS=2"));
						#endif

						// Get Socket Status
						AT_Command_Set::SS(2, this->IoT_Status.Socket_State);

						// Print Command State
						#ifdef DEBUG
							Console::OK_Decide(this->IoT_Status.Connection, 14, 35);
						#endif

						// Print Command State
						#ifdef DEBUG
							
							// Print Socket State
							Console::Text(14, 44, CYAN, F("Socket [2] :                        "));
							
							// Print Socket State
							if (this->IoT_Status.Socket_State == 0) Console::Text(14, 57, RED, F("Closed"));
							else if (this->IoT_Status.Socket_State == 1) Console::Text(14, 57, GREEN, F("Active Transfer"));
							else if (this->IoT_Status.Socket_State == 2) Console::Text(14, 57, GREEN, F("Suspended"));
							else if (this->IoT_Status.Socket_State == 3) Console::Text(14, 57, GREEN, F("Pending Data"));
							else if (this->IoT_Status.Socket_State == 4) Console::Text(14, 57, GREEN, F("Listening"));

						#endif

						// Control Socket
						if (this->IoT_Status.Socket_State != 0) return(false);

						// End Function
						return(true);

					}

					// End Function
					return(false);

				} else { 

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 4, CYAN, F("                                    "));
						Console::Text(14, 4, RED, F("Modem Not Connected"));
					#endif

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// Update GSM Parameters
			void Update_Connection_Variables(void) {

				// Declare Watchdog Variable
				uint8_t _Error_WD = 0;

				// Set Response Variable
				bool _Response = false;

				// Process Command
				while (!_Response) {

					// Process Command
					_Response = AT_Command_Set::MONIZIP(this->IoT_Operator.Code, this->IoT_Operator.LAC, this->IoT_Operator.Cell_ID, this->IoT_Operator.dBm, this->IoT_Operator.Signal);

					// Set WD Variable
					_Error_WD++;

					// Control for WD
					if (_Error_WD > 5) break;

				}

				// Print Command State
				#ifdef DEBUG

					// Print Signal Level Value
					Console::Text(18, 65, WHITE, F("[-   ]"));
					Console::Text(18, 67, CYAN, String(this->IoT_Operator.dBm));

					// Print Signal Level Bar
					Console::Text(18, 74, GRAY, F("_____"));
					for (uint8_t i = 1; i <= this->IoT_Operator.Signal; i++) Console::Text(18, 73 + i, CYAN, F("X"));

					// Print Operator Value
					Console::Text(19, 74, CYAN, String(this->IoT_Operator.Code));

					// Print Modem LAC Value
					Console::Text(21, 75, CYAN, String(uint64ToString(this->IoT_Operator.LAC)));

					// Print Modem Cell ID Value
					Console::Text(22, 75, CYAN, String(uint64ToString(this->IoT_Operator.Cell_ID)));

				#endif

			}

			// Parse JSON Pack
			uint16_t Parse_JSON(uint8_t _Pack_Type) {

				// JSON Document Segments
				#define JSON_Segment_Info
				#define JSON_Segment_Battery
				#define JSON_Segment_GSM
				#define JSON_Segment_Payload

				// Clear Pack
				memset(this->JSON_Pack, '\0', Send_JSON_Size);

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

					// Update Connection Variables
					this->Update_Connection_Variables();

					// Set Device GSM Connection Detail Section
					if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update) JSON_Operator[F("SIM_Type")] = 1;
					if (_Pack_Type == Pack_Online or _Pack_Type == Pack_Update)JSON_Operator[F("ICCID")] = this->IoT_Operator.ICCID;
					JSON_Operator[F("Code")] = this->IoT_Operator.Code;
					JSON_Operator[F("dBm")] = this->IoT_Operator.dBm;
					JSON_Operator[F("LAC")] = uint64ToString(this->IoT_Operator.LAC);
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
				uint16_t _JSON_Size = serializeJson(JSON, this->JSON_Pack);

				// Print Command State
				#ifdef DEBUG
					Console::Text(25, 4, WHITE,String(this->JSON_Pack).substring(0, 75));
					Console::Text(26, 4, WHITE,String(this->JSON_Pack).substring(75, 150));
					Console::Text(27, 4, WHITE,String(this->JSON_Pack).substring(150, 225));
					Console::Text(28, 4, WHITE,String(this->JSON_Pack).substring(225, 300));
					Console::Text(29, 4, WHITE,String(this->JSON_Pack).substring(300, 375));
					Console::Text(30, 4, WHITE,String(this->JSON_Pack).substring(375, 450));
					Console::Text(31, 4, WHITE,String(this->JSON_Pack).substring(450, 525));
				#endif

				// End Function
				return(_JSON_Size);

			}

			// LOG JSON Pack Function
			void JSON_LOG(void) {

				// Declare LOG File Object
				File LOG_File;

				// Activate Mux
				SD_MUX(true); delay(200);

				// Open File for Write
				LOG_File = SD.open(_LOG_SD_File_Name_, O_WRITE | O_CREAT);

				// Control for File Open
				if (LOG_File) {

					// Command Delay
					delay(5);

					// Flush File
					LOG_File.flush();

					// Print Data
					LOG_File.println(this->JSON_Pack);

					// Print Line Feed
					LOG_File.println("");

					// Command Delay
					delay(10);

					// Close File
					LOG_File.close();

					// Console Print
					#ifdef DEBUG
						Console::Text(14, 44, RED, F("                               "));
						Console::Text(14, 44, RED, F("Error : Pack Writen to SD Card."));
					#endif

					// Clear Pack
					memset(this->JSON_Pack, '\0', 1024);

				}

				// Turn SD MUX Enable LOW
				SD_MUX(false);

			}

			// Reset Function
			void(* Reset) (void) = 0;

		// Public Functions
		public:

			// Define JSON
			char JSON_Pack[Send_JSON_Size];

			// Define Time Structure
			struct Struct_Time {
				uint16_t 	Year				= 0;
				uint16_t 	Month				= 0;
				uint16_t 	Day					= 0;
				uint16_t 	Hour				= 0;
				uint16_t 	Minute				= 0;
				uint16_t 	Second				= 0;
			} Time;

			// Define Interrupt Structure
			struct Struct_Interrupt {

				// Send Interrupt
				bool 		Online				= false;
				bool 		Update				= false;
				bool 		Timed				= false;
				bool 		Interrupt			= false;
				bool 		Alarm				= false;
				bool 		Offline				= false;

				// Ring Interrupt
				bool 		Ring				= false;

			} Interrupt;

			// Define PowerStat Structure
			struct PowerStat {

				// Device ID
				char Device_ID[17];

				// Define Status Structure
				struct Struct_Status {

					// Device Status
					uint32_t Status_Register = 0x00000000;

					// Device Status Buffer
					uint32_t Buffer_Register = 0x00000000;

					// Publish Mask
					uint32_t Publish_Mask = 0x0000006F;

					// Stop Mask
					uint32_t Stop_Mask = 0x00000000;

				} Status;

				// Define Command Variable
				struct Struct_Command {
					bool 		Interval_Update			= false;
					bool		Energy_Limit_Update		= false;
					bool		Pressure_Limit_Update	= false;
					bool		Mask_Update				= false;
				} Command;

			} PowerStat;

			// PostMan Constructor
			PostMan(Stream &_Serial) : AT_Command_Set(_Serial), Console(Serial_Terminal) {

				// Clear All Interrupts
				this->Interrupt.Online = false;
				this->Interrupt.Update = false;
				this->Interrupt.Timed = false;
				this->Interrupt.Interrupt = false;
				this->Interrupt.Alarm = false;
				this->Interrupt.Offline = false;
				this->Interrupt.Ring = false;

			}

			// ************************************************************

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

					// Set On/Off Signal HIGH
					ONOFF_SIGNAL(true);

					// Command Delay
					for (uint8_t i = 0; i < 36; i++) {

						// Calculate Delay (2000)
						uint8_t _Delay = 5000 / 37;

						// Terminal Bar
						#ifdef DEBUG
							Console::Text(14, 4 + i, WHITE, F("▒"));
						#endif

						// Wait
						delay(_Delay); 

					}

					// Set On/Off Signal LOW [PJ6]
					ONOFF_SIGNAL(false);

					// Clear Bar
					#ifdef DEBUG
						for (uint8_t i = 0; i < 36; i++) Console::Text(14, 4 + i, WHITE, F(" "));
					#endif

					// Control for PWMon (PH7)
					if (POWER_MON) {

						// End Function
						return(true);

					} else {

						// Set Shut Down Signal HIGH
						SHUTDOWN_SIGNAL(true);

						// Command Delay
						delay(200);

						// Set Shut Down Signal LOW
						SHUTDOWN_SIGNAL(false);

					}

				}

				// End Function
				return (false);

			}

			// Power OFF Sequence of Modem
			bool OFF(void) {

				// Turn Off Modem
				if (POWER_MON) {

					// Set On/Off Signal HIGH
					ONOFF_SIGNAL(true);

					// Command Delay
					for (uint8_t i = 0; i < 36; i++) {

						// Calculate Delay (2000)
						uint8_t _Delay = 3000 / 37;

						// Terminal Bar
						#ifdef DEBUG
							Console::Text(14, 4 + i, WHITE, F("▒"));
						#endif

						// Wait
						delay(_Delay); 

					}

					// Set On/Off Signal LOW [PJ6]
					ONOFF_SIGNAL(false);

					// Clear Bar
					#ifdef DEBUG
						for (uint8_t i = 0; i < 36; i++) Console::Text(14, 4 + i, WHITE, F(" "));
					#endif

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
			void Start_CallBack(void (*_Start_Pump_CallBack)(void)) {

				// Set CallBack Functions
				this->_Start_Pump_CallBack = _Start_Pump_CallBack;

			}
			void Stop_CallBack(void (*_Stop_Pump_CallBack)(void)) {

				// Set CallBack Functions
				this->_Stop_Pump_CallBack = _Stop_Pump_CallBack;

			}
			void Pressure_CallBack(void (*_Pressure_CallBack)(float&, float&, float&, float&, float&, float&, uint16_t&)) {

				// Set CallBack Functions
				this->_Pressure_CallBack = _Pressure_CallBack;

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

				// GSM Connection Squence
				if (this->OFF()) {

					// GSM Initialize Sequence
					if (this->Initialize()) {

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, CYAN, F("GSM Initialized"));
						#endif

						// GSM Connect Sequence
						if (this->Connect()) {

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 44, CYAN, F("                                    "));
								Console::Text(14, 44, CYAN, F("GSM Connected"));
							#endif

							// Define RTC
							I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);

							// RTC Object Definitions	
							RV3028 RTC(true, 1);

							// Update Time
							RTC.Set_Time(this->Time.Second, this->Time.Minute, this->Time.Hour, this->Time.Day, this->Time.Month, this->Time.Year);

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 44, CYAN, F("                                    "));
								Console::Text(14, 44, CYAN, F("Device Time Updated"));
							#endif

							// GSM Socket Open Sequence
							if (this->Listen(true)) {

								// Print Command State
								#ifdef DEBUG
									Console::Text(14, 44, CYAN, F("                                    "));
									Console::Text(14, 44, CYAN, F("GSM Socket Listening"));
								#endif

							} else {

								// Print Command State
								#ifdef DEBUG
									Console::Text(14, 44, CYAN, F("                                    "));
									Console::Text(14, 44, RED, F("GSM Socket Open Error"));
								#endif
							}

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 44, CYAN, F("                                    "));
							#endif

							// Publish Interrupt Status
							this->Interrupt.Online = true;

							// Clear Ring Interrupt
							this->Interrupt.Ring = false;

						} else {
							
							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 44, CYAN, F("                                    "));
								Console::Text(14, 44, RED, F("GSM Connect Error"));
							#endif

							// Power Down GSM
							this->OFF();

						}

					} else {

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, RED, F("GSM Initialize Error"));
						#endif

						// Power Down GSM
						this->OFF();

					}

				} else {

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, RED, F("GSM Power Down Error"));
					#endif

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
					if (AT_Command_Set::ATSD(3, 0, 80, 255, 88, 1, _BackEnd_Server_)) {

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
						if (AT_Command_Set::SSEND(3, 2, 0, _BackEnd_Server_, _BackEnd_EndPoint_, this->JSON_Pack)) {

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 44, CYAN, F("                                    "));
								Console::Text(14, 44, GREEN, F("Waiting Response..."));
							#endif

							// Declare Ring Length
							uint16_t _Length;

							// Get Ring Port
							if (AT_Command_Set::Send_SRING(_Length)) {

								// Declare Response Variable
								char _Response[Response_JSON_Size];
								memset(_Response, '\0', Response_JSON_Size);

								// Command Delay
								delay(50);

								// Print Command State
								#ifdef DEBUG
									Console::Text(14, 44, CYAN, F("                                    "));
									Console::Text(14, 44, GREEN, F("Getting Response..."));
								#endif

								// Get Request Data
								if (AT_Command_Set::SRECV(3, _Length, _Response)) {

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
									if (AT_Command_Set::SH(3)) {

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

											// End Function
											return(true);

										} else if (_Response_Command == 201) {

											// Print Command State
											#ifdef DEBUG
												Console::Text(14, 44, CYAN, F("                                    "));
												Console::Text(14, 44, RED, F("Wrong Data Format"));
											#endif

											// End Function
											return(true);

										}

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, CYAN, F("                                    "));
											Console::Text(14, 44, RED, F("Closing Socket Error."));
										#endif

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
									_Send_Response_CallBack(0, 1);

									// Print Command State
									#ifdef DEBUG
										Console::Text(14, 44, CYAN, F("                                    "));
										Console::Text(14, 44, GREEN, F("Server Don't Response"));
									#endif

									// Port Control
									this->Listen(true);

									// End Function
									return(false);
						
								}
								
							} else {

								// Send Data CallBack Error
								_Send_Response_CallBack(0, 2);

								// Port Control
								this->Listen(true);

								// End Function
								return(false);

							}

						} else {

							// End Function
							return(false);

						}

					} else {

						// Send Data CallBack Error
						_Send_Response_CallBack(0, 3);

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, RED, F("Socket Dial Error"));
						#endif

						// Port Control
						this->Listen(true);

						// End Function
						return(false);

					}

				} else {

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, RED, F("No Connection"));
					#endif

					// LOG JSON Data
					this->JSON_LOG();

					// Send Data CallBack Error
					_Send_Response_CallBack(0, 4);

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// Get Server Command Function
			void Get(void) {

				// Control for Connection
				if (this->IoT_Status.Connection) {

					// Wait for Sring
					if (AT_Command_Set::Receive_SRING()) {

						// Declare Request Pack Length
						uint16_t _Request_Length;

						// Answer Socket
						AT_Command_Set::SA(2, 1, _Request_Length);

						// Declare JSON Variable
						char _JSON_Data[Recieve_JSON_Size];
						memset(_JSON_Data, '\0', Recieve_JSON_Size);

						// Print Command State
						#ifdef DEBUG
							Console::Beep();
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, GREEN, F("Recieving Response..."));
						#endif

						// Get Request Data
						AT_Command_Set::SRECV(2, _Request_Length, _JSON_Data);

						// Declare JSON Object
						StaticJsonDocument<Recieve_JSON_Size> Incoming_JSON;

						// Deserialize the JSON document
						DeserializationError Error = deserializeJson(Incoming_JSON, _JSON_Data);

						// Declare Event Variable
						uint16_t _Event = 0;

						// Handle JSON
						if (!Error) _Event = Incoming_JSON["Request"]["Event"];

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 44, CYAN, F("                                    "));
							Console::Text(14, 44, GREEN, F("Response --> [   ]"));
							Console::Text(14, 58, YELLOW, String(_Event));
						#endif

						// Declare Response
						uint8_t _Response = Command_NOK;
						uint16_t _Response_HTTP_Status = HTTP_NotFound;

						// Handle Command
						if (_Event == Command_Reset) {

							// Beep Sound
							#ifdef DEBUG
								Console::Beep();
							#endif

							// Send Response
							this->Response(HTTP_OK, Command_OK);

							// Reset Device
							Reset();

						} else if (_Event == Command_Update) {

							// Send Response
							this->Response(HTTP_OK, Command_OK);

							// Set Command
							this->Interrupt.Update = true;

						} else if (_Event == Command_Start) {

							// Control for pump status
							if (bitRead(PowerStat.Status.Status_Register, 0) == 1) {

								// Send Response
								this->Response(HTTP_MethodNotAllowed, Command_NOK);

							} else {

								// Send Response
								this->Response(HTTP_OK, Command_OK);

								// Start Pump CallBack
								_Start_Pump_CallBack();

							}

						} else if (_Event == Command_Stop) {

							// Control for pump status
							if (bitRead(PowerStat.Status.Status_Register, 0) == 0) {

								// Send Response
								this->Response(HTTP_MethodNotAllowed, Command_NOK);

							} else {

								// Send Response
								this->Response(HTTP_OK, Command_OK);

								// Stop Pump CallBack
								_Stop_Pump_CallBack();

							}

						} else if (_Event == Command_Parameter) {

							// Handle JSON
							uint8_t _Address = Incoming_JSON["Request"]["Address"];
							uint32_t _Value = Incoming_JSON["Request"]["Value"];

							// RTC Object Definitions	
							I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);
							RV3028 RTC(true, 1);

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 44, CYAN, F("                                    "));
							#endif

							// Online Interval Update
							#ifdef EEPROM_Online_Interval

								if (_Address == EEPROM_Online_Interval) {

									// Handle Interval
									if (_Value >= 2 and _Value <= 30) {

										// Update EEPROM
										bool _EEPROM_Response = RTC.Write_EEPROM(EEPROM_Online_Interval, (uint8_t)_Value);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, (_EEPROM_Response ? GREEN : RED), F("Online Interval Update"));
										#endif

										// Set Response Code
										_Response = (_EEPROM_Response ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Interval_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Interval Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Offline Interval Update
							#ifdef EEPROM_Offline_Interval

								if (_Address == EEPROM_Offline_Interval) {

									// Handle Interval
									if (_Value >= 30 and _Value <= 60) {

										// Update EEPROM
										bool _EEPROM_Response = RTC.Write_EEPROM(EEPROM_Offline_Interval, (uint8_t)_Value);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, (_EEPROM_Response ? GREEN : RED), F("Offline Interval Update"));
										#endif

										// Set Response Code
										_Response = (_EEPROM_Response ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Interval_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Interval Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Min Voltage Update
							#ifdef EEPROM_V_Min

								if (_Address == EEPROM_V_Min) {

									// Handle Voltage
									if (_Value >= 150 and _Value <= 300) {

										// Handle Low & High Byte
										uint8_t _Low_Byte = lowByte(_Value);
										uint8_t _High_Byte = highByte(_Value);

										// Update EEPROM
										bool _EEPROM_Response_H = RTC.Write_EEPROM(EEPROM_V_Min, _High_Byte);
										bool _EEPROM_Response_L = RTC.Write_EEPROM(EEPROM_V_Min + 0x01, _Low_Byte);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, ((_EEPROM_Response_H and _EEPROM_Response_L) ? GREEN : RED), F("Min Voltage Limit Update"));
										#endif

										// Set Response Code
										_Response = ((_EEPROM_Response_H and _EEPROM_Response_L) ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Energy_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Limit Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Max Voltage Update
							#ifdef EEPROM_V_Max

								if (_Address == EEPROM_V_Max) {

									// Handle Voltage
									if (_Value >= 150 and _Value <= 300) {

										// Handle Low & High Byte
										uint8_t _Low_Byte = lowByte(_Value);
										uint8_t _High_Byte = highByte(_Value);

										// Update EEPROM
										bool _EEPROM_Response_H = RTC.Write_EEPROM(EEPROM_V_Max, _High_Byte);
										bool _EEPROM_Response_L = RTC.Write_EEPROM(EEPROM_V_Max + 0x01, _Low_Byte);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, ((_EEPROM_Response_H and _EEPROM_Response_L) ? GREEN : RED), F("Max Voltage Limit Update"));
										#endif

										// Set Response Code
										_Response = ((_EEPROM_Response_H and _EEPROM_Response_L) ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Energy_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Limit Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Max Current Update
							#ifdef EEPROM_I_Max

								if (_Address == EEPROM_I_Max) {

									// Handle Current
									if (_Value >= 0 and _Value <= 5) {

										// Handle Low & High Byte
										uint8_t _Low_Byte = lowByte(_Value);
										uint8_t _High_Byte = highByte(_Value);

										// Update EEPROM
										bool _EEPROM_Response_H = RTC.Write_EEPROM(EEPROM_I_Max, _High_Byte);
										bool _EEPROM_Response_L = RTC.Write_EEPROM(EEPROM_I_Max + 0x01, _Low_Byte);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, ((_EEPROM_Response_H and _EEPROM_Response_L) ? GREEN : RED), F("Max Current Limit Update"));
										#endif

										// Set Response Code
										_Response = ((_EEPROM_Response_H and _EEPROM_Response_L) ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Energy_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Limit Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Min Frequency Update
							#ifdef EEPROM_FQ_Min

								if (_Address == EEPROM_FQ_Min) {

									// Handle Frequency
									if (_Value >= 40 and _Value <= 60) {

										// Update EEPROM
										bool _EEPROM_Response = RTC.Write_EEPROM(EEPROM_FQ_Min, (uint8_t)_Value);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, (_EEPROM_Response ? GREEN : RED), F("Min Frequency Limit Update"));
										#endif

										// Set Response Code
										_Response = (_EEPROM_Response ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Energy_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Frequency Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Max Frequency Update
							#ifdef EEPROM_FQ_Max

								if (_Address == EEPROM_FQ_Max) {

									// Handle Frequency
									if (_Value >= 40 and _Value <= 60) {

										// Update EEPROM
										bool _EEPROM_Response = RTC.Write_EEPROM(EEPROM_FQ_Max, (uint8_t)_Value);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, (_EEPROM_Response ? GREEN : RED), F("Max Frequency Limit Update"));
										#endif

										// Set Response Code
										_Response = (_EEPROM_Response ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Energy_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Frequency Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Max VIMB Update
							#ifdef EEPROM_VIMB_Max

								if (_Address == EEPROM_VIMB_Max) {

									// Handle Voltage Imbalance
									if (_Value >= 2 and _Value <= 20) {

										// Update EEPROM
										bool _EEPROM_Response = RTC.Write_EEPROM(EEPROM_VIMB_Max, (uint8_t)_Value);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, (_EEPROM_Response ? GREEN : RED), F("Max VIMB Limit Update"));
										#endif

										// Set Response Code
										_Response = (_EEPROM_Response ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Energy_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("VIMB Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Max IIMB Update
							#ifdef EEPROM_IIMB_Max

								if (_Address == EEPROM_IIMB_Max) {

									// Handle Current Imbalance
									if (_Value >= 2 and _Value <= 20) {

										// Update EEPROM
										bool _EEPROM_Response = RTC.Write_EEPROM(EEPROM_IIMB_Max, (uint8_t)_Value);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, (_EEPROM_Response ? GREEN : RED), F("Max IIMB Limit Update"));
										#endif

										// Set Response Code
										_Response = (_EEPROM_Response ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Energy_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("IIMB Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Max Pressure Regresion Update
							#ifdef EEPROM_P_Regression

								if (_Address == EEPROM_P_Regression) {

									// Handle Pressure Regression
									if (_Value >= 2 and _Value <= 20) {

										// Update EEPROM
										bool _EEPROM_Response = RTC.Write_EEPROM(EEPROM_P_Regression, (uint8_t)_Value);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, (_EEPROM_Response ? GREEN : RED), F("Max P Slope Limit Update"));
										#endif

										// Set Response Code
										_Response = (_EEPROM_Response ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Pressure_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("P Slope Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Max Pressure Update
							#ifdef EEPROM_PMAX

								if (_Address == EEPROM_PMAX) {

									// Handle Pressure Max Limit
									if (_Value >= 0 and _Value <= 10000) {

										// Handle Low & High Byte
										uint8_t _Low_Byte = lowByte(_Value);
										uint8_t _High_Byte = highByte(_Value);

										// Update EEPROM
										bool _EEPROM_Response_H = RTC.Write_EEPROM(EEPROM_PMAX, _High_Byte);
										bool _EEPROM_Response_L = RTC.Write_EEPROM(EEPROM_PMAX + 0x01, _Low_Byte);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, ((_EEPROM_Response_H and _EEPROM_Response_L) ? GREEN : RED), F("Max Pressure Limit Update"));
										#endif

										// Set Response Code
										_Response = ((_EEPROM_Response_H and _EEPROM_Response_L) ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Pressure_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Pressure Limit Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// Min Pressure Update
							#ifdef EEPROM_PMIN

								if (_Address == EEPROM_PMIN) {

									// Handle Pressure Min Limit
									if (_Value >= 0 and _Value <= 10000) {

										// Handle Low & High Byte
										uint8_t _Low_Byte = lowByte(_Value);
										uint8_t _High_Byte = highByte(_Value);

										// Update EEPROM
										bool _EEPROM_Response_H = RTC.Write_EEPROM(EEPROM_PMIN, _High_Byte);
										bool _EEPROM_Response_L = RTC.Write_EEPROM(EEPROM_PMIN + 0x01, _Low_Byte);

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, ((_EEPROM_Response_H and _EEPROM_Response_L) ? GREEN : RED), F("Min Pressure Limit Update"));
										#endif

										// Set Response Code
										_Response = ((_EEPROM_Response_H and _EEPROM_Response_L) ? Command_OK : Command_NOK);

										// Set Response HTTP Status
										_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

										// Set Command
										this->PowerStat.Command.Pressure_Limit_Update = true;

									} else {

										// Print Command State
										#ifdef DEBUG
											Console::Text(14, 44, RED, F("Pressure Limit Out of Range"));
										#endif

										// Set Response Code
										_Response = Command_NOK;

										// Set Response HTTP Status
										_Response_HTTP_Status = HTTP_NotAcceptable;

									}

								} 

							#endif

							// STOP Mask Update
							#ifdef EEPROM_STOP_MASK

								if (_Address == EEPROM_STOP_MASK) {

									// Handle Value
									uint8_t _Payload[4];
									
									// Convert Value to Bytes
									for (uint8_t i = 0; i < 4; i++) _Payload[i] = (_Value >> (8 * i)) & 0xFF;

									// MSB1 MSB2 LSB1 LSB2
									// 0x00 0x00 0x00 0x00

									// Update EEPROM
									bool _EEPROM_Response_MSB_1 = RTC.Write_EEPROM(EEPROM_STOP_MASK, _Payload[3]);
									bool _EEPROM_Response_MSB_2 = RTC.Write_EEPROM(EEPROM_STOP_MASK + 0x01, _Payload[2]);
									bool _EEPROM_Response_LSB_1 = RTC.Write_EEPROM(EEPROM_STOP_MASK + 0x02, _Payload[1]);
									bool _EEPROM_Response_LSB_2 = RTC.Write_EEPROM(EEPROM_STOP_MASK + 0x03, _Payload[0]);

									// Set Variable
									PowerStat.Status.Stop_Mask = _Value;

									// Print Stop Variables
									#ifdef DEBUG
										Console::Status_Variables(4, PowerStat.Status.Stop_Mask);
									#endif

									// Print Command State
									#ifdef DEBUG
										Console::Text(14, 44, ((_EEPROM_Response_MSB_1 and _EEPROM_Response_MSB_2 and _EEPROM_Response_LSB_1 and _EEPROM_Response_LSB_2) ? GREEN : RED), F("STOP Mask Update"));
									#endif

									// Set Response Code
									_Response = ((_EEPROM_Response_MSB_1 and _EEPROM_Response_MSB_2 and _EEPROM_Response_LSB_1 and _EEPROM_Response_LSB_2) ? Command_OK : Command_NOK);

									// Set Response HTTP Status
									_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

									// Set Command
									this->PowerStat.Command.Mask_Update = true;

								} 

							#endif

							// PUBLISH Mask Update
							#ifdef EEPROM_PUBLISH_MASK

								if (_Address == EEPROM_PUBLISH_MASK) {

									// Handle Value
									uint8_t _Payload[4];
									
									// Convert Value to Bytes
									for (uint8_t i = 0; i < 4; i++) _Payload[i] = (_Value >> (8 * i)) & 0xFF;

									// MSB1 MSB2 LSB1 LSB2
									// 0x00 0x00 0x00 0x00

									// Update EEPROM
									bool _EEPROM_Response_MSB_1 = RTC.Write_EEPROM(EEPROM_PUBLISH_MASK, _Payload[3]);
									bool _EEPROM_Response_MSB_2 = RTC.Write_EEPROM(EEPROM_PUBLISH_MASK + 0x01, _Payload[2]);
									bool _EEPROM_Response_LSB_1 = RTC.Write_EEPROM(EEPROM_PUBLISH_MASK + 0x02, _Payload[1]);
									bool _EEPROM_Response_LSB_2 = RTC.Write_EEPROM(EEPROM_PUBLISH_MASK + 0x03, _Payload[0]);

									// Set Variable
									PowerStat.Status.Publish_Mask = _Value;

									// Print Publish Variables
									#ifdef DEBUG
										Console::Status_Variables(3, PowerStat.Status.Publish_Mask);
									#endif

									// Print Command State
									#ifdef DEBUG
										Console::Text(14, 44, ((_EEPROM_Response_MSB_1 and _EEPROM_Response_MSB_2 and _EEPROM_Response_LSB_1 and _EEPROM_Response_LSB_2) ? GREEN : RED), F("EEPROM PUBLISH Mask Update"));
									#endif

									// Set Response Code
									_Response = ((_EEPROM_Response_MSB_1 and _EEPROM_Response_MSB_2 and _EEPROM_Response_LSB_1 and _EEPROM_Response_LSB_2) ? Command_OK : Command_NOK);

									// Set Response HTTP Status
									_Response_HTTP_Status = (_Response ? HTTP_OK : HTTP_MethodNotAllowed);

									// Set Command
									this->PowerStat.Command.Mask_Update = true;

								} 

							#endif

							// Print Command State
							#ifdef DEBUG
								if (_Response == Command_NOK) Console::Text(14, 44, RED, F("EEPROM Address Error"));
							#endif

							// Call Command CallBack
							_Command_CallBack(_Event, _JSON_Data);

							// Send Response
							this->Response(_Response_HTTP_Status, _Response);

						} else if (_Event == Command_FOTA_Download) {

							// Get File ID
							this->IoT_FOTA.File_ID = Incoming_JSON["Request"]["Firmware"];

							// Control for File ID
							if (this->IoT_FOTA.File_ID != 0) {

								// Send Response
								this->Response(200, Command_OK);

								// Download Firmware
								this->Download();

								// Publish FOTA Info
								this->Publish(Pack_FOTA_Info);

							} else {

								// Send Response
								this->Response(HTTP_NotAcceptable, Command_NOK);
								
							}

						} else if (_Event == Command_FOTA_Burn) {

							// Send Response
							this->Response(HTTP_OK, Command_OK);

							// Command Delay
							delay(500);

							// Enable FOTA
							FOTA_BURN(true);

						} else {

							// Send Response
							this->Response(HTTP_NotFound, Command_NOK);

						}

					} 

					// Clear Interrupt
					this->Interrupt.Ring = false;

					// Port Control
					this->Listen(true);

				}

			}

			// Send Request Response Function
			bool Response(const uint16_t _Response_Code, const uint16_t _Response) {

				// Declare Response Array
				char _Response_JSON[Response_JSON_Size];
				memset(_Response_JSON, '\0', Response_JSON_Size);

				// Declare JSON Object
				StaticJsonDocument<Response_JSON_Size> Response_JSON;

				// Declare JSON Data
				Response_JSON[F("Response")] = _Response;

				// Clear Unused Data
				Response_JSON.garbageCollect();

				// Serialize JSON	
				serializeJson(Response_JSON, _Response_JSON);

				// Send Socket Answer
				if (AT_Command_Set::SSEND(2, 1, _Response_Code, "", "", _Response_JSON)) {

					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 44, CYAN, F("                                    "));
						Console::Text(14, 44, GREEN, F("Response Sended"));
					#endif

					// Command Delay
					delay(20);

					// Close Socket
					if (AT_Command_Set::SH(2)) {

						// Command Delay
						delay(20);

						// ReOpen Socket
						bool Socket_Open = this->Listen(true);

						// End Function
						return(Socket_Open);

					} else {

						// End Function
						return(false);

					}

				} else {

					// End Function
					return(false);

				}
				
			}

			// Firmware Download Function
			bool Download(void) {

				// Check Connection Status
				if (this->IoT_Status.Connection) {

					// SD Message
					#ifdef DEBUG
						Console::Text(14, 44, GREEN, F("                               "));
						Console::Text(14, 44, GREEN, F("Firmware download started."));
					#endif

					// Activate SD Mux
					SD_MUX(true);
					
					// SD Wait Delay
					delay(200);

					// Declare SD File Object
					File SD_File;

					// Control for Existing File
					if (SD.exists(_FOTA_SD_FileName_)) {

						// Remove Existing File
						SD.remove(_FOTA_SD_FileName_);

						// SD Message
						#ifdef DEBUG
							Console::Text(14, 44, GREEN, F("                               "));
							Console::Text(14, 44, GREEN, F("File Exist and Deleted."));
						#endif

						// Command Delay
						delay(100);

					}

					// Open File for Write
					SD_File = SD.open(_FOTA_SD_FileName_, O_WRITE | O_CREAT);

					// Command Delay
					delay(100);

					// Control for File Open
					if (SD_File) {

						// Declare Prosedure State
						bool _State = true;

						// Open FTP Socket
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 4, GRAY, F(".............................."));
								Console::Text(14, 4, WHITE, F("AT#FTPOPEN=***,***,***"));
								Console::Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPOPEN="165.227.154.147","fota","123456",1\r\n

							// Process Command
							while (!_Response) {

								// Send Command
								_Response = AT_Command_Set::FTPOPEN(_FOTA_Server_, _FOTA_UserName_, _FOTA_PassWord_, 1);

								// Set WD Variable
								_WD++;

								// Control for WD
								if (_WD > 4) break;

							}

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->IoT_FOTA.Download_Status = FOTA_FTP_Open_Error;

							}

						}

						// Set FTP Type
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 4, GRAY, F(".............................."));
								Console::Text(14, 4, WHITE, F("AT#FTPTYPE=0"));
								Console::Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPTYPE=0\r\n

							// Process Command
							while (!_Response) {

								// Send Command
								_Response = AT_Command_Set::FTPTYPE(0);

								// Set WD Variable
								_WD++;

								// Control for WD
								if (_WD > 4) break;

							}

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->IoT_FOTA.Download_Status = FOTA_FTP_Type_Error;

							}

						}

						// Set FTP Folder
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 4, GRAY, F(".............................."));
								Console::Text(14, 4, WHITE, F("AT#FTPCWD=\"firmware\""));
								Console::Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPCWD="firmware"\r\n

							// Process Command
							while (!_Response) {

								// Send Command
								_Response = AT_Command_Set::FTPCWD(_FOTA_Folder_);

								// Set WD Variable
								_WD++;

								// Control for WD
								if (_WD > 4) break;

							}

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->IoT_FOTA.Download_Status = FOTA_FTP_Folder_Error;

							}

						}

						// Get File Size
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 4, GRAY, F(".............................."));
								Console::Text(14, 4, WHITE, F("AT#FTPFSIZE=\"xxx.hex\""));
								Console::Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPFSIZE="6.hex"\r\n

							// Process Command
							while (!_Response) {

								// Send Command
								_Response = AT_Command_Set::FTPFSIZE(this->IoT_FOTA.File_ID, this->IoT_FOTA.File_Size);

								// Set WD Variable
								_WD++;

								// Control for WD
								if (_WD > 4) break;

							}

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->IoT_FOTA.Download_Status = FOTA_FTP_File_Size_Error;

							}

						}

						// Get File
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Console::Text(14, 4, GRAY, F(".............................."));
								Console::Text(14, 4, WHITE, F("AT#FTPGETPKT=\"xxx.hex\",0"));
								Console::Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPGETPKT="6.hex",0\r\n

							// Process Command
							while (!_Response) {

								// Send Command
								_Response = AT_Command_Set::FTPGETPKT(this->IoT_FOTA.File_ID, 0);

								// Set WD Variable
								_WD++;

								// Control for WD
								if (_WD > 4) break;

							}

							// Print Command State
							#ifdef DEBUG
								Console::OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->IoT_FOTA.Download_Status = FOTA_FTP_Get_Error;

							}

						}

						#ifdef DEBUG
							Console::Text(17, 113, GREEN, String(this->IoT_FOTA.File_ID));
							Console::Text(19, 112, GREEN, String(this->IoT_FOTA.File_Size));
						#endif

						// SD Message
						#ifdef DEBUG
							Console::Text(14, 44, GREEN, F("                               "));
							Console::Text(14, 44, GREEN, F("SD Opened"));
						#endif

						// Get Time
						uint32_t _Download_Start_Time = millis();

						// Set Data Variable
						char _Data[250];

						// Define Variable
						uint32_t _SD_Recieve_Size = 0;
						uint16_t _Download_Size = 0;
						uint8_t _Download_State = 0;

						// SD Message
						#ifdef DEBUG
							Console::Text(14, 44, GREEN, F("                               "));
							Console::Text(14, 44, GREEN, F("Downloading"));
						#endif

						// Get Data Pack From Buffer
						if (_State) {

							// Reset Control Variables
							bool _Response = false;

							// Get Pack
							while (!_Response) {

								// Set Data Variable
								memset(_Data, '\0', 250);

								// Reset Variable
								_Download_Size = 0;

								// Send Command
								if (AT_Command_Set::FTPRECV(200, _Download_Size, _Download_State, _Data)) {

									if (_Download_Size > 0) {

										// Write Data
										SD_File.write(_Data, _Download_Size);

										// Handle Recieve Size
										_SD_Recieve_Size += _Download_Size;

									}

								} 

								// Control for State
								if (_Download_State == 1) break;
								if (_Download_State == 2) break;

								// Control for File End
								if (_SD_Recieve_Size == this->IoT_FOTA.File_Size) _Response = true;

								// Work Delay
								delay(50);

								// Calculate Download Time
								this->IoT_FOTA.Download_Time = (millis() - _Download_Start_Time) / 1000;

								// SD Message
								#ifdef DEBUG
									
									// Print File Size
									Console::Text(21, 114, CYAN, String("   "));
									Console::Text(21, 114, CYAN, String(map(_SD_Recieve_Size, 0, this->IoT_FOTA.File_Size, 0, 100)));

									// Print Download Time
									Console::Text(22, 111, CYAN, String("    "));
									Console::Text(22, 111, CYAN, String(this->IoT_FOTA.Download_Time));

									Console::Text(20, 112, WHITE, F("       "));
									Console::Text(20, 112, WHITE, String(_SD_Recieve_Size));

								#endif

								// End Function
								if (this->IoT_FOTA.Download_Time > 1200) {

									// Set Download Status
									this->IoT_FOTA.Download_Status = FOTA_Download_TimeOut;
	
									// Break Loop
									break;

								}

							}

							//Work Delay
							delay(8);

							// Close File
							SD_File.close();

							// SD Message
							#ifdef DEBUG
								Console::Text(14, 44, GREEN, F("                               "));
								Console::Text(14, 44, GREEN, F("SD Closed"));
							#endif

						}

						// Reset Control Variables
						uint8_t _WD = 0;
						bool _Response = false;

						// Print Command State
						#ifdef DEBUG
							Console::Text(14, 4, GRAY, F(".............................."));
							Console::Text(14, 4, WHITE, F("AT#FTPCLOSE"));
							Console::Text(14, 35, BLUE, F(" .. "));
						#endif

						// Process Command
						while (!_Response) {

							// Send Command
							_Response = AT_Command_Set::FTPCLOSE();

							// Set WD Variable
							_WD++;

							// Control for WD
							if (_WD > 4) break;

						}

						// Print Command State
						#ifdef DEBUG
							Console::OK_Decide(_Response, 14, 35);
						#endif

						// Set Function Variable
						if (!_Response) {

							// Set State Variable
							_State = false;

							// Set Download Status
							this->IoT_FOTA.Download_Status = FOTA_FTP_Close_Error;

						}

					} else {

						// SD Message
						#ifdef DEBUG
							Console::Text(14, 44, GREEN, F("                               "));
							Console::Text(14, 44, RED, F("File Not Opened"));
						#endif

						// Set Download Status
						this->IoT_FOTA.Download_Status = FOTA_SD_Error;

					}

					// FOTA Download Status
					#ifdef DEBUG
						Console::Text(17, 113, GREEN, F("      "));
						Console::Text(18, 115, GREEN, F("    "));
						Console::Text(19, 112, GREEN, F("       "));
						Console::Text(20, 112, GREEN, F("       "));
						Console::Text(21, 114, GREEN, F("    "));
						Console::Text(22, 111, GREEN, F("     "));
					#endif

					// Control for Existing File
					if (SD.exists(_FOTA_SD_FileName_)) {

						// Open File for Write
						SD_File = SD.open(_FOTA_SD_FileName_, FILE_READ);

						// Get File Size
						this->IoT_FOTA.SD_File_Size = SD_File.size();

						// Control for File Size
						if (this->IoT_FOTA.File_Size == this->IoT_FOTA.SD_File_Size) {

							// SD Message
							#ifdef GSM_Debug
								Console::OK_Decide(true, 18, 115);
								Console::Text(19, 112, GREEN, String(this->IoT_FOTA.SD_File_Size));
							#endif

							// Turn Off SD MUX
							SD_MUX(false);

							// Set Download Status
							this->IoT_FOTA.Download_Status = FOTA_Download_OK;

							// End Function
							return(true);

						} else {

							// SD Message
							#ifdef GSM_Debug
								Console::OK_Decide(false, 18, 115);
								Console::Text(19, 112, RED, String(this->IoT_FOTA.SD_File_Size));
							#endif

							// Set Download Status
							this->IoT_FOTA.Download_Status = FOTA_FTP_File_Size_Error;

							// Turn Off SD MUX
							SD_MUX(false);

							// End Function
							return(false);

						}

						// End Function
						return(false);

					} else {

						// Turn Off SD MUX
						SD_MUX(false);

						// Set Download Status
						this->IoT_FOTA.Download_Status = FOTA_Download_Not_Save;

						// End Function
						return(false);

					}

				}

				// Set Download Status
				this->IoT_FOTA.Download_Status = 0;

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

#endif /* defined(__PostMan__) */