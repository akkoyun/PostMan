#ifndef __PostMan_FOTA__
#define __PostMan_FOTA__

	// Define Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Define Library Includes
	#ifndef __Telit_xE910_Includes__
		#include "Config/Includes.h"
	#endif

	// Define Config Includes
	#ifndef __PostMan_Config__
		#include "Config/Config.h"
	#endif

	// Define Library Includes
	#ifndef __Telit_AT_Command_Set__
		#include "AT_Command_Set.h"
	#endif

	// FOTA Functions
	class FOTA : private AT_Command_Set {

		// Public Functions
		public:

			// Define Variables
			struct Struct_FOTA {
				uint32_t		File_ID				= 0;
				uint32_t		Download_Time		= 0;
				uint32_t		File_Size			= 0;
				uint32_t		SD_File_Size		= 0;
			} Variables;

			// FOTA Constructor
			FOTA(Stream &_Serial) : AT_Command_Set(_Serial) {

			}

			// Firmware Download Function
			bool Download(const uint16_t _File_ID) {

				// Control Variable Structure
				struct Struct_Function_Control {
					uint8_t Error_WD				= 0;
					bool Response					= true;
					bool Function					= true;
				} Control;

				// Activate Mux
				DDRC |= 0b00000001; PORTC |= 0b00000001;
				delay(200);

				// Declare SD File Object
				File SD_File;

				// Control for Existing File
				if (SD.exists(_FOTA_SD_FileName_)) {

					// Remove Existing File
					SD.remove(_FOTA_SD_FileName_);

					// Command Delay
					delay(100);

					// SD Message
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, GREEN, F("                               "));
						Terminal_GSM.Text(14, 44, GREEN, F("File Exist and Deleted."));
					#endif

				}

				// Open File for Write
				SD_File = SD.open(_FOTA_SD_FileName_, O_WRITE | O_CREAT);

				// Command Delay
				delay(100);

				// Control for File Open
				if (SD_File) {

					// FTPTO Command
					#ifdef _AT_FTPTO_

						// Reset Control Variables
						Control.Error_WD = 0;
						Control.Response = false;

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
							Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPTO=500"));
							Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
						#endif

						// Process Command
						while (!Control.Response) {

							// Send Command
							Control.Response = AT_Command_Set::FTPTO(FTP_TimeOut);

							// Set WD Variable
							Control.Error_WD++;

							// Control for WD
							if (Control.Error_WD > 4) break;

						}
					
						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.OK_Decide(Control.Response, 14, 35);
						#endif

						// End Function
						if (!Control.Response) return (false);

						// Set Function Variable
						if (!Control.Response) Control.Function = false;

					#endif

					// FTPOPEN Command
					#ifdef _AT_FTPOPEN_

						// Chain Control
						if (Control.Function) {

							// Reset Control Variables
							Control.Error_WD = 0;
							Control.Response = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPOPEN=***,***,***"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPOPEN="165.227.154.147","fota","123456",1\r\n

							// Process Command
							while (!Control.Response) {

								// Send Command
								Control.Response = AT_Command_Set::FTPOPEN(_FOTA_Server_, _FOTA_UserName_, _FOTA_PassWord_, 1);

								// Set WD Variable
								Control.Error_WD++;

								// Control for WD
								if (Control.Error_WD > 4) break;

							}

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(Control.Response, 14, 35);
							#endif

							// Set Function Variable
							if (!Control.Response) Control.Function = false;

						}

					#endif

					// FTPTYPE Command
					#ifdef _AT_FTPTYPE_

						// Chain Control
						if (Control.Function) {

							// Reset Control Variables
							Control.Error_WD = 0;
							Control.Response = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPTYPE=0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPTYPE=0\r\n

							// Process Command
							while (!Control.Response) {

								// Send Command
								Control.Response = AT_Command_Set::FTPTYPE(0);

								// Set WD Variable
								Control.Error_WD++;

								// Control for WD
								if (Control.Error_WD > 4) break;

							}

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(Control.Response, 14, 35);
							#endif

							// Set Function Variable
							if (!Control.Response) Control.Function = false;

						}

					#endif

					// FTPCWD Command
					#ifdef _AT_FTPCWD_

						// Chain Control
						if (Control.Function) {

							// Reset Control Variables
							Control.Error_WD = 0;
							Control.Response = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPCWD=\"firmware\""));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPCWD="firmware"\r\n

							// Process Command
							while (!Control.Response) {

								// Send Command
								Control.Response = AT_Command_Set::FTPCWD(_FOTA_Folder_);

								// Set WD Variable
								Control.Error_WD++;

								// Control for WD
								if (Control.Error_WD > 4) break;

							}

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(Control.Response, 14, 35);
							#endif

							// Set Function Variable
							if (!Control.Response) Control.Function = false;

						}				

					#endif

					// FTPSFIZE Command
					#ifdef _AT_FTPFSIZE_

						// Chain Control
						if (Control.Function) {

							// Reset Control Variables
							Control.Error_WD = 0;
							Control.Response = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPFSIZE=\"xxx.hex\""));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPFSIZE="6.hex"\r\n

							// Process Command
							while (!Control.Response) {

								// Send Command
								Control.Response = AT_Command_Set::FTPFSIZE(_File_ID, this->Variables.File_Size);

								// Set WD Variable
								Control.Error_WD++;

								// Control for WD
								if (Control.Error_WD > 4) break;

							}

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(Control.Response, 14, 35);
							#endif

							// Set Function Variable
							if (!Control.Response) Control.Function = false;

						}

					#endif

					// FTPGETPKT Command
					#ifdef _AT_FTPGETPKT_

						// Chain Control
						if (Control.Function) {

							// Reset Control Variables
							Control.Error_WD = 0;
							Control.Response = false;

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 4, GRAY, F(".............................."));
								Terminal_GSM.Text(14, 4, WHITE, F("AT#FTPGETPKT=\"xxx.hex\",0"));
								Terminal_GSM.Text(14, 35, BLUE, F(" .. "));
							#endif

							// AT#FTPGETPKT="6.hex",0\r\n

							// Process Command
							while (!Control.Response) {

								// Send Command
								Control.Response = AT_Command_Set::FTPGETPKT(_File_ID, 0);

								// Set WD Variable
								Control.Error_WD++;

								// Control for WD
								if (Control.Error_WD > 4) break;

							}

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.OK_Decide(Control.Response, 14, 35);
							#endif

							// Set Function Variable
							if (!Control.Response) Control.Function = false;

						}				

					#endif

					#ifdef GSM_Debug
						Terminal_GSM.Text(17, 113, GREEN, String(_File_ID));
						Terminal_GSM.Text(19, 112, GREEN, String(Variables.File_Size));
					#endif

					// SD Message
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, GREEN, F("                               "));
						Terminal_GSM.Text(14, 44, GREEN, F("SD Opened"));
					#endif

					// Get Time
					uint32_t _Download_Start_Time = millis();

					// Set Data Variable
					char _Data[250];

					// Define Variable
					uint32_t _SD_Recieve_Size = 0;
					uint16_t _RecieveSize = 0;
					uint8_t _State = 0;

					// SD Message
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, GREEN, F("                               "));
						Terminal_GSM.Text(14, 44, GREEN, F("Downloading"));
					#endif

					// Reset Control Variables
					Control.Response = false;

					// Get Pack
					while (!Control.Response) {

						// Set Data Variable
						memset(_Data, '\0', 250);

						// Reset Variable
						_RecieveSize = 0;

						// Send Command
						if (AT_Command_Set::FTPRECV(200, _RecieveSize, _State, _Data)) {

							if (_RecieveSize > 0) {

								// Write Data
								SD_File.write(_Data, _RecieveSize);

								// Handle Recieve Size
								_SD_Recieve_Size += _RecieveSize;

								// Control for File End
								if (_SD_Recieve_Size == Variables.File_Size) Control.Response = true;

							}

						} else {

							// Control for State
							if (_State == 1) break;

						}

						// Work Delay
						delay(50);

						// Calculate Download Time
						Variables.Download_Time = (millis() - _Download_Start_Time) / 1000;

						// SD Message
						#ifdef GSM_Debug
							Terminal_GSM.Text(21, 114, CYAN, String("   "));
							Terminal_GSM.Text(21, 114, CYAN, String(map(_SD_Recieve_Size, 0, Variables.File_Size, 0, 100)));

							Terminal_GSM.Text(22, 111, CYAN, String("    "));
							Terminal_GSM.Text(22, 111, CYAN, String(Variables.Download_Time));

							Terminal_GSM.Text(20, 112, WHITE, F("       "));
							Terminal_GSM.Text(20, 112, WHITE, String(_SD_Recieve_Size));

							Terminal_GSM.Text(16, 90, WHITE, F(" "));
							Terminal_GSM.Text(16, 90, WHITE, String(_State));

						#endif

						// End Function
						if (Variables.Download_Time > 1200) break;

					}

					delay(8);
					SD_File.close();

					// SD Message
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, GREEN, F("                               "));
						Terminal_GSM.Text(14, 44, GREEN, F("SD Closed"));
					#endif

					// FTPCLOSE Command
					#ifdef _AT_FTPCLOSE_

						// Reset Control Variables
						Control.Error_WD = 0;
						Control.Response = false;

						// Process Command
						while (!Control.Response) {

							// Send Command
							Control.Response = AT_Command_Set::FTPCLOSE();

							// Set WD Variable
							Control.Error_WD++;

							// Control for WD
							if (Control.Error_WD > 4) break;

						}
					
						// End Function
						if (!Control.Response) return (false);

						// Set Function Variable
						if (!Control.Response) Control.Function = false;

					#endif

				} else {

					// SD Message
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, GREEN, F("                               "));
						Terminal_GSM.Text(14, 44, RED, F("File Not Opened"));
					#endif

				}

				// Control for Existing File
				if (SD.exists(_FOTA_SD_FileName_)) {

					// Open File for Write
					SD_File = SD.open(_FOTA_SD_FileName_, FILE_READ);

					// Get File Size
					this->Variables.SD_File_Size = SD_File.size();

					// Control for File Size
					if (this->Variables.File_Size == this->Variables.SD_File_Size) {

						// SD Message
						#ifdef GSM_Debug
							Terminal_GSM.OK_Decide(true, 18, 115);
							Terminal_GSM.Text(19, 112, GREEN, String(this->Variables.SD_File_Size));
						#endif

						// Turn SD MUX Enable LOW
						PORTC &= 0b11111110;

						// End Function
						return(true);

					} else {

						// SD Message
						#ifdef GSM_Debug
							Terminal_GSM.OK_Decide(false, 18, 115);
							Terminal_GSM.Text(19, 112, RED, String(this->Variables.SD_File_Size));
						#endif

						// Turn SD MUX Enable LOW
						PORTC &= 0b11111110;

						// End Function
						return(false);

					}

					// End Function
					return(false);

				} else {

					// Turn SD MUX Enable LOW
					PORTC &= 0b11111110;

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

	};

#endif /* defined(__PostOffice_FOTA__) */