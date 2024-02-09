// Include Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Include Hardware Functions
#include "GSM_Hardware.h"

// Include AT Commands
#if defined(_LE910C1_EUX_)
    
    // Include AT Command Set
    #include "AT_Command/LE910C1_EUX.h"
    
    // Define AT Command Set
    typedef AT_Command_Set_LE910C1_EUX AT_Command_Set;

#elif defined(_LE910S1_EAG_)
    
    // Include AT Command Set
    #include "AT_Command/LE910S1_EAG.h"

    // Define AT Command Set
    typedef AT_Command_Set_LE910S1_EAG AT_Command_Set;
    
#endif

// Include Libraries
#include <DS28C.h>
#include <RV3028.h>
#include <Environment.h>
#include <MAX17055.h>
#include <BQ24298.h>
#include <Variable.h>
#include <SPI.h>
#include "SdFat.h"

// Include Definitions
#include "AT_Command/Definitions/Command.h"
#include "AT_Command/Definitions/Pack.h"

// Cloud Functions
class Postman_PowerStatV4 : private AT_Command_Set, private GSM_Hardware, public Variable {

	// Private Context
	private:

		// Define Objects
		PowerStat_Console* GSM_Terminal;
		RV3028 GSM_RTC;
		HDC2010 GSM_TH_Sensor;
		MAX17055 GSM_Battery_Gauge;
		BQ24298 GSM_Charger;
		DS28C GSM_Serial_ID;
		SdFat GSM_SD;

		// Define Callback Function
		typedef void (*CallBack_Interval_Update)();
		typedef void (*CallBack_Energy_Update)();
		typedef void (*CallBack_Pressure_Update)();
		typedef void (*CallBack_Mask_Update)();

		// Define Callback Function
		CallBack_Interval_Update _CallBack_Interval_Update;
		CallBack_Energy_Update _CallBack_Energy_Update;
		CallBack_Pressure_Update _CallBack_Pressure_Update;
		CallBack_Mask_Update _CallBack_Mask_Update;

		// Define IoT Structures
		struct Struct_Status {
			bool		Terminal			= false;
			bool		SIM_Inserted		= false;
			uint8_t		SIM_PIN				= 0;
			bool 		Initialize		 	= false;
			bool		Connection			= false;
			uint8_t		Socket_State		= 0;
			bool		Ring				= false;
		} Status;
		struct Struct_Module {
			char 		IMEI[17];
			uint8_t 	Manufacturer 		= 0;
			uint8_t 	Model 				= 0;
			char 		Firmware[15];
		} Module;
		struct Struct_Operator {
			char 		ICCID[21];
			uint16_t 	MCC 				= 0;
			uint16_t	MNC					= 0;
			uint8_t		WDS					= 0;
			uint16_t	RSSI				= 0;
			uint8_t		Signal				= 0;
			uint16_t	TAC					= 0;
			uint32_t	Cell_ID				= 0;
			uint32_t	PCell_ID			= 0;
			uint8_t		IP_Address[4];
			uint16_t 	Connection_Time		= 0;
		} Operator;
		struct Struct_Time {
			uint8_t 	Year				= 0;
			uint8_t 	Month				= 0;
			uint8_t 	Day					= 0;
			uint8_t 	Hour				= 0;
			uint8_t 	Minute				= 0;
			uint8_t 	Second				= 0;
			uint8_t		Time_Zone			= 0;
		} Time;
		struct Struct_FOTA {
			uint32_t		Download_Time		= 0;
			uint32_t		File_Size			= 0;
			uint32_t		Download_Size		= 0;
			uint32_t		SD_File_Size 		= 0;
			uint8_t 		Download_Status		= 0;
		} FOTA;
		struct Struct_Buffer {
			uint32_t 	Connection_Time_Buffer	= 0;
		} Buffer;
		struct Struct_JSON_Variable {

			// Define Variable Count
			uint8_t	Variable_Count				= 0;

			// Define Variable Structure
			struct Struct_Variable {
				
				// Define Variable Name
				char * Name;

				// Define Variable Value
				float Value;

			};

			// Define Variable Array
			Struct_Variable Variable[MAX_VARIABLE_COUNT];

		} Data;

		// Declare JSON Variable
		char JSON_Pack[_PostMan_Send_JSON_Size_];

		// Initialize GSM Modem
		bool Initialize(void) {

			// Declare WatchDog Variable
			uint8_t _WD = 0;

			// Initialize Modem Parameters
			while (!this->Status.Initialize) {

				// Control for Power Monitor
				if (GSM_Hardware::PowerMonitor()) {

					// Print Batch Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 30, _Console_WHITE_, F("[    ]"));

						}

					#endif

					// Set Control Variable
					this->Status.Initialize = true;

					// Print Command Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command Description
							GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT"));

						}

					#endif

					// Send Command
					if (!AT_Command_Set::AT()) this->Status.Initialize = false;

					// Print Command State
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command State
							GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

							// Calculate Connection Time
							this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

							// Print Connection Time
							GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

						}

					#endif

