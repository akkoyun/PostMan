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
	#ifndef __PostMan_GSM__
		#include "GSM.h"
	#endif

	// Define Library Includes
	#ifndef __PostMan_FOTA__
		#include "FOTA.h"
	#endif

	// Cloud Functions
	class PostMan : private GSM {

		// Private Functions
		private:

			// Define CallBack Functions
			void (*_Send_Data_CallBack)(uint8_t);
			void (*_Send_Response_CallBack)(uint16_t, uint8_t);
			void (*_Command_CallBack)(uint16_t, char*);

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
			void Get_Serial_ID(void) {
				
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
				String(uint64ToString(_Serial)).toCharArray(this->JSON_Data.Device_ID, 17);

				// Print Command State
				#ifdef GSM_Debug
					Terminal_GSM.Text(5, 63, GREEN, String(this->JSON_Data.Device_ID));
				#endif

			}

			// Environment Read Function
			void Get_Environment(void) {
				
				// Define Sensor Object
				HDC2010 _Sensor(true, 3, 10, true);

				// Set Device Environment Variable
				this->JSON_Data.JSON_Environment.Temperature = _Sensor.Temperature();
				this->JSON_Data.JSON_Environment.Humidity = _Sensor.Humidity();

				// Print Command State
				#ifdef GSM_Debug
					Terminal_GSM.Text(8, 72, CYAN, String(_Sensor.Temperature(), 2));
					Terminal_GSM.Text(9, 72, CYAN, String(_Sensor.Humidity(), 2));
				#endif

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
					_Response = AT_Command_Set::MONIZIP();

					// Set WD Variable
					_Error_WD++;

					// Control for WD
					if (_Error_WD > 5) break;

				}

				// Print Command State
				#ifdef GSM_Debug

					// Print Signal Level Value
					Terminal_GSM.Text(18, 65, WHITE, F("[-   ]"));
					Terminal_GSM.Text(18, 67, CYAN, String(GSM::GSM_Operator.dBm));

					// Print Signal Level Bar
					Terminal_GSM.Text(18, 74, GRAY, F("_____"));
					for (uint8_t i = 1; i <= GSM::GSM_Operator.Signal; i++) Terminal_GSM.Text(18, 73 + i, CYAN, F("X"));

					// Print Operator Value
					Terminal_GSM.Text(19, 74, CYAN, String(GSM::GSM_Operator.Operator));

					// Print Modem LAC Value
					Terminal_GSM.Text(21, 75, CYAN, String(GSM::GSM_Operator.LAC, HEX));

					// Print Modem Cell ID Value
					Terminal_GSM.Text(22, 75, CYAN, String(GSM::GSM_Operator.Cell_ID, HEX));

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
				if (_Pack_Type == Pack_Types::Online) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Online"));

					// Get Data
					_Send_Data_CallBack(Pack_Types::Online);

				} else if (_Pack_Type == Pack_Types::Update) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Update"));

					// Get Data
					_Send_Data_CallBack(Pack_Types::Update);


				} else if (_Pack_Type == Pack_Types::Timed) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Timed"));

					// Get Data
					_Send_Data_CallBack(Pack_Types::Timed);

				} else if (_Pack_Type == Pack_Types::Interrupt) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Interrupt"));

					// Get Data
					_Send_Data_CallBack(Pack_Types::Interrupt);

				} else if (_Pack_Type == Pack_Types::Alarm) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Alarm"));

					// Get Data
					_Send_Data_CallBack(Pack_Types::Alarm);

				} else if (_Pack_Type == Pack_Types::Offline) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Offline"));

					// Get Data
					_Send_Data_CallBack(Pack_Types::Offline);

				} else if (_Pack_Type == Pack_Types::FOTA_Info) {

					// Set Command
					JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("FOTA_Download"));

					// Get Data
					_Send_Data_CallBack(Pack_Types::FOTA_Info);

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
					#ifdef GSM_Debug
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

						// Declare Variable
						char _Firmware[10];

						// Handle TimeStamp
						sprintf(_Firmware, "%02d.%02d.%03d", GSM::GSM_Module.Firmware.Segment_1, GSM::GSM_Module.Firmware.Segment_2, GSM::GSM_Module.Firmware.Segment_3);

						// Define IoT Module
						JsonObject JSON_Module = JSON_GSM.createNestedObject(F("Module"));

						// Set IoT Parameters
						JSON_Module[F("Manufacturer")] = GSM::GSM_Module.Manufacturer;
						JSON_Module[F("Model")] = GSM::GSM_Module.Model;
						JSON_Module[F("Firmware")] = _Firmware;
						JSON_Module[F("Serial")] = GSM::GSM_Module.Serial_ID;
						JSON_Module[F("IMEI")] = GSM::GSM_Module.IMEI;

					}

					// Define GSM Operator Section
					JsonObject JSON_Operator = JSON_GSM.createNestedObject(F("Operator"));

					// Update Connection Variables
					this->Update_Connection_Variables();

					// Set Device GSM Connection Detail Section
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update) JSON_Operator[F("SIM_Type")] = 1;
					if (_Pack_Type == Pack_Types::Online or _Pack_Type == Pack_Types::Update)JSON_Operator[F("ICCID")] = GSM::GSM_Operator.ICCID;
					JSON_Operator[F("Code")] = GSM::GSM_Operator.Operator;
					JSON_Operator[F("dBm")] = GSM::GSM_Operator.dBm;
					JSON_Operator[F("LAC")] = GSM::GSM_Operator.LAC;
					JSON_Operator[F("Cell_ID")] = GSM::GSM_Operator.Cell_ID;

					// Clear IoT Variables
					GSM::GSM_Operator.Operator = 0;
					GSM::GSM_Operator.dBm = 0;
					GSM::GSM_Operator.LAC = 0;
					GSM::GSM_Operator.Cell_ID = 0;

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
						JSON_Payload[F("File_ID")] = this->JSON_Data.JSON_FOTA.File_ID;
						JSON_Payload[F("Status")] = this->JSON_Data.JSON_FOTA.DownloadStatus;
						JSON_Payload[F("SD_Size")] = this->JSON_Data.JSON_FOTA.SD_Size;
						JSON_Payload[F("Download_Time")] = this->JSON_Data.JSON_FOTA.Download_Time;

					} else {


					}

				#endif

				// Clear Unused Data
				JSON.garbageCollect();

				// Serialize JSON	
				uint16_t _JSON_Size = serializeJson(JSON, this->JSON_Data.JSON_Pack);

				// Print Command State
				#ifdef GSM_Debug
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

			// Send Response Function
			bool Send_Response(const uint16_t _Response) {

				// Declare Response JSON Variable
				String _Response_JSON;

				// Declare JSON Object
				StaticJsonDocument<32> Response_JSON;

				// Declare JSON Data
				Response_JSON[F("Response")] = _Response;

				// Clear Unused Data
				Response_JSON.garbageCollect();

				// Serialize JSON	
				uint8_t _JSON_Length = serializeJson(Response_JSON, _Response_JSON) + 1;

				// Declare Response Array
				char JSON[_JSON_Length];

				// Convert Response
				_Response_JSON.toCharArray(JSON, _JSON_Length);

				// Send Response
				bool _Res = this->Response(200, JSON);

				// End Function
				return(_Res);

			}

			// Reset Function
			void(* Reset) (void) = 0;

		// Public Functions
		public:

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

				// Define JSON FOTA Structure
				struct JSON_FOTA_Structure {
					uint16_t		File_ID				= 0;
					bool 			DownloadStatus		= false;
					uint32_t		Download_Time		= 0;
					uint32_t		FTP_Size			= 0;
					uint32_t		SD_Size				= 0;
				} JSON_FOTA;

				// Define JSON
				String JSON_Pack;

			} JSON_Data;

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

			// PostMan Constructor
			PostMan(Stream &_Serial) : GSM(_Serial) {

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

			// GSM Modem Power Sequence
			void Power(const bool _State) {

				Hardware::Power(_State);

			}

			// Initialize GSM Modem
			void Initialize(void) {

				// Get Serial ID
				this->Get_Serial_ID();

				// Get Environment
				this->Get_Environment();

				// Initialize Modem
				GSM::Initialize();

			}

			// Connect GSM Modem
			void Connect(void) {

				GSM::Connect();

				// Detect RTC
				I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);

				// RTC Object Definitions	
				RV3028 RTC(true, 1);

				// Update Time
				RTC.Set_Time(GSM::Time.Second, GSM::Time.Minute, GSM::Time.Hour, GSM::Time.Day, GSM::Time.Month, GSM::Time.Year);

				// Publish Interrupt Status
				this->Interrupt.Online = true;

			}

			// LOG JSON Pack Function
			void LOG(void) {

				// Declare LOG File Object
				File LOG_File;

				// Activate Mux
				DDRC |= 0b00000001; PORTC |= 0b00000001;
				delay(200);

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
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, RED, F("                               "));
						Terminal_GSM.Text(14, 44, RED, F("Error : Pack Writen to SD Card."));
					#endif

					// Clear Pack
					this->JSON_Data.JSON_Pack = "";

				}

				// Turn SD MUX Enable LOW
				PORTC &= 0b11111110;

			}

			// ************************************************************

			// Connect Cloud
			bool Listen(void) {

				// Control for Connection
				if (GSM::Status.Connection) {

					// Blink
					Hardware::MCU_LED(__WHITE__, 1, 200);

					// Listen Port
					bool _Response = GSM::Listen(true);

					// Print Command State
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						if (_Response) Terminal_GSM.Text(14, 44, GREEN, F("Cloud Online"));
						if (!_Response) Terminal_GSM.Text(14, 44, RED, F("Cloud Offline"));
					#endif

					// End Function
					return(true);

				} else {

					// Blink
					Hardware::MCU_LED(__RED__, 1, 200);

					// Print Command State
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						Terminal_GSM.Text(14, 44, RED, F("No GSM Connection"));
					#endif

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// Send Data Batch Function
			bool Publish(const uint8_t _Pack_Type) {

				// Parse JSON
				uint16_t _JSON_Size = this->Parse_JSON(_Pack_Type);

				// Control for Connection
				if (GSM::Status.Connection) {

					// Open Connection
					if (AT_Command_Set::ATSD(3, 0, 80, 255, 88, 1, _BackEnd_Server_)) {

						// Blink
						Hardware::MCU_LED(__GREEN__, 1, 200);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, YELLOW, F("JSON Pack"));
						#endif

						// Clear UART Buffer
						AT_Command_Set::Clear_UART_Buffer();

						// Declare Buffer Object
						Serial_Buffer Buffer_Set = {
							false, 	// Response State
							0, 		// Read Order
							0, 		// Data Order
							5000, 	// Time Out
							255		// Buffer Size
						};

						// Declare Buffer
						char Buffer_Variable[Buffer_Set.Size];
						memset(Buffer_Variable, '\0', Buffer_Set.Size);

						// Command Chain Delay (Advice by Telit)
						delay(20);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, YELLOW, F("Sending Pack"));
						#endif

						// Send UART Command
						GSM_Serial->print(F("AT#SSEND=3"));
						GSM_Serial->write(0x0D);
						GSM_Serial->write(0x0A);

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
						GSM_Serial->print(F("POST ")); GSM_Serial->print(_BackEnd_EndPoint_); GSM_Serial->print(F(" HTTP/1.1\r\n"));
						GSM_Serial->print(F("Host: ")); GSM_Serial->print(_BackEnd_Server_); GSM_Serial->print(F("\r\n"));
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
							Buffer_Get.Response = this->Find_OK(Buffer_Variable, Buffer_Get.Read_Order);

							// Increase Read Order
							if (isAscii(Buffer_Variable[Buffer_Get.Read_Order])) Buffer_Get.Read_Order++;

							// Handle for timeout
							if (millis() - Current_Time >= Buffer_Get.Time_Out) return(false);

						}

						// Response Wait Delay
						delay(10);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, GREEN, F("Pack Sended"));
						#endif

						// Declare Ring Status
						uint16_t _Length;

						// Get Ring Port
						if (AT_Command_Set::Send_SRING(_Length)) {

							// Blink
							Hardware::MCU_LED(__GREEN__, 1, 200);

							// Declare Response Status
							char _Response[32];
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
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
									Terminal_GSM.Text(14, 44, GREEN, F("Response --> [   ]"));
									Terminal_GSM.Text(14, 58, YELLOW, String(_Response_Command));
								#endif

								// Command Delay
								delay(50);

								// Close Socket
								AT_Command_Set::SH(3);

								// Port Control
								GSM::Listen(true);

								// Clear Interrupt
								this->Clear_Interrupt(_Pack_Type);

								// Send Data CallBack
								_Send_Response_CallBack(_Response_Command, 0);

								// Clear JSON Print
								#ifdef GSM_Debug
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
									#ifdef GSM_Debug
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
								#ifdef GSM_Debug
									Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
									Terminal_GSM.Text(14, 44, GREEN, F("Server Don't Response"));
								#endif

								// Port Control
								GSM::Listen(true);

								// End Function
								return(false);
					
							}
							
						} else {

							// Blink
							Hardware::MCU_LED(__RED__, 1, 200);

							// Send Data CallBack Error
							_Send_Response_CallBack(0, 2);

							// Port Control
							GSM::Listen(true);

							// End Function
							return(false);

						}

					} else {

						// Blink
						Hardware::MCU_LED(__RED__, 1, 200);

						// Send Data CallBack Error
						_Send_Response_CallBack(0, 3);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, RED, F("Socket Dial Error"));
						#endif

						// Port Control
						GSM::Listen(true);

						// Clear Interrupt
						this->Clear_Interrupt(_Pack_Type);

						// End Function
						return(false);

					}

					// End Function
					return(true);

				} else {

					// Print Command State
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						Terminal_GSM.Text(14, 44, RED, F("No Connection"));
					#endif

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
				if (GSM::Status.Connection) {

					// Declare Variable
					char _JSON_Data[50];

					// Declare Request Length
					uint16_t _Request_Length;

					// Handle Ring
					if (Receive_SRING()) {

						// Blink
						Hardware::MCU_LED(__BLUE__, 1, 200);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, GREEN, F("Ring.."));
						#endif

						// Answer Socket
						AT_Command_Set::SA(2, 1, _Request_Length);

						// Get Request Data
						AT_Command_Set::SRECV(2, _Request_Length, _JSON_Data);

						// Handle JSON Data
						uint16_t _Event = this->Handle_JSON_Request(_JSON_Data);

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
							Terminal_GSM.Text(14, 44, GREEN, String(_Event));
						#endif

						// Declare Response
						uint8_t _Response = Command_NOK;

						// Handle Command
						if (_Event == Command_Reset) {

							// Blink
							Hardware::MCU_LED(__RED__, 1, 200);

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
								Terminal_GSM.Text(14, 44, GREEN, F("Reset.."));
							#endif

							// Send Response
							this->Send_Response(Command_OK);

							// Reset
							Reset();

						} else if (_Event == Command_Update) {

							// Blink
							Hardware::MCU_LED(__RED__, 1, 200);

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
								Terminal_GSM.Text(14, 44, GREEN, F("Update Request"));
							#endif

							// Send Response
							this->Send_Response(Command_OK);

							// Set Command Interrupt
							this->Interrupt.Update = true;

						} else if (_Event == Command_Parameter) {

							// Blink
							Hardware::MCU_LED(__RED__, 1, 200);

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
								Terminal_GSM.Text(14, 44, GREEN, F("Parameter Update"));
							#endif

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
							this->Send_Response(_Response);

						} else if (_Event == Command_FOTA_Download) {

							// Blink
							Hardware::MCU_LED(__RED__, 1, 200);

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
								Terminal_GSM.Text(14, 44, GREEN, F("FOTA Download Request"));
							#endif

							// Declare JSON Object
							StaticJsonDocument<64> Incoming_JSON;

							// Deserialize the JSON document
							deserializeJson(Incoming_JSON, _JSON_Data);

							// Handle JSON
							this->JSON_Data.JSON_FOTA.File_ID = Incoming_JSON["Request"]["Firmware"];

							// Set Command Interrupt
							this->Interrupt.FOTA_Download = true;

							// Send Response
							this->Send_Response(Command_OK);

						} else if (_Event == Command_FOTA_Burn) {

							// Blink
							Hardware::MCU_LED(__RED__, 1, 200);

							// Print Command State
							#ifdef GSM_Debug
								Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
								Terminal_GSM.Text(14, 44, GREEN, F("FOTA Burn Request"));
							#endif

							// Set Command Interrupt
							this->Interrupt.FOTA_Burn = true;

							// Send Response
							this->Send_Response(Command_OK);

						} else {

							// Send Data CallBack
							_Command_CallBack(_Event, _JSON_Data);

						}

						// Print Command State
						#ifdef GSM_Debug
							Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						#endif

					}

					// Clear Interrupt
					this->Interrupt.Ring = false;

					// Port Control
					GSM::Listen(true);

				}

			}

			// Send Request Response Function
			bool Response(uint16_t _Response_Code, char * _Data) {

				// Send Socket Answer
				if (SSEND(2, 1, _Response_Code, "", "", _Data)) {

					// Blink
					Hardware::MCU_LED(__PURPLE__, 1, 200);

					// Print Command State
					#ifdef GSM_Debug
						Terminal_GSM.Text(14, 44, CYAN, F("                                    "));
						Terminal_GSM.Text(14, 44, GREEN, F("Response Sended"));
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

			// Set Status
			void SetStatus(uint16_t _Device, uint16_t _Fault) {

				// Set Device Status Variables

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

#endif /* defined(__PostMan__) */