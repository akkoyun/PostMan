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
#include <SPI.h>
#include "SD.h"

// Include Definitions
#include "AT_Command/Definitions/Command.h"
#include "AT_Command/Definitions/Pack.h"

// Cloud Functions
class Postman_PowerStatV4 : private AT_Command_Set, private GSM_Hardware {

	// Private Context
	private:

		// Define Objects
		PowerStat_Console* GSM_Terminal;
		RV3028 GSM_RTC;
		HDC2010 GSM_TH_Sensor;
		MAX17055 GSM_Battery_Gauge;
		BQ24298 GSM_Charger;
		DS28C GSM_Serial_ID;

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
				char Name[10];

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

					// AT Command
					if (this->Status.Initialize) {

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

							}

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

					} else break;

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

							}

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

							}

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

							}

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

							}

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

							}

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

							}

						#endif

						// Print SIM State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

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

							}

						#endif

						// Print IMEI
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print IMEI
								GSM_Terminal->Text(16, 24, _Console_GRAY_, String(this->Module.IMEI));

							}

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

							}

						#endif

						// Print ICCID
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print ICCID
								GSM_Terminal->Text(18, 20, _Console_GRAY_, String(this->Operator.ICCID));

							}

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

							}

						#endif

						// Print Manufacturer
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Manufacturer
								GSM_Terminal->Text(13, 37, _Console_GRAY_, String(this->Module.Manufacturer));

							}

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

							}

						#endif

						// Print Model
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Model
								GSM_Terminal->Text(14, 37, _Console_GRAY_, String(this->Module.Model));

							}

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

							}

						#endif

						// Print Firmware
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Firmware Version
								GSM_Terminal->Text(15, 30, _Console_GRAY_, String(this->Module.Firmware));							

							}

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

							}

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

							}

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

							}

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

							}

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

							}

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

							}

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

							}

						#endif

						// Print IP Address
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print IP Address
								GSM_Terminal->Text(16, 64, _Console_GRAY_, this->IPAddress_to_String(this->Operator.IP_Address));

							}

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

							}

						#endif

						// Print Connection Type
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Connection Type
								GSM_Terminal->Show_Connection_Type(17, 70, this->Operator.WDS);

							}

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

							}

						#endif

						// Print Command State
						#ifdef _DEBUG_

							// Control for Terminal State
							if (this->Status.Terminal) {

								// Print Signal Level Value
								GSM_Terminal->Show_RSSI(14, 65, this->Operator.RSSI);

								// Print Signal Level Bar
								GSM_Terminal->Show_Signal_Quality(14, 74, this->Operator.Signal);

								// Print Operator Value
								char _Operator[6];
								sprintf(_Operator, "%d%d", this->Operator.MCC, this->Operator.MNC);
								GSM_Terminal->Text(15, 74, _Console_GRAY_, String(_Operator));

							}

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

							}

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

							}

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

							}

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

							}

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

							}

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

					}

				#endif

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

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

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

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

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

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

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

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

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

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

					}

				#endif

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

						}

					#endif

					// Print Socket State
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Socket State
							if (this->Status.Socket_State == 0) GSM_Terminal->Text(18, 64, _Console_RED_, F("    Closed     "));
							else if (this->Status.Socket_State == 1) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("Active Transfer"));
							else if (this->Status.Socket_State == 2) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("   Suspended   "));
							else if (this->Status.Socket_State == 3) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("  Pending Data "));
							else if (this->Status.Socket_State == 4) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("   Listening   "));

						}

					#endif

					// Print Batch Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

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

						}

					#endif

					// Print Socket State
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

							// Print Socket State
							if (this->Status.Socket_State == 0) GSM_Terminal->Text(18, 64, _Console_RED_, F("    Closed     "));
							else if (this->Status.Socket_State == 1) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("Active Transfer"));
							else if (this->Status.Socket_State == 2) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("   Suspended   "));
							else if (this->Status.Socket_State == 3) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("  Pending Data "));
							else if (this->Status.Socket_State == 4) GSM_Terminal->Text(18, 64, _Console_GRAY_, F("   Listening   "));

						}

					#endif

					// Print Batch Description
					#ifdef _DEBUG_

						// Control for Terminal State
						if (this->Status.Terminal) {

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
					for (uint16_t i = 0; i < _PostMan_Recieve_JSON_Size_; i++) {

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
					if (_Event == Command_Reset) {

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

						// Send Response
						this->Response(HTTP_OK, Command_OK);

						// Handle FW ID
						uint16_t _Firmware_ID = Incoming_JSON["Request"]["FW_ID"];

						// Call User Callback
						if (_CallBack_FOTA != nullptr) _CallBack_FOTA(_Firmware_ID);

					} else {

						// Send Response
						this->Response(HTTP_BadRequest, Command_NOK);

					}

					// Port Control
					this->Listen(true);

				} 

			}

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

					}

				#endif

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

				#endif

				// Clear Message Field
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));

					}

				#endif

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

				#endif

				// Print Command State
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

				#endif

				// Clear Message Field
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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
			else if (_Pack_Type == Pack_FOTA_Info) JSON_Info[F("Command")] = F("FOTA_Download");
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
			this->Clear_Variable();

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

		// Define Callback Function
		typedef void (*CallBack_FOTA)(uint16_t);

		// Define Callback Function
		CallBack_FOTA _CallBack_FOTA;

		// Set Callback Function
		void Set_FOTA_CallBack(CallBack_FOTA _CallBack) {
			
			// Set Callback Function
			_CallBack_FOTA = _CallBack;

		}

		// Define Interrupt Structure
		struct Interrupt_Structure {
			
			// Ring Interrupt
			bool Ring = false;

			// Publish Interrupt Variable
			uint8_t Publish = Pack_None;

		};

		// Define Interrupt Variables
		static Interrupt_Structure PostMan_Interrupt;

		// PCMSK1 Mask Handler Function
		static void PCMSK1_Handler(void) {

			// Set Interrupt State
			PostMan_Interrupt.Ring = true;

		}

		// PostMan Constructor
		Postman_PowerStatV4(Stream &_Serial, PowerStat_Console& _Terminal) : 
			AT_Command_Set(_Serial), 
			GSM_Hardware(), 
			GSM_Terminal(&_Terminal), 
			_CallBack_FOTA(nullptr) {

			// Control Terminal
			if (GSM_Terminal != nullptr) {this->Status.Terminal = true;} else {this->Status.Terminal = false;}

		}
		Postman_PowerStatV4(Stream &_Serial) : 
			AT_Command_Set(_Serial), 
			GSM_Hardware(), 
			GSM_Terminal(nullptr), 
			_CallBack_FOTA(nullptr) {

			// Control Terminal
			if (GSM_Terminal != nullptr) {this->Status.Terminal = true;} else {this->Status.Terminal = false;}

		}

		// Begin Function
		void Begin(void) {

			// Set SD Mulptiplexer ON
			GSM_Hardware::SD_Multiplexer(true);

			// Start SD Card
			SD.begin(53);

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

					}

				#endif

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

					}

				#endif

				// Print Command Description
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

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

		// Set Variables Function
		bool Variable(const char* _Name, float _Value) {

			// Control for Existing Variable
			for (uint8_t i = 0; i < this->Data.Variable_Count; i++) {

				// Check for Existing Variable
				if (strcmp(Data.Variable[i].Name, _Name) == 0) {

					// Update Value
					Data.Variable[i].Value = _Value;

					// End Function
					return true;

				}

			}

			// Control for Max Variable Count
			if (this->Data.Variable_Count < MAX_VARIABLE_COUNT) {

				//	Set Variable Name
				strncpy(Data.Variable[this->Data.Variable_Count].Name, _Name, sizeof(Data.Variable[this->Data.Variable_Count].Name) - 1);

				//	Set Variable Name Terminator
				Data.Variable[this->Data.Variable_Count].Name[sizeof(Data.Variable[this->Data.Variable_Count].Name) - 1] = '\0';

				// Set Variable Value
				Data.Variable[this->Data.Variable_Count].Value = _Value;

				// Increase Variable Count
				this->Data.Variable_Count++;

				// End Function
				return true;

			}

			// Failed to add variable (likely due to reaching max count)
			return false;

		}
		void Clear_Variable(void) {

			// Loop through all variables
			for (uint8_t i = 0; i < MAX_VARIABLE_COUNT; i++) {

				// Clear the name of the variable
				memset(Data.Variable[i].Name, '\0', sizeof(Data.Variable[i].Name));

			}

			// Clear the variable count
			this->Data.Variable_Count = 0;

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

		// Download Firmware
		bool Download(const uint16_t _Firmware_ID) {

			// Control for Connection
			if (this->Status.Connection) {

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

					}

				#endif

				// Print Message
				#ifdef _DEBUG_

					// Control for Terminal State
					if (this->Status.Terminal) {

						// Print Batch Description
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("                                     "));
						GSM_Terminal->Text(_Terminal_Message_X_, _Terminal_Message_Y_, _Console_CYAN_, F("Connecting to Server...              "));

						// Print File Name
						GSM_Terminal->Text(13, 116, _Console_CYAN_, String(_Firmware_ID));

					}

				#endif

				// Declare SD File Object
				File SD_File;

				// Control for Existing File
				if (SD.exists(_PostMan_Firmware_Name_)) {

					// Remove Existing File
					SD.remove(_PostMan_Firmware_Name_);

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
				SD_File = SD.open(_PostMan_Firmware_Name_, O_WRITE | O_CREAT);

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
					if (SD.exists(_PostMan_Firmware_Name_)) {

						// Open File for Write
						SD_File = SD.open(_PostMan_Firmware_Name_, FILE_READ);

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
					FOTA.Download_Time = (millis() - _Download_Start_Time) / 1000;

					// Add Variable to Payload
					this->Variable("FOTA_ID", _Firmware_ID);
					this->Variable("FOTA_Status", FOTA.Download_Status);

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

					// End Function
					return(false);

				}

			}

			// End Function
			return(false);

		}

		// Control for Process Function
		void Heartbeat(void) {

			// Ring Control
			if (PostMan_Interrupt.Ring) {

				// Reset Ring
				PostMan_Interrupt.Ring = false;

				// Get Server Command
				this->Get();

			}

		}

};

// Define Interrupt Variables Structure
Postman_PowerStatV4::Interrupt_Structure Postman_PowerStatV4::PostMan_Interrupt;

// Interrupt Routine PCMSK1 [PCINT11]
ISR(PCINT1_vect, ISR_NOBLOCK) {

	// PCMSK1 Handler
	Postman_PowerStatV4::PCMSK1_Handler();

}