					// ATE Command (Echo Off)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT0"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::ATE(false)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SIMDET Command (SIM Card Detect)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+SIMDET?"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SIMDET(GET, 0, this->Status.SIM_Inserted)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

							}

						#endif

						// No SIM Card
						if (!this->Status.SIM_Inserted) return(false);

						// Print Connection Time
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// CFUN Command (Full Functionality)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+CFUN=1,0"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::CFUN(1)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// CMEE Command (Error Messages)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+CMEE=1"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::CMEE(1)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// FCLASS Command (Connection Mode)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+FCLASS=0"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::FCLASS(0)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// ATK Command (No Flow Control)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT&K0"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::K(0)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// CPIN Command (SIM PIN Control)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+CPIN?"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::CPIN(this->Status.SIM_PIN)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print SIM Status
								if (this->Status.SIM_Inserted and this->Status.SIM_PIN) {
									GSM_Terminal->Text(17, 29, _Console_GRAY_, F("  SIM OK  "));
								} else {
									GSM_Terminal->Text(17, 29, _Console_RED_, F("SIM ERROR "));
								}

							}

						#endif

						// End Function
						if (this->Status.SIM_PIN != _SIM_READY_) break;

						// Print Connection Time
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// CGSN Command (Get IMEI)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+CGSN"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::CGSN(this->Module.IMEI)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print IMEI
								GSM_Terminal->Text(16, 24, _Console_GRAY_, String(this->Module.IMEI));

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// ICCID Command (Get SIM Card ID)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+CCID"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::CCID(this->Operator.ICCID)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print ICCID
								GSM_Terminal->Text(18, 20, _Console_GRAY_, String(this->Operator.ICCID));

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// CGMI Command (Get Manufacturer)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+GMI"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::CGMI(this->Module.Manufacturer)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print Manufacturer
								GSM_Terminal->Text(13, 37, _Console_GRAY_, String(this->Module.Manufacturer));

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// CGMM Command (Get Model)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+GMM"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::CGMM(this->Module.Model)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print Model
								GSM_Terminal->Text(14, 37, _Console_GRAY_, String(this->Module.Model));

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// CGMR Command (Get Firmware Version)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+GMR"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SWPKGV(this->Module.Firmware)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print Firmware Version
								GSM_Terminal->Text(15, 30, _Console_GRAY_, String(this->Module.Firmware));							

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// E2RI Command (Set RING Indicator)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#E2RI=50,50"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::E2SLRI(50)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// GPIO Command (Set Status LED)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#GPIO=1,0,2"));

							}

						#endif

						// Send Command
						AT_Command_Set::GPIO(SET, 1, 0, 2);

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SLED Command (Set Status LED)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SLED=2"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SLED(2)) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SLEDSAV Command (Save Status LED)
					if (this->Status.Initialize) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SLEDSAV"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SLEDSAV()) this->Status.Initialize = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// Initialize Complete
					return(true);

				} else {

					// Print Batch Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                    "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Power ON Modem"));

						}

					#endif

					// Power ON Modem
					GSM_Hardware::ON();

				}

				// Handle WatchDog
				if (_WD > 2) {

					// Print Batch Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                    "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_RED_, F("GSM Initializing Failed"));

						}

					#endif

					// Clear States
					this->Status.Initialize = false;
					this->Status.SIM_Inserted = false;
					this->Status.SIM_PIN = _SIM_UNKNOWN_;

					// Not Initialized
					return(false);

				} else {

					// Print Batch Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                    "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_YELLOW_, F("Retry Initializing"));

						}

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
			while (!this->Status.Connection) {

				// Control for Initialize
				if (this->Status.Initialize) {

					// Print Batch Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 30, _Console_WHITE_, F("[    ]"));

						}

					#endif

					// Set Control Variable
					this->Status.Connection = true;

					// Get CREG Command (Get Network Registration Mode)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+CREG?"));

							}

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

							// Print Command Description
							#ifdef _DEBUG_

								// Control for Terminal State
								if (this->Status.Terminal) {

									// Print Connection Status
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 31, _Console_CYAN_, F("    "));
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 31, _Console_CYAN_, String(_CREG_Connection_Stat));

								}

							#endif

							// Control for Connection
							if (_CREG_Connection_Stat == 0) {

								// Set Variable
								this->Status.Connection = false;

								// Declare Response Status
								_Conn_WD = false;

								// Connection Wait Delay
								for (uint8_t i = 0; i < 5000; i++) {

									// Print Connection Time
									#ifdef _DEBUG_

										// Control for Terminal State
										if (this->Status.Terminal) {

											// Calculate Connection Time
											this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

											// Print Connection Time
											GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

										}

									#endif

									// Connection Wait Delay
									delay(1);

								}

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
								for (uint8_t i = 0; i < 5000; i++) {

									// Print Connection Time
									#ifdef _DEBUG_

										// Control for Terminal State
										if (this->Status.Terminal) {

											// Calculate Connection Time
											this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

											// Print Connection Time
											GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

										}

									#endif

									// Connection Wait Delay
									delay(1);

								}

							} else if (_CREG_Connection_Stat == 3) {

								// Set Variable
								this->Status.Connection = false;

								// Declare Response Status
								_Conn_WD = false;

								// Connection Wait Delay
								for (uint8_t i = 0; i < 3000; i++) {

									// Print Connection Time
									#ifdef _DEBUG_

										// Control for Terminal State
										if (this->Status.Terminal) {

											// Calculate Connection Time
											this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

											// Print Connection Time
											GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

										}

									#endif

									// Connection Wait Delay
									delay(1);

								}

							} else if (_CREG_Connection_Stat == 4) {

								// Set Variable
								this->Status.Connection = false;

								// Declare Response Status
								_Conn_WD = false;

								// Connection Wait Delay
								for (uint8_t i = 0; i < 5000; i++) {

									// Print Connection Time
									#ifdef _DEBUG_

										// Control for Terminal State
										if (this->Status.Terminal) {

											// Calculate Connection Time
											this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

											// Print Connection Time
											GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

										}

									#endif

									// Connection Wait Delay
									delay(1);

								}

							} else if (_CREG_Connection_Stat == 5) {

								// Set Variable
								this->Status.Connection = false;

								// Declare Response Status
								_Conn_WD = true;

							}

							// Print Command State
							#ifdef DEBUG
								Console::Text(19, 75, CYAN, String(_CREG_Connection_Stat));
							#endif

							// Print Connection Time
							#ifdef _DEBUG_

								// Control for Terminal State
								if (this->Status.Terminal) {

									// Calculate Connection Time
									this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

									// Print Connection Time
									GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

								}

							#endif

							// Set WD Variable
							_Conn_WD_Counter++;

							// Control for WD
							if (_Conn_WD_Counter > 300) _Conn_WD = true;

						}

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// CGDCONT Command (Set PDP Context)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+CGDCONT=1,IP,mgbs"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::CGDCONT(1, "IP", _PostMan_APN_)) this->Status.Connection = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SGACT Command (Activate PDP Context)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SGACT=1,1"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SGACT(1, 1, this->Operator.IP_Address)) this->Status.Connection = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print IP Address
								GSM_Terminal->Text(16, 64, _Console_GRAY_, this->IPAddress_to_String(this->Operator.IP_Address));

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// WS46 Command (Network Type)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+WS46?"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::WS46(GET, this->Operator.WDS)) this->Status.Connection = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print Connection Type
								GSM_Terminal->Show_Connection_Type(17, 70, this->Operator.WDS);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// RFSTS Command (Network Status)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#RFSTS"));

							}

						#endif

						// Send Command
						AT_Command_Set::RFSTS(this->Operator.MCC, this->Operator.MNC, this->Operator.RSSI, this->Operator.Signal, this->Operator.Cell_ID, this->Operator.TAC);

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Print Signal Level Value
								GSM_Terminal->Show_RSSI(14, 65, this->Operator.RSSI);

								// Print Signal Level Bar
								GSM_Terminal->Show_Signal_Quality(14, 74, this->Operator.Signal);

								// Print Operator Value
								char _Operator[6];
								sprintf(_Operator, "%d%d", this->Operator.MCC, this->Operator.MNC);
								GSM_Terminal->Text(15, 74, _Console_GRAY_, String(_Operator));

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SCFG (Send Port) Command (Send Data Port Configuration)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SCFG=3,1,1500,90,1200,0"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SCFG(_PostMan_Outgoing_Socket_, 1, 1500, 90, 1200, 0)) this->Status.Connection = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SCFGEXT (Send Port) Command (Send Data Port Extended Configuration)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SCFGEXT=3,1,0,0,0,0"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SCFGEXT(_PostMan_Outgoing_Socket_, 1, 0, 0, 0, 0)) this->Status.Connection = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SCFGEXT2 (Send Port) Command (Send Data Port Extended 2 Configuration)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SCFGEXT2=3,1,0,0,0,0"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SCFGEXT2(_PostMan_Outgoing_Socket_, 1, 0, 0, 0, 0)) this->Status.Connection = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SCFG (In Port) Command (In Port Configuration)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SCFG=2,1,1500,90,300,50"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SCFG(_PostMan_Incomming_Socket_, 1, 1500, 90, 1200, 50)) this->Status.Connection = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// SCFGEXT (In Port) Command (In Port Extended Configuration)
					if (this->Status.Connection) {

						// Print Command Description
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command Description
								GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SCFGEXT=2,1,0,1,0,0"));

							}

						#endif

						// Send Command
						if (!AT_Command_Set::SCFGEXT(_PostMan_Incomming_Socket_, 1, 0, 1, 0, 0)) this->Status.Connection = false;

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Command State
								GSM_Terminal->OK(this->Status.Initialize, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

								// Calculate Connection Time
								this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

								// Print Connection Time
								GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

							}

						#endif

					} else break;

					// Print Batch Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

						}

					#endif

					// Connection Success
					return(true);

				} else {

					// TODO: modem hazır değiş yazısı ekrana yazdırılacak
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

					// TODO: bağlantı kurulamadı hatası yazdırılacak
					// Print Command State
					#ifdef DEBUG
						Console::Text(14, 4, CYAN, F("                                    "));
						Console::Text(14, 4, RED, F("Connection Failed"));
					#endif

					// Clear States
					this->Status.SIM_Inserted = false;
					this->Status.SIM_PIN = 0;
					this->Status.Initialize = false;
					this->Status.Connection = false;

					// TODO: modem kapatılacak
					// Turn Off Modem
//					this->OFF();

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

		// Set Firewall Configuration
		bool Firewall(void) {

			// Control for Connection
			if (this->Status.Connection) {

				// Print Batch Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 30, _Console_WHITE_, F("[    ]"));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#FRWL=2"));

					}

				#endif

				// FRWL Clear (Firewall Configuration)
				bool _FireWall_Clear_State = AT_Command_Set::FRWL(SET, 2, _PostMan_Firewall_1_);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_FireWall_Clear_State, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						// Calculate Connection Time
						this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

						// Print Connection Time
						GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#FRWL=1,***"));

					}

				#endif

				// FRWL Command 1 (Firewall Configuration)
				bool _FireWall_State_1 = AT_Command_Set::FRWL(SET, 1, _PostMan_Firewall_1_);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_FireWall_State_1, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						// Calculate Connection Time
						this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

						// Print Connection Time
						GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#FRWL=1,***"));

					}

				#endif

				// FRWL Command 2 (Firewall Configuration)
				bool _FireWall_State_2 = AT_Command_Set::FRWL(SET, 1, _PostMan_Firewall_2_);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_FireWall_State_2, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						// Calculate Connection Time
						this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

						// Print Connection Time
						GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#FRWL=1,***"));

					}

				#endif

				// FRWL Command 3 (Firewall Configuration)
				bool _FireWall_State_3 = AT_Command_Set::FRWL(SET, 1, _PostMan_Firewall_3_);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_FireWall_State_3, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						// Calculate Connection Time
						this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

						// Print Connection Time
						GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#ICMP=1"));

					}

				#endif

				// ICMP Command (Ping Configuration)
				bool _ICMP_State = AT_Command_Set::ICMP(1);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_ICMP_State, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						// Calculate Connection Time
						this->Operator.Connection_Time = ((millis() - this->Buffer.Connection_Time_Buffer));

						// Print Connection Time
						GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

					}

				#endif

				// Connection Success
				return(true);

			} 

			// FireWall Configuration Failed
			return(false);

		}

		// Configure Socket for Listen
		bool Listen(const bool _State) {

			// Check Connection Status
			if (this->Status.Connection) {

				// Clear Socket Status
				this->Status.Socket_State = 0;

				// Print Batch Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 30, _Console_WHITE_, F("[    ]"));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SS=2"));

					}

				#endif

				// Send Command
				bool _Socket_State = AT_Command_Set::SS(_PostMan_Incomming_Socket_, this->Status.Socket_State);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_Socket_State, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

					}

				#endif

				// Activate Socket
				if (_State and this->Status.Socket_State != 4) {

					// Print Command Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command Description
							GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SL=*,1,80,255"));

						}

					#endif

					// Activate Socket for Listen
					bool _SL_Command = AT_Command_Set::SL(_PostMan_Incomming_Socket_, 1, 80, 255);

					// Print Command State
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command State
							GSM_Terminal->OK(_SL_Command, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						}

					#endif

					// Command Delay
					delay(50);

					// Print Command Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command Description
							GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SS=*"));

						}

					#endif

					// Get Socket Status
					bool _SS_Command = AT_Command_Set::SS(_PostMan_Incomming_Socket_, this->Status.Socket_State);

					// Print Command State
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command State
							GSM_Terminal->OK(_SS_Command, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

							// Print Socket State
							if (this->Status.Socket_State == 0) GSM_Terminal->Text(18, 64, _Console_RED_, F("    Closed     "));
							else if (this->Status.Socket_State == 1) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("Active Transfer"));
							else if (this->Status.Socket_State == 2) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("   Suspended   "));
							else if (this->Status.Socket_State == 3) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("  Pending Data "));
							else if (this->Status.Socket_State == 4) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("   Listening   "));

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

						}

					#endif

					// Control Socket
					if (this->Status.Socket_State != 4) return(false);

					// End Function
					return(true);

				}

				// DeActivate Socket
				if (!_State and this->Status.Socket_State != 0) {

					// Print Command Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command Description
							GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SL=*,0,80,255"));

						}

					#endif

					// DeActivate Socket for Listen
					bool _SL_Command = AT_Command_Set::SL(_PostMan_Incomming_Socket_, 0, 80, 255);

					// Print Command State
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command State
							GSM_Terminal->OK(_SL_Command, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						}

					#endif

					// Command Delay
					delay(50);

					// Print Command Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command Description
							GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#SS=*"));

						}

					#endif

					// Get Socket Status
					bool _SS_Command = AT_Command_Set::SS(_PostMan_Incomming_Socket_, this->Status.Socket_State);

					// Print Command State
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Command State
							GSM_Terminal->OK(_SS_Command, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

							// Print Socket State
							if (this->Status.Socket_State == 0) GSM_Terminal->Text(18, 64, _Console_RED_, F("    Closed     "));
							else if (this->Status.Socket_State == 1) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("Active Transfer"));
							else if (this->Status.Socket_State == 2) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("   Suspended   "));
							else if (this->Status.Socket_State == 3) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("  Pending Data "));
							else if (this->Status.Socket_State == 4) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("   Listening   "));

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

						}

					#endif

					// Control Socket
					if (this->Status.Socket_State != 0) return(false);

					// End Function
					return(true);

				}

				// Print Batch Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

					}

				#endif

				// End Function
				return(false);

			} else { 

				// Print Command State

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// Send Request Response Function
		bool Response(const uint16_t _Response_Code, const uint16_t _Response) {

			// Clear Message Field
			#ifdef _DEBUG_

				// Control for Terminal State
				if (this->Status.Terminal) {

					// Print Batch Description
					GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

				}

			#endif

			// Declare Response JSON Array
			char Buffer_Variable[_PostMan_Response_JSON_Size_];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', _PostMan_Response_JSON_Size_);

			// Declare JSON Object
			StaticJsonDocument<_PostMan_Response_JSON_Size_> Response_JSON;

			// Declare JSON Data
			Response_JSON[F("Response")] = _Response;

			// Clear Unused Data
			Response_JSON.garbageCollect();

			// Serialize JSON	
			serializeJson(Response_JSON, Buffer_Variable);

			// Send Socket Answer
			if (AT_Command_Set::SSEND(_PostMan_Incomming_Socket_, HTTP_RESPONSE, _Response_Code, "", Buffer_Variable)) {

				// Print Message
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Sending Response..."));

					}

				#endif

				// Command Delay
				delay(20);

				// Closing Socket
				if (AT_Command_Set::SH(_PostMan_Incomming_Socket_)) {

					// Command Delay
					delay(20);

					// Control for Incoming Call
					this->Listen(true);

					// Print Message
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

						}

					#endif

					// End Function
					return(true);

				} else {

					// End Function
					return(false);

				}

			} else {

				// End Function
				return(false);

			}
			
		}

		// Get Server Command Function
		void Get(void) {

			// Control for Connection
			if (this->Status.Connection) {

				if (this->PostMan_Interrupt.Ring) {

					// Reset Ring
					this->PostMan_Interrupt.Ring = false;

					// Clear Message Field
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

						}

					#endif

					// Wait for Sring
					if (AT_Command_Set::SRING()) {

						// Declare JSON Variable
						char _JSON_Data[_PostMan_Recieve_JSON_Size_];

						// Clear JSON Data
						memset(_JSON_Data, '\0', _PostMan_Recieve_JSON_Size_);

						// Print Message
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Batch Description
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Recieving Message..."));

							}

						#endif

						// Declare Request Pack Length
						uint16_t _Length = 0;

						// Answer Socket
						AT_Command_Set::SA(_PostMan_Incomming_Socket_, 1, _Length);

						// Handle Max Length
						if (_Length > _PostMan_Recieve_JSON_Size_) _Length = _PostMan_Recieve_JSON_Size_;

						// Get Request Data
						AT_Command_Set::SRECV(_PostMan_Incomming_Socket_, _Length, _JSON_Data);

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

						// Declare JSON Object
						StaticJsonDocument<_PostMan_Recieve_JSON_Size_> Incoming_JSON;

						// Deserialize the JSON document
						DeserializationError Error = deserializeJson(Incoming_JSON, _Data);

						// Declare Event Variable
						uint16_t _Event = 0;

						// Handle JSON
						if (!Error) _Event = Incoming_JSON["Request"]["Event"];

						// Print Message
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Batch Description
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Response --> [   ]"));
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 14, _Console_CYAN_, String(_Event));

							}

						#endif

						// Handle Command
						if (_Event == Command_FOTA_Burn) {

							// Print Message
							#ifdef _DEBUG_

								// Control for Terminal State
								if (this->Status.Terminal) {

									// Beep
									GSM_Terminal->Beep();

									// Print Batch Description
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Resetting..."));

								}

							#endif

							// Send Response
							this->Response(HTTP_OK, Command_OK);

							// Disable SD Multiplexer
							GSM_Hardware::SD_Multiplexer(false);

							// Set FOTA Power Enable Pin
							PORT_FOTA_POWER_EN |= (1 << PIN_FOTA_POWER_EN);
							delay(50);
							PORT_FOTA_POWER_EN &= ~(1 << PIN_FOTA_POWER_EN);


						} else if (_Event == Command_FOTA_Download) {

							// Print Message
							#ifdef _DEBUG_

								// Control for Terminal State
								if (this->Status.Terminal) {

									// Print Batch Description
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Firmware Download..."));

								}

							#endif

							// Handle FW ID
							uint16_t _Firmware_ID = 0;

							// Control for JSON (Request)
							if (Incoming_JSON.containsKey("Request")) {

								// Control for JSON (FW_ID)
								if (Incoming_JSON["Request"].containsKey("FW_ID")) {

									// Handle FW ID
									_Firmware_ID = Incoming_JSON["Request"]["FW_ID"];

									// Send Response
									this->Response(HTTP_OK, Command_OK);

								} else {

									// Send Response
									this->Response(HTTP_NotAcceptable, Command_NOK);

								}

							} else {

								// Send Response
								this->Response(HTTP_BadRequest, Command_NOK);

							}

							// Stop RTC Timer
							GSM_RTC.Timer(false);

							// Download Firmware
							this->Download(_Firmware_ID);

							// Add Variable to Payload
							Variable::Add("FOTA_ID", _Firmware_ID);
							Variable::Add("FOTA_Status", FOTA.Download_Status);

							// Set Interrupt
							this->PostMan_Interrupt.Pack_Type = Pack_FOTA_Download;

						} else if (_Event == Command_Update) {

							// Send Response
							this->Response(HTTP_OK, Command_OK);

							this->PostMan_Interrupt.Pack_Type = Pack_Alarm;

						} else if (_Event == Command_Parameter) {

							// Print Message
							#ifdef _DEBUG_

								// Control for Terminal State
								if (this->Status.Terminal) {

									// Print Batch Description
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Parameter Update..."));

								}

							#endif

							// Parameter Update Pack
							/*
							
							 {
							 	"Request": {
							 		"Event": 501,
							 		"Parameter": {
							 			"Dt1": 1,
							 			"Dt2": 2,
							 			"Vmin": 250,
							 			"Vmax": 150,
							 			"Imax": 5.10,
							 			"FQmin": 49,
							 			"FQmax": 51,
							 			"VIMB": 9,
							 			"IIMB": 9,
							 			"Ratio": 20,
							 			"Pmin": 1.20,
							 			"Pmax": 9.90,
							 			"Pslope": 3,
							 			"MASK_Stop": 4294967295,
							 			"MASK_Publish": 4294967295
							 		}
							 	}
							 }
							
							 */

							// Control for JSON (Request)
							if (Incoming_JSON.containsKey("Request")) {

								// Control for JSON (Parameter)
								if (Incoming_JSON["Request"].containsKey("Parameter")) {

									// Declare Variables
									bool _Limit_Error = false;

									// Declare Update Variables
									bool _Interval_Update = false;
									bool _Energy_Update = false;
									bool _Pressure_Update = false;
									bool _Mask_Update = false;

									// Control for JSON (Dt1)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Dt1")) {

										// EEPROM Online Interval Value is Minute Value (1 min - 255 min)

										// Handle Online Interval
										uint8_t _Online_Interval = Incoming_JSON["Request"]["Parameter"]["Dt1"];

										// Control for Default
										if (_Online_Interval == 0) _Online_Interval = _SEND_INTERVAL_ONLINE_;

										// Write EEPROM
										if (_Online_Interval >= 1 and _Online_Interval <= 255) {
											
											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_Online_Interval__, _Online_Interval);

											// Add Variable to Payload
											Variable::Add("Dt1", _Online_Interval);

											// Set Update Variable
											_Interval_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (Dt2)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Dt2")) {

										// EEPROM Offline Interval Value is Minute Value (1 min - 255 min)

										// Handle Offline Interval
										uint8_t _Offline_Interval = Incoming_JSON["Request"]["Parameter"]["Dt2"];

										// Control for Default
										if (_Offline_Interval == 0) _Offline_Interval = _SEND_INTERVAL_OFFLINE_;

										// Write EEPROM
										if (_Offline_Interval >= 1 and _Offline_Interval <= 255) {
											
											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_Offline_Interval__, _Offline_Interval);

											// Add Variable to Payload
											Variable::Add("Dt2", _Offline_Interval);

											// Set Update Variable
											_Interval_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for Variable Update
									if (_Interval_Update) {

										// Variable Update Callback
										if (_CallBack_Interval_Update != nullptr) _CallBack_Interval_Update();

									}

									// Control for JSON (Vmin)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Vmin")) {

										// Handle Vmin
										uint16_t _Vmin = Incoming_JSON["Request"]["Parameter"]["Vmin"];

										// Control for Default
										if (_Vmin == 0) _Vmin = __MAX78630_Limit_Voltage_Min__;

										// Write EEPROM
										if (_Vmin >= 100 and _Vmin <= 300) {

											// Handle MSB Value (Vmin)
											uint8_t _Vmin_MSB = (_Vmin >> 8);

											// Handle LSB Value (Vmin)
											uint8_t _Vmin_LSB = (_Vmin & 0xFF);

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_V_Min_MSB__, _Vmin_MSB);
											GSM_RTC.Write_EEPROM(__EEPROM_V_Min_LSB__, _Vmin_LSB);

											// Add Variable to Payload
											Variable::Add("Vmin", _Vmin);

											// Set Update Variable
											_Energy_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (Vmax)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Vmax")) {

										// Handle Vmax
										uint16_t _Vmax = Incoming_JSON["Request"]["Parameter"]["Vmax"];

										// Control for Default
										if (_Vmax == 0) _Vmax = __MAX78630_Limit_Voltage_Max__;

										// Write EEPROM
										if (_Vmax >= 100 and _Vmax <= 300) {

											// Handle MSB Value (Vmax)
											uint8_t _Vmax_MSB = (_Vmax >> 8);

											// Handle LSB Value (Vmax)
											uint8_t _Vmax_LSB = (_Vmax & 0xFF);

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_V_Max_MSB__, _Vmax_MSB);
											GSM_RTC.Write_EEPROM(__EEPROM_V_Max_LSB__, _Vmax_LSB);

											// Add Variable to Payload
											Variable::Add("Vmax", _Vmax);

											// Set Update Variable
											_Energy_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (Imax)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Imax")) {

										// Handle Imax
										float _Imax = Incoming_JSON["Request"]["Parameter"]["Imax"];

										// Control for Default
										if (_Imax == 0) _Imax = __MAX78630_Limit_Current_Max__;

										// Write EEPROM
										if (_Imax >= 0.1 and _Imax <= 10.0) {

											// Handle MSB Value (Imax)
											uint8_t _Imax_MSB = (_Imax * 10);

											// Handle LSB Value (Imax)
											uint8_t _Imax_LSB = ((_Imax * 10) - _Imax_MSB) * 10;

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_I_Max_MSB__, _Imax_MSB);
											GSM_RTC.Write_EEPROM(__EEPROM_I_Max_LSB__, _Imax_LSB);

											// Add Variable to Payload
											Variable::Add("Imax", _Imax);

											// Set Update Variable
											_Energy_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (FQmin)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("FQmin")) {

										// Handle FQmin
										uint8_t _FQmin = Incoming_JSON["Request"]["Parameter"]["FQmin"];

										// Control for Default
										if (_FQmin == 0) _FQmin = __MAX78630_Limit_Frequency_Min__;

										// Write EEPROM
										if (_FQmin >= 40 and _FQmin <= 60) {

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_FQ_Min__, _FQmin);

											// Add Variable to Payload
											Variable::Add("FQmin", _FQmin);

											// Set Update Variable
											_Energy_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (FQmax)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("FQmax")) {

										// Handle FQmax
										uint8_t _FQmax = Incoming_JSON["Request"]["Parameter"]["FQmax"];

										// Control for Default
										if (_FQmax == 0) _FQmax = __MAX78630_Limit_Frequency_Max__;

										// Write EEPROM
										if (_FQmax >= 40 and _FQmax <= 60) {

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_FQ_Max__, _FQmax);

											// Add Variable to Payload
											Variable::Add("FQmax", _FQmax);

											// Set Update Variable
											_Energy_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (VIMB)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("VIMB")) {

										// Handle VIMB
										uint8_t _VIMB = Incoming_JSON["Request"]["Parameter"]["VIMB"];

										// Control for Default
										if (_VIMB == 0) _VIMB = __MAX78630_Limit_VImb_Max__ * 100;

										// Write EEPROM
										if (_VIMB >= 1 and _VIMB <= 255) {

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_VIMB_Max__, _VIMB);

											// Add Variable to Payload
											Variable::Add("VIMB", _VIMB);

											// Set Update Variable
											_Energy_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (IIMB)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("IIMB")) {

										// Handle IIMB
										uint8_t _IIMB = Incoming_JSON["Request"]["Parameter"]["IIMB"];

										// Control for Default
										if (_IIMB == 0) _IIMB = __MAX78630_Limit_IImb_Max__ * 100;

										// Write EEPROM
										if (_IIMB >= 1 and _IIMB <= 255) {

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_IIMB_Max__, _IIMB);

											// Add Variable to Payload
											Variable::Add("IIMB", _IIMB);

											// Set Update Variable
											_Energy_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for Variable Update
									if (_Energy_Update) {

										// Variable Update Callback
										if (_CallBack_Energy_Update != nullptr) _CallBack_Energy_Update();

									}

									// Control for JSON (Ratio)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Ratio")) {

										// Handle Ratio
										uint8_t _Ratio = Incoming_JSON["Request"]["Parameter"]["Ratio"];

										// Control for Default

										// Write EEPROM
										if (_Ratio >= 1 and _Ratio <= 255) {

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_Current_Ratio__, _Ratio);

											// Add Variable to Payload
											Variable::Add("Ratio", _Ratio);

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (Pmin)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Pmin")) {

										// Handle Pmin
										uint16_t _Pmin = Incoming_JSON["Request"]["Parameter"]["Pmin"];

										// Control for Default

										// Write EEPROM
										if (_Pmin * 100 >= 10 and _Pmin * 100 <= 999) {

											// Handle MSB Value (Vmax)
											uint8_t _Pmin_MSB = ((_Pmin * 100) >> 8);

											// Handle LSB Value (Vmax)
											uint8_t _Pmin_LSB = ((_Pmin * 100) & 0xFF);

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_PMIN_MSB__, _Pmin_MSB);
											GSM_RTC.Write_EEPROM(__EEPROM_PMIN_LSB__, _Pmin_LSB);

											// Add Variable to Payload
											Variable::Add("Pmin", _Pmin / 100);

											// Set Update Variable
											_Pressure_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (Pmax)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Pmax")) {

										// Handle Pmax
										uint16_t _Pmax = Incoming_JSON["Request"]["Parameter"]["Pmax"];

										// Control for Default

										// Write EEPROM
										if (_Pmax * 100 >= 10 and _Pmax * 100 <= 999) {

											// Handle MSB Value (Vmax)
											uint8_t _Pmax_MSB = ((_Pmax * 100) >> 8);

											// Handle LSB Value (Vmax)
											uint8_t _Pmax_LSB = ((_Pmax * 100) & 0xFF);

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_PMAX_MSB__, _Pmax_MSB);
											GSM_RTC.Write_EEPROM(__EEPROM_PMAX_LSB__, _Pmax_LSB);

											// Add Variable to Payload
											Variable::Add("Pmax", _Pmax / 100);

											// Set Update Variable
											_Pressure_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for JSON (Pslope)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("Pslope")) {

										// Handle Pslope
										uint8_t _Pslope = Incoming_JSON["Request"]["Parameter"]["Pslope"];

										// Control for Default

										// Write EEPROM
										if (_Pslope >= 1 and _Pslope <= 255) {

											// Write EEPROM
											GSM_RTC.Write_EEPROM(__EEPROM_PSLOP_EMAX__, _Pslope);

											// Add Variable to Payload
											Variable::Add("Pslope", _Pslope);

											// Set Update Variable
											_Pressure_Update = true;

										} else {

											// Set Error
											_Limit_Error = true;

										}

									}

									// Control for Variable Update
									if (_Pressure_Update) {

										// Variable Update Callback
										if (_CallBack_Pressure_Update != nullptr) _CallBack_Pressure_Update();

									}

									// Control for JSON (MASK_Stop)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("MASK_Stop")) {

										// Handle MASK_Stop
										uint32_t _MASK_Stop = Incoming_JSON["Request"]["Parameter"]["MASK_Stop"];

										// Control for Default
										if (_MASK_Stop == 0) _MASK_Stop = __STOP_REGISTER_DEFAULT__;

										// Register Structure
										// MSB2-MSB1-LSB2-LSB1

										// Handle MSB2 Value (MASK_Stop)
										uint8_t _MASK_Stop_MSB_2 = (uint8_t)(_MASK_Stop >> 24);

										// Handle MSB1 Value (MASK_Stop)
										uint8_t _MASK_Stop_MSB_1 = (uint8_t)((_MASK_Stop >> 16) & 0xFF);

										// Handle LSB2 Value (MASK_Stop)
										uint8_t _MASK_Stop_LSB_2 = (uint8_t)((_MASK_Stop >> 8) & 0xFF);

										// Handle LSB1 Value (MASK_Stop)
										uint8_t _MASK_Stop_LSB_1 = (uint8_t)(_MASK_Stop & 0xFF);

										// Write EEPROM
										GSM_RTC.Write_EEPROM(__EEPROM_STOP_MASK_MSB_2__, _MASK_Stop_MSB_2);
										GSM_RTC.Write_EEPROM(__EEPROM_STOP_MASK_MSB_1__, _MASK_Stop_MSB_1);
										GSM_RTC.Write_EEPROM(__EEPROM_STOP_MASK_LSB_2__, _MASK_Stop_LSB_2);
										GSM_RTC.Write_EEPROM(__EEPROM_STOP_MASK_LSB_1__, _MASK_Stop_LSB_1);

										// Add Variable to Payload
										Variable::Add("MASK_Stop", _MASK_Stop);

										// Set Update Variable
										_Mask_Update = true;

									}

									// Control for JSON (MASK_Publish)
									if (Incoming_JSON["Request"]["Parameter"].containsKey("MASK_Publish")) {

										// Handle MASK_Publish
										uint32_t _MASK_Publish = Incoming_JSON["Request"]["Parameter"]["MASK_Publish"];

										// Control for Default
										if (_MASK_Publish == 0) _MASK_Publish = __PUBLISH_REGISTER_DEFAULT__;

										// Register Structure
										// MSB2-MSB1-LSB2-LSB1

										// Handle MSB2 Value (MASK_Publish)
										uint8_t _MASK_Publish_MSB_2 = (uint8_t)(_MASK_Publish >> 24);

										// Handle MSB1 Value (MASK_Publish)
										uint8_t _MASK_Publish_MSB_1 = (uint8_t)((_MASK_Publish >> 16) & 0xFF);

										// Handle LSB2 Value (MASK_Publish)
										uint8_t _MASK_Publish_LSB_2 = (uint8_t)((_MASK_Publish >> 8) & 0xFF);

										// Handle LSB1 Value (MASK_Publish)
										uint8_t _MASK_Publish_LSB_1 = (uint8_t)(_MASK_Publish & 0xFF);

										// Write EEPROM
										GSM_RTC.Write_EEPROM(__EEPROM_PUBLISH_MASK_MSB_2__, _MASK_Publish_MSB_2);
										GSM_RTC.Write_EEPROM(__EEPROM_PUBLISH_MASK_MSB_1__, _MASK_Publish_MSB_1);
										GSM_RTC.Write_EEPROM(__EEPROM_PUBLISH_MASK_LSB_2__, _MASK_Publish_LSB_2);
										GSM_RTC.Write_EEPROM(__EEPROM_PUBLISH_MASK_LSB_1__, _MASK_Publish_LSB_1);

										// Add Variable to Payload
										Variable::Add("MASK_Publish", _MASK_Publish);

										// Set Update Variable
										_Mask_Update = true;

									}

									// Control for Variable Update
									if (_Mask_Update) {

										// Variable Update Callback
										if (_CallBack_Mask_Update != nullptr) _CallBack_Mask_Update();

									}

									// Control for Limit Error
									if (_Limit_Error) {

										// Send Response
										this->Response(HTTP_NotAcceptable, Command_NOK);

									} else {

										// Send Response
										this->Response(HTTP_Accepted, Command_OK);

									}

								} else {

									// Send Response
									this->Response(HTTP_BadRequest, Command_NOK);

								}


							} else {

								// Send Response
								this->Response(HTTP_BadRequest, Command_NOK);

							}

						} else {

							// Send Response
							this->Response(HTTP_BadRequest, Command_NOK);

						}

						// Port Control
						this->Listen(true);

					} 

				}

			}

		}

		// Download Firmware
		bool Download(const uint16_t _Firmware_ID) {

			// Control for Connection
			if (this->Status.Connection) {

				// Beep
				GSM_Terminal->Beep();

				// Enable SD Multiplexer
				GSM_Hardware::SD_Multiplexer(true);

				// Clear Variables
				this->FOTA.Download_Time = 0;
				this->FOTA.File_Size = 0;
				this->FOTA.Download_Size = 0;
				this->FOTA.SD_File_Size = 0;
				this->FOTA.Download_Status = 0;

				// Clear Message Field
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Connecting to Server...              "));

						// Print File Name
						GSM_Terminal->Text(13, 116, _Console_CYAN_, String(_Firmware_ID));

					}

				#endif

				// Declare SD File Object
				File SD_File;

				// Delay
				delay(100);

				// Control for Existing File
				if (GSM_SD.exists(_PostMan_Firmware_Name_)) {

					// Remove Existing File
					GSM_SD.remove(_PostMan_Firmware_Name_);

					// Print Message
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("File Exist and Deleted...."));

						}

					#endif

					// Command Delay
					delay(100);

				} else {

					// Print Message
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("File Not Exist...."));

						}

					#endif

				}

				// Open File for Write
				SD_File = GSM_SD.open(_PostMan_Firmware_Name_, O_WRITE | O_CREAT);

				// Command Delay
				delay(100);

				// Control for File Open
				if (SD_File) {

					// Set Download Start Time
					uint32_t _Download_Start_Time = millis();

					// Set Socket Configuration
					AT_Command_Set::SCFGEXT(_PostMan_Outgoing_Socket_, 0, 0, 0, 0, 0);

					// Open Connection
					if (AT_Command_Set::ATSD(_PostMan_Outgoing_Socket_, 0, 80, 255, 88, 1, _PostMan_Server_)) {

						// Print Message
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Batch Description
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Downloading Firmware...              "));

							}

						#endif

						// Set Download Path
						char _Download_Path[16];
						sprintf(_Download_Path, "/Firmware/%d", _Firmware_ID);

						// Sending Data
						if (AT_Command_Set::SSEND(_PostMan_Outgoing_Socket_, HTTP_GET, 0, _Download_Path, "")) {

							// Get Ring Port
							if (AT_Command_Set::SRING()) {

								// Define Response Buffer Size
								uint16_t _Response_Buffer_Size = 500;

								// Define Response Variable
								char Buffer_Variable[_Response_Buffer_Size];
								char Data_Variable[_Response_Buffer_Size];

								// Clear Buffer Variable
								memset(Buffer_Variable, '\0', _Response_Buffer_Size);
								memset(Data_Variable, '\0', _Response_Buffer_Size);

								// Get Request Data Head
								bool _SRECV = AT_Command_Set::SRECV(_PostMan_Outgoing_Socket_, (_Response_Buffer_Size - 100), Buffer_Variable);

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
										if (i > 5) {

											// Find End Position
											if (
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

									// Handle Header Variables
									strtok(Buffer_Variable, "\r\n");
									strtok(NULL, "\r\n");
									strtok(NULL, "\r\n");
									strtok(NULL, "\r\n");
									strtok(NULL, "\r\n");
									char * _Content_Length = strtok(NULL, "\r\n");

									// Handle Content Length
									sscanf(_Content_Length, "Content-Length: %lu", &FOTA.File_Size);

									// Print Message
									#ifdef _DEBUG_

										// Control for Terminal State
										if (this->Status.Terminal) {

											// Declare File Size Char Buffer
											char _File_Size_Buffer[8];

											// Set Buffer
											sprintf(_File_Size_Buffer, "%07lu", this->FOTA.File_Size);

											// Print File Size
											GSM_Terminal->Text(15, 112, _Console_CYAN_, String(_File_Size_Buffer));

										}

									#endif

									// Parse Data
									for (uint16_t i = _Start_Position; i < _End_Position; i++) {

										// Parse Data
										Data_Variable[i - _Start_Position] = Buffer_Variable[i];

										// Calculate Total Size
										if (Data_Variable[i - _Start_Position] > 31 and Data_Variable[i - _Start_Position] < 127) FOTA.Download_Size += 1;
										if (Data_Variable[i - _Start_Position] == '\r') FOTA.Download_Size += 1;
										if (Data_Variable[i - _Start_Position] == '\n') FOTA.Download_Size += 1;

									}

									// Write Data
									if (_End_Position - _Start_Position > 0) SD_File.write(Data_Variable, (_End_Position - _Start_Position));

									// SD Print Delay
									delay(100);

									// Print Message
									#ifdef _DEBUG_

										// Control for Terminal State
										if (this->Status.Terminal) {

											// Declare Download Size Char Buffer
											char _Download_Size_Buffer[8];

											// Set Buffer
											sprintf(_Download_Size_Buffer, "%07lu", this->FOTA.Download_Size);

											// Print Download Time
											GSM_Terminal->Text(18, 111, _Console_CYAN_, String((millis() - _Download_Start_Time) / 1000));

										}

									#endif

								}

								// Declare WD Variable
								uint8_t _WD = 0;

								// Get Data
								while (FOTA.Download_Size < FOTA.File_Size) {

									// Reset Variables
									memset(Buffer_Variable, '\0', _Response_Buffer_Size);
									memset(Data_Variable, '\0', _Response_Buffer_Size);

									// Handle Response
									if (AT_Command_Set::SRECV(_PostMan_Outgoing_Socket_, (_Response_Buffer_Size - 100), Buffer_Variable)) {

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
											if (i > 5) {

												// Find End Position
												if (
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

											// Calculate Total Size
											if (Data_Variable[i - _Start_Position] > 31 and Data_Variable[i - _Start_Position] < 127) FOTA.Download_Size += 1;
											if (Data_Variable[i - _Start_Position] == '\r') FOTA.Download_Size += 1;
											if (Data_Variable[i - _Start_Position] == '\n') FOTA.Download_Size += 1;

										}

										// Write Data
										if (_End_Position - _Start_Position > 0) SD_File.write(Data_Variable, (_End_Position - _Start_Position));

									} else {

										// Increase WD
										_WD++;

									}

									// Print Message
									#ifdef _DEBUG_

										// Control for Terminal State
										if (this->Status.Terminal) {

											// Declare Download Size Char Buffer
											char _Download_Size_Buffer[8];

											// Set Buffer
											sprintf(_Download_Size_Buffer, "%07lu", this->FOTA.Download_Size);

											// Print SD File Size
											GSM_Terminal->Text(16, 112, _Console_CYAN_, String(_Download_Size_Buffer));

											// Declare Percent Char Buffer
											char _Download_Percent_Buffer[8];

											// Define frac function
											#define frac(x)     (int(1000*(x - int(x))))

											// Calculate Percent
											double _Percent = (double)this->FOTA.Download_Size / (double)this->FOTA.File_Size * 100;

											// Set Buffer
											sprintf(_Download_Percent_Buffer, "%03d.%02d", int(_Percent), frac(_Percent));

											// Print Download Percentage
											GSM_Terminal->Text(17, 112, _Console_CYAN_, String(_Download_Percent_Buffer));

											// Declare Percent Char Buffer
											char _Download_Time_Buffer[10];

											// Calculate Time
											uint32_t _Time = (millis() - _Download_Start_Time) / 1000;

											// Set Buffer
											sprintf(_Download_Time_Buffer, "%04lu", _Time);

											// Print Download Time
											GSM_Terminal->Text(18, 111, _Console_CYAN_, String(_Download_Time_Buffer));

										}

									#endif

									// Control for WD
									if (_WD > 100) break;

									// Turn ON HeartBeat 
									GSM_Hardware::WD_Heartbeat();

								}

							}

						}

						// Closing Socket
						if (AT_Command_Set::SH(_PostMan_Outgoing_Socket_)) {

							// Control for Incoming Call
							this->Listen(true);

						}

					}

					// Set Socket Configuration
					AT_Command_Set::SCFGEXT(_PostMan_Outgoing_Socket_, 1, 0, 0, 0, 0);

					//Work Delay
					delay(8);

					// Close File
					SD_File.close();

					// Control for Existing File
					if (GSM_SD.exists(_PostMan_Firmware_Name_)) {

						// Open File for Write
						SD_File = GSM_SD.open(_PostMan_Firmware_Name_, FILE_READ);

						// Get File Size
						this->FOTA.SD_File_Size = SD_File.size();

						// Control for File Size
						if (this->FOTA.File_Size == this->FOTA.SD_File_Size) {

							// Set Download Status
							this->FOTA.Download_Status = FOTA_Download_OK;

							// Print Message
							#ifdef _DEBUG_

								// Control for Terminal State
								if (this->Status.Terminal) {

									// Print File Name
									GSM_Terminal->Text(14, 116, _Console_GREEN_, F("OK"));

								}

							#endif

						} else {

							// Set Download Status
							this->FOTA.Download_Status = FOTA_Download_Size_Error;

							// Print Message
							#ifdef _DEBUG_

								// Control for Terminal State
								if (this->Status.Terminal) {

									// Print File Name
									GSM_Terminal->Text(14, 115, _Console_RED_, F("FAIL"));

								}

							#endif

						}

					} else {

						// Set Download Status
						this->FOTA.Download_Status = FOTA_Download_Not_Save;

					}

					// Set Download Duration
					this->FOTA.Download_Time = (millis() - _Download_Start_Time) / 1000;

					// Disable SD Multiplexer
					GSM_Hardware::SD_Multiplexer(false);

					// End Function
					if (this->FOTA.Download_Status == FOTA_Download_OK)	return(true);

					// End Function
					return(false);

				} else {

					// Print Message
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("File Not Opened...."));

						}

					#endif

					// Disable SD Multiplexer
					GSM_Hardware::SD_Multiplexer(false);

					// End Function
					return(false);

				}

			}

			// Disable SD Multiplexer
			GSM_Hardware::SD_Multiplexer(false);

			// End Function
			return(false);

		}

		// Update Signal Quality
		void Signal_Update(void) {

			// MONI Command (Network Status)
			if (this->Status.Connection) {

				// Clear Message Field
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT#MONI"));

					}

				#endif

				// Send Command
				bool _MONI_Command = AT_Command_Set::MONI(this->Operator.TAC, this->Operator.Cell_ID, this->Operator.RSSI, this->Operator.Signal, this->Operator.PCell_ID);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_MONI_Command, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+WS46?"));

					}

				#endif

				// Send Command
				bool _WS46_Command = AT_Command_Set::WS46(GET, this->Operator.WDS);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_WS46_Command, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						// Print Signal Level Value
						GSM_Terminal->Show_RSSI(14, 65, this->Operator.RSSI);

						// Print Signal Level Bar
						GSM_Terminal->Show_Signal_Quality(14, 74, this->Operator.Signal);

						// Print Operator Value
						char _Operator[6];
						sprintf(_Operator, "%d%d", this->Operator.MCC, this->Operator.MNC);
						GSM_Terminal->Text(15, 74, _Console_GRAY_, String(_Operator));

						// Show Connection Type
						GSM_Terminal->Show_Connection_Type(17, 70, this->Operator.WDS);

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

					}

				#endif

			} 

		}

		// Parse JSON Pack
		uint16_t Parse_JSON(const uint8_t _Pack_Type) {

			// Clear Pack
			memset(this->JSON_Pack, '\0', _PostMan_Send_JSON_Size_);

			// Define JSON
			StaticJsonDocument<_PostMan_Send_JSON_Size_> JSON;

			// Define Info Section
			JsonObject JSON_Info = JSON.createNestedObject(F("Info"));

			// Set Device ID Variable
			if (_Pack_Type == Pack_Online) JSON_Info[F("Command")] = F("Online");
			else if (_Pack_Type == Pack_Update) JSON_Info[F("Command")] = F("Update");
			else if (_Pack_Type == Pack_Timed) JSON_Info[F("Command")] = F("Timed");
			else if (_Pack_Type == Pack_Interrupt) JSON_Info[F("Command")] = F("Interrupt");
			else if (_Pack_Type == Pack_Alarm) JSON_Info[F("Command")] = F("Alarm");
			else if (_Pack_Type == Pack_Offline) JSON_Info[F("Command")] = F("Offline");
			else if (_Pack_Type == Pack_FOTA_Info) JSON_Info[F("Command")] = F("Pack_FOTA_Info");
			else if (_Pack_Type == Pack_FOTA_Download) JSON_Info[F("Command")] = F("FOTA_Download");
			else if (_Pack_Type == Pack_FOTA_Burn) JSON_Info[F("Command")] = F("FOTA_Burn");
			else JSON_Info[F("Command")] = F("Unknown");

			// Update RTC TimeStamp
			GSM_RTC.Update_Time_Stamp();

			// Set TimeStamp Variable
			JSON_Info[F("TimeStamp")] = GSM_RTC.Time_Stamp;

			// Set Device ID Variable
			JSON_Info[F("ID")] = GSM_Serial_ID.SerialID;

			// Set Firmware Version Variable
			JSON_Info[F("Firmware")] = String(_FIRMWARE_);

			// Define Device Section
			JsonObject JSON_Device = JSON.createNestedObject(F("Device"));

			// Define Power Section
			JsonObject JSON_Power = JSON_Device.createNestedObject(F("Power"));

			// Set Battery Variables
			JSON_Power[F("B_IV")] = (float)GSM_Battery_Gauge.Instant_Voltage();
			JSON_Power[F("B_AC")] = (float)GSM_Battery_Gauge.Average_Current();
			JSON_Power[F("B_IC")] = (float)GSM_Battery_Gauge.Instant_Capacity();
			JSON_Power[F("B_FC")] = (float)GSM_Battery_Gauge.Design_Capacity();
			JSON_Power[F("B_SOC")] = (float)GSM_Battery_Gauge.State_Of_Charge();
			JSON_Power[F("B_T")] = (float)GSM_Battery_Gauge.IC_Temperature();
			JSON_Power[F("B_CS")] = GSM_Charger.Charge_Status();

			// IoT Signal Quality Update
			this->Signal_Update();

			// Define Power Section
			JsonObject JSON_IoT = JSON_Device.createNestedObject(F("IoT"));

			// Set IoT Variables
			JSON_IoT[F("Firmware")] = this->Module.Firmware;
			JSON_IoT[F("IMEI")] = this->Module.IMEI;
			JSON_IoT[F("ICCID")] = this->Operator.ICCID;
			JSON_IoT[F("RSSI")] = this->Operator.RSSI;
			JSON_IoT[F("WDS")] = this->Operator.WDS;
			JSON_IoT[F("ConnTime")] = (float)this->Operator.Connection_Time / 1000;
			JSON_IoT[F("TAC")] = this->Operator.TAC;
			JSON_IoT[F("LAC")] = 0;
			JSON_IoT[F("Cell_ID")] = this->Operator.Cell_ID;

			// Define Payload Section
			JsonObject JSON_Payload = JSON.createNestedObject(F("Payload"));

			// Add TH Sensor Data
			JSON_Payload[F("PCB_T")] = GSM_TH_Sensor.Temperature();
			JSON_Payload[F("PCB_H")] = GSM_TH_Sensor.Humidity();

			// Set Payload Variables
			for (uint8_t i = 0; i < this->Data.Variable_Count; i++) {

				// Set Payload Variables
				JSON_Payload[this->Data.Variable[i].Name] = this->Data.Variable[i].Value;

			}

			// Clear Unused Data
			JSON.garbageCollect();

			// Clear Variables
			Variable::Clear();

			// Serialize JSON	
			uint16_t _JSON_Size = serializeJson(JSON, this->JSON_Pack);

			// Print JSON Pack
			#ifdef _DEBUG_

				// Control for Terminal State
				if (this->Status.Terminal) {

					// Print JSON
					GSM_Terminal->Text(24, 4, _Console_WHITE_, String(this->JSON_Pack).substring(0, 75));
					GSM_Terminal->Text(25, 4, _Console_WHITE_, String(this->JSON_Pack).substring(75, 150));
					GSM_Terminal->Text(26, 4, _Console_WHITE_, String(this->JSON_Pack).substring(150, 225));
					GSM_Terminal->Text(27, 4, _Console_WHITE_, String(this->JSON_Pack).substring(225, 300));
					GSM_Terminal->Text(28, 4, _Console_WHITE_, String(this->JSON_Pack).substring(300, 375));
					GSM_Terminal->Text(29, 4, _Console_WHITE_, String(this->JSON_Pack).substring(375, 450));
					GSM_Terminal->Text(30, 4, _Console_WHITE_, String(this->JSON_Pack).substring(450, 525));


				}

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

		// IP Address to String Converter Function
		String IPAddress_to_String(const uint8_t _IP_Address[4]) {

			// Define IP char Array
			char _IP_Char[16];

			// Set IP char Array
			sprintf(_IP_Char, "%03d.%03d.%03d.%03d", _IP_Address[0], _IP_Address[1], _IP_Address[2], _IP_Address[3]);

			// End Function
			return(String(_IP_Char));

		}

		// Module PCIEx Mask Function [PCIE0]
		inline void PCIEx_Mask(bool _PCIE0 = false, bool _PCIE1 = false, bool _PCIE2 = false) {

			// Define PCICR Mask
			uint8_t _PCICR_Mask = PCICR;

			// Set PCICR Mask
			if (_PCIE0) _PCICR_Mask |= (1 << PCIE0);	// Set PCIE0
			if (_PCIE1) _PCICR_Mask |= (1 << PCIE1);	// Set PCIE1
			if (_PCIE2) _PCICR_Mask |= (1 << PCIE2);	// Set PCIE2

			// Set PCICR with the calculated mask
			PCICR = _PCICR_Mask;

		}

		// Interrupt Function
		inline void PCINTxx_Interrupt(uint8_t _PCINT, bool _Status = false) {

			// Set PCINTxx Interrupt
			if (_PCINT >= 0 && _PCINT <= 7) {

				// Control for Status
				if (_Status) {

					// Set PCINTxx [0 - 7]
					PCMSK0 |= (1 << _PCINT);

				} else {

					// Clear PCINTxx [0 - 7]
					PCMSK0 &= ~(1 << _PCINT);

				}

			} else if (_PCINT >= 8 && _PCINT <= 15) {

				// Control for Status
				if (_Status) {

					// Set PCINTxx [8 - 15]
					PCMSK1 |= (1 << (_PCINT - 8));

				} else {

					// Clear PCINTxx [8 - 15]
					PCMSK1 &= ~(1 << (_PCINT - 8));

				}

			} else if (_PCINT >= 16 && _PCINT <= 23) {

				// Control for Status
				if (_Status) {

					// Set PCINTxx [16 - 23]
					PCMSK2 |= (1 << (_PCINT - 16));

				} else {

					// Clear PCINTxx [16 - 23]
					PCMSK2 &= ~(1 << (_PCINT - 16));

				}

			}

		}

	// Public Functions
	public:

		// Set Callback Function
		void Set_Interval_Update_CallBack(CallBack_Interval_Update _CallBack) {
			
			// Set Callback Function
			_CallBack_Interval_Update = _CallBack;

		}
		void Set_Energy_Update_CallBack(CallBack_Energy_Update _CallBack) {
			
			// Set Callback Function
			_CallBack_Energy_Update = _CallBack;

		}
		void Set_Pressure_Update_CallBack(CallBack_Pressure_Update _CallBack) {
			
			// Set Callback Function
			_CallBack_Pressure_Update = _CallBack;

		}
		void Set_Mask_Update_CallBack(CallBack_Mask_Update _CallBack) {
			
			// Set Callback Function
			_CallBack_Mask_Update = _CallBack;

		}

		// Define Interrupt Structure
		struct Interrupt_Structure {

			// Ring Interrupt
			bool Ring = false;

			// FOTA Download Interrupt
			bool FOTA_Download = false;

			// Pack Type Interrupt
			uint8_t Pack_Type = Pack_None;

		};

		// Define Interrupt Variables
		static Interrupt_Structure PostMan_Interrupt;

		// PCMSK1 Mask Handler Function
		static void PCMSK1_Handler(void) {

			// Set Interrupt State
			PostMan_Interrupt.Ring = true;

		}

		// PostMan Constructor
		Postman_PowerStatV4(Stream &_Serial, PowerStat_Console& _Terminal) : AT_Command_Set(_Serial), GSM_Hardware(), Variable(), GSM_Terminal(&_Terminal), _CallBack_Interval_Update(nullptr), _CallBack_Energy_Update(nullptr), _CallBack_Pressure_Update(nullptr), _CallBack_Mask_Update(nullptr) {

			// Control Terminal
			if (GSM_Terminal != nullptr) {this->Status.Terminal = true;} else {this->Status.Terminal = false;}

			// Initialize Modem Variables
			this->Module.Manufacturer = 0;
			this->Module.Model = 0;
			memset(this->Module.IMEI, '\0', 17);
			memset(this->Module.Firmware, '\0', 15);

			// Initialize Operator Variables
			memset(this->Operator.ICCID, '\0', 21);
			this->Operator.MCC = 0;
			this->Operator.MNC = 0;
			this->Operator.WDS = 0;
			this->Operator.RSSI = 0;
			this->Operator.Signal = 0;
			this->Operator.TAC = 0;
			this->Operator.Cell_ID = 0;
			this->Operator.PCell_ID = 0;
			memset(this->Operator.IP_Address, 0, 4);
			this->Operator.Connection_Time = 0;	

			// Initialize JSON_Pack Variable
			memset(this->JSON_Pack, '\0', _PostMan_Send_JSON_Size_);

		}
		Postman_PowerStatV4(Stream &_Serial) : AT_Command_Set(_Serial), GSM_Hardware(), Variable(), GSM_Terminal(nullptr), _CallBack_Interval_Update(nullptr), _CallBack_Energy_Update(nullptr), _CallBack_Pressure_Update(nullptr), _CallBack_Mask_Update(nullptr) {

			// Control Terminal
			if (GSM_Terminal != nullptr) {this->Status.Terminal = true;} else {this->Status.Terminal = false;}

			// Initialize Modem Variables
			this->Module.Manufacturer = 0;
			this->Module.Model = 0;
			memset(this->Module.IMEI, '\0', 17);
			memset(this->Module.Firmware, '\0', 15);

			// Initialize Operator Variables
			memset(this->Operator.ICCID, '\0', 21);
			this->Operator.MCC = 0;
			this->Operator.MNC = 0;
			this->Operator.WDS = 0;
			this->Operator.RSSI = 0;
			this->Operator.Signal = 0;
			this->Operator.TAC = 0;
			this->Operator.Cell_ID = 0;
			this->Operator.PCell_ID = 0;
			memset(this->Operator.IP_Address, 0, 4);
			this->Operator.Connection_Time = 0;	

			// Initialize JSON_Pack Variable
			memset(this->JSON_Pack, '\0', _PostMan_Send_JSON_Size_);

		}

		// Begin Function
		void Begin(void) {

			// Enable SD Multiplexer
			GSM_Hardware::SD_Multiplexer(true);

			// Start SD Card
			GSM_SD.begin(53);

			// Set Device_ID Variable
			GSM_Serial_ID.Begin();

			// Start RTC
			GSM_RTC.Begin();

			// Start TH Sensor
			GSM_TH_Sensor.Begin();

			// Start Battery Gauge
			GSM_Battery_Gauge.Begin();

			// Start Charger
			GSM_Charger.Begin();

		}

		// Print GSM Variables
		void Print_GSM_Variables(void) {

			// Print Batch Description
			#ifdef _DEBUG_

				// Control for Terminal State
				if (this->Status.Terminal) {

					// Print GSM Detail Variables
					GSM_Terminal->Text(13, 37, _Console_GRAY_, String(this->Module.Manufacturer));
					GSM_Terminal->Text(14, 37, _Console_GRAY_, String(this->Module.Model));
					GSM_Terminal->Text(15, 30, _Console_GRAY_, String(this->Module.Firmware));
					GSM_Terminal->Text(16, 24, _Console_GRAY_, String(this->Module.IMEI));
					GSM_Terminal->Text(18, 20, _Console_GRAY_, String(this->Operator.ICCID));

					// Print GSM Connection Variables
					GSM_Terminal->Text(13, 74, _Console_GRAY_, String(this->Operator.Connection_Time));

					// Print Signal Level Value
					GSM_Terminal->Show_RSSI(14, 65, this->Operator.RSSI);

					// Print Signal Level Bar
					GSM_Terminal->Show_Signal_Quality(14, 74, this->Operator.Signal);

					// Print Operator Value
					char _Operator[6];
					sprintf(_Operator, "%d%d", this->Operator.MCC, this->Operator.MNC);
					GSM_Terminal->Text(15, 74, _Console_GRAY_, String(_Operator));

					// Print IP
					GSM_Terminal->Text(16, 64, _Console_GRAY_, this->IPAddress_to_String(this->Operator.IP_Address));

					// Show Connection Type
					GSM_Terminal->Show_Connection_Type(17, 70, this->Operator.WDS);


				}

			#endif

		}

		// Cloud Connect Function
		void Cloud_Connect(void) {

			// Begin Function
			this->Begin();

			// Power On Modem
			GSM_Hardware::ON();

			// Set Connection Time
			this->Buffer.Connection_Time_Buffer = millis();

			// Initialize Modem
			this->Initialize();
			
			// Connect Modem
			this->Connect();

			// Set Firewall
			this->Firewall();
			
			// Update Clock
			this->Syncronize_Time();

			// Open Socket
			this->Listen(true);

			// Disable Interrupts
			cli();

			// Set PCICR Mask
			this->PCIEx_Mask(false, true, false);

			// Enable Ring Interrupt
			this->PCINTxx_Interrupt(11, true);

			// Enable Interrupts
			sei();

		}

		// Set Clock Configuration
		bool Syncronize_Time(void) {

			// Control for Connection
			if (this->Status.Connection) {

				// Print Batch Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 30, _Console_WHITE_, F("[    ]"));

						// Print Command Description
						GSM_Terminal->AT_Command(_Terminal_Message_X_, _Terminal_Message_Y_, F("AT+CCLK"));

					}

				#endif

				// CCLK Command (Real Time Clock Configuration)
				bool _Clock_State = AT_Command_Set::CCLK(this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second, this->Time.Time_Zone);

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Command State
						GSM_Terminal->OK(_Clock_State, _Terminal_Message_X_, _Terminal_Message_Y_ + 31);

						// Print Command Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Updating RTC Clock..."));

					}

				#endif

				// Set RTC Clock
				GSM_RTC.Set_Time(this->Time.Second, this->Time.Minute, this->Time.Hour, this->Time.Day, this->Time.Month, this->Time.Year);

				// Clear Message Field
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

					}

				#endif

				// Connection Success
				return(true);

			}

			// Time Configuration Failed
			return(false);

		}

		// Add Variable Function
		void Add_Variable(const char* _Name, float _Value) {

			// Add Variable
			Variable::Add(_Name, _Value);

		}

		// Send Data Batch Function
		bool Publish(const uint8_t _Pack_Type) {

			// Control for Connection
			if (this->Status.Connection) {

				// Stop Socket Listen
				this->Listen(false);

				// Clear Message Field
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

					}

				#endif

				// Set Send Time Start
				uint32_t _Send_Time_Start = millis();

				// Print Message
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Connecting to Server...              "));

					}

				#endif

				// Open Connection
				if (AT_Command_Set::ATSD(_PostMan_Outgoing_Socket_, 0, 80, 255, 88, 1, _PostMan_Server_)) {

					// Print Message
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Parsing JSON Pack...                 "));

						}

					#endif

					// Parse JSON
					this->Parse_JSON(_Pack_Type);

					// Print Message
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Batch Description
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
							GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Sending Pack...                      "));

						}

					#endif

					// Sending Data
					if (AT_Command_Set::SSEND(_PostMan_Outgoing_Socket_, HTTP_POST, 0, _PostMan_EndPoint_, this->JSON_Pack)) {

						// Print Message
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Batch Description
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
								GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Waiting Response...                  "));

							}

						#endif

						// Declare Ring Length
						uint16_t _Length;

						// Get Ring Port
						if (AT_Command_Set::SRING(_Length)) {

							// Declare Response Variable
							char _Response[_PostMan_Response_JSON_Size_];

							// Clear Response
							memset(_Response, '\0', _PostMan_Response_JSON_Size_);

							// Command Delay
							delay(10);

							// Print Message
							#ifdef _DEBUG_

								// Control for Terminal State
								if (this->Status.Terminal) {

									// Print Batch Description
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
									GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Getting Response...                  "));

								}

							#endif

							// Handle Length
							if (_Length > _PostMan_Response_JSON_Size_) _Length = _PostMan_Response_JSON_Size_;

							// Get Request Data
							if (AT_Command_Set::SRECV(_PostMan_Outgoing_Socket_, _Length, _Response)) {

								// Declare Response Variable
								char _JSON_Pack[_PostMan_Response_JSON_Size_];

								// Clear Response
								memset(_JSON_Pack, '\0', _PostMan_Response_JSON_Size_);

								// Define JSON Object
								StaticJsonDocument<_PostMan_Response_JSON_Size_> Incoming_JSON;

								// Deserialize the JSON document
								deserializeJson(Incoming_JSON, _JSON_Pack);
								
								// Get Response Command
								uint16_t _Response_Command = Incoming_JSON["Event"];

								// Print Message
								#ifdef _DEBUG_

									// Control for Terminal State
									if (this->Status.Terminal) {

										// Print Batch Description
										GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
										GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Response --> [   ]                   "));
										GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_ + 14, _Console_CYAN_, String(_Response_Command));

									}

								#endif

								// Calculate Send Time
								uint32_t _Send_Duration = millis() - _Send_Time_Start;

								// Print Message
								#ifdef _DEBUG_

									// Control for Terminal State
									if (this->Status.Terminal) {

										// Print Batch Description
										GSM_Terminal->Text(2, 116, _Console_CYAN_, F("     "));
										GSM_Terminal->Text(2, 116, _Console_CYAN_, String(_Send_Duration));

									}

								#endif

								// Command Delay
								delay(50);

								// Print Message
								#ifdef _DEBUG_

									// Control for Terminal State
									if (this->Status.Terminal) {

										// Print Batch Description
										GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
										GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Closing Socket...                    "));

									}

								#endif

								// Closing Socket
								if (AT_Command_Set::SH(_PostMan_Outgoing_Socket_)) {

									// Control for Incoming Call
									this->Listen(true);

									// Print Message
									#ifdef _DEBUG_

										// Control for Terminal State
										if (this->Status.Terminal) {

											// Print Batch Description
											GSM_Terminal->Beep();

										}

									#endif

									// Clear Variables
									if (_Response_Command == 200) {

										// End Function
										return(true);

									}

									// End Function
									return(false);

								}

							}

						}

					}

				}

				// Port Control
				this->Listen(true);

			}

			// End Function
			return(false);

		}

		// Control for Process Function
		void Heartbeat(void) {

			// Get Ring Port
			this->Get();

		}

};

// Define Interrupt Variables Structure
Postman_PowerStatV4::Interrupt_Structure Postman_PowerStatV4::PostMan_Interrupt;

// Interrupt Routine PCMSK1 [PCINT11]
ISR(PCINT1_vect) {

	// PCMSK1 Handler
	Postman_PowerStatV4::PCMSK1_Handler();

}
