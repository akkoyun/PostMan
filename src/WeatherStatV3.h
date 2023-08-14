// Library include guard
#pragma once

// Include WeatherStat V3 Hardware Functions
#if defined(B108AA)

	// Include B108AA Hardware Functions
	#include "Hardware/B108AA.h"

#endif

// Include LE910S1-EAG AT Command Library
#if defined(LE910S1_EAG)
	
	// Include AT Command Set
	#include "AT_Command/LE910S1_EAG.h"

	// Define AT Command Set Name
	typedef AT_Command_Set_LE910S1_EAG AT_Command_Set;

#endif

// Cloud Functions
class Postman_WeatherStatV3 : private AT_Command_Set, private Hardware {

	// Private Functions
	private:

		// Define IoT Status Structure
		struct Struct_Status {
			bool		SIM_Inserted		= _SIM_NOT_INSERTED_;
			uint8_t		SIM_PIN				= _SIM_UNKNOWN_;
			bool 		Initialize		 	= _MODEM_NOT_INITIALIZED_;
			bool		Connection			= _MODEM_NOT_CONNECTED_;
			uint8_t		Socket_State		= 0;
		} IoT_Status;

		// Define IoT Module Structure
		struct Struct_Module {

			// Modem IMEI
			char 		IMEI[17];

			// GSM Serial ID
			uint32_t	Serial_ID 			= 0;

			// Manufacturer and Model
			uint8_t 	Manufacturer 		= _UNKNOWN_MANUFACTURER_;
			uint8_t 	Model 				= _UNKNOWN_MODEL_;

			// Modem Firmware Version
			char 		Firmware[15];

		} IoT_Module;

		// Define IoT Operator Structure
		struct Struct_Network {

			// SIM ICCID
			char 		ICCID[21];

			// Operator
			uint16_t 	MCC 				= 0;
			uint16_t	MNC					= 0;
			uint16_t	WDS					= 0;

			// Signal Level
			uint16_t	RSSI				= 0;
			uint8_t		Signal				= 0;

			// Location
			uint16_t	TAC					= 0;
			uint32_t	Cell_ID				= 0;

			// IP Address
			char 		IP_Address[16];

			// Connection Time
			uint8_t 	Connection_Time;

		} IoT_Operator;

		// Define Time Structure
		struct Struct_Time {
			uint16_t 	Year				= 0;
			uint16_t 	Month				= 0;
			uint16_t 	Day					= 0;
			uint16_t 	Hour				= 0;
			uint16_t 	Minute				= 0;
			uint16_t 	Second				= 0;
			uint16_t	Time_Zone			= 0;
		} Time;

		// Define Runtime Buffer Structure
		struct Struct_Buffer {

			// Connection Time
			uint32_t 	Connection_Time_Buffer	= 0;

			// Define JSON
			char 		JSON_Pack[Send_JSON_Size];

		} Buffer;

		// Define CallBack Functions
		void (*_Environment_CallBack)(float&, float&);
		void (*_Battery_CallBack)(float&, float&, float&, float&, uint16_t&, uint16_t&, uint8_t&);
		void (*_Send_Data_CallBack)(uint8_t);

