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
				bool 		Initialize		 	= false;
				bool		Connection			= false;
			} Status;

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

					// Define Terminal Row Variable
					uint8_t GSM_Console_Initialize_ROW = GSM_Console_Initialize_X;

					// Get PowerMon
					Hardware::Status.Power = Hardware::PowerMonitor();

					// Control for Power Monitor
					if (Hardware::Status.Power) {

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 34, WHITE, F("[    ]"));
						#endif

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("Initializing Modem"));
						#endif

						// Set Control Variable
						this->Status.Initialize = true;

						// AT Command
						#ifdef _AT_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::AT()) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// SEARCHLIM Command
						#ifdef _AT_SEARCHLIM_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SEARCHLIM=100,100"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SEARCHLIM()) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// ATF Command
						#ifdef _AT_ATF_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT&F0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::ATF(1)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// CFUN Command
						#ifdef _AT_CFUN_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CFUN=1,0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CFUN(1)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// ATE Command
						#ifdef _AT_ATE_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("ATE0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::ATE(0)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// CMEE Command
						#ifdef _AT_CMEE_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CMEE=1"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CMEE(1)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// FCLASS Command
						#ifdef _AT_FCLASS_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+FCLASS=0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::FCLASS(0)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// K Command
						#ifdef _AT_K_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT&K0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::K(0)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// CPIN Command
						#ifdef _AT_CPIN_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CPIN?"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CPIN()) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif

							// End Function
							if (!this->Status.Initialize) return(false);
							if (Modem.PIN != 1) return(false);

							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// CGSN Command
						#ifdef _AT_CGSN_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CGSN"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CGSN(Modem.IMEI)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								Terminal_GSM.Text(20, 24, CYAN, String(Modem.IMEI));
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// GSN Command
						#ifdef _AT_GSN_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+GSN"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::GSN()) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								Terminal_GSM.Text(21, 29, CYAN, String(AT_Command_Set::Modem.Serial_ID));
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// ICCID Command
						#ifdef _AT_ICCID_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+CCID"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::CCID(Modem.ICCID)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								Terminal_GSM.Text(22, 20, CYAN, String(Modem.ICCID));
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// GMI Command
						#ifdef _AT_GMI_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+GMI"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::GMI()) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14,35);
								Terminal_GSM.Text(17, 38, CYAN, String(AT_Command_Set::Modem.Manufacturer));
							#endif

							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// GMM Command
						#ifdef _AT_GMM_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+GMM"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::GMM()) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								Terminal_GSM.Text(18, 38, CYAN, String(AT_Command_Set::Modem.Model));
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// GMR Command
						#ifdef _AT_GMR_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT+GMR"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::GMR()) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug

								// Declare Variable
								char _Firmware[10];

								// Handle TimeStamp
								sprintf(_Firmware, "%02d.%02d.%03d", AT_Command_Set::Modem.Firmware.Segment_1, AT_Command_Set::Modem.Firmware.Segment_2, AT_Command_Set::Modem.Firmware.Segment_3);

								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
								Terminal_GSM.Text(19, 30, CYAN, String(_Firmware));
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// SLED Command
						#ifdef _AT_SLED_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SLED=2"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SLED(2)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// SLEDSAV Command
						#ifdef _AT_SLEDSAV_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#SLEDSAV"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::SLEDSAV()) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// E2SLRI Command
						#ifdef _AT_E2SLRI_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#E2SLRI=50"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::E2SLRI(50)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

						// TXMONMODE Command
						#ifdef _AT_TXMONMODE_

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#TXMONMODE=1"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// Send Command
							if (!AT_Command_Set::TXMONMODE(1)) this->Status.Initialize = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(this->Status.Initialize, 14, 35);
							#endif
						
							// Set Variable
							GSM_Console_Initialize_ROW += 1;

						#endif

					} else {

						// Power ON Modem
						Hardware::Power(true);

					}

					// Handle WatchDog
					if (_WD > 4) return(false);

					// Set WatchDog Variable
					_WD++;

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

					// Define Terminal Row Variable
					uint8_t GSM_Console_Connect_ROW = GSM_Console_Connect_X;

					// Control for Initialize
					if (this->Status.Initialize) {

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 34, WHITE, F("[    ]"));
						#endif

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("Connecting"));
						#endif

						// Set Control Variable
						this->Status.Connection = true;

						// REGMODE Command
						#ifdef _AT_REGMODE_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT#REGMODE=1"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::REGMODE(1)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}
							
						#endif

						// AUTOBND Command
						#ifdef _AT_AUTOBND_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT#AUTOBND=2"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::AUTOBND(2)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// COPS Command
						#ifdef _AT_COPS_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT+COPS=0,2,28601"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::COPS(0, 2, 28601)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// Get Time
						uint32_t _Connection_Start_Time = millis();

						// CREG Command
						#ifdef _AT_CREG_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT+CREG=0"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::Set_CREG(0)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

							// Control for Ex Commands
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
									AT_Command_Set::Get_CREG(_CREG_Connection_Mode, _CREG_Connection_Stat);

									// Print Command State
									#ifdef GSM_Debug
										Terminal_GSM.Text(14, 35, CYAN, F("    "));
										Terminal_GSM.Text(14, 36, RED, String(_CREG_Connection_Stat));
									#endif

									// Control for Connection
									if (_CREG_Connection_Stat == 0) {

										// Set Variable
										this->Status.Connection = false;

										// CFUN Function
//										AT_Command_Set::CFUN(4);

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

										// Wait Delay
										delay(1000);

									} else if (_CREG_Connection_Stat == 3) {

										// Set Variable
										this->Status.Connection = false;

										// Declare Response Status
										_Conn_WD = false;

										// Ask For CEER Code
										uint16_t _CeerCode = 0;
										AT_Command_Set::CEER(_CeerCode);

										// Print Command State
										#ifdef GSM_Debug
											Terminal_GSM.Text(GSM_Console_Connect_ROW, GSM_Console_Connect_Y + 19, WHITE, F("[CEER:   ]"));
											Terminal_GSM.Text(GSM_Console_Connect_ROW, GSM_Console_Connect_Y + 25, RED, String(_CeerCode));
										#endif

										// Send Command
//										AT_Command_Set::AT_Test();

										// Send Command
//										AT_Command_Set::AUTOBND(2);

										// Wait Delay
										delay(3000);

									} else if (_CREG_Connection_Stat == 4) {

										// Set Variable
										this->Status.Connection = false;

										// Declare Response Status
										_Conn_WD = false;

										// Wait Delay
										delay(1000);

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
									if (_Conn_WD_Counter > 100) _Conn_WD = true;

								}

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// CGREG Command
						#ifdef _AT_CGREG_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT+CGREG=0"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::Set_CGREG(0)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

							// Control for Ex Commands
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
									AT_Command_Set::Get_CGREG(_CGREG_Connection_Mode, _CGREG_Connection_Stat);

									// Print Command State
									#ifdef GSM_Debug
										Terminal_GSM.Text(14, 35, CYAN, F("    "));
										Terminal_GSM.Text(14, 36, RED, String(_CGREG_Connection_Stat));
									#endif

									// Control for Connection
									if (_CGREG_Connection_Stat == 0) {

										// Set Variable
										this->Status.Connection = false;

										// Declare Response Status
										_Conn_WD = false;

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

										// Wait Delay
										delay(1000);

									} else if (_CGREG_Connection_Stat == 3) {

										// Set Variable
										this->Status.Connection = false;

										// Declare Response Status
										_Conn_WD = false;

										// Wait Delay
										delay(10000);

									} else if (_CGREG_Connection_Stat == 4) {

										// Set Variable
										this->Status.Connection = false;

										// Declare Response Status
										_Conn_WD = false;

										// Wait Delay
										delay(1000);

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
									if (_Conn_WD_Counter > 100) _Conn_WD = true;

								}

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// CGDCONT Command Socket
						#ifdef _AT_CGDCONT_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT++CGDCONT=1,**,**,**,0,0"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::CGDCONT(1, "IP", "mgbs", "0.0.0.0", 0, 0)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// SGACT Command
						#ifdef _AT_SGACT_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT#SGACT=1,1"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::Set_SGACT(1, 1)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug

									// Declare Variable
									char _IP[16];

									// Handle TimeStamp
									sprintf(_IP, "%03d.%03d.%03d.%03d", AT_Command_Set::Modem.IP_Address.Segment_1, AT_Command_Set::Modem.IP_Address.Segment_2, AT_Command_Set::Modem.IP_Address.Segment_3, AT_Command_Set::Modem.IP_Address.Segment_4);

									Terminal_GSM.Text(20, 64, CYAN, String(_IP));
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// Set Connection Time
						Modem.Connection_Time = ((millis() - _Connection_Start_Time) / 1000);

						// SCFG (Send Port) Command
						#ifdef _AT_SCFG_Out_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFG=3,1,1500,90,300,50"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::SCFG(3, 1, 1500, 90, 1200, 0)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// SCFGEXT (Send Port) Command
						#ifdef _AT_SCFGEXT_Out_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT#SCFGEXT=3,1,0,1,0,0"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::SCFGEXT(3, 1, 0, 1, 0, 0)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// SCFG (In Port) Command
						#ifdef _AT_SCFG_In_

							// Control for Ex Commands
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

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// SCFGEXT (In Port) Command
						#ifdef _AT_SCFGEXT_In_

							// Control for Ex Commands
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

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("Setting Firewall"));
						#endif

						// FRWL Command 1
						#ifdef _AT_FRWL_1_

							// Control for Ex Commands
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

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// FRWL Command 2
						#ifdef _AT_FRWL_2_

							// Control for Ex Commands
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

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// FRWL Command 3
						#ifdef _AT_FRWL_3_

							// Control for Ex Commands
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

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// ICMP Command
						#ifdef _AT_ICMP_

							// Control for Ex Commands
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

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// Set MONI Command
						#ifdef _AT_MONIZIP_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT#MONIZIP"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::MONIZIP()) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug

									// Print Signal Level Value
									Terminal_GSM.Text(18, 65, WHITE, F("[-   ]"));
									Terminal_GSM.Text(18, 67, CYAN, String(GSM::Modem.dBm));

									// Print Signal Level Bar
									Terminal_GSM.Text(18, 74, GRAY, F("_____"));
									for (uint8_t i = 1; i <= GSM::Modem.Signal; i++) Terminal_GSM.Text(18, 73 + i, CYAN, F("X"));

									// Print Operator Value
									Terminal_GSM.Text(19, 74, CYAN, String(GSM::Modem.Operator));

									// Print Modem LAC Value
									Terminal_GSM.Text(21, 75, CYAN, String(GSM::Modem.LAC, HEX));

									// Print Modem Cell ID Value
									Terminal_GSM.Text(22, 75, CYAN, String(GSM::Modem.Cell_ID, HEX));

									// Command Status
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);

								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, CYAN, F("Updating RTC"));
						#endif

						// Set CCLK Command
						#ifdef _AT_CCLK_

							// Control for Ex Commands
							if (this->Status.Connection) {

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
									Terminal_GSM.Text(14, 4, WHITE, F("AT+CCLK"));
									Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
								#endif

								// Send Command
								if (!AT_Command_Set::CCLK(this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second)) this->Status.Connection = false;

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.OK_Decide(this->Status.Connection, 14, 35);
								#endif

								// Set Variable
								GSM_Console_Connect_ROW += 1;

							}

						#endif

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(17, 75, CYAN, String(Modem.Connection_Time));
						#endif

						// Print Command State
						#ifdef GSM_Debug
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

				// Declare Status Variable
				uint8_t Socket_Status;

				// Get Socket Status
				SS(2, Socket_Status);

				// Activate Socket
				if (_State and Socket_Status != 4) {

					// Activate Socket for Listen
					SL(2, 1, 80, 255);

					// Command Delay
					delay(20);

					// Get Socket Status
					SS(2, Socket_Status);

					// Command Delay
					delay(20);

					// Control Socket
					if (Socket_Status != 4) return(false);

				}

				// DeActivate Socket
				if (!_State and Socket_Status != 0) {

					// DeActivate Socket for Listen
					SL(2, 0, 80, 255);

					// Command Delay
					delay(20);

					// Get Socket Status
					SS(2, Socket_Status);

					// Command Delay
					delay(20);

					// Control Socket
					if (Socket_Status != 0) return(false);

				}

				// End Function
				return(true);

			}

	};


#endif /* defined(__PostMan__) */