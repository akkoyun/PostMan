#ifndef __PostOffice__
#define __PostOffice__

	// Define Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Define Library Includes
	#ifndef __PostOffice_Includes__
		#include "Config/Includes.h"
	#endif

	// Define Library Includes
	#ifndef __PostOffice_GSM__
		#include "GSM.h"
	#endif

	// Define Library Includes
	#ifndef __PostOffice_FOTA__
		#include "FOTA.h"
	#endif

	// Cloud Functions
	class PostOffice : public GSM {

		// Private Functions
		private:

			// Define JSON Status Structure
			struct JSON_Device_Structure {

				// Define JSON Status Structure
				struct JSON_Info_Structure {
					char * 		Device_ID;
					float 		Temperature;
					float 		Humidity;
				} JSON_Info;

				// Define JSON Battery Structure
				struct JSON_Battery_Structure {
					float IV;
					float AC;
					float SOC;
					uint8_t Charge;
					float T;
					uint16_t FB;
					uint16_t IB;
				} JSON_Battery;

				// Define JSON Status Structure
				struct JSON_Status_Structure {
					uint16_t Device;
					uint16_t Fault;
				} JSON_Status;

				// Define JSON FOTA Structure
				struct JSON_FOTA_Structure {
					uint16_t		File_ID				= 0;
					bool 			DownloadStatus		= false;
					uint32_t		Download_Time		= 0;
					uint32_t		FTP_Size			= 0;
					uint32_t		SD_Size				= 0;
				} JSON_FOTA;

				// Time Stamp
				char * Time_Stamp;

				// Define JSON
				String JSON_Pack;

			} JSON_Data;

			// Define CallBack Functions
			void (*_Send_Data_CallBack)(uint8_t);
			void (*_Send_Response_CallBack)(uint16_t, uint8_t);
			void (*_Command_CallBack)(uint16_t, char*);

			// Update GSM Parameters
			void Update_Connection_Variables(void) {

				// Declare Watchdog Variable
				uint8_t _Error_WD = 0;

				// Set Response Variable
				bool _Response = false;

				// Process Command
				while (!_Response) {

					// Process Command
					_Response = AT_Command_Set::MONIZIP(GSM::Modem.Operator, GSM::Modem.LAC, GSM::Modem.Cell_ID, GSM::Modem.dBm);

					// Set WD Variable
					_Error_WD++;

					// Control for WD
					if (_Error_WD > 5) break;

				}

				// Print Command State
				#ifdef GSM_Debug
					Terminal_GSM.Text(GSM_CellLAC_X, GSM_CellLAC_Y, CYAN, String(GSM::Modem.LAC));
					Terminal_GSM.Text(GSM_CellID_X, GSM_CellID_Y, CYAN, String(GSM::Modem.Cell_ID));
					Terminal_GSM.Text(GSM_Operator_X, GSM_Operator_Y, CYAN, String(GSM::Modem.Operator));
					Terminal_GSM.Text(GSM_RSSI_X, GSM_RSSI_Y, CYAN, String(GSM::Modem.dBm));
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
				this->JSON_Data.JSON_Pack = "";

				// Define JSON
				StaticJsonDocument<1024> JSON;

				// Set Device ID Variable
				if (_Pack_Type == Pack_Online) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Online"));

					// Get Data
					_Send_Data_CallBack(Pack_Online);

				} else if (_Pack_Type == Pack_Timed) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Timed"));

					// Get Data
					_Send_Data_CallBack(Pack_Timed);

				} else if (_Pack_Type == Pack_Interrupt) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Interrupt"));

					// Get Data
					_Send_Data_CallBack(Pack_Interrupt);

				} else if (_Pack_Type == Pack_Alarm) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Alarm"));

					// Get Data
					_Send_Data_CallBack(Pack_Alarm);

				} else if (_Pack_Type == Pack_Offline) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Offline"));

					// Get Data
					_Send_Data_CallBack(Pack_Offline);

				} else if (_Pack_Type == FOTA_Download) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("FOTA_Download"));

					// Get Data
					_Send_Data_CallBack(FOTA_Download);

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
					JSON_Info[F("ID")] = this->JSON_Data.JSON_Info.Device_ID;
					
					// Set Device Hardware Version Variable
					if (_Pack_Type == Pack_Online) JSON_Info[F("Hardware")] = F(__Hardware__);

					// Set Device Firmware Version Variable
					if (_Pack_Type == Pack_Online) JSON_Info[F("Firmware")] = F(__Firmware__);

					// Set Device Environment Variable
					JSON_Info[F("Temperature")] = this->JSON_Data.JSON_Info.Temperature;
					JSON_Info[F("Humidity")] = this->JSON_Data.JSON_Info.Humidity;

				#endif

				// Parse Battery Segment
				#ifdef JSON_Segment_Battery

					// Define Power Section
					JsonObject JSON_Battery = JSON_Device["Power"].createNestedObject("Battery");

					// Set Battery Variables
					JSON_Battery[F("IV")] = this->JSON_Data.JSON_Battery.IV;
					JSON_Battery[F("AC")] = this->JSON_Data.JSON_Battery.AC;
					JSON_Battery[F("SOC")] = this->JSON_Data.JSON_Battery.SOC;
					JSON_Battery[F("Charge")] = this->JSON_Data.JSON_Battery.Charge;
					if (_Pack_Type == Pack_Online) JSON_Battery[F("T")] = this->JSON_Data.JSON_Battery.T;
					if (_Pack_Type == Pack_Online) JSON_Battery[F("FB")] = this->JSON_Data.JSON_Battery.FB;
					if (_Pack_Type == Pack_Online) JSON_Battery[F("IB")] = this->JSON_Data.JSON_Battery.IB;

				#endif

				// Parse Battery Segment
				#ifdef JSON_Segment_GSM

					// Define GSM Section
					JsonObject JSON_GSM = JSON_Device["IoT"].createNestedObject(F("GSM"));

					// Get GSM Parameters
					if (_Pack_Type == Pack_Online) {

						// Define IoT Module
						JsonObject JSON_Module = JSON_GSM.createNestedObject(F("Module"));

						// Set IoT Parameters
						JSON_Module[F("Manufacturer")] = GSM::Modem.Manufacturer;
						JSON_Module[F("Model")] = GSM::Modem.Model;
						JSON_Module[F("Firmware")] = GSM::Modem.Firmware;
						JSON_Module[F("Serial")] = GSM::Modem.Serial_ID;
						JSON_Module[F("IMEI")] = GSM::Modem.IMEI;

						// Define SIM
						JsonObject JSON_SIM = JSON_GSM.createNestedObject(F("SIM"));

						// Set SIM Parameters
						JSON_SIM[F("SIM_Type")] = 1;
						JSON_SIM[F("ICCID")] = GSM::Modem.ICCID;

					}

					// Define GSM Operator Section
					JsonObject JSON_Operator = JSON_GSM.createNestedObject(F("Operator"));

					// Update Connection Variables
					this->Update_Connection_Variables();

					// Set Device GSM Connection Detail Section
					JSON_Operator[F("Code")] = GSM::Modem.Operator;
					JSON_Operator[F("dBm")] = GSM::Modem.dBm;
					JSON_Operator[F("LAC")] = GSM::Modem.LAC;
					JSON_Operator[F("Cell_ID")] = GSM::Modem.Cell_ID;

				#endif

				// Parse Payload Segment
				#ifdef JSON_Segment_Payload

					// Define Data Section
					JsonObject JSON_Payload = JSON.createNestedObject(F("Payload"));

					// Set Device Time Variable
					JSON_Payload[F("TimeStamp")] = this->JSON_Data.Time_Stamp;

					// Online Payload
					if (_Pack_Type == Pack_Online) {

						// Set Device Status Variable
						JSON_Payload[F("Device")] = this->JSON_Data.JSON_Status.Device;

						// Set Device Fault Variable
						JSON_Payload[F("Fault")] = this->JSON_Data.JSON_Status.Fault;

					}

					// Timed Payload
					if (_Pack_Type == Pack_Timed) {

						// Set Device Status Variable
						JSON_Payload[F("Device")] = this->JSON_Data.JSON_Status.Device;

						// Set Device Fault Variable
						JSON_Payload[F("Fault")] = this->JSON_Data.JSON_Status.Fault;

					}

					// Interrupt Payload
					if (_Pack_Type == Pack_Interrupt) {

						// Set Device Status Variable
						JSON_Payload[F("Device")] = this->JSON_Data.JSON_Status.Device;

						// Set Device Fault Variable
						JSON_Payload[F("Fault")] = this->JSON_Data.JSON_Status.Fault;

					}

					// Alarm Payload
					if (_Pack_Type == Pack_Alarm) {

						// Set Device Status Variable
						JSON_Payload[F("Device")] = this->JSON_Data.JSON_Status.Device;

						// Set Device Fault Variable
						JSON_Payload[F("Fault")] = this->JSON_Data.JSON_Status.Fault;

					}

					// Offline Payload
					if (_Pack_Type == Pack_Offline) {

						// Set Device Status Variable
						JSON_Payload[F("Device")] = this->JSON_Data.JSON_Status.Device;

						// Set Device Fault Variable
						JSON_Payload[F("Fault")] = this->JSON_Data.JSON_Status.Fault;

					}

					// FOTA Download Payload
					if (_Pack_Type == FOTA_Download) {

						// Set FOTA Status Variable
						JSON_Payload[F("File_ID")] = this->JSON_Data.JSON_FOTA.File_ID;

						// Set FOTA Status Variable
						JSON_Payload[F("Status")] = this->JSON_Data.JSON_FOTA.DownloadStatus;

						// Set FOTA File Size Variable
						JSON_Payload[F("FTP_Size")] = this->JSON_Data.JSON_FOTA.FTP_Size;

						// Set FOTA SD File Size Variable
						JSON_Payload[F("SD_Size")] = this->JSON_Data.JSON_FOTA.SD_Size;

						// Set FOTA Download Time
						JSON_Payload[F("DownloadTime")] = this->JSON_Data.JSON_FOTA.Download_Time;

						// Clear Variables
						this->JSON_Data.JSON_FOTA.DownloadStatus = false;
						this->JSON_Data.JSON_FOTA.FTP_Size = 0;
						this->JSON_Data.JSON_FOTA.SD_Size = 0;
						this->JSON_Data.JSON_FOTA.Download_Time = 0;

					}

				#endif

				// Clear Unused Data
				JSON.garbageCollect();

				// Serialize JSON	
				uint16_t _JSON_Size = serializeJson(JSON, this->JSON_Data.JSON_Pack);

				// End Function
				return(_JSON_Size);

			}

			// Handle Send Response
			uint16_t Handle_JSON_Request(const char *_Data) {

				// Declare Variable
				uint16_t Event = 0;

				// Declare JSON Object
				StaticJsonDocument<512> Incoming_JSON;

				// Deserialize the JSON document
				DeserializationError Error = deserializeJson(Incoming_JSON, _Data);

				// Handle JSON
				if (!Error) Event = Incoming_JSON["Request"]["Event"];

				// End Function
				return(Event);

			}

		// Public Functions
		public:

			// Define Modem Interrupt Structure
			struct Interrupt_Status {
				bool 		Send				= false;
				bool 		Ring				= false;
			} Interrupt;

			// PostOffice Constructor
			PostOffice(Stream &_Serial) : GSM(_Serial) {

				// Clear Interrupt
				this->Interrupt.Send = false;
				this->Interrupt.Ring = false;

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

			// GSM Modem Power Sequence
			void Power(const bool _State) {

				Hardware::Power(_State);

			}

			// Initialize GSM Modem
			void Initialize(void) {

				GSM::Initialize();

			}

			// Connect GSM Modem
			void Connect(void) {

				GSM::Connect();

			}

			// Get Time
			uint8_t Get_Time(uint8_t _Detail) {

				// Return Time Detail
				switch (_Detail) {

					case 1:
						return(GSM::Time.Year);
						break;
					case 2:
						return(GSM::Time.Month);
						break;
					case 3:
						return(GSM::Time.Day);
						break;
					case 4:
						return(GSM::Time.Hour);
						break;
					case 5:
						return(GSM::Time.Minute);
						break;
					case 6:
						return(GSM::Time.Second);
						break;
					default:
						return(0);
						break;

				}

				// End Function
				return(0);

			}

			// ************************************************************

			// Connect Cloud
			bool Subscribe(char * _Device_ID) {

				// Set Device ID
				this->JSON_Data.JSON_Info.Device_ID = _Device_ID;

				// Control for Connection
				if (GSM::Status.Connection) {

					// Listen Port
					bool _Response = GSM::Listen(true);

					// Print Command State
					#ifdef GSM_Debug
						if (_Response) Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("Cloud Online"));
					#endif

					// End Function
					return(true);

				} else {

					// Print Command State
					#ifdef GSM_Debug
						Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, F("No GSM Connection"));
					#endif

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// Send Data Batch Function
			bool Publish(const uint8_t _Pack_Type) {

				// Control for Connection
				if (GSM::Status.Connection) {

					// Open Connection
					if (AT_Command_Set::SocketDial(3, 0, 80, 255, 88, 1, PostOffice_Server)) {

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, YELLOW, F("                    "));
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, YELLOW, F("JSON Pack           "));
						#endif

						// Parse JSON
						uint16_t _JSON_Size = this->Parse_JSON(_Pack_Type);

						// Clear UART Buffer
						AT_Command_Set::Clear_UART_Buffer();

						// Declare Buffer Object
						Serial_Buffer Buffer_Set = {false, 0, 0, 2000};

						// Declare Buffer
						char Buffer_Variable[255];
						memset(Buffer_Variable, '\0', 255);

						// Command Chain Delay (Advice by Telit)
						delay(20);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("                    "));
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("Sending Pack...     "));
						#endif

						// Send UART Command
						GSM_Serial->print(F("AT#SSEND=3"));
						GSM_Serial->print(F("\r\n"));

						// Read Current Time
						uint32_t Current_Time = millis();

						// Response Wait Delay
						delay(10);

						// Read UART Response
						while (!Buffer_Set.Response) {

							// Read Serial Char
							Buffer_Variable[Buffer_Set.Read_Order] = GSM_Serial->read();

							// Control for <OK> Response
							if (Buffer_Variable[Buffer_Set.Read_Order - 1] == '>' and Buffer_Variable[Buffer_Set.Read_Order] == ' ') Buffer_Set.Response = true;

							// Increase Read Order
							if (Buffer_Variable[Buffer_Set.Read_Order] > 31 and Buffer_Variable[Buffer_Set.Read_Order] < 127) Buffer_Set.Read_Order += 1;

							// Handle for timeout
							if (millis() - Current_Time >= Buffer_Set.Time_Out) return(false);

						}

						// Send Delay
						delay(10);

						// Print HTTP Header
						GSM_Serial->print(F("POST ")); GSM_Serial->print(PostOffice_EndPoint); GSM_Serial->print(F(" HTTP/1.1\r\n"));
						GSM_Serial->print(F("Host: ")); GSM_Serial->print(PostOffice_Server); GSM_Serial->print(F("\r\n"));
						GSM_Serial->print(F("Content-Length: ")); GSM_Serial->print(_JSON_Size); GSM_Serial->print(F("\r\n"));
						GSM_Serial->print(F("Content-Type: application/json\r\n"));
						GSM_Serial->print(F("User-Agent: ")); GSM_Serial->print(__Device__); GSM_Serial->print(F("\r\n"));
						GSM_Serial->print(F("\r\n"));

						// Send Data Pack
						GSM_Serial->print(this->JSON_Data.JSON_Pack);

						// Print End Char
						GSM_Serial->print((char)26);

						// Declare Buffer Object
						Serial_Buffer Buffer_Get = {false, 0, 0, 2000};

						// Declare Buffer
						memset(Buffer_Variable, '\0', 255);

						// Read Current Time
						Current_Time = millis();

						// Read UART Response
						while (!Buffer_Get.Response) {

							// Read Serial Char
							Buffer_Variable[Buffer_Get.Read_Order] = GSM_Serial->read();

							// Control for <OK> Response
							if (Buffer_Variable[Buffer_Get.Read_Order - 1] == 'O' and Buffer_Variable[Buffer_Get.Read_Order] == 'K') Buffer_Get.Response = true;

							// Increase Read Order
							if (Buffer_Variable[Buffer_Get.Read_Order] > 31 and Buffer_Variable[Buffer_Get.Read_Order] < 127) Buffer_Get.Read_Order += 1;

							// Handle for timeout
							if (millis() - Current_Time >= Buffer_Get.Time_Out) return(false);

						}

						// Response Wait Delay
						delay(10);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("                    "));
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("Pack Sended...      "));
						#endif

						// Declare Ring Status
						uint16_t _Length;

						// Get Ring Port
						if (AT_Command_Set::Send_SRING(_Length)) {

							// Declare Response Status
							char _Response[32];
							uint16_t _Response_Command;

							// Command Delay
							delay(200);

							// Get Request Data
							if (AT_Command_Set::SRECV(3, _Length, _Response)) {

								// Deserialize the JSON document
								StaticJsonDocument<32> Incoming_JSON;
								deserializeJson(Incoming_JSON, _Response);
								_Response_Command = Incoming_JSON["Event"];

								// Print Command State
								#ifdef GSM_Debug
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("Response --> [   ]"));
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y + 14, YELLOW, String(_Response_Command));
									Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("                    "));
								#endif

								// Command Delay
								delay(200);

								// Close Socket
								AT_Command_Set::SH(3);

								// Port Control
								GSM::Listen(true);

								// Clear Interrupt
								this->Interrupt.Send = false;

								// Send Data CallBack
								_Send_Response_CallBack(_Response_Command, 0);

								// End Function
								if (_Response_Command == 200) return(true);

							} else {

								// Send Data CallBack Error
								_Send_Response_CallBack(0, 1);

							}
							
						} else {

							// Send Data CallBack Error
							_Send_Response_CallBack(0, 2);

						}

					} else {

						// Send Data CallBack Error
						_Send_Response_CallBack(0, 3);

					}

					// Port Control
					GSM::Listen(true);

					// Clear Interrupt
					this->Interrupt.Send = false;

					// End Function
					return(true);

				} else {

					// Print Command State
					#ifdef GSM_Debug
						Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, YELLOW, F("                    "));
						Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, F("No Connection       "));
					#endif

					// Clear Interrupt
					this->Interrupt.Send = false;

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// Get Server Command Function
			void Get(void) {

				// Control for Connection
				if (GSM::Status.Connection) {

					// Declare Variable
					char _JSON_Data[50];

					// Declare Request Length
					uint16_t _Request_Length;

					// Handle Ring
					if (Receive_SRING()) {

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("Ring             "));
						#endif

						// Answer Socket
						SA(2, 1, _Request_Length);

						// Get Request Data
						SRECV(2, _Request_Length, _JSON_Data);

						// Handle JSON Data
						uint16_t _Event = this->Handle_JSON_Request(_JSON_Data);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("                 "));
							Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, String(_Event));
						#endif

						// Clear Interrupt
						this->Interrupt.Ring = false;

						// Send Data CallBack
						_Command_CallBack(_Event, _JSON_Data);

					}

				}

			}

			// Send Request Response Function
			bool Response(uint16_t _Response_Code, char * _Data) {

				// Send Socket Answer
				if (SSEND(2, 1, _Response_Code, "", "", _Data)) {

					#ifdef GSM_Debug
						Terminal_GSM.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, F("Response Sended  "));
					#endif

					// Command Delay
					delay(20);

					// Close Socket
					if (SH(2)) {

						// Command Delay
						delay(20);

						// ReOpen Socket
						bool Socket_Open = GSM::Listen(true);

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

			// ************************************************************

			// Set TimeStamp
			void TimeStamp(char * _TimeStamp) {

				// Set Time Stamp
				this->JSON_Data.Time_Stamp = _TimeStamp;

			}

			// Set Environment Variables
			void Environment(float _Temperature, float _Humidity) {

				// Set Environment
				this->JSON_Data.JSON_Info.Temperature = _Temperature;
				this->JSON_Data.JSON_Info.Humidity = _Humidity;

			}

			// Set Battery Variables
			void Battery(float _IV, float _AC, float _SOC, uint8_t _Charge, float _T = 0, uint16_t _FB = 0, uint16_t _IB = 0) {

				// Set Battery Parameters
				this->JSON_Data.JSON_Battery.IV = _IV;
				this->JSON_Data.JSON_Battery.AC = _AC;
				this->JSON_Data.JSON_Battery.SOC = _SOC;
				this->JSON_Data.JSON_Battery.Charge = _Charge;
				this->JSON_Data.JSON_Battery.T = _T;			// Optional
				this->JSON_Data.JSON_Battery.FB = _FB;			// Optional
				this->JSON_Data.JSON_Battery.IB = _IB;			// Optional

			}

			// Set Status
			void SetStatus(uint16_t _Device, uint16_t _Fault) {

				// Set Device Status Variables
				this->JSON_Data.JSON_Status.Device = _Device;
				this->JSON_Data.JSON_Status.Fault = _Fault;

			}

			// Set FOTA Status
			void FOTA(const uint16_t _ID, const bool _Status, const uint32_t _FTP_Size, const uint32_t _SD_Size, const uint32_t _Time) {

				// Set Device Status Variables
				this->JSON_Data.JSON_FOTA.File_ID = _ID;
				this->JSON_Data.JSON_FOTA.DownloadStatus = _Status;
				this->JSON_Data.JSON_FOTA.FTP_Size = _FTP_Size;
				this->JSON_Data.JSON_FOTA.SD_Size = _SD_Size;
				this->JSON_Data.JSON_FOTA.Download_Time = _Time;

			}

	};


#endif /* defined(__PostOffice__) */