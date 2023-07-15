#ifndef __PostMan_GSM__
#define __PostMan_GSM__

	// Define Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Define Library Includes
	#ifndef __PostMan_Includes__
		#include "Config/Includes.h"
	#endif

	// Define Hardware Includes
	#ifndef __PostMan_Hardware__
		#include "Hardware.h"
	#endif

	// Define Library Includes
	#ifndef __Telit_AT_Command_Set__
		#include "AT_Command_Set.h"
	#endif
	
	// GSM Functions
	class GSM : public Hardware, public AT_Command_Set {

		// Public Functions
		public:

			// Define Modem Status Structure
			struct Struct_Status {
				bool 		Power				= false;
				bool		SIM_Inserted		= false;
				uint8_t		SIM_PIN				= 0;
				bool 		Initialize		 	= false;
				bool		Connection			= false;
				uint8_t		Socket_State		= 0;
			} Status;

			// Define Module Structure
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

			} Module;

			// Define Network Structure
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

			} Operator;

			// Define Time Structure
			Struct_Time Time;

			// GSM Constructor
			GSM(Stream &_Serial) : AT_Command_Set(_Serial) {

			}

            // Initialize GSM Modem
			bool Initialize(void) {

				// Declare WatchDog Variable
				uint8_t _WD = 0;

				// Initialize Modem Parameters
				while (!this->Status.Initialize) {

					// Get PowerMon
					this->Status.Power = Hardware::PowerMonitor();

					// Control for Power Monitor
					if (this->Status.Power) {

						// Print Batch Description
						#ifdef DEBUG

							// Print Description
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("Initializing Modem"));
							Terminal_GSM.Text(14, 34, WHITE, F("[    ]"));

						#endif

						// Set Control Variable
						this->Status.Initialize = true;

						// AT Command
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::AT()) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// ATE Command (Echo Off)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("ATE0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::ATE(false)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// SIMDET Command (SIM Card Detect)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+SIMDET?"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));

							#endif

							// Send Command
							if (!AT_Command_Set::SIMDET(GET, 0, this->Status.SIM_Inserted)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);

								// Print SIMDET State
								if (this->Status.SIM_Inserted) Terminal_GSM.Text(14, 44, GREEN, F("SIM Card Detected"));
								if (!this->Status.SIM_Inserted) Terminal_GSM.Text(14, 44, RED, F("SIM Card Not Detected"));

							#endif

							// No SIM Card
							if (!this->Status.SIM_Inserted) return(false);

						} else break;

						// SEARCHLIM Command (Search Network)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SEARCHLIM=100,100"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SEARCHLIM(100, 100)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// CFUN Command (Full Functionality)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CFUN=1,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CFUN(1)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// CMEE Command (Error Messages)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CMEE=1"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CMEE(1)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// FCLASS Command (Connection Mode)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+FCLASS=0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::FCLASS(0)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// ATK Command (No Flow Control)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT&K0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::K(0)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// CPIN Command (SIM PIN Control)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CPIN?"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CPIN(this->Status.SIM_PIN)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

							// End Function
							if (this->Status.SIM_PIN != 1) break;

						} else break;

						// CGSN Command (Get IMEI)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CGSN"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CGSN(this->Module.IMEI)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								
								// Print IMEI
								Terminal_GSM.Text(20, 24, CYAN, String(this->Module.IMEI));

							#endif

						} else break;

						// GSN Command (Get Serial Number)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+GSN"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::GSN(this->Module.Serial_ID)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								
								// Print Serial ID
								Terminal_GSM.Text(21, 29, CYAN, String(this->Module.Serial_ID));
							
							#endif

						} else break;

						// ICCID Command (Get SIM Card ID)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CCID"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CCID(this->Operator.ICCID)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								
								// Print ICCID
								Terminal_GSM.Text(22, 20, CYAN, String(this->Operator.ICCID));

							#endif

						} else break;

						// GMI Command (Get Manufacturer)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+GMI"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::GMI(this->Module.Manufacturer)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14,35);
								
								// Print Manufacturer
								Terminal_GSM.Text(17, 38, CYAN, String(this->Module.Manufacturer));

							#endif

						} else break;

						// GMM Command (Get Model)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+GMM"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::GMM(this->Module.Model)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								
								// Print Model
								Terminal_GSM.Text(18, 38, CYAN, String(this->Module.Model));

							#endif

						} else break;

						// GMR Command (Get Firmware Version)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+GMR"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::GMR(this->Module.Firmware)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);

								// Print Firmware Version
								Terminal_GSM.Text(19, 30, CYAN, String(this->Module.Firmware));

							#endif

						} else break;

						// SLED Command (Set Status LED)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SLED=2"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SLED(2)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// SLEDSAV Command (Save Status LED)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SLEDSAV"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SLEDSAV()) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// E2RI Command (Set RING Indicator)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#E2RI=50,50"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::E2RI(50, 50)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// TXMONMODE Command (Set TX Monitor Mode)
						if (this->Status.Initialize) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#TXMONMODE=1"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::TXMONMODE(1)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

					} else {

						// Power ON Modem
						Hardware::Power(true);

					}

					// Handle WatchDog
					if (_WD > 2) {

						// Clear States
						this->Status.Initialize = false;
						this->Status.SIM_Inserted = false;
						this->Status.SIM_PIN = 0;

						// End Function
						return(false);

					} else {

						// Set WatchDog Variable
						_WD++;

					}

				}

				// End Function
				return(true);
				
			}

            // Connect GSM Modem
			bool Connect(void) {

				// Declare WatchDog Variable
				uint8_t _WD = 0;

				// Connect to Internet
				while (!this->Status.Connection) {

					// Control for Initialize
					if (this->Status.Initialize) {

						// Print Batch Description
						#ifdef DEBUG

							// Print Description
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("Connecting"));
							Terminal_GSM.Text(14, 34, WHITE, F("[    ]"));

						#endif

						// Set Control Variable
						this->Status.Connection = true;

						// COPS Command (Set Operator Selection Mode)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+COPS=0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::COPS(0)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// AUTOBND Command (Set Band Selection Mode)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#AUTOBND=2"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::AUTOBND(2)) this->Status.Initialize = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

						} else break;

						// Get Time
						uint32_t _Connection_Start_Time = millis();

						// Set CREG Command (Set Network Registration Mode)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CREG=1"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Declare Local Variables
							uint8_t _Mode = 1;
							uint8_t _Stat = 0;

							// Send Command
							if (!AT_Command_Set::CREG(true, _Mode, _Stat)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// Connection Wait Delay
						for (uint8_t i = 0; i < 5; i++) {

							// Print Connection Time
							#ifdef DEBUG
								Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
							#endif

							// Connection Wait Delay
							delay(1000);

						}

						// Get CREG Command (Get Network Registration Mode)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CREG?"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
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
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 35, CYAN, F("    "));
									Terminal_GSM.Text(14, 36, RED, String(_CREG_Connection_Stat));
								#endif

								// Control for Connection
								if (_CREG_Connection_Stat == 0) {

									// Set Variable
									this->Status.Connection = false;

									// End Function
									return(false);

								} else if (_CREG_Connection_Stat == 1) {

									// Set Variable
									this->Status.Connection = true;

									// Declare Response Status
									_Conn_WD = true;

								} else if (_CREG_Connection_Stat == 2) {

									// Set Variable
									this->Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CREG_Connection_Stat == 3) {

									// Set Variable
									this->Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 3; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CREG_Connection_Stat == 4) {

									// Set Variable
									this->Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CREG_Connection_Stat == 5) {

									// Set Variable
									this->Status.Connection = false;

									// Declare Response Status
									_Conn_WD = true;

								}

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
								#endif

								// Set WD Variable
								_Conn_WD_Counter++;

								// Control for WD
								if (_Conn_WD_Counter > 300) _Conn_WD = true;

							}

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// Set CGREG Command (Set Network Registration Mode)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CGREG=1"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Declare Local Variables
							uint8_t _Mode = 1;
							uint8_t _Stat = 0;

							// Send Command
							if (!AT_Command_Set::CGREG(true, _Mode, _Stat)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// Connection Wait Delay
						for (uint8_t i = 0; i < 5; i++) {

							// Print Connection Time
							#ifdef DEBUG
								Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
							#endif

							// Connection Wait Delay
							delay(1000);

						}

						// Get CGREG Command (Get Network Registration Mode)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CGREG?"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
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
								AT_Command_Set::CREG(false, _CGREG_Connection_Mode, _CGREG_Connection_Stat);

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 35, CYAN, F("    "));
									Terminal_GSM.Text(14, 36, RED, String(_CGREG_Connection_Stat));
								#endif

								// Control for Connection
								if (_CGREG_Connection_Stat == 0) {

									// Set Variable
									this->Status.Connection = false;

									// End Function
									return(false);

								} else if (_CGREG_Connection_Stat == 1) {

									// Set Variable
									this->Status.Connection = true;

									// Declare Response Status
									_Conn_WD = true;

								} else if (_CGREG_Connection_Stat == 2) {

									// Set Variable
									this->Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CGREG_Connection_Stat == 3) {

									// Set Variable
									this->Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 3; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CGREG_Connection_Stat == 4) {

									// Set Variable
									this->Status.Connection = false;

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5; i++) {

										// Print Connection Time
										#ifdef DEBUG
											Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
										#endif

										// Connection Wait Delay
										delay(1000);

									}

								} else if (_CGREG_Connection_Stat == 5) {

									// Set Variable
									this->Status.Connection = false;

									// Declare Response Status
									_Conn_WD = true;

								}

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
								#endif

								// Set WD Variable
								_Conn_WD_Counter++;

								// Control for WD
								if (_Conn_WD_Counter > 300) _Conn_WD = true;

							}

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// CGDCONT Command (Set PDP Context)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CGDCONT=1,IP,mgbs"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CGDCONT(1, "IP", _PDP_APN_)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SGACT Command (Activate PDP Context)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SGACT=1,1"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SGACT(1, 1, this->Operator.IP_Address)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG

								// Print Command State
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								
								// Print IP Address
								Terminal_GSM.Text(20, 64, CYAN, String(this->Operator.IP_Address));

							#endif

						} else break;

						// Set Connection Time
						this->Operator.Connection_Time = ((millis() - _Connection_Start_Time) / 1000);

						// SCFG (Send Port) Command (Send Data Port Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFG=3,1,1500,90,1200,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFG(3, 1, 1500, 90, 1200, 0)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT (Send Port) Command (Send Data Port Extended Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFGEXT=3,1,0,0,0,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT(3, 1, 0, 0, 0, 0)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT2 (Send Port) Command (Send Data Port Extended 2 Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFGEXT2=3,1,0,0,0,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT2(3, 1, 0, 0, 0, 0)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFG (In Port) Command (In Port Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFG=2,1,1500,90,300,50"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFG(2, 1, 1500, 90, 300, 50)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT (In Port) Command (In Port Extended Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFGEXT=2,1,0,1,0,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT(2, 1, 0, 1, 0, 0)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("Setting Firewall"));
						#endif

						// FRWL Command 1 (Firewall Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FRWL=1,***"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_1_IP_)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// FRWL Command 2 (Firewall Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FRWL=1,***"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_2_IP_)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// FRWL Command 3 (Firewall Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FRWL=1,***"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_3_IP_)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// ICMP Command (Ping Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#ICMP=2"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::ICMP(2)) this->Status.Connection = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;
			
						// MONI Command (Monitor Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#MONIZIP"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::MONIZIP(this->Operator.Code, this->Operator.LAC, this->Operator.Cell_ID, this->Operator.dBm, this->Operator.Signal)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG

								// Print Signal Level Value
								Terminal_GSM.Text(18, 65, WHITE, F("[-   ]"));
								Terminal_GSM.Text(18, 67, CYAN, String(this->Operator.dBm));

								// Print Signal Level Bar
								Terminal_GSM.Text(18, 74, GRAY, F("_____"));
								for (uint8_t i = 1; i <= this->Operator.Signal; i++) Terminal_GSM.Text(18, 73 + i, CYAN, F("X"));

								// Print Operator Value
								Terminal_GSM.Text(19, 74, CYAN, String(this->Operator.Code));

								// Print Modem LAC Value
								Terminal_GSM.Text(21, 75, CYAN, String(this->Operator.LAC, HEX));

								// Print Modem Cell ID Value
								Terminal_GSM.Text(22, 75, CYAN, String(this->Operator.Cell_ID, HEX));

								// Command Status
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);

							#endif

						} else break;

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("Updating RTC"));
						#endif

						// CCLK Command (Real Time Clock Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CCLK"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CCLK(this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;
		
						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(17, 75, CYAN, String(this->Operator.Connection_Time));
						#endif

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 4, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 4, CYAN, F("PostOffice Connected"));
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						#endif

					} else {

						// Initialize Modem
						this->Initialize();

					}

					// Handle WatchDog
					if (_WD > 4) {

						// Clear States
						this->Status.SIM_Inserted = false;
						this->Status.SIM_PIN = 0;
						this->Status.Initialize = false;
						this->Status.Connection = false;

						// Turn Off Modem
						Hardware::Power(false);

						// End Function
						return(false);

					}

					// Set WatchDog Variable
					_WD++;

				}
								
				// End Function
				return(true);

			}

			// Configure Socket for Listen
			bool Listen(const bool _State) {

				// Check Connection Status
				if (this->Status.Connection) {

					// Clear Socket Status
					this->Status.Socket_State = 0;

					// Print Command State
					#ifdef DEBUG
						Terminal_GSM.Text(14, 34, WHITE, F("[    ]"));
					#endif

					// Print Command State
					#ifdef DEBUG
						Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
						Terminal_GSM.Text(14, 4, WHITE, F("AT#SS=2"));
						Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
					#endif

					// Send Command
					AT_Command_Set::SS(2, this->Status.Socket_State);

					// Print Command State
					#ifdef DEBUG
						Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
					#endif

					// Print Command State
					#ifdef DEBUG

						// Print Socket State
						Terminal_GSM.Text(14, 44, CYAN, F("Socket [2] :                        "));

						// Print Socket State
						if (this->Status.Socket_State == 0) Terminal_GSM.Text(14, 57, RED, F("Closed"));
						else if (this->Status.Socket_State == 1) Terminal_GSM.Text(14, 57, GREEN, F("Active Transfer"));
						else if (this->Status.Socket_State == 2) Terminal_GSM.Text(14, 57, GREEN, F("Suspended"));
						else if (this->Status.Socket_State == 3) Terminal_GSM.Text(14, 57, GREEN, F("Pending Data"));
						else if (this->Status.Socket_State == 4) Terminal_GSM.Text(14, 57, GREEN, F("Listening"));

					#endif

					// Activate Socket
					if (_State and this->Status.Socket_State != 4) {

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
							Terminal_GSM.Text(14, 4, WHITE, F("AT#SL=2,1,80,255"));
							Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
						#endif

						// Activate Socket for Listen
						bool _SL_Command = AT_Command_Set::SL(2, 1, 80, 255);

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.OK_Decide(_SL_Command, 14, 35);
						#endif

						// Command Delay
						delay(50);

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
							Terminal_GSM.Text(14, 4, WHITE, F("AT#SS=2"));
							Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
						#endif

						// Send Command
						bool _SS_Command = AT_Command_Set::SS(2, this->Status.Socket_State);

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.OK_Decide(_SS_Command, 14, 35);
						#endif

						// Print Command State
						#ifdef DEBUG
							
							// Print Socket State
							Terminal_GSM.Text(14, 44, CYAN, F("Socket [2] :                        "));
							
							// Print Socket State
							if (this->Status.Socket_State == 0) Terminal_GSM.Text(14, 57, RED, F("Closed"));
							else if (this->Status.Socket_State == 1) Terminal_GSM.Text(14, 57, GREEN, F("Active Transfer"));
							else if (this->Status.Socket_State == 2) Terminal_GSM.Text(14, 57, GREEN, F("Suspended"));
							else if (this->Status.Socket_State == 3) Terminal_GSM.Text(14, 57, GREEN, F("Pending Data"));
							else if (this->Status.Socket_State == 4) Terminal_GSM.Text(14, 57, GREEN, F("Listening"));

						#endif

						// Control Socket
						if (this->Status.Socket_State != 4) return(false);

						// End Function
						return(true);

					}

					// DeActivate Socket
					if (!_State and this->Status.Socket_State != 0) {

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
							Terminal_GSM.Text(14, 4, WHITE, F("AT#SL=2,0,80,255"));
							Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
						#endif

						// DeActivate Socket for Listen
						bool _SL_Command = AT_Command_Set::SL(2, 0, 80, 255);

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.OK_Decide(_SL_Command, 14, 35);
						#endif

						// Command Delay
						delay(50);

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
							Terminal_GSM.Text(14, 4, WHITE, F("AT#SS=2"));
							Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
						#endif

						// Get Socket Status
						AT_Command_Set::SS(2, this->Status.Socket_State);

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
						#endif

						// Print Command State
						#ifdef DEBUG
							
							// Print Socket State
							Terminal_GSM.Text(14, 44, CYAN, F("Socket [2] :                        "));
							
							// Print Socket State
							if (this->Status.Socket_State == 0) Terminal_GSM.Text(14, 57, RED, F("Closed"));
							else if (this->Status.Socket_State == 1) Terminal_GSM.Text(14, 57, GREEN, F("Active Transfer"));
							else if (this->Status.Socket_State == 2) Terminal_GSM.Text(14, 57, GREEN, F("Suspended"));
							else if (this->Status.Socket_State == 3) Terminal_GSM.Text(14, 57, GREEN, F("Pending Data"));
							else if (this->Status.Socket_State == 4) Terminal_GSM.Text(14, 57, GREEN, F("Listening"));

						#endif

						// Control Socket
						if (this->Status.Socket_State != 0) return(false);

						// End Function
						return(true);

					}

					// Print Command State
					#ifdef DEBUG
						Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
					#endif

					// End Function
					return(true);

				} else { 

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

	};


#endif /* defined(__PostMan__) */