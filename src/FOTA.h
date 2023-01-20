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
				uint32_t		Download_Time		= 0;
				uint32_t		File_Size			= 0;
				uint32_t		SD_File_Size		= 0;
			} Variables;

			// FOTA Constructor
			FOTA(Stream &_Serial) : AT_Command_Set(_Serial) {

				// Define FTP Command Parameters
				#define FTP_TimeOut			(uint16_t)500
				#define FTP_OpenMode		(uint8_t)1
				#define FTP_TransferType	(uint8_t)0
				#define FTP_ViewMode		(uint8_t)0

			}

			// Firmware Download Function
			bool Download(const uint16_t _File_ID) {

				// Control Variable Structure
				struct Struct_Function_Control {
					uint8_t Error_WD				= 0;
					bool Response					= true;
					bool Function					= true;
				} Control;

				// FTPTO Command
				#ifdef _AT_FTPTO_

					// Reset Control Variables
					Control.Error_WD = 0;
					Control.Response = false;

					// Process Command
					while (!Control.Response) {

						// Send Command
						Control.Response = AT_Command_Set::FTPTO(FTP_TimeOut);

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

				// FTPOPEN Command
				#ifdef _AT_FTPOPEN_

					// Chain Control
					if (Control.Function) {

						// Reset Control Variables
						Control.Error_WD = 0;
						Control.Response = false;

						// Process Command
						while (!Control.Response) {

							// Send Command
							Control.Response = AT_Command_Set::FTPOPEN(FOTA_Server, FOTA_UserName, FOTA_PassWord, FTP_OpenMode);

							// Set WD Variable
							Control.Error_WD++;

							// Control for WD
							if (Control.Error_WD > 4) break;

						}

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

						// Process Command
						while (!Control.Response) {

							// Send Command
							Control.Response = AT_Command_Set::FTPTYPE(FTP_TransferType);

							// Set WD Variable
							Control.Error_WD++;

							// Control for WD
							if (Control.Error_WD > 4) break;

						}

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

						// Process Command
						while (!Control.Response) {

							// Send Command
							Control.Response = AT_Command_Set::FTPCWD(FOTA_Folder);

							// Set WD Variable
							Control.Error_WD++;

							// Control for WD
							if (Control.Error_WD > 4) break;

						}

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

						// Process Command
						while (!Control.Response) {

							// Send Command
							Control.Response = AT_Command_Set::FTPFSIZE(_File_ID, Variables.File_Size);

							// Set WD Variable
							Control.Error_WD++;

							// Control for WD
							if (Control.Error_WD > 4) break;

						}

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

						// Process Command
						while (!Control.Response) {

							// Send Command
							Control.Response = AT_Command_Set::FTPGETPKT(_File_ID, FTP_ViewMode);

							// Set WD Variable
							Control.Error_WD++;

							// Control for WD
							if (Control.Error_WD > 4) break;

						}

						// Set Function Variable
						if (!Control.Response) Control.Function = false;

					}				

				#endif

				#ifdef GSM_Debug
					Terminal_GSM.Text(23, 110, GREEN, String(_File_ID + ".hex"));
					Terminal_GSM.Text(23, 110, GREEN, String(Variables.File_Size));
				#endif

				// FTPRECV Command
				#ifdef _AT_FTPRECV_

					// Chain Control
					if (Control.Function) {

						// Reset Control Variables
						Control.Error_WD = 0;
						Control.Response = false;

						// Declare SD File Object
						File SD_File;

						// Activate Mux
						DDRC |= 0b00000001;
						PORTC |= 0b00000001;
						delay(10);

						// Start SD Card
						if (SD.begin(53)) {

							// Control for Existing File
 							if (SD.exists(FOTA_SD_FileName)) {

								// Remove Existing File
								SD.remove(FOTA_SD_FileName);

								// SD Message
								#ifdef GSM_Debug
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("                               "));
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("File Exist and Deleted."));
								#endif

							}

							// Open File for Write
							SD_File = SD.open(FOTA_SD_FileName, FILE_WRITE);

							// Control for File Open
							if (SD_File) {

								// SD Message
								#ifdef GSM_Debug
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("                               "));
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("SD Opened"));
								#endif

								// Get Time
								uint32_t _Download_Start_Time = millis();

								// Set Data Variable
								char _Data[250];

								// Define Variable
								uint32_t _Remain = Variables.File_Size;
								uint16_t _RequestSize = 200;
								uint16_t _RecieveSize = 0;
								uint32_t _WriteSize = 0;

								// Get Pack
								while (!Control.Response) {

									// Set Data Variable
									memset(_Data, '\0', 250);

									// Clear Variable
									_RecieveSize = 0;

									// Send Command
									if (AT_Command_Set::FTPRECV(_RequestSize, _RecieveSize, _Data)) {

										// Calculate Remaining
										_Remain -= _RecieveSize;

										// Calculate Write Size
										_WriteSize += _RecieveSize;

										// Calculate Pack Size
										if (_Remain <= _RequestSize) _RequestSize = _Remain;

										// Print File
										SD_File.flush();
										SD_File.write(_Data);
										delay(5);

									}

									// Calculate Download Time
									Variables.Download_Time = (millis() - _Download_Start_Time) / 1000;

									// SD Message
									#ifdef GSM_Debug
										Terminal_GSM.Text(25, 112, WHITE, String(map((Variables.File_Size - _Remain), 0, Variables.File_Size, 0, 100)));
										Terminal_GSM.Text(26, 109, WHITE, String(Variables.Download_Time));
										Terminal_GSM.Text(29, 100, CYAN, String("        "));
										Terminal_GSM.Text(29, 100, CYAN, String(Variables.File_Size - _Remain));
										Terminal_GSM.Text(29, 70, CYAN, String("        "));
										Terminal_GSM.Text(29, 70, CYAN, String(_WriteSize));
									#endif

									// Control for File End
									if (_Remain == 0) Control.Response = true;

									// End Function
									if (Variables.Download_Time > 1200) break;

								}

								delay(8);
								SD_File.close();

								// SD Message
								#ifdef GSM_Debug
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("                               "));
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("SD Closed"));
								#endif

							} else {

								// SD Message
								#ifdef GSM_Debug
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("                               "));
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, F("File Not Opened"));
								#endif

							}

						} else {

							// SD Message
							#ifdef GSM_Debug
								Terminal_GSM.Text(GSM_Console_Boot_X, 50, RED, F("SD Can Not Open"));
							#endif

						}

						// Turn SD MUX Enable LOW
						PORTC &= 0b11111110;

						// Set Function Variable
						if (!Control.Response) Control.Function = false;

					}

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

				// Activate Mux
				DDRC |= 0b00000001;
				PORTC |= 0b00000001;
				delay(10);

				// Declare SD File Object
				File Firmware_File;

				// Control for Existing File
				if (SD.exists(FOTA_SD_FileName)) {

					// Open File for Write
					Firmware_File = SD.open(FOTA_SD_FileName, FILE_READ);

					// Get File Size
					this->Variables.SD_File_Size = Firmware_File.size();

					// Control for File Size
					if (this->Variables.File_Size != this->Variables.SD_File_Size) Control.Function = false;

					// SD Message
					#ifdef GSM_Debug
						Terminal_GSM.OK_Decide(Control.Function, 22, 113);
						Terminal_GSM.Text(24, 110, GREEN, String(this->Variables.SD_File_Size));
					#endif

				}

				// Turn SD MUX Enable LOW
				PORTC &= 0b11111110;

				// Post Delay
				delay(500);

				// End Function
				return(Control.Function);

			}

	};

#endif /* defined(__PostOffice_FOTA__) */