		// Initialize GSM Modem
		bool Initialize(void) {

			// Declare WatchDog Variable
			uint8_t _WD = 0;

			// Initialize Modem Parameters
			while (!this->IoT_Status.Initialize) {

				// Control for Power Monitor
				if (Hardware::PowerMonitor()) {

					// AT Command
					bool _Response_AT = AT_Command_Set::AT();

					// ATE Command (Echo Off)
					bool _Response_ATE = AT_Command_Set::ATE(false);

					// SIMDET Command (SIM Card Detect)
					bool _Response_SIMDET = AT_Command_Set::SIMDET(GET, 0, this->IoT_Status.SIM_Inserted);

					// CFUN Command (Full Functionality)
					// bool _Response_CFUN = AT_Command_Set::CFUN(1);

					// CMEE Command (Error Messages)
					bool _Response_CMEE = AT_Command_Set::CMEE(1);

					// FCLASS Command (Connection Mode)
					bool _Response_FCLASS = AT_Command_Set::FCLASS(0);

					// ATK Command (No Flow Control)
					bool _Response_K = AT_Command_Set::K(0);

					// CPIN Command (SIM PIN Control)
					bool _Response_CPIN = AT_Command_Set::CPIN(this->IoT_Status.SIM_PIN);

					// CGSN Command (Get IMEI)
					bool _Response_CGSN = AT_Command_Set::CGSN(this->IoT_Module.IMEI);

					// ICCID Command (Get SIM Card ID)
					bool _Response_CCID = AT_Command_Set::CCID(this->IoT_Operator.ICCID);

					// CGMI Command (Get Manufacturer)
					bool _Response_CGMI = AT_Command_Set::CGMI(this->IoT_Module.Manufacturer);

					// CGMM Command (Get Model)
					bool _Response_CGMM = AT_Command_Set::CGMM(this->IoT_Module.Model);

					// CGMR Command (Get Firmware Version)
					bool _Response_SWPKGV = AT_Command_Set::SWPKGV(this->IoT_Module.Firmware);

					// GPIO Command (Set Status LED)
					AT_Command_Set::GPIO(SET, 1, 0, 2);

					// SLED Command (Set Status LED)
					AT_Command_Set::SLED(2);

					// SLEDSAV Command (Save Status LED)
					AT_Command_Set::SLEDSAV();

					// Control for Modem Initialization
					if (_Response_AT && _Response_ATE && _Response_SIMDET && _Response_CMEE && _Response_FCLASS && _Response_K && _Response_CPIN && _Response_CGSN && _Response_CCID && _Response_CGMI && _Response_CGMM && _Response_SWPKGV && (this->IoT_Status.SIM_Inserted == _SIM_INSERTED_) && (this->IoT_Status.SIM_PIN == _SIM_READY_)) {

						// Set IoT Status
						this->IoT_Status.Initialize = _MODEM_INITIALIZED_;

						// End Function
						return(true);

					} else {

						// Clear States
						this->IoT_Status.Initialize = _MODEM_NOT_INITIALIZED_;
						this->IoT_Status.SIM_Inserted = _SIM_NOT_INSERTED_;
						this->IoT_Status.SIM_PIN = _SIM_UNKNOWN_;

					}

				} else {

					// Power ON Modem
					Hardware::ON();

				}

				// Handle WatchDog
				if (_WD > 2) return(false);

				// Set WatchDog Variable
				_WD++;

			}

			// Not Initialized
			return(false);
			
		}

		// Connect GSM Modem
		bool Connect(void) {

			// Declare WatchDog Variable
			uint8_t _WD = 0;

			// Define Commands
			#define _CREG_
			#define _CGDCONT_
			#define _SGACT_
			#define _WS46_
			//#define _MONI_
			#define _SCFG_

			// Connect to Internet
			while (!this->IoT_Status.Connection) {

				// Control for Initialize
				if (this->IoT_Status.Initialize) {

					// Get CREG Command (Get Network Registration Mode)
					#ifdef _CREG_

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

							// Control for Connection
							if (_CREG_Connection_Stat == 0 or _CREG_Connection_Stat == 2 or _CREG_Connection_Stat == 3 or _CREG_Connection_Stat == 4) {

								// Set Variable
								this->IoT_Status.Connection = false;

								// Declare Response Status
								_Conn_WD = false;

								// Connection Wait Delay
								delay(1000);

							} else if (_CREG_Connection_Stat == 1 or _CREG_Connection_Stat == 5) {

								// Set Variable
								this->IoT_Status.Connection = true;

								// Declare Response Status
								_Conn_WD = true;

							}

							// Set WD Variable
							_Conn_WD_Counter++;

							// Control for WD
							if (_Conn_WD_Counter > 150) _Conn_WD = true;

						}

					#endif

					// CGDCONT Command (Set PDP Context)
					#ifdef _CGDCONT_
						AT_Command_Set::CGDCONT(1, "IP", _PDP_APN_);
					#endif

					// SGACT Command (Activate PDP Context)
					#ifdef _SGACT_
						AT_Command_Set::SGACT(1, 1, this->IoT_Operator.IP_Address);
					#endif

					// WS46 Command (Network Type)
					#ifdef _WS46_
						AT_Command_Set::WS46(SET, this->IoT_Operator.WDS);
					#endif

					// MONI Command (Network Status)
					#ifdef _MONI_
						AT_Command_Set::MONI(this->IoT_Operator.MCC, this->IoT_Operator.MNC, this->IoT_Operator.RSSI, this->IoT_Operator.Signal);
					#endif

					// Socket Configuration
					#ifdef _SCFG_

						// SCFG (Send Port) Command (Send Data Port Configuration)
						AT_Command_Set::SCFG(Socket_Outgoing, 1, 1500, 90, 1200, 0);

						// SCFGEXT (Send Port) Command (Send Data Port Extended Configuration)
						AT_Command_Set::SCFGEXT(Socket_Outgoing, 1, 0, 0, 0, 0);

						// SCFGEXT2 (Send Port) Command (Send Data Port Extended 2 Configuration)
						AT_Command_Set::SCFGEXT2(Socket_Outgoing, 1, 0, 0, 0, 0);

					#endif

					// Connection Success
					return(true);

					// Clear States
//					this->IoT_Status.SIM_Inserted = _SIM_NOT_INSERTED_;
//					this->IoT_Status.SIM_PIN = _SIM_UNKNOWN_;
//					this->IoT_Status.Initialize = _MODEM_NOT_INITIALIZED_;
//					this->IoT_Status.Connection = _MODEM_NOT_CONNECTED_;

				} else {

					// Initialize Modem
					this->Initialize();

				}

				// Handle WatchDog
				if (_WD > 2) return(false);

				// Set WatchDog Variable
				_WD++;

			}
							
