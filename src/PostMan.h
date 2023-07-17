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

	// Define Library Includes
	#ifndef __PostMan_Hardware__
		#include "Hardware.h"
	#endif

	// Cloud Functions
	class PostMan : private AT_Command_Set, private Hardware {

		// Private Functions
		private:

			// Define CallBack Functions
			void (*_Send_Data_CallBack)(uint8_t);
			void (*_Send_Response_CallBack)(uint16_t, uint8_t);
			void (*_Command_CallBack)(uint16_t, char*);

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
							#ifdef DEBUG
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
								#ifdef DEBUG
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
								#ifdef DEBUG
									Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
								#endif

								// Set WD Variable
								_Conn_WD_Counter++;

								// Control for WD
								if (_Conn_WD_Counter > 300) _Conn_WD = true;

							}

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// Set CGREG Command (Set Network Registration Mode)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
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

						// Get CGREG Command (Get Network Registration Mode)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
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
								AT_Command_Set::CGREG(false, _CGREG_Connection_Mode, _CGREG_Connection_Stat);

								// Print Command State
								#ifdef DEBUG
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
								#ifdef DEBUG
									Terminal_GSM.Text(17, 75, CYAN, String((millis() - _Connection_Start_Time) / 1000));
								#endif

								// Set WD Variable
								_Conn_WD_Counter++;

								// Control for WD
								if (_Conn_WD_Counter > 300) _Conn_WD = true;

							}

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// CGDCONT Command (Set PDP Context)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CGDCONT=1,IP,mgbs"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CGDCONT(1, "IP", _PDP_APN_)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
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
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFG=3,1,1500,90,1200,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFG(3, 1, 1500, 90, 1200, 0)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT (Send Port) Command (Send Data Port Extended Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFGEXT=3,1,0,0,0,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT(3, 1, 0, 0, 0, 0)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT2 (Send Port) Command (Send Data Port Extended 2 Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFGEXT2=3,1,0,0,0,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT2(3, 1, 0, 0, 0, 0)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFG (In Port) Command (In Port Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFG=2,1,1500,90,300,50"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFG(2, 1, 1500, 90, 300, 50)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// SCFGEXT (In Port) Command (In Port Extended Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFGEXT=2,1,0,1,0,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SCFGEXT(2, 1, 0, 1, 0, 0)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
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
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FRWL=1,***"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_1_IP_)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// FRWL Command 2 (Firewall Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FRWL=1,***"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_2_IP_)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// FRWL Command 3 (Firewall Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FRWL=1,***"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::FRWL(1, _AT_FRWL_3_IP_)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
							#endif

						} else break;

						// ICMP Command (Ping Configuration)
						if (this->Status.Connection) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#ICMP=2"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::ICMP(2)) this->Status.Connection = false;

							// Print Command State
							#ifdef DEBUG
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

			// Clear Interrupt Function
			void Clear_Interrupt(uint8_t _Pack_Type) {

				// Clear Interrupt
				switch (_Pack_Type) {

					// Clear Online Interrupt
					case Pack_Types::Online: {

						// Clear Interrupt
						this->Interrupt.Online = false;

						// End Case
						break;

					}

					// Clear Timed Interrupt
					case Pack_Types::Timed: {

						// Clear Interrupt
						this->Interrupt.Timed = false;

						// End Case
						break;

					}

					// Clear Update Interrupt
					case Pack_Types::Update: {

						// Clear Interrupt
						this->Interrupt.Update = false;

						// End Case
						break;

					}

					// Clear Interrupt Interrupt
					case Pack_Types::Interrupt: {

						// Clear Interrupt
						this->Interrupt.Interrupt = false;

						// End Case
						break;

					}

					// Clear Alarm Interrupt
					case Pack_Types::Alarm: {

						// Clear Interrupt
						this->Interrupt.Alarm = false;

						// End Case
						break;

					}

					// Clear Offline Interrupt
					case Pack_Types::Offline: {

						// Clear Interrupt
						this->Interrupt.Offline = false;

						// End Case
						break;

					}

					// Clear FOTA_Info Interrupt
					case Pack_Types::FOTA_Info: {

						// Clear Interrupt
						this->Interrupt.FOTA_Info = false;

						// End Case
						break;

					}

					// Clear FOTA_Download Interrupt
					case Pack_Types::FOTA_Download: {

						// Clear Interrupt
						this->Interrupt.FOTA_Download = false;

						// End Case
						break;

					}

					// Clear FOTA_Burn Interrupt
					case Pack_Types::FOTA_Burn: {

						// Clear Interrupt
						this->Interrupt.FOTA_Burn = false;

						// End Case
						break;

					}

					// Clear Other Interrupt
					default: {

						// Clear All Interrupts
						this->Interrupt.Online = false;
						this->Interrupt.Update = false;
						this->Interrupt.Timed = false;
						this->Interrupt.Interrupt = false;
						this->Interrupt.Alarm = false;
						this->Interrupt.Offline = false;
						this->Interrupt.FOTA_Info = false;
						this->Interrupt.FOTA_Download = false;
						this->Interrupt.FOTA_Burn = false;

						// End Case
						break;

					}

				}

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
					_Response = AT_Command_Set::MONIZIP(this->Operator.Code, this->Operator.LAC, this->Operator.Cell_ID, this->Operator.dBm, this->Operator.Signal);

					// Set WD Variable
					_Error_WD++;

					// Control for WD
					if (_Error_WD > 5) break;

				}

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
				memset(this->JSON_Data.JSON_Pack, '\0', 1024);

				// Define JSON
				StaticJsonDocument<1024> JSON;

				// Set Device ID Variable
				if (_Pack_Type == Pack_Types::Online) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Online"));

				} else if (_Pack_Type == Pack_Types::Update) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Update"));

				} else if (_Pack_Type == Pack_Types::Timed) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Timed"));

				} else if (_Pack_Type == Pack_Types::Interrupt) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Interrupt"));

					// Get Data
				} else if (_Pack_Type == Pack_Types::Alarm) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Alarm"));

				} else if (_Pack_Type == Pack_Types::Offline) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Offline"));

				} else if (_Pack_Type == Pack_Types::FOTA_Info) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("FOTA_Download"));

				} else {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("UnKnown"));

				}

				// Get Data
				_Send_Data_CallBack(_Pack_Type);

				// Define Device Section
				JsonObject JSON_Device = JSON.createNestedObject(F("Device"));

				// Parse Info Segment
				#ifdef JSON_Segment_Info

					// Define Device Section
					JsonObject JSON_Info = JSON_Device.createNestedObject(F("Info"));

					// Set Device ID Variable
					JSON_Info[F("ID")] = this->JSON_Data.Device_ID;
					
					// Set Device Hardware Version Variable
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update) JSON_Info[F("Hardware")] = F(__Hardware__);

					// Set Device Firmware Version Variable
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update) JSON_Info[F("Firmware")] = F(__Firmware__);

				#endif

				// Parse Battery Segment
				#ifdef JSON_Segment_Battery

					// Define Power Section
					JsonObject JSON_Battery = JSON_Device["Power"].createNestedObject("Battery");

					// Battery Object
					I2C_Functions I2C_MAX17055(__I2C_Addr_MAX17055__, true, 4);

					// Create Charger Object
					MAX17055 Battery_Gauge(true, 4);

					// Set Battery Variables
					JSON_Battery[F("IV")] = Battery_Gauge.Instant_Voltage();
					JSON_Battery[F("AC")] = Battery_Gauge.Average_Current();
					JSON_Battery[F("SOC")] = Battery_Gauge.State_Of_Charge();
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update) JSON_Battery[F("T")] = Battery_Gauge.Temperature();
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update) JSON_Battery[F("FB")] = Battery_Gauge.Design_Capacity();
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update) JSON_Battery[F("IB")] = Battery_Gauge.Instant_Capacity();

					// Print Command State
					#ifdef DEBUG
						Terminal_GSM.Text(5, 113, CYAN, String(Battery_Gauge.Instant_Voltage(), 2));
						Terminal_GSM.Text(6, 112, CYAN, String(Battery_Gauge.Temperature(), 2));
						Terminal_GSM.Text(7, 110, CYAN, String(Battery_Gauge.Average_Current(), 2));
						Terminal_GSM.Text(8, 112, CYAN, String(Battery_Gauge.State_Of_Charge(), 2));
						Terminal_GSM.Text(9, 112, CYAN, String(Battery_Gauge.Design_Capacity()));
						Terminal_GSM.Text(10, 112, CYAN, String(Battery_Gauge.Instant_Capacity()));
					#endif

					// Charger
					I2C_Functions I2C_BQ24298(__I2C_Addr_BQ24298__, true, 5);

					// Create Charger Object
					BQ24298 Charger(false, true, 5);

					// Set Battery Variables
					JSON_Battery[F("Charge")] = Charger.Charge_Status();


				#endif

				// Parse Battery Segment
				#ifdef JSON_Segment_GSM

					// Define GSM Section
					JsonObject JSON_GSM = JSON_Device["IoT"].createNestedObject(F("GSM"));

					// Get GSM Parameters
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update) {

						// Define IoT Module
						JsonObject JSON_Module = JSON_GSM.createNestedObject(F("Module"));

						// Set IoT Parameters
						JSON_Module[F("Manufacturer")] = this->Module.Manufacturer;
						JSON_Module[F("Model")] = this->Module.Model;
						JSON_Module[F("Firmware")] = this->Module.Firmware;
						JSON_Module[F("Serial")] = this->Module.Serial_ID;
						JSON_Module[F("IMEI")] = this->Module.IMEI;

					}

					// Define GSM Operator Section
					JsonObject JSON_Operator = JSON_GSM.createNestedObject(F("Operator"));

					// Update Connection Variables
					this->Update_Connection_Variables();

					// Set Device GSM Connection Detail Section
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update) JSON_Operator[F("SIM_Type")] = 1;
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update)JSON_Operator[F("ICCID")] = this->Operator.ICCID;
					JSON_Operator[F("Code")] = this->Operator.Code;
					JSON_Operator[F("dBm")] = this->Operator.dBm;
					JSON_Operator[F("LAC")] = this->Operator.LAC;
					JSON_Operator[F("Cell_ID")] = this->Operator.Cell_ID;

				#endif

				// Parse Payload Segment
				#ifdef JSON_Segment_Payload

					// Detect RTC
					I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);

					// RTC Object Definitions	
					RV3028 RTC(true, 1);

					// Declare TimeStamp Variable
					char _TimeStamp[25];
					memset(_TimeStamp, '\0', 25);

					// Handle TimeStamp
					sprintf(_TimeStamp, "20%02hhu-%02hhu-%02hhu %02hhu:%02hhu:%02hhu", RTC.Get_Year(), RTC.Get_Month(), RTC.Get_Date(), RTC.Get_Hour(), RTC.Get_Minute(), RTC.Get_Second());

					// Define Data Section
					JsonObject JSON_Payload = JSON.createNestedObject(F("Payload"));

					// Set Device Time Variable
					JSON_Payload[F("TimeStamp")] = _TimeStamp;

					// Set Device ID Variable
					if (_Pack_Type == Pack_Types::Online) {

						// Get Environment
						this->Get_Environment();

						// Set Device Environment Variable
						JSON_Payload[F("PCBT")] = this->JSON_Data.JSON_Environment.Temperature;
						JSON_Payload[F("PCBH")] = this->JSON_Data.JSON_Environment.Humidity;

						// Set Device Status Variable
						JSON_Payload[F("Status")] = this->JSON_Data.JSON_Status.Device_State;

					} else if (_Pack_Type == Pack_Types::Update) {

						// Get Environment
						this->Get_Environment();

						// Set Device Environment Variable
						JSON_Payload[F("PCBT")] = this->JSON_Data.JSON_Environment.Temperature;
						JSON_Payload[F("PCBH")] = this->JSON_Data.JSON_Environment.Humidity;

						// Set Device Status Variable
						JSON_Payload[F("Status")] = this->JSON_Data.JSON_Status.Device_State;

					} else if (_Pack_Type == Pack_Types::Timed) {


					} else if (_Pack_Type == Pack_Types::Interrupt) {


					} else if (_Pack_Type == Pack_Types::Alarm) {


					} else if (_Pack_Type == Pack_Types::Offline) {


					} else if (_Pack_Type == Pack_Types::FOTA_Info) {

						// Set Device Environment Variable
						JSON_Payload[F("File_ID")] = this->FOTA_Variables.File_ID;
						JSON_Payload[F("Status")] = this->FOTA_Variables.Download_Status;
						JSON_Payload[F("SD_Size")] = this->FOTA_Variables.SD_File_Size;
						JSON_Payload[F("Download_Time")] = this->FOTA_Variables.Download_Time;

					} else {


					}

				#endif

				// Clear Unused Data
				JSON.garbageCollect();

				// Serialize JSON	
				uint16_t _JSON_Size = serializeJson(JSON, this->JSON_Data.JSON_Pack);

				// Print Command State
				#ifdef DEBUG
					Terminal_GSM.Text(25, 4, WHITE,String(this->JSON_Data.JSON_Pack).substring(0, 75));
					Terminal_GSM.Text(26, 4, WHITE,String(this->JSON_Data.JSON_Pack).substring(75, 150));
					Terminal_GSM.Text(27, 4, WHITE,String(this->JSON_Data.JSON_Pack).substring(150, 225));
					Terminal_GSM.Text(28, 4, WHITE,String(this->JSON_Data.JSON_Pack).substring(225, 300));
					Terminal_GSM.Text(29, 4, WHITE,String(this->JSON_Data.JSON_Pack).substring(300, 375));
					Terminal_GSM.Text(30, 4, WHITE,String(this->JSON_Data.JSON_Pack).substring(375, 450));
					Terminal_GSM.Text(31, 4, WHITE,String(this->JSON_Data.JSON_Pack).substring(450, 525));
				#endif

				// End Function
				return(_JSON_Size);

			}














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

			// Serial ID Read Function
			void Get_Serial_ID(char * _Serial_ID) {
				
				// Define Variable
				uint64_t _Serial = 0x00;
				uint8_t _Read_Byte;

				// Define I2C Device
				I2C_Functions I2C_DS28C(__I2C_Addr_DS28C__, true, 2);

				// Set DS28C to I2C Mode
				I2C_DS28C.Write_Register(0x08, 0x01, false);

				// Send CRC  Read Request to DS28C and read
				_Read_Byte = I2C_DS28C.Read_Register(0x07);
				_Serial |= (uint64_t)_Read_Byte;

				// Send 40-47 bit Read Request to DS28C and read
				_Read_Byte = I2C_DS28C.Read_Register(0x06);
				_Serial = _Serial << 8;
				_Serial |= (uint64_t)_Read_Byte;

				// Send 32-39 bit Read Request to DS28C and read
				_Read_Byte = I2C_DS28C.Read_Register(0x05);
				_Serial = _Serial << 8;
				_Serial |= (uint64_t)_Read_Byte;

				// Send 24-31 bit Read Request to DS28C and read
				_Read_Byte = I2C_DS28C.Read_Register(0x04);
				_Serial = _Serial << 8;
				_Serial |= (uint64_t)_Read_Byte;

				// Send 16-23 bit Read Request to DS28C and read
				_Read_Byte = I2C_DS28C.Read_Register(0x03);
				_Serial = _Serial << 8;
				_Serial |= (uint64_t)_Read_Byte;

				// Send 08-15 bit Read Request to DS28C and read
				_Read_Byte = I2C_DS28C.Read_Register(0x02);
				_Serial = _Serial << 8;
				_Serial |= (uint64_t)_Read_Byte;

				// Send 00-07 bit Read Request to DS28C and read
				_Read_Byte = I2C_DS28C.Read_Register(0x01);
				_Serial = _Serial << 8;
				_Serial |= (uint64_t)_Read_Byte;

				// Send Device Family bit Read Request to DS28C and read
				_Read_Byte = I2C_DS28C.Read_Register(0x00);
				_Serial = _Serial << 8;
				_Serial |= (uint64_t)_Read_Byte;

				// Set Array
				String(uint64ToString(_Serial)).toCharArray(_Serial_ID, 17);

			}

			// Environment Read Function
			void Get_Environment(void) {
				
				// Define Sensor Object
				HDC2010 _Sensor(true, 3, 10, true);

				// Set Device Environment Variable
				this->JSON_Data.JSON_Environment.Temperature = _Sensor.Temperature();
				this->JSON_Data.JSON_Environment.Humidity = _Sensor.Humidity();

				// Print Command State
				#ifdef DEBUG
					Terminal_GSM.Text(8, 72, CYAN, String(_Sensor.Temperature(), 2));
					Terminal_GSM.Text(9, 72, CYAN, String(_Sensor.Humidity(), 2));
				#endif

			}

			// Reset Function
			void(* Reset) (void) = 0;

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

			// Define Variables
			struct Struct_FOTA {
				uint32_t		File_ID				= 0;
				uint32_t		Download_Time		= 0;
				uint32_t		File_Size			= 0;
				uint32_t		SD_File_Size		= 0;
				uint8_t 		Download_Status		= 0;
			} FOTA_Variables;

			// Define Time Structure
			struct Struct_Time {
				uint16_t 	Year				= 0;
				uint16_t 	Month				= 0;
				uint16_t 	Day					= 0;
				uint16_t 	Hour				= 0;
				uint16_t 	Minute				= 0;
				uint16_t 	Second				= 0;
			} Time;

			// Define Modem Interrupt Structure
			struct Interrupt_Status {

				// Send Interrupt
				bool 		Online				= false;
				bool 		Update				= false;
				bool 		Timed				= false;
				bool 		Interrupt			= false;
				bool 		Alarm				= false;
				bool 		Offline				= false;
				bool 		FOTA_Info			= false;
				bool 		FOTA_Download		= false;
				bool 		FOTA_Burn			= false;

				// Ring Interrupt
				bool 		Ring				= false;

			} Interrupt;

			// Define JSON Status Structure
			struct JSON_Device_Structure {

				// Device ID
				char Device_ID[17];

				// Define JSON Environment Structure
				struct JSON_Environment_Structure {
					float Temperature;
					float Humidity;
				} JSON_Environment;

				// Define JSON Status Structure
				struct JSON_Status_Structure {
					uint32_t Device_State = 0;
					uint32_t Publish_Mask = 0;
					uint32_t Stop_Mask = 0;
				} JSON_Status;

				// Define JSON
				char JSON_Pack[1024];

			} JSON_Data;

			// PostMan Constructor
			PostMan(Stream &_Serial) : AT_Command_Set(_Serial) {

				// Clear Interrupt
				this->Clear_Interrupt(0);

			}

			// CallBack Definitions
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

				// Get Serial ID
				this->Get_Serial_ID(this->JSON_Data.Device_ID);

				// Print Command State
				#ifdef DEBUG
					Terminal_GSM.Text(5, 63, GREEN, String(this->JSON_Data.Device_ID));
				#endif

				// Get Environment
				this->Get_Environment();

				// GSM Connection Squence
				if (Hardware::Power(false)) {

					// GSM Initialize Sequence
					if (this->Initialize()) {

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("GSM Initialized"));
						#endif

						// GSM Connect Sequence
						if (this->Connect()) {

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
								Terminal_GSM.Text(14, 44, CYAN, F("GSM Connected"));
							#endif

							// Define RTC
							I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);

							// RTC Object Definitions	
							RV3028 RTC(true, 1);

							// Update Time
							RTC.Set_Time(this->Time.Second, this->Time.Minute, this->Time.Hour, this->Time.Day, this->Time.Month, this->Time.Year);

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
								Terminal_GSM.Text(14, 44, CYAN, F("Device Time Updated"));
							#endif

							// GSM Socket Open Sequence
							if (this->Listen(true)) {

								// Print Command State
								#ifdef DEBUG
									Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
									Terminal_GSM.Text(14, 44, CYAN, F("GSM Socket Listening"));
								#endif

							} else {

								// Print Command State
								#ifdef DEBUG
									Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
									Terminal_GSM.Text(14, 44, RED, F("GSM Socket Open Error"));
								#endif
							}

							// Publish Interrupt Status
							this->Interrupt.Online = true;

						} else {
							
							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
								Terminal_GSM.Text(14, 44, RED, F("GSM Connect Error"));
							#endif

							// Power Down GSM
							Hardware::Power(false);

						}

					} else {

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, RED, F("GSM Initialize Error"));
						#endif

						// Power Down GSM
						Hardware::Power(false);

					}

				} else {

					// Print Command State
					#ifdef DEBUG
						Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						Terminal_GSM.Text(14, 44, RED, F("GSM Power Down Error"));
					#endif

				}

			}

			// ************************************************************

			// Send Data Batch Function
			bool Publish(const uint8_t _Pack_Type) {

				// Control for Connection
				if (this->Status.Connection) {

					// Open Connection
					if (AT_Command_Set::ATSD(3, 0, 80, 255, 88, 1, _BackEnd_Server_)) {

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, GREEN, F("Open Connection"));
						#endif

						// Parse JSON
						this->Parse_JSON(_Pack_Type);

						// Send Data
						AT_Command_Set::SSEND(3, 2, 0, _BackEnd_Server_, _BackEnd_EndPoint_, this->JSON_Data.JSON_Pack);

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, GREEN, F("Pack Sended"));
						#endif

						// Declare Ring Status
						uint16_t _Length;

						// Get Ring Port
						if (AT_Command_Set::Send_SRING(_Length)) {

							// Declare Response Variable
							char _Response[32];
							memset(_Response, '\0', 32);
							
							// Declare Response Command
							uint16_t _Response_Command;

							// Command Delay
							delay(50);

							// Get Request Data
							if (AT_Command_Set::SRECV(3, _Length, _Response)) {

								// Define JSON Object
								StaticJsonDocument<32> Incoming_JSON;

								// Deserialize the JSON document
								deserializeJson(Incoming_JSON, _Response);
								
								// Get Response Command
								_Response_Command = Incoming_JSON["Event"];

								// Print Command State
								#ifdef DEBUG
									Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
									Terminal_GSM.Text(14, 44, GREEN, F("Response --> [   ]"));
									Terminal_GSM.Text(14, 58, YELLOW, String(_Response_Command));
								#endif

								// Command Delay
								delay(50);

								// Close Socket
								AT_Command_Set::SH(3);

								// Port Control
								this->Listen(true);

								// Clear Interrupt
								this->Clear_Interrupt(_Pack_Type);

								// Send Data CallBack
								_Send_Response_CallBack(_Response_Command, 0);

								// Clear JSON Print
								#ifdef DEBUG
									for (size_t i = 3; i < 79; i++) {
										Terminal_GSM.Text(25, i, CYAN, F(" "));
										Terminal_GSM.Text(26, i, CYAN, F(" "));
										Terminal_GSM.Text(27, i, CYAN, F(" "));
										Terminal_GSM.Text(28, i, CYAN, F(" "));
										Terminal_GSM.Text(29, i, CYAN, F(" "));
										Terminal_GSM.Text(30, i, CYAN, F(" "));
										Terminal_GSM.Text(31, i, CYAN, F(" "));
									}
								#endif

								// Handle Response
								if (_Response_Command == 200) {

									// End Function
									return(true);

								} else if (_Response_Command == 201) {

									// Print Command State
									#ifdef DEBUG
										Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
										Terminal_GSM.Text(14, 44, RED, F("Wrong Data Format"));
									#endif

									// End Function
									return(true);

								}

							} else {

								// Send Data CallBack Error
								_Send_Response_CallBack(0, 1);

								// Print Command State
								#ifdef DEBUG
									Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
									Terminal_GSM.Text(14, 44, GREEN, F("Server Don't Response"));
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

						// Send Data CallBack Error
						_Send_Response_CallBack(0, 3);

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, RED, F("Socket Dial Error"));
						#endif

						// Port Control
						this->Listen(true);

						// Clear Interrupt
						this->Clear_Interrupt(_Pack_Type);

						// End Function
						return(false);

					}

					// End Function
					return(true);

				} else {

					// Print Command State
					#ifdef DEBUG
						Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						Terminal_GSM.Text(14, 44, RED, F("No Connection"));
					#endif

					// LOG JSON Data
					this->JSON_LOG();

					// Send Data CallBack Error
					_Send_Response_CallBack(0, 4);

					// Clear Interrupt
					this->Clear_Interrupt(_Pack_Type);

					// End Function
					return(false);

				}

			}

			// Get Server Command Function
			void Get(void) {

				// Control for Connection
				if (this->Status.Connection) {

					// Handle Ring
					if (Receive_SRING()) {

						// Declare Request Length
						uint16_t _Request_Length;

						// Answer Socket
						AT_Command_Set::SA(2, 1, _Request_Length);

						// Declare Variable
						char _JSON_Data[Recieve_JSON_Size];

						// Get Request Data
						AT_Command_Set::SRECV(2, _Request_Length, _JSON_Data);





						// Declare Variable
						uint16_t _Event = 0;

						// Declare JSON Object
						StaticJsonDocument<Recieve_JSON_Size> Incoming_JSON;

						// Deserialize the JSON document
						DeserializationError Error = deserializeJson(Incoming_JSON, _JSON_Data);

						// Handle JSON
						if (!Error) _Event = Incoming_JSON["Request"]["Event"];






						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, GREEN, F("Response --> [   ]"));
							Terminal_GSM.Text(14, 58, YELLOW, String(_Event));
						#endif

						// Declare Response
						uint8_t _Response = Command_NOK;

						// Handle Command
						if (_Event == Command_Reset) {

							// Send Response
							this->Response(200, Command_OK);

							// Reset
							Reset();

						} else if (_Event == Command_Update) {

							// Send Response
							this->Response(200, Command_OK);

							// Set Command Interrupt
							this->Interrupt.Update = true;

						} else if (_Event == Command_Parameter) {

							// Declare JSON Object
							StaticJsonDocument<64> Incoming_JSON;

							// Deserialize the JSON document
							deserializeJson(Incoming_JSON, _JSON_Data);

							// Handle JSON
							uint8_t _Address = Incoming_JSON["Request"]["Address"];
							uint16_t _Value = Incoming_JSON["Request"]["Value"];

							// Control for Address
							if ((_Address & 0x01) == 0) {

								// 0x1234
								// HB   LB
								// 0x12 0x34

								// Handle Low & High Byte
								uint8_t _Low_Byte = lowByte(_Value);
								uint8_t _High_Byte = highByte(_Value);

								// RTC Object Definitions	
								I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);
								RV3028 RTC(true, 1);

								// Update EEPROM
								bool _Response_High = RTC.Write_EEPROM(_Address, _High_Byte);
								bool _Response_Low = RTC.Write_EEPROM(_Address + 0x01, _Low_Byte);

								// Set Response Code
								if (_Response_High and _Response_Low) _Response = Command_OK;

							} else {

								// Declare Response
								_Response = Command_NOK;

							}

							// Send Response
							this->Response(200, _Response);

						} else if (_Event == Command_FOTA_Download) {

							// Get File ID
							this->FOTA_Variables.File_ID = Incoming_JSON["Request"]["Firmware"];

							// Set Command Interrupt
							this->Interrupt.FOTA_Download = true;

							// Send Response
							this->Response(200, Command_OK);

						} else if (_Event == Command_FOTA_Burn) {

							// Send Response
							this->Response(200, Command_OK);

							// Set Command Interrupt
							this->Interrupt.FOTA_Burn = true;

						} else {

							// Send Data CallBack
							_Command_CallBack(_Event, _JSON_Data);

						}

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						#endif

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
						Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						Terminal_GSM.Text(14, 44, GREEN, F("Response Sended"));
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
			bool Download(const uint16_t _File_ID) {

				// Check Connection Status
				if (this->Status.Connection) {

					// SD Message
					#ifdef DEBUG
						Terminal_GSM.Text(14, 44, GREEN, F("                               "));
						Terminal_GSM.Text(14, 44, GREEN, F("Firmware download started."));
					#endif

					// Activate SD Mux
					DDRC |= 0b00000001; PORTC |= 0b00000001;
					
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
							Terminal_GSM.Text(14, 44, GREEN, F("                               "));
							Terminal_GSM.Text(14, 44, GREEN, F("File Exist and Deleted."));
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
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPOPEN=***,***,***"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
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
								Terminal_GSM.OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->FOTA_Variables.Download_Status = FOTA_FTP_Open_Error;

							}

						}

						// Set FTP Type
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPTYPE=0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
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
								Terminal_GSM.OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->FOTA_Variables.Download_Status = FOTA_FTP_Type_Error;

							}

						}

						// Set FTP Folder
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPCWD=\"firmware\""));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
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
								Terminal_GSM.OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->FOTA_Variables.Download_Status = FOTA_FTP_Folder_Error;

							}

						}

						// Get File Size
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPFSIZE=\"xxx.hex\""));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPFSIZE="6.hex"\r\n

							// Process Command
							while (!_Response) {

								// Send Command
								_Response = AT_Command_Set::FTPFSIZE(_File_ID, this->FOTA_Variables.File_Size);

								// Set WD Variable
								_WD++;

								// Control for WD
								if (_WD > 4) break;

							}

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->FOTA_Variables.Download_Status = FOTA_FTP_File_Size_Error;

							}

						}

						// Get File
						if (_State) {

							// Reset Control Variables
							uint8_t _WD = 0;
							bool _Response = false;

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPGETPKT=\"xxx.hex\",0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPGETPKT="6.hex",0\r\n

							// Process Command
							while (!_Response) {

								// Send Command
								_Response = AT_Command_Set::FTPGETPKT(_File_ID, 0);

								// Set WD Variable
								_WD++;

								// Control for WD
								if (_WD > 4) break;

							}

							// Print Command State
							#ifdef DEBUG
								Terminal_GSM.OK_Decide(_Response, 14, 35);
							#endif

							// Set Function Variable
							if (!_Response) {

								// Set State Variable
								_State = false;

								// Set Download Status
								this->FOTA_Variables.Download_Status = FOTA_FTP_Get_Error;

							}

						}

						#ifdef DEBUG
							Terminal_GSM.Text(17, 113, GREEN, String(_File_ID));
							Terminal_GSM.Text(19, 112, GREEN, String(this->FOTA_Variables.File_Size));
						#endif

						// SD Message
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, GREEN, F("                               "));
							Terminal_GSM.Text(14, 44, GREEN, F("SD Opened"));
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
							Terminal_GSM.Text(14, 44, GREEN, F("                               "));
							Terminal_GSM.Text(14, 44, GREEN, F("Downloading"));
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
								if (_SD_Recieve_Size == this->FOTA_Variables.File_Size) _Response = true;

								// Work Delay
								delay(50);

								// Calculate Download Time
								this->FOTA_Variables.Download_Time = (millis() - _Download_Start_Time) / 1000;

								// SD Message
								#ifdef DEBUG
									
									// Print File Size
									Terminal_GSM.Text(21, 114, CYAN, String("   "));
									Terminal_GSM.Text(21, 114, CYAN, String(map(_SD_Recieve_Size, 0, this->FOTA_Variables.File_Size, 0, 100)));

									// Print Download Time
									Terminal_GSM.Text(22, 111, CYAN, String("    "));
									Terminal_GSM.Text(22, 111, CYAN, String(this->FOTA_Variables.Download_Time));

									Terminal_GSM.Text(20, 112, WHITE, F("       "));
									Terminal_GSM.Text(20, 112, WHITE, String(_SD_Recieve_Size));

								#endif

								// End Function
								if (this->FOTA_Variables.Download_Time > 1200) {

									// Set Download Status
									this->FOTA_Variables.Download_Status = FOTA_Download_TimeOut;
	
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
								Terminal_GSM.Text(14, 44, GREEN, F("                               "));
								Terminal_GSM.Text(14, 44, GREEN, F("SD Closed"));
							#endif

						}

						// Reset Control Variables
						uint8_t _WD = 0;
						bool _Response = false;

						// Print Command State
						#ifdef DEBUG
							Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
							Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPCLOSE"));
							Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
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
							Terminal_GSM.OK_Decide(_Response, 14, 35);
						#endif

						// Set Function Variable
						if (!_Response) {

							// Set State Variable
							_State = false;

							// Set Download Status
							this->FOTA_Variables.Download_Status = FOTA_FTP_Close_Error;

						}

					} else {

						// SD Message
						#ifdef DEBUG
							Terminal_GSM.Text(14, 44, GREEN, F("                               "));
							Terminal_GSM.Text(14, 44, RED, F("File Not Opened"));
						#endif

						// Set Download Status
						this->FOTA_Variables.Download_Status = FOTA_SD_Error;

					}

					// FOTA Download Status
					#ifdef DEBUG
						Terminal_GSM.Text(17, 113, GREEN, F("      "));
						Terminal_GSM.Text(18, 115, GREEN, F("    "));
						Terminal_GSM.Text(19, 112, GREEN, F("       "));
						Terminal_GSM.Text(20, 112, GREEN, F("       "));
						Terminal_GSM.Text(21, 114, GREEN, F("    "));
						Terminal_GSM.Text(22, 111, GREEN, F("     "));
					#endif

					// Control for Existing File
					if (SD.exists(_FOTA_SD_FileName_)) {

						// Open File for Write
						SD_File = SD.open(_FOTA_SD_FileName_, FILE_READ);

						// Get File Size
						this->FOTA_Variables.SD_File_Size = SD_File.size();

						// Control for File Size
						if (this->FOTA_Variables.File_Size == this->FOTA_Variables.SD_File_Size) {

							// SD Message
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(true, 18, 115);
								Terminal_GSM.Text(19, 112, GREEN, String(this->FOTA_Variables.SD_File_Size));
							#endif

							// Turn SD MUX Enable LOW
							PORTC &= 0b11111110;

							// Set Download Status
							this->FOTA_Variables.Download_Status = FOTA_Download_OK;

							// End Function
							return(true);

						} else {

							// SD Message
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(false, 18, 115);
								Terminal_GSM.Text(19, 112, RED, String(this->FOTA_Variables.SD_File_Size));
							#endif

							// Turn SD MUX Enable LOW
							PORTC &= 0b11111110;

							// Set Download Status
							this->FOTA_Variables.Download_Status = FOTA_FTP_File_Size_Error;

							// End Function
							return(false);

						}

						// End Function
						return(false);

					} else {

						// Turn SD MUX Enable LOW
						PORTC &= 0b11111110;

						// Set Download Status
						this->FOTA_Variables.Download_Status = FOTA_Download_Not_Save;

						// End Function
						return(false);

					}

				}

				// Set Download Status
				this->FOTA_Variables.Download_Status = 0;

				// End Function
				return(false);

			}

			// ************************************************************

			// LOG JSON Pack Function
			void JSON_LOG(void) {

				// Declare LOG File Object
				File LOG_File;

				// Activate Mux
				DDRC |= 0b00000001; PORTC |= 0b00000001; delay(200);

				// Open File for Write
				LOG_File = SD.open(_LOG_SD_File_Name_, O_WRITE | O_CREAT);

				// Control for File Open
				if (LOG_File) {

					// Command Delay
					delay(5);

					// Flush File
					LOG_File.flush();

					// Print Data
					LOG_File.println(this->JSON_Data.JSON_Pack);

					// Print Line Feed
					LOG_File.println("");

					// Command Delay
					delay(10);

					// Close File
					LOG_File.close();

					// Console Print
					#ifdef DEBUG
						Terminal_GSM.Text(14, 44, RED, F("                               "));
						Terminal_GSM.Text(14, 44, RED, F("Error : Pack Writen to SD Card."));
					#endif

					// Clear Pack
					memset(this->JSON_Data.JSON_Pack, '\0', 1024);

				}

				// Turn SD MUX Enable LOW
				PORTC &= 0b11111110;

			}

	};

#endif /* defined(__PostMan__) */