			// Connection Failed
			return(false);

		}

		// Set Clock Configuration
		bool Clock(void) {

			// Control for Connection
			if (this->IoT_Status.Connection) {

				// CCLK Command (Real Time Clock Configuration)
				AT_Command_Set::CCLK(this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second, this->Time.Time_Zone);

				// Detect RTC
				I2C_Functions I2C_RTC(__I2C_Addr_RV3028C7__, true, 1);

				// RTC Object Definitions	
				RV3028 RTC(true, 1);

				// Update Time
				RTC.Set_Time(this->Time.Second, this->Time.Minute, this->Time.Hour, this->Time.Day, this->Time.Month, this->Time.Year);

				// Connection Success
				return(true);

			}

			// Time Configuration Failed
			return(false);

		}

		// Set GNSS Configuration
		bool GNSS(void) {

			// Turn On GNSS
			Hardware::GNSS(true);

			// Power On GNSS
			AT_Command_Set::GPSP(true);

			// Set GNSS Refresh Rate (1Hz)
			AT_Command_Set::GPSNHZ(0);

			// GPS Configuration
			AT_Command_Set::GPSCFG(5);

			// Time Configuration Failed
			return(true);

		}

		// Parse JSON Pack
		uint16_t Parse_JSON(uint8_t _Pack_Type) {

			// JSON Document Segments
			#define JSON_Segment_Info
			#define JSON_Segment_Battery
			#define JSON_Segment_GSM
			#define JSON_Segment_Payload

			// Clear Pack
			memset(this->Buffer.JSON_Pack, '\0', Send_JSON_Size);

			// Define JSON
			StaticJsonDocument<Send_JSON_Size> JSON;

			// Set Command
			JSON[F("Command")] = (String(__Company__) + F(":") + String(__Device__) + F(".") + F("Timed"));

			// Define Device Section
			JsonObject JSON_Device = JSON.createNestedObject(F("Device"));

			// Parse Info Segment
			#ifdef JSON_Segment_Info

				// Define Device Section
				JsonObject JSON_Info = JSON_Device.createNestedObject(F("Info"));

				// Set Device ID Variable
				JSON_Info[F("ID")] = "1111";
				
				// Set Device Hardware Version Variable
				JSON_Info[F("Hardware")] = F(__Hardware__);

				// Set Device Firmware Version Variable
				JSON_Info[F("Firmware")] = F(__Firmware__);

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
				JSON_Battery[F("T")] = _Temp;
				JSON_Battery[F("FB")] = _DCAP;
				JSON_Battery[F("IB")] = _ICAP;
				JSON_Battery[F("Charge")] = _State;

			#endif

			// Parse Battery Segment
			#ifdef JSON_Segment_GSM

				// WS46 Command (Network Type)
				AT_Command_Set::WS46(GET, this->IoT_Operator.WDS);

				// RFSTS Command (Network Status)
				AT_Command_Set::MONI(this->IoT_Operator.MCC, this->IoT_Operator.MNC, this->IoT_Operator.RSSI, this->IoT_Operator.Signal);

				// Define GSM Section
				JsonObject JSON_GSM = JSON_Device["IoT"].createNestedObject(F("GSM"));

				// Define IoT Module
				JsonObject JSON_Module = JSON_GSM.createNestedObject(F("Module"));

				// Set IoT Parameters
				JSON_Module[F("Manufacturer")] = this->IoT_Module.Manufacturer;
				JSON_Module[F("Model")] = this->IoT_Module.Model;
				JSON_Module[F("Firmware")] = this->IoT_Module.Firmware;
				JSON_Module[F("Serial")] = this->IoT_Module.Serial_ID;
				JSON_Module[F("IMEI")] = this->IoT_Module.IMEI;

				// Define GSM Operator Section
				JsonObject JSON_Operator = JSON_GSM.createNestedObject(F("Operator"));

				// Set Device GSM Connection Detail Section
				JSON_Operator[F("SIM_Type")] = 1;
				JSON_Operator[F("ICCID")] = this->IoT_Operator.ICCID;
				JSON_Operator[F("Country")] = this->IoT_Operator.MCC;
				JSON_Operator[F("Operator")] = this->IoT_Operator.MNC;
				JSON_Operator[F("RSSI")] = this->IoT_Operator.RSSI;
				JSON_Operator[F("TAC")] = uint64ToString(this->IoT_Operator.TAC);
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

				// Get Data
				_Send_Data_CallBack(_Pack_Type);

				// Set Device Environment Variable
				JSON_Payload[F("PCBT")] = 11;
				JSON_Payload[F("PCBH")] = 11;

			#endif

			// Clear Unused Data
			JSON.garbageCollect();

			// Serialize JSON	
			uint16_t _JSON_Size = serializeJson(JSON, this->Buffer.JSON_Pack);

			// End Function
			return(_JSON_Size);

		}

	// Public Functions
	public:

		// PostMan Constructor
		Postman_WeatherStatV3(Stream &_Serial) : AT_Command_Set(_Serial), Hardware() {

		}

		// CallBack Definitions
		void Environment_CallBack(void (*_Environment_CallBack)(float&, float&)) {

			// Set CallBack Functions
			this->_Environment_CallBack = _Environment_CallBack;

		}
		void Battery_CallBack(void (*_Battery_CallBack)(float&, float&, float&, float&, uint16_t&, uint16_t&, uint8_t&)) {

			// Set CallBack Functions
			this->_Battery_CallBack = _Battery_CallBack;

		}
		void Event_PackData(void (*_Send_Data_CallBack)(uint8_t)) {

			// Set CallBack Functions
			this->_Send_Data_CallBack = _Send_Data_CallBack;

		}

		// Connect GSM Modem
		void Online(void) {

			// Set Connection Start Time
			this->Buffer.Connection_Time_Buffer = millis();

			// GSM Initialize Sequence
			if (this->Initialize()) {

				// GSM Connect Sequence
				if (this->Connect()) {

					// Power Up GNSS
					this->GNSS();

					// Set Clock
					this->Clock();

					// Turn Off GNSS
					Hardware::GNSS(false);

					// Command Delay
					delay(50);

				} else {
					
					// Power Down GSM
					Hardware::OFF();

				}

			} else {

				// Power Down GSM
				Hardware::OFF();

			}

		}

		// Disconnect GSM Modem
		void Offline(void) {

			// Power Down GSM
			Hardware::OFF();

		}

		// Send Data Batch Function
		bool Publish(const uint8_t _Pack_Type) {

			// Control for Connection
			if (this->IoT_Status.Connection) {

				// Open Connection
				if (AT_Command_Set::ATSD(Socket_Outgoing, 0, 80, 255, 88, 1, _BackEnd_Server_)) {

					// Parse JSON
					this->Parse_JSON(_Pack_Type);

					// Sending Data
					if (AT_Command_Set::SSEND(Socket_Outgoing, 2, 0, _BackEnd_Server_, _BackEnd_EndPoint_, this->Buffer.JSON_Pack)) {

						// Declare Ring Length
						uint16_t _Length;

						// Get Ring Port
						if (AT_Command_Set::SRING(_Length)) {

							// Declare Response Variable
							char _Response[Response_JSON_Size];

							// Clear Response
							memset(_Response, '\0', Response_JSON_Size);

							// Command Delay
							delay(50);

							// Handle Length
							if (_Length > Response_JSON_Size) _Length = Response_JSON_Size;

							// Get Request Data
							if (AT_Command_Set::SRECV(Socket_Outgoing, _Length, _Response)) {

								// Define JSON Object
								StaticJsonDocument<Response_JSON_Size> Incoming_JSON;

								// Deserialize the JSON document
								deserializeJson(Incoming_JSON, _Response);
								
								// Get Response Command
								uint16_t _Response_Command = Incoming_JSON["Event"];

								// Control for Response Command
								if (_Response_Command == 200) {

									// End Function
									return(true);

								} else {

									// End Function
									return(false);

								}

								// Command Delay
								delay(50);

								// Closing Socket
								if (AT_Command_Set::SH(Socket_Outgoing)) return(true);

							} 
							
						} 

					} 

				} 

			} 

			// End Function
			return(false);

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

};
