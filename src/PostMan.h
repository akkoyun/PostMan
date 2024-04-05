#ifndef __PostMan__
#define __PostMan__

	// Include Arduino Library
	#ifndef Arduino_h
		#include <Arduino.h>
	#endif

	// Include Configurations
	#include "Config.h"

	// Include Definitions
	#include "Definitions.h"

	// Include AT Commands
	#ifndef __AT_Command__
		#include <AT_Command.h>
	#endif

	// Include MD5 Hashing
	#ifndef _MD5_
		#include <MD5.h>
	#endif

	// Include JSON Library
	#ifndef __JSON__
		#include <JSON.h>
	#endif

	// Cloud Functions
	class Postman : private LE910C1_EUX {

		// Private Context
		private:

			// Define Pointers
			B107AA* Hardware;
			Variable<float>* Payload;
			PowerStat_Console* Terminal;

			// Define Variables
			uint8_t Socket_Status = _SOCKET_CLOSED_;

			// Define MD5 Hash Variable
			char MD5_Hash[33];

			// Define Operator Structure
			struct Struct_Operator {
				uint8_t 	SIM_PIN = _SIM_UNKNOWN_;
				char 		ICCID[21];
				uint16_t 	MCC;
				uint16_t 	MNC;
				uint8_t 	IP_Address[4];
				uint8_t		WDS = _CONNECTION_UNKNOWN_;
				uint16_t	RSSI;
				uint16_t	TAC;
				uint32_t	CellID;
				uint8_t		Signal;
				float 		Connection_Time;
				uint32_t 	Connection_Start;
			} Operator;

			// Define Module Structure
			struct Struct_Module {
				uint8_t 	Manufacturer = _MODEM_MANUFACTURER_UNKNOWN_;
				uint8_t 	Model = _MODEM_MODEL_UNKNOWN_;
				char 		IMEI[17];
				char 		Firmware[15];
			} Module;

			// Define Variables
			uint8_t Status = 0x00;

			// Clear Variables
			void Clear_Variables(void) {

				// Initialize Status Variables
				bitClear(this->Status, PostMan_Status_Initialize);
				bitClear(this->Status, PostMan_Status_SIM_Inserted);
				bitClear(this->Status, PostMan_Status_Connection);

				// Initialize Module Variables
				this->Module.Manufacturer = 0;
				this->Module.Model = 0;
				memset(this->Module.IMEI, 0, sizeof(this->Module.IMEI));
				memset(this->Module.Firmware, 0, sizeof(this->Module.Firmware));

				// Initialize Operator Variables
				this->Operator.SIM_PIN = _SIM_UNKNOWN_;
				memset(this->Operator.ICCID, 0, sizeof(this->Operator.ICCID));
				this->Operator.MCC = 0;
				this->Operator.MNC = 0;
				memset(this->Operator.IP_Address, 0, sizeof(this->Operator.IP_Address));
				this->Operator.WDS = _CONNECTION_UNKNOWN_;
				this->Operator.RSSI = 0;
				this->Operator.Signal = 0;
				this->Operator.Connection_Start = 0;
				this->Operator.Connection_Time = 0;

				// Initialize Socket Variables
				this->Socket_Status = _SOCKET_CLOSED_;

			}

			// Calculate SD File MD5 Hash
			bool Calculate_MD5(char * _MD5) {

				// Enable SD Multiplexer
				Hardware->SD_Multiplexer(true);

				// Declare MD5 Object
				MD5 _Hash;

				// Declare File Object
				File _SD_File;

				// Open File for Open
				_SD_File = Hardware->open(_PostMan_Firmware_Name_, O_READ);

				// Control for File Open
				if (_SD_File) {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Calculating MD5 Hash..."));

					// Declare MD5 Buffer Size
					uint8_t _MD5_Buffer_Size = 10;

					// Read File
					while (_SD_File.available() > 0) {

						// Declare Data Buffer
						char _Data[_MD5_Buffer_Size];

						// Clear Buffer
						memset(_Data, 0, sizeof(_Data));

						// Read 10 Byte Data
						_SD_File.readBytes(_Data, _MD5_Buffer_Size - 1);

						// MD5 Checksum
						_Hash.Update(_Data, _MD5_Buffer_Size - 1);

						// Turn ON HeartBeat
						PORT_HEARTBEAT |= (1 << PIN_HEARTBEAT);
						PORT_HEARTBEAT &= ~(1 << PIN_HEARTBEAT);

					}

				} else {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Firmware File Open Failed!!"));

					// End Function
					return(false);

				}

				// Declare MD5 Buffer
				uint8_t _File_Hash[16];

				// Calculate MD5
				_Hash.Finalize(_File_Hash);

				// MD5 to Char Array
				_Hash.Digest(_File_Hash, _MD5);

				// Close SD File
				_SD_File.close();

				// Print Hash
				if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(47, 4, _Console_GRAY_, _MD5);

				// Disable SD Multiplexer
				Hardware->SD_Multiplexer(false);

				// End Function
				return(true);

			}

			// Calculate Length of Char Array
			uint16_t Length(const char * _Array) {

				// Declare Length Variable
				uint16_t _Length = 0;

				// Calculate Length
				while (_Array[_Length] != '\0') _Length++;

				// Return Length
				return(_Length);

			}

			// Remove Spaces from Char Array
			void Remove_Spaces(char * _Buffer) {

				// Declare Buffer Variables
				char * _Source = _Buffer;
				char * _Desitination = _Buffer;

				// Loop for Buffer
				while (* _Source) {

					// Control for Space
					if (* _Source != ' ') * _Desitination++ = * _Source;

					// Increment Source
					 _Source++;

				}

				// Set End of Buffer
				* _Desitination = '\0';

			}

			// Generate JSON Data
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key) {
				
				// Add Key
				strcat(_Buffer, "\"");
				strcat_P(_Buffer, (const char *)_Key);
				strcat(_Buffer, "\":");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, const char * _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Add Key Value
				strcat(_Buffer, "\"");
				strcat(_Buffer, _Key_Value);
				strcat(_Buffer, "\"");

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, const __FlashStringHelper * _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Add Key Value
				strcat(_Buffer, "\"");
				strcat_P(_Buffer, (const char *)_Key_Value);
				strcat(_Buffer, "\"");

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, bool _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Add Key Value
				strcat(_Buffer, _Key_Value ? "true" : "false");

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, uint32_t _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Convert Value to Char Array
				char _Value_Buffer[11];
				snprintf(_Value_Buffer, sizeof(_Value_Buffer), "%lu", _Key_Value);

				// Add Key Value
				strcat(_Buffer, _Value_Buffer);

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, uint16_t _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Convert Value to Char Array
				char _Value_Buffer[6];
				snprintf(_Value_Buffer, sizeof(_Value_Buffer), "%u", _Key_Value);

				// Add Key Value
				strcat(_Buffer, _Value_Buffer);

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, uint8_t _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Convert Value to Char Array
				char _Value_Buffer[5];
				snprintf(_Value_Buffer, sizeof(_Value_Buffer), "%u", _Key_Value);

				// Add Key Value
				strcat(_Buffer, _Value_Buffer);

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, float _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Convert Value to Char Array
				char _Value_Buffer[20];
				dtostrf(_Key_Value, 4, 2, _Value_Buffer);

				// Add Key Value
				strcat(_Buffer, _Value_Buffer);

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, const float * _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Set Variable
				float _Data = * _Key_Value;

				// Convert Value to Char Array
				char _Value_Buffer[20];
				dtostrf(_Data, 4, 2, _Value_Buffer);

				// Add Key Value
				strcat(_Buffer, _Value_Buffer);

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}
			void Add_JSON_Key(char * _Buffer, const __FlashStringHelper * _Key, double _Key_Value, const bool _Comma = false) {

				// Add Key
				this->Add_JSON_Key(_Buffer, _Key);

				// Convert Value to Char Array
				char _Value_Buffer[20];
				dtostrf(_Key_Value, 4, 2, _Value_Buffer);

				// Add Key Value
				strcat(_Buffer, _Value_Buffer);

				// Add Comma
				if (_Comma) strcat(_Buffer, ",");

			}

			// JSON Info Segment Parser
			uint16_t JSON_Info_Segment(char * _Buffer, uint8_t _Pack_Type) {

				// Info Segment
				// ----------------

				// 	"Info": {
				// 		"Command": "Interrupt",
				// 		"TimeStamp": "2024-02-13 17:11:15",
				// 		"ID": "C50000011D05A970",
				// 		"Firmware": "04.00.15"
				// 	},

				// Add Info Segment Start
				strcpy(_Buffer, "\"Info\":{");

				// Command Type
				switch (_Pack_Type) {

					// Case Pack_Online
					case Pack_Online : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("Online"));

						// End Case
						break;

					}

					// Case Pack_Update
					case Pack_Update : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("Update"));

						// End Case
						break;

					}

					// Case Pack_Timed
					case Pack_Timed : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("Timed"));

						// End Case
						break;

					}

					// Case Pack_Interrupt
					case Pack_Interrupt : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("Interrupt"));

						// End Case
						break;

					}

					// Case Pack_Alarm
					case Pack_Alarm : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("Alarm"));

						// End Case
						break;

					}

					// Case Pack_Offline
					case Pack_Offline : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("Offline"));

						// End Case
						break;

					}

					// Case Pack_FOTA_Info
					case Pack_FOTA_Info : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("FOTA_Info"));

						// End Case
						break;

					}

					// Case Pack_FOTA_Download
					case Pack_FOTA_Download : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("FOTA_Download"));

						// End Case
						break;

					}

					// Case Pack_FOTA_Burn
					case Pack_FOTA_Burn : {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("FOTA_Burn"));

						// End Case
						break;

					}

					// Case Pack Unknown
					default: {

						// Set Pack Type
						this->Add_JSON_Key(_Buffer, F("Command"), F("Unknown"));

						// End Case
						break;

					}

				}

				// Add Comma
				strcat(_Buffer, ",");

				// Add TimeStamp
				this->Add_JSON_Key(_Buffer, F("TimeStamp"), this->Time.TimeStamp, true);

				// Add ID Segment
				this->Add_JSON_Key(_Buffer, F("ID"), Payload->Device_ID, true);	

				// Add Firmware Segment
				this->Add_JSON_Key(_Buffer, F("Firmware"), _FIRMWARE_);

				// Add End of Info Segment
				strcat(_Buffer, "}");

				// Return Length
				return(this->Length(_Buffer));

			}

			// JSON Post Segment Parser
			uint16_t JSON_Power_Segment(char * _Buffer, uint8_t _Pack_Type) {

				//	"Power": {
				// 		"B_IV": 4.137187,
				// 		"B_AC": 0.46875,
				// 		"B_IC": 2100,
				// 		"B_FC": 2000,
				// 		"B_SOC": 99,
				// 		"B_T": 24.95313,
				// 		"B_CS": 3
				// 	},

				// Add Power Segment Start
				strcpy(_Buffer, "\"Power\":{");

				// Get some data
				const uint16_t _Keys[8] = {_Data_B_IV_, _Data_B_AC_, _Data_B_ICAP_, _Data_B_FCAP_, _Data_B_SOC_, _Data_B_T_, _Data_B_CS_};

				// Declare Comma Status
				bool _Comma = false;

				// Print the data
				for (uint16_t _Key : _Keys) {

					// Get the value
					const float* _Value = Payload->Get(_Key);

					// Control for Value
					if (_Value != nullptr) {

						// Add Comma
						if (_Comma) strcat(_Buffer, ",");

						// Add Key
						switch (_Key) {

							// Case _Data_B_IV_
							case _Data_B_IV_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("B_IV"), *_Value);

								// End Case
								break;

							}

							// Case _Data_B_AC_
							case _Data_B_AC_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("B_AC"), *_Value);

								// End Case
								break;

							}

							// Case _Data_B_ICAP_
							case _Data_B_ICAP_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("B_IC"), (uint16_t)*_Value);

								// End Case
								break;

							}

							// Case _Data_B_FCAP_
							case _Data_B_FCAP_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("B_FC"), (uint16_t)*_Value);

								// End Case
								break;

							}

							// Case _Data_B_SOC_
							case _Data_B_SOC_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("B_SOC"), *_Value);

								// End Case
								break;

							}

							// Case _Data_B_T_
							case _Data_B_T_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("B_T"), *_Value);

								// End Case
								break;

							}

							// Case _Data_B_CS_
							case _Data_B_CS_: {

								// Convert Value to uint32_t
								float _ValueFloat = *_Value;
								uint32_t _Status = static_cast<uint32_t>(_ValueFloat);

								// Add Key
								this->Add_JSON_Key(_Buffer, F("B_CS"), _Status);

								// End Case
								break;

							}

						}

						// Set Comma
						_Comma = true;

					}

				}

				// Add End of Power Segment
				strcat(_Buffer, "}");

				// Remove Spaces from Buffer
				this->Remove_Spaces(_Buffer);

				// Return Length
				return(this->Length(_Buffer));

			}

			// JSON IoT Segment Parser
			uint16_t JSON_IoT_Segment(char * _Buffer, uint8_t _Pack_Type) {

				//	"IoT": {
				//		"Firmware": "25.30.226",
				//		"IMEI": "354485417649444",
				//		"ICCID": "8990011936290169339",
				//		"RSSI": 69,
				//		"WDS": 25,
				//		"ConnTime": 1.978,
				//		"MCC": 286,
				//		"MNC": 1
				//	}

				// Add IoT Segment Start
				strcpy(_Buffer, "\"IoT\":{");

				// Add Firmware Segment
				this->Add_JSON_Key(_Buffer, F("Firmware"), this->Module.Firmware);

				// Add Comma
				strcat(_Buffer, ",");

				// Add IMEI Segment
				this->Add_JSON_Key(_Buffer, F("IMEI"), this->Module.IMEI);

				// Add Comma
				strcat(_Buffer, ",");

				// Add ICCID Segment
				this->Add_JSON_Key(_Buffer, F("ICCID"), this->Operator.ICCID);

				// Add Comma
				strcat(_Buffer, ",");

				// Control for Pack Type
				if (_Pack_Type == Pack_Online) {

					// Add Connection Time Segment
					this->Add_JSON_Key(_Buffer, F("ConnTime"), this->Operator.Connection_Time);

					// Add Comma
					strcat(_Buffer, ",");

					// Add MCC Segment
					this->Add_JSON_Key(_Buffer, F("MCC"), this->Operator.MCC);

					// Add Comma
					strcat(_Buffer, ",");

					// Add MNC Segment
					this->Add_JSON_Key(_Buffer, F("MNC"), this->Operator.MNC);

					// Add Comma
					strcat(_Buffer, ",");

				}

				// Add TAC Segment
				this->Add_JSON_Key(_Buffer, F("TAC"), this->Operator.TAC);

				// Add Comma
				strcat(_Buffer, ",");

				// Add CellID Segment
				this->Add_JSON_Key(_Buffer, F("CELLID"), this->Operator.CellID);

				// Add Comma
				strcat(_Buffer, ",");

				// Add RSSI Segment
				this->Add_JSON_Key(_Buffer, F("RSSI"), this->Operator.RSSI);

				// Add Comma
				strcat(_Buffer, ",");

				// Add WDS Segment
				this->Add_JSON_Key(_Buffer, F("WDS"), this->Operator.WDS);	

				// Add End of IoT Segment
				strcat(_Buffer, "}");

				// Return Length
				return(this->Length(_Buffer));

			}

			// JSON Payload Segment Parser
			uint16_t JSON_Payload_Segment(char * _Buffer, uint8_t _Pack_Type) {

				//	"Payload": {
				//		"PCB_T": 25.82779,
				//		"PCB_H": 27.48261,
				//		"STATUS": 15
				//	}

				// Copy Buffer Start
				strcpy(_Buffer, "\"Payload\":{");

				// Control for Pack Type
				if (_Pack_Type == Pack_FOTA_Download) {

					// Get some data
					const uint16_t _Keys[4] = {_Data_STATUS_, _Firmware_ID_, _FOTA_Download_Status_, _FOTA_Download_Time_};

					// Declare Comma Status
					bool _Comma = false;

					// Print the data
					for (uint16_t _Key : _Keys) {

						// Get the value
						const float* _Value = Payload->Get(_Key);

						// Control for Value
						if (_Value != nullptr) {

							// Add Comma
							if (_Comma) strcat(_Buffer, ",");

							// Add Key
							switch (_Key) {

								// Case _Data_STATUS_
								case _Data_STATUS_: {

									// Convert Value to uint8_t
									float _ValueFloat = *_Value;
									uint8_t _Status = static_cast<uint8_t>(_ValueFloat);

									// Add Key
									this->Add_JSON_Key(_Buffer, F("STATUS"), _Status);

									// End Case
									break;

								}

								// Case _Firmware_ID_
								case _Firmware_ID_: {

									// Convert Value to uint32_t
									float _ValueFloat = *_Value;
									uint32_t _ID = static_cast<uint32_t>(_ValueFloat);

									// Add Key
									this->Add_JSON_Key(_Buffer, F("Firmware_ID"), _ID);

									// End Case
									break;

								}

								// Case _FOTA_Download_Status_
								case _FOTA_Download_Status_: {

									// Convert Value to uint32_t
									float _ValueFloat = *_Value;
									uint8_t _Status = static_cast<uint8_t>(_ValueFloat);

									// Add Key
									this->Add_JSON_Key(_Buffer, F("FOTA_Download_Status"), _Status);

									// End Case
									break;

								}

								// Case _FOTA_Download_Time_
								case _FOTA_Download_Time_: {

									// Convert Value to uint8_t
									float _ValueFloat = *_Value;
									uint16_t _Time = static_cast<uint8_t>(_ValueFloat);

									// Add Key
									this->Add_JSON_Key(_Buffer, F("FOTA_Download_Time"), _Time);

									// End Case
									break;

								}

							}

							// Set Comma
							_Comma = true;

						}

					}

					// Add Comma
					if (_Comma) strcat(_Buffer, ",");

					// Add Key
					this->Add_JSON_Key(_Buffer, F("MD5"), this->MD5_Hash);

				} else {

					// Get some data
					const uint16_t _Keys[15] = {
						_Data_STATUS_, 
						_Data_PCB_T_, _Data_PCB_H_, 
						_Data_T_Pump_,
						_Data_V_R_, _Data_V_MAX_R_, _Data_V_MIN_R_,
						_Data_VRMS_R_, _Data_VRMS_MAX_R_, _Data_VRMS_MIN_R_, 
						_Data_VRMS_S_, 
						_Data_VRMS_T_, 
						_Data_FQ_, _Data_FQ_MIN_, _Data_FQ_MAX_
					};

					// Declare Comma Status
					bool _Comma = false;

					// Print the data
					for (uint16_t _Key : _Keys) {

						// Get the value
						const float* _Value = Payload->Get(_Key);

						// Control for Value
						if (_Value != nullptr) {

							// Add Comma
							if (_Comma) strcat(_Buffer, ",");

							// Add Key
							switch (_Key) {

								// Case _Data_STATUS_
								case _Data_STATUS_: {

									// Convert Value to uint32_t
									float _ValueFloat = *_Value;
									uint32_t _Status = static_cast<uint32_t>(_ValueFloat);

									// Add Key
									this->Add_JSON_Key(_Buffer, F("STATUS"), _Status);

									// End Case
									break;

								}

								// Case _Data_PCB_T_
								case _Data_PCB_T_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("PCB_T"), *_Value);

									// End Case
									break;

								}

								// Case _Data_PCB_H_
								case _Data_PCB_H_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("PCB_H"), *_Value);

									// End Case
									break;

								}

								// Case _Data_T_Pump_
								case _Data_T_Pump_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("T_Pump"), *_Value);

									// End Case
									break;

								}

								// Case _Data_V_R_
								case _Data_V_R_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("V_R"), *_Value);

									// End Case
									break;

								}

								// Case _Data_V_MAX_R_
								case _Data_V_MAX_R_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("V_MAX_R"), *_Value);

									// End Case
									break;

								}

								// Case _Data_V_MIN_R_
								case _Data_V_MIN_R_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("V_MIN_R"), *_Value);

									// End Case
									break;

								}

								// Case _Data_VRMS_R_
								case _Data_VRMS_R_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("VRMS_R"), *_Value);

									// End Case
									break;

								}

								// Case _Data_VRMS_MAX_R_
								case _Data_VRMS_MAX_R_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("VRMS_MAX_R"), *_Value);

									// End Case
									break;

								}

								// Case _Data_VRMS_MIN_R_
								case _Data_VRMS_MIN_R_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("VRMS_MIN_R"), *_Value);

									// End Case
									break;

								}

								// Case _Data_VRMS_S_
								case _Data_VRMS_S_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("VRMS_S"), *_Value);

									// End Case
									break;

								}

								// Case _Data_VRMS_T_
								case _Data_VRMS_T_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("VRMS_T"), *_Value);

									// End Case
									break;

								}

								// Case _Data_FQ_
								case _Data_FQ_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("FQ"), *_Value);

									// End Case
									break;

								}

								// Case _Data_FQ_MIN_
								case _Data_FQ_MIN_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("FQ_MIN"), *_Value);

									// End Case
									break;

								}

								// Case _Data_FQ_MAX_
								case _Data_FQ_MAX_: {

									// Add Key
									this->Add_JSON_Key(_Buffer, F("FQ_MAX"), *_Value);

									// End Case
									break;

								}

							}

							// Set Comma
							_Comma = true;

						}

					}

				}

				// Close Payload Buffer
				strcat(_Buffer, "}");

				// Remove Spaces from Buffer
				this->Remove_Spaces(_Buffer);

				// Return Length
				return(this->Length(_Buffer));

			}

			// JSON Payload Segment Parser
			uint16_t JSON_Energy_Segment(char * _Buffer) {

				// Copy Buffer Start
				strcpy(_Buffer, "\"Energy\":{");

				// Get some data
				const uint16_t _Keys[8] = {_Data_VRMS_R_, _Data_VRMS_S_, _Data_VRMS_T_};

				// Declare Comma Status
				bool _Comma = false;

				// Print the data
				for (uint16_t _Key : _Keys) {

					// Get the value
					const float* _Value = Payload->Get(_Key);

					// Control for Value
					if (_Value != nullptr) {

						// Add Comma
						if (_Comma) strcat(_Buffer, ",");

						// Add Key
						switch (_Key) {

							// Case _Data_VRMS_R_
							case _Data_VRMS_R_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("VRMS_R"), *_Value);

								// End Case
								break;

							}

							// Case _Data_VRMS_S_
							case _Data_VRMS_S_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("VRMS_S"), *_Value);

								// End Case
								break;

							}

							// Case _Data_VRMS_T_
							case _Data_VRMS_T_: {

								// Add Key
								this->Add_JSON_Key(_Buffer, F("VRMS_T"), *_Value);

								// End Case
								break;

							}

						}

						// Set Comma
						_Comma = true;

					}

				}

				// Close Payload Buffer
				strcat(_Buffer, "}");

				// Remove Spaces from Buffer
				this->Remove_Spaces(_Buffer);

				// Return Length
				return(this->Length(_Buffer));

			}

			// JSON Parser Handler Function
			void JSON_Parser(uint8_t _Pack_Type) {

				// Declare JSON Size
				uint16_t _JSON_Size = 16;

				// Info Segment
				// ----------------

				// Declare Info Buffer
				char _Info_Buffer[128];
				memset(_Info_Buffer, '\0', sizeof(_Info_Buffer));

				// Parse Info
				_JSON_Size += this->JSON_Info_Segment(_Info_Buffer, _Pack_Type);

				// Power Segment
				// ----------------

				// Declare Power Buffer
				char _Power_Buffer[100];
				memset(_Power_Buffer, '\0', sizeof(_Power_Buffer));

				// Parse Power
				_JSON_Size += this->JSON_Power_Segment(_Power_Buffer, _Pack_Type);

				// IoT Segment
				// ----------------

				// Declare IoT Buffer
				char _IoT_Buffer[200];
				memset(_IoT_Buffer, '\0', sizeof(_IoT_Buffer));

				// Parse IoT
				_JSON_Size += this->JSON_IoT_Segment(_IoT_Buffer, _Pack_Type);

				// Payload Segment
				// ----------------

				// Declare Payload Buffer
				char _Payload_Buffer[600];
				memset(_Payload_Buffer, '\0', sizeof(_Payload_Buffer));

				// Parse Payload
				_JSON_Size += this->JSON_Payload_Segment(_Payload_Buffer, _Pack_Type);

				// Print Header
				// ----------------

				SERIAL_GSM.print(F("POST ")); SERIAL_GSM.print(_PostMan_EndPoint_); SERIAL_GSM.print(F(" HTTP/1.1\r\n"));
				SERIAL_GSM.print(F("Host: ")); SERIAL_GSM.print(_PostMan_Server_); SERIAL_GSM.print(F("\r\n"));
				SERIAL_GSM.print(F("Content-Length: ")); SERIAL_GSM.print(_JSON_Size); SERIAL_GSM.print(F("\r\n"));
				SERIAL_GSM.print(F("Content-Type: application/json\r\n"));
				SERIAL_GSM.print(F("User-Agent: PostOffice\r\n"));
				SERIAL_GSM.print(F("\r\n"));

				// Print Body
				// ----------------

				SERIAL_GSM.print(F("{"));
				SERIAL_GSM.print(_Info_Buffer);
				SERIAL_GSM.print(F(",\"Device\":{"));
				SERIAL_GSM.print(_Power_Buffer);
				SERIAL_GSM.print(F(","));
				SERIAL_GSM.print(_IoT_Buffer);
				SERIAL_GSM.print(F("},"));
				SERIAL_GSM.print(_Payload_Buffer);
				SERIAL_GSM.print(F("}"));

			}

		// Protected Context
		protected:

			// Define Instance
			static Postman* Instance;

			// Initalize GSM Module
			bool Initialize(void) {

				// Declare WatchDog Variable
				uint8_t _WD = 0;

				// Initialize Modem Parameters
				while (!bitRead(this->Status, PostMan_Status_Initialize)) {

					// Control for Modem Power Monitor
					if (Hardware->PowerMonitor()) {

						// Set Control Variable
						bitSet(this->Status, PostMan_Status_Initialize);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT"));

						// Send Command
						if (!LE910C1_EUX::AT()) bitClear(this->Status, PostMan_Status_Initialize);

						// Calculate Connection Time
						this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) {

							// Print Message
							Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

							// Print Connection Time
							Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						}

						// ATE Command (Echo Off)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT0"));

							// Send Command
							if (!LE910C1_EUX::ATE(false)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SIMDET Command (SIM Detect)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+SIMDET?"));

							// Declare Buffer Variable
							bool _SIMDET = false;

							// Send Command
							if (!LE910C1_EUX::SIMDET(_AT_GET_, 0, _SIMDET)) bitClear(this->Status, PostMan_Status_Initialize);

							// Handle Variable
							if (_SIMDET) {bitSet(this->Status, PostMan_Status_SIM_Inserted);} else {bitClear(this->Status, PostMan_Status_SIM_Inserted);}

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

							// Control for SIM State
							if (!bitRead(this->Status, PostMan_Status_SIM_Inserted)) {

								// Clear Variables
								bitClear(this->Status, PostMan_Status_Initialize);

								// End Function
								return(false);

							}

						} else break;

						// CFUN Command (Set Full Functionality)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CFUN=1,0"));

							// Send Command
							if (!LE910C1_EUX::CFUN(1)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// CMEE Command (Set Error Format)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CMEE=1"));

							// Send Command
							if (!LE910C1_EUX::CMEE(1)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// FCLASS Command (Set Internet Mode)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+FCLASS=0"));

							// Send Command
							if (!LE910C1_EUX::FCLASS(0)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// ATK0 Command (Set Data Mode)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT&K0"));

							// Send Command
							if (!LE910C1_EUX::K(0)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// CPIN Command (Check SIM Pin)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CPIN?"));

							// Send Command
							if (!LE910C1_EUX::CPIN(this->Operator.SIM_PIN)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print SIM Status
								if (bitRead(this->Status, PostMan_Status_SIM_Inserted) && this->Operator.SIM_PIN == _SIM_READY_) {

									// Print SIM Status
									Terminal->Text(17, 29, _Console_GRAY_, F("  SIM OK  "));

								} else {

									// Print SIM Status
									Terminal->Text(17, 29, _Console_GRAY_, F("SIM ERROR "));

								}

							}

							// Control for SIM State
							if (this->Operator.SIM_PIN != _SIM_READY_) {

								// Clear Variables
								bitClear(this->Status, PostMan_Status_Initialize);

								// End Function
								return(false);

							}

						} else break;

						// CGSN Command (Get IMEI Number)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CGSN"));

							// Send Command
							if (!LE910C1_EUX::CGSN(this->Module.IMEI)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print IMEI Number
								Terminal->Text(16, 24, _Console_GRAY_, this->Module.IMEI);

							}

						} else break;

						// ICCID Command (Get SIM ICCID Number)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CCID"));

							// Send Command
							if (!LE910C1_EUX::CCID(this->Operator.ICCID)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print ICCID Number
								Terminal->Text(18, 20, _Console_GRAY_, this->Operator.ICCID);

							}

						} else break;

						// CGMI Command (Get Manufacturer)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CGMI"));

							// Send Command
							if (!LE910C1_EUX::CGMI(this->Module.Manufacturer)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Manufacturer
								if (this->Module.Manufacturer < 9) {
									Terminal->Text(13, 37, _Console_GRAY_, F("0"));
									Terminal->Text(13, 38, _Console_GRAY_, this->Module.Manufacturer);
								} else {
									Terminal->Text(13, 37, _Console_GRAY_, this->Module.Manufacturer);
								}

							}

						} else break;

						// CGMM Command (Get Model)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+GMM"));

							// Send Command
							if (!LE910C1_EUX::CGMM(this->Module.Model)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Model
								Terminal->Text(14, 37, _Console_GRAY_, this->Module.Model);

							}

						} else break;

						// CGMR Command (Get Firmware)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SWPKGV"));

							// Send Command
							if (!LE910C1_EUX::SWPKGV(this->Module.Firmware)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Firmware
								Terminal->Text(15, 30, _Console_GRAY_, this->Module.Firmware);

							}

						} else break;

						// E2RI Command (Set RING Indicator)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#E2RI=50,50"));

							// Send Command
							if (!LE910C1_EUX::E2SLRI(50)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// GPIO Command (Set GPIO)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#GPIO=1,0,2"));

							// Send Command
							if (!LE910C1_EUX::GPIO(_AT_SET_, 1, 0, 2)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SLED Command (Set LED)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SLED=2"));

							// Send Command
							if (!LE910C1_EUX::SLED(2)) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SLEDSAV Command (Set LED Save)
						if (bitRead(this->Status, PostMan_Status_Initialize)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SLEDSAV"));

							// Send Command
							if (!LE910C1_EUX::SLEDSAV()) bitClear(this->Status, PostMan_Status_Initialize);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Initialize));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(21, 84, _Console_WHITE_, F("                                    "));

						// Initialize Complete
						return(true);

					} else {

						// Power ON GSM Module
						Hardware->ON();

					}

					// Handle WatchDog
					if (_WD > 2) {

						// Initialize Variables
						this->Clear_Variables();

						// End Function
						return(false);

					} else {

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) {

							// Print Message
							Terminal->Show_Message(_Console_YELLOW_, F("Retry Initializing..."));

						}

						// Set WatchDog Variable
						_WD++;

					}

				}

				// Not Initialized
				return(false);

			}

			// Connect GSM Module
			bool Connection(void) {

				// Declare WatchDog Variable
				uint8_t _WD = 0;

				// Connect Modem Parameters
				while (!bitRead(this->Status, PostMan_Status_Connection)) {

					// Control for Modem Power Monitor
					if (bitRead(this->Status, PostMan_Status_Initialize)) {

						// Set Control Variable
						bitSet(this->Status, PostMan_Status_Connection);

						// CREG Command (Get Network Registration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CREG?"));

							// Set Connection WatchDog
							bool _Conn_WD = false;
							uint8_t _Conn_WD_Counter = 0;

							// Wait for Connection
							while (!_Conn_WD) {

								// Declare Variable
								uint8_t _CREG_Connection_Mode = 99;
								uint8_t _CREG_Connection_Stat = 99;

								// Get CREG Status
								LE910C1_EUX::CREG(_AT_GET_, _CREG_Connection_Mode, _CREG_Connection_Stat);

								// Control for Connection
								if (_CREG_Connection_Stat == 0) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5000; i++) {

										// Calculate Connection Time
										this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

										// Control for Terminal State
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Print Connection Time
											Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

										}

										// Connection Wait Delay
										delay(1);

									}

								} else if (_CREG_Connection_Stat == 1) {

									// Set Variable
									bitSet(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = true;

								} else if (_CREG_Connection_Stat == 2) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5000; i++) {

										// Calculate Connection Time
										this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

										// Control for Terminal State
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Print Connection Time
											Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

										}

										// Connection Wait Delay
										delay(1);

									}

								} else if (_CREG_Connection_Stat == 3) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 3000; i++) {

										// Calculate Connection Time
										this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

										// Control for Terminal State
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Print Connection Time
											Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

										}

										// Connection Wait Delay
										delay(1);

									}

								} else if (_CREG_Connection_Stat == 4) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = false;

									// Connection Wait Delay
									for (uint8_t i = 0; i < 5000; i++) {

										// Calculate Connection Time
										this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

										// Control for Terminal State
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Print Connection Time
											Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

										}

										// Connection Wait Delay
										delay(1);

									}

								} else if (_CREG_Connection_Stat == 5) {

									// Set Variable
									bitClear(this->Status, PostMan_Status_Connection);

									// Declare Response Status
									_Conn_WD = true;

								}

								// Calculate Connection Time
								this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

								// Control for Terminal State
								if (bitRead(this->Status, PostMan_Status_Terminal)) {

									// Print Connection Time
									Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								}

								// Set WD Variable
								_Conn_WD_Counter++;

								// Control for WD
								if (_Conn_WD_Counter > 300) _Conn_WD = true;

							}

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// CGDCONT Command (Set PDP Context)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CGDCONT=1,IP,mgbs"));

							// Send Command
							if (!LE910C1_EUX::CGDCONT(1, "IP", _PostMan_APN_)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SGACT Command (Activate PDP Context)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SGACT=1,1"));

							// Send Command
							if (!LE910C1_EUX::SGACT(1, 1, this->Operator.IP_Address)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Define IP char Array
								char _IP_Char[16];

								// Set IP char Array
								sprintf(_IP_Char, "%03d.%03d.%03d.%03d", this->Operator.IP_Address[0], this->Operator.IP_Address[1], this->Operator.IP_Address[2], this->Operator.IP_Address[3]);

								// Print IP Address
								Terminal->Text(16, 64, _Console_GRAY_, _IP_Char);

							}

						} else break;

						// WS46 Command (Connection Status)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+WS46?"));

							// Send Command
							if (!LE910C1_EUX::WS46(_AT_GET_, this->Operator.WDS)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Connection Status
								Terminal->Show_Connection_Type(2, 116, this->Operator.WDS);

							}

						} else break;

						// RFSTS Command (Get Signal Quality)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#RFSTS"));

							// Send Command
							if (!LE910C1_EUX::RFSTS(this->Operator.WDS, this->Operator.MCC, this->Operator.MNC, this->Operator.RSSI, this->Operator.Signal, this->Operator.TAC, this->Operator.CellID)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

								// Print Operator Value
								char _Operator[6];
								sprintf(_Operator, "%03d%02d", this->Operator.MCC, this->Operator.MNC);
								Terminal->Text(15, 74, _Console_GRAY_, String(_Operator));

								// Print Signal Level Value
								Terminal->Text(14, 75, _Console_GRAY_, this->Operator.RSSI);

								// Print Signal Quality
								Terminal->Show_Signal_Quality(2, 110, this->Operator.Signal);

							}

						} else break;

						// SCFG Command (Data Port Configuration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SCFG=1,1,1500,90,1200,50"));

							// Send Command
							if (!LE910C1_EUX::SCFG(_PostMan_CID_, 1, 1500, 90, 1200, 50)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SCFGEXT Command (Data Port Extended Configuration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SCFGEXT=1,1,0,0,0,0"));

							// Send Command
							if (!LE910C1_EUX::SCFGEXT(_PostMan_CID_, 1, 0, 0, 0, 0)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// SCFGEXT2 Command (Data Port Extended 2 Configuration)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SCFGEXT2=3,1,0,0,0,0"));

							// Send Command
							if (!LE910C1_EUX::SCFGEXT2(_PostMan_CID_, 1, 0, 0)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							}

						} else break;

						// TCPMAXWIN Command (Set TCP Window Size)
						if (bitRead(this->Status, PostMan_Status_Connection)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#TCPMAXWIN=65535,3"));

							// Send Command
							if (!LE910C1_EUX::TCPMAXWIN(65535, 0)) bitClear(this->Status, PostMan_Status_Connection);

							// Calculate Connection Time
							this->Operator.Connection_Time = (float)((millis() - this->Operator.Connection_Start)) / 1000;

							// Control for Terminal State
							if (bitRead(this->Status, PostMan_Status_Terminal)) {

								// Print Message
								Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

								// Print Connection Time
								Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

							} else break;

						} else break;

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(21, 84, _Console_WHITE_, F("                                    "));

						// Connection Success
						return(true);

					} else {

						// Power ON GSM Module
						this->Initialize();

					}

					// Handle WatchDog
					if (_WD > 2) {

						// Initialize Variables
						this->Clear_Variables();

						// End Function
						return(false);

					} else {

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) {

							// Print Message
							Terminal->Show_Message(_Console_YELLOW_, F("Retry Connection..."));

						}

						// Set WatchDog Variable
						_WD++;

					}

				}

				// Not Initialized
				return(false);

			}

			// Update Connection Parameters
			bool Update_Connection_Status(void) {

				// Update Connection Parameters
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+WS46?"));

					// Send Command
					if (!LE910C1_EUX::WS46(_AT_GET_, this->Operator.WDS)) bitClear(this->Status, PostMan_Status_Connection);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

						// Print Connection Status
						Terminal->Show_Connection_Type(2, 116, this->Operator.WDS);

					}

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#RFSTS"));

					// Send Command
					if (!LE910C1_EUX::RFSTS(this->Operator.WDS, this->Operator.MCC, this->Operator.MNC, this->Operator.RSSI, this->Operator.Signal, this->Operator.TAC, this->Operator.CellID)) bitClear(this->Status, PostMan_Status_Connection);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(bitRead(this->Status, PostMan_Status_Connection));

						// Print Operator Value
						char _Operator[6];
						sprintf(_Operator, "%03d%02d", this->Operator.MCC, this->Operator.MNC);
						Terminal->Text(15, 74, _Console_GRAY_, String(_Operator));

						// Print Signal Level Value
						Terminal->Text(14, 75, _Console_GRAY_, this->Operator.RSSI);

						// Print Signal Quality
						Terminal->Show_Signal_Quality(2, 110, this->Operator.Signal);

					}

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Send Request Response Function
			bool Response(const char * _Buffer) {

				// Print Message
				if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Sending Response..."));

				// Send Answer
				if (LE910C1_EUX::SSEND(_PostMan_CID_, _HTTP_RESPONSE_, _PostMan_Server_, _PostMan_EndPoint_, _Buffer)) {

					// Command Delay
					delay(10);

					// Closing Socket
					if (LE910C1_EUX::SH(_PostMan_CID_)) {

						// Command Delay
						delay(10);

						// Listen Incoming Socket
						this->Listen(true);

						// Clear Terminal Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Space(21, 84, 36);

						// End Function
						return(true);

					}

				}

				// End Function
				return(false);

			}

			// Download Firmware Function
			bool Download(const bool _Method, const uint16_t _Firmware_ID) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Declare FOTA Variable Struct
					struct PostMan_FOTA_Variables {
						uint16_t Download_Time = 0;
						uint32_t Download_Start_Time = 0;
						uint32_t Download_Size = 0;
						uint32_t Server_File_Size = 0;
						uint8_t Download_Status = FOTA_Download_Unknown;
					} FOTA;

					// Add Variable
					this->Payload->Add(9001, _Firmware_ID);

					// Set LED Blue
					PORT_MCU_LED_BLUE |= (1 << PIN_MCU_LED_BLUE);

					// Enable SD Multiplexer
					Hardware->SD_Multiplexer(true);

					// FOTA GET Download
					if (_Method == _FOTA_GET_) {

						// Declare Variables
						uint16_t _FOTA_GET_Status = _HTTP_Unknown_;

						// Define ETag Variable
						char _FOTA_Server_ETag[33];
						memset(_FOTA_Server_ETag, '\0', sizeof(_FOTA_Server_ETag));

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Connecting FOTA Server..."));

						// Declare SD File
						File FOTA_File;

						// Delay
						delay(100);

						// Control for Existing File
						if (Hardware->exists(_PostMan_Firmware_Name_)) {

							// Delete Existing File
							Hardware->remove(_PostMan_Firmware_Name_);

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Deleting Existing Firmware File..."));

							// Command Delay
							delay(100);

						} else {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Creating Firmware File..."));

						}

						// Open File for Write
						FOTA_File = Hardware->open(_PostMan_Firmware_Name_, O_WRITE | O_CREAT);

						// Command Delay
						delay(100);

						// Control for File Open
						if (FOTA_File) {

							// Set Download Start Time
							uint32_t _FOTA_Download_Start_Time = millis();

							// Open Socket for Download
							if (LE910C1_EUX::ATSD(_PostMan_CID_, _AT_TCP_, _PostMan_Server_, _PostMan_Port_, _CONNECTION_MANUAL_CLOSE_, 88, _CONNECTION_COMMAND_)) {

								// Print Message
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Firmware Downloading..."));

								// Set Download Path
								char _Download_Path[16];
								sprintf(_Download_Path, "/Firmware/%d", _Firmware_ID);

								// Send Download Request
								if (LE910C1_EUX::SSEND(_PostMan_CID_, _HTTP_GET_, _PostMan_Server_, _Download_Path, "")) {

									// Get Ring
									if (LE910C1_EUX::SRING()) {

										// Define Response Buffer Size
										const uint16_t _Response_Buffer_Size = 1024;

										// Declare Buffer Variable
										char Buffer_Variable[_Response_Buffer_Size];
										char Data_Variable[_Response_Buffer_Size];

										// Clear Buffer Variable
										memset(Buffer_Variable, '\0', _Response_Buffer_Size);
										memset(Data_Variable, '\0', _Response_Buffer_Size);

										// Get Request Data Head
										bool _SRECV = LE910C1_EUX::SRECV(_PostMan_CID_, (_Response_Buffer_Size - 150), Buffer_Variable);

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
												if (i > 7) {

													// Find End Position
													if (
														Buffer_Variable[i-7] == '\r' and 
														Buffer_Variable[i-6] == '\n' and 
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

												// Control for HEX Char
												if (Data_Variable[i - _Start_Position] == 'A' || Data_Variable[i - _Start_Position] == 'B' || Data_Variable[i - _Start_Position] == 'C' || Data_Variable[i - _Start_Position] == 'D' || Data_Variable[i - _Start_Position] == 'E' || Data_Variable[i - _Start_Position] == 'F' || Data_Variable[i - _Start_Position] == '0' || Data_Variable[i - _Start_Position] == '1' || Data_Variable[i - _Start_Position] == '2' || Data_Variable[i - _Start_Position] == '3' || Data_Variable[i - _Start_Position] == '4' || Data_Variable[i - _Start_Position] == '5' || Data_Variable[i - _Start_Position] == '6' || Data_Variable[i - _Start_Position] == '7' || Data_Variable[i - _Start_Position] == '8' || Data_Variable[i - _Start_Position] == '9' || Data_Variable[i - _Start_Position] == '\r' || Data_Variable[i - _Start_Position] == '\n' || Data_Variable[i - _Start_Position] == ':') {

													// Calculate Size
													FOTA.Download_Size += 1;

												}

											}

											// Get First Line
											char* _Response_Line = strtok(Buffer_Variable, "\r\n");

											// Control for Response Line
											while (_Response_Line != NULL) {

												// Control for Response
												if (strstr(_Response_Line, "Content-Length:") != NULL) {

													// Content-Length: 321838\r\n

													// Get Content Length
													sscanf(_Response_Line, "Content-Length: %lu", &FOTA.Server_File_Size);
								
												} else if (strstr(_Response_Line, "etag:") != NULL) {

													// etag: 43720fe3f56006394f3414b9ca9164f9\r\n\r\n

													// Get etag
													sscanf(_Response_Line, "etag: %32[^\r\n]", _FOTA_Server_ETag);

												} else if (strstr(_Response_Line, "HTTP/1.1") != NULL) {

													// HTTP/1.1 200 OK\r\n

													// Get Last Modified
													sscanf(_Response_Line, "HTTP/1.1 %u OK\r\n", &_FOTA_GET_Status);

												} else if (strstr(_Response_Line, "content-disposition:") != NULL) {

													// content-disposition: attachment; filename="PS_041023.hex"\r\n

													// Declare File Name Variable
													char _FOTA_File_Name[14];

													// Get File Name
													sscanf(_Response_Line, "content-disposition: attachment; filename=\"%13[^\"]", _FOTA_File_Name);

													// Print File Name
													if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(13, 106, _Console_GRAY_, _FOTA_File_Name);

												}

												// Get Line
												_Response_Line = strtok(NULL, "\r\n");

											}

											// Control for GET Status
											if (_FOTA_GET_Status == _HTTP_OK_) {

												// Print Message
												if (bitRead(this->Status, PostMan_Status_Terminal)) {

													// Declare File Size Char Buffer
													char _File_Size_Buffer[8];

													// Set Buffer
													sprintf(_File_Size_Buffer, "%07lu", FOTA.Server_File_Size);

													// Print File Size
													Terminal->Text(15, 112, _Console_CYAN_, _File_Size_Buffer);

												}

												// Write Data
												if (_End_Position - _Start_Position > 0) FOTA_File.write(Data_Variable, (_End_Position - _Start_Position));

												// SD Print Delay
												delay(100);

												// Print Message
												if (bitRead(this->Status, PostMan_Status_Terminal)) {

													// Declare Download Size Char Buffer
													char _Download_Size_Buffer[8];

													// Set Buffer
													sprintf(_Download_Size_Buffer, "%07lu", FOTA.Download_Size);

													// Print Download Size
													Terminal->Text(16, 112, _Console_CYAN_, _Download_Size_Buffer);

													// Declare Percent Char Buffer
													char _Download_Time_Buffer[10];

													// Calculate Time
													FOTA.Download_Time = (millis() - _FOTA_Download_Start_Time) / 1000;

													// Set Buffer
													sprintf(_Download_Time_Buffer, "%04u", FOTA.Download_Time);

													// Print Download Time
													Terminal->Text(18, 111, _Console_CYAN_, String(_Download_Time_Buffer));

												}

											} else {

												// Print Message
												if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Server Error..."));

												// Add Variable
												this->Payload->Add(9002, FOTA.Download_Status);

												// End Function
												return(false);

											}
		
										}

										// Declare WD Variable
										uint8_t _WD = 0;

										// Get Data
										while (FOTA.Download_Size < FOTA.Server_File_Size) {

											// Reset Variables
											memset(Buffer_Variable, '\0', sizeof(Buffer_Variable));
											memset(Data_Variable, '\0', sizeof(Data_Variable));

											// Handle Response
											if (LE910C1_EUX::SRECV(_PostMan_CID_, (_Response_Buffer_Size - 100), Buffer_Variable)) {

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
													if (i > 7) {

														// Find End Position
														if (
															Buffer_Variable[i-7] == '\r' and 
															Buffer_Variable[i-6] == '\n' and 
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

													// Control for HEX Char
													if (Data_Variable[i - _Start_Position] == 'A' || Data_Variable[i - _Start_Position] == 'B' || Data_Variable[i - _Start_Position] == 'C' || Data_Variable[i - _Start_Position] == 'D' || Data_Variable[i - _Start_Position] == 'E' || Data_Variable[i - _Start_Position] == 'F' || Data_Variable[i - _Start_Position] == '0' || Data_Variable[i - _Start_Position] == '1' || Data_Variable[i - _Start_Position] == '2' || Data_Variable[i - _Start_Position] == '3' || Data_Variable[i - _Start_Position] == '4' || Data_Variable[i - _Start_Position] == '5' || Data_Variable[i - _Start_Position] == '6' || Data_Variable[i - _Start_Position] == '7' || Data_Variable[i - _Start_Position] == '8' || Data_Variable[i - _Start_Position] == '9' || Data_Variable[i - _Start_Position] == '\r' || Data_Variable[i - _Start_Position] == '\n' || Data_Variable[i - _Start_Position] == ':') {

														// Calculate Size
														FOTA.Download_Size += 1;

													}

												}

												// Write Data
												if (_End_Position - _Start_Position > 0) FOTA_File.write(Data_Variable, (_End_Position - _Start_Position));

											} 

											// Control for CME ERROR 4
											// \r\n+CME ERROR: 4\r\n
											if (strstr(Buffer_Variable, "+CME ERROR: 4") != NULL) {

												// Increase WD
												_WD++;

											} else if (strstr(Buffer_Variable, "CME ERROR: 551") != NULL) {

												// \r\n+CME ERROR: 551\r\n

												// Print Message
												if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Firmware Download Error..."));

												// Set Status
												FOTA.Download_Status = FOTA_Download_CME_Error;

												// Add Variable
												this->Payload->Add(9002, FOTA.Download_Status);

												// End Function
												return(false);

											}

											// Print Message
											if (bitRead(this->Status, PostMan_Status_Terminal)) {

												// Declare Download Size Char Buffer
												char _Download_Size_Buffer[8];

												// Set Buffer
												sprintf(_Download_Size_Buffer, "%07lu", FOTA.Download_Size);

												// Print Download Size
												Terminal->Text(16, 112, _Console_CYAN_, _Download_Size_Buffer);

												// Declare Percent Char Buffer
												char _Download_Percent_Buffer[8];

												// Calculate Percent
												uint8_t _Percent = (uint8_t)((FOTA.Download_Size * 100) / FOTA.Server_File_Size);

												// Set Buffer for Percent (2 Digit Float)
												sprintf(_Download_Percent_Buffer, "%hu", _Percent);

												// Print Download Percent
												Terminal->Text(17, 114, _Console_CYAN_, _Download_Percent_Buffer);

												// Declare Percent Char Buffer
												char _Download_Time_Buffer[10];

												// Calculate Time
												FOTA.Download_Time = (millis() - _FOTA_Download_Start_Time) / 1000;

												// Set Buffer
												sprintf(_Download_Time_Buffer, "%04u", FOTA.Download_Time);

												// Print Download Time
												Terminal->Text(18, 111, _Console_CYAN_, String(_Download_Time_Buffer));

											}

											// Control for WD
											if (_WD > 100) {

												// Print Message
												if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("Firmware Download TimeOut..."));

												// Set Status
												FOTA.Download_Status = FOTA_Download_TimeOut;

												// Add Variable
												this->Payload->Add(9002, FOTA.Download_Status);

												// End While	
												break;

											}

											// Heart Beat
											Hardware->Heartbeat();

										}

									} else {

										// Set Status
										FOTA.Download_Status = FOTA_Server_Error;

										// Print Message
										if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Server Error..."));

										// Add Variable
										this->Payload->Add(9002, FOTA.Download_Status);

										// End Function
										return(false);

									}

								}

							} else {

								// Set Status
								FOTA.Download_Status = FOTA_Server_Error;

								// Print Message
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Server Error..."));

								// Add Variable
								this->Payload->Add(9002, FOTA.Download_Status);

								// End Function
								return(false);

							}

							// Set Download Duration
							FOTA.Download_Time = (millis() - _FOTA_Download_Start_Time) / 1000;

							// Work Delay
							delay(8);

							// Close SD File
							FOTA_File.close();

						} else {

							// Set Status
							FOTA.Download_Status = FOTA_SD_Error;

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("Firmware File Open Failed!!"));

							// Disable SD Multiplexer
							Hardware->SD_Multiplexer(false);

							// Add Variable
							this->Payload->Add(9002, FOTA.Download_Status);

							// End Function
							return(false);

						}

						// Clear LED Blue
						PORT_MCU_LED_BLUE &= ~(1 << PIN_MCU_LED_BLUE);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(46, 4, _Console_GRAY_, _FOTA_Server_ETag);

						// Calculate MD5 Hash
						// TODO: MD5 kütüphanesinde bir karşılaştırma yapan bir fonksiyon oluşturulmalı
						this->Calculate_MD5(this->MD5_Hash);

						// Compare MD5 With ETag
						if (strcmp(this->MD5_Hash, _FOTA_Server_ETag) == 0) {

							// Set Status
							FOTA.Download_Status = FOTA_Download_OK;

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Firmware Download Success!!"));

						} else {

							// Set Status
							FOTA.Download_Status = FOTA_Download_MD5_Error;

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Firmware MD5 Error!!"));

						}

						// Close SD File
						FOTA_File.close();

						// Add Variable
						this->Payload->Add(9002, FOTA.Download_Status);
						this->Payload->Add(9003, FOTA.Download_Time);

					}

					// FOTA FTP Download
					if (_Method == _FOTA_FTP_) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Downloading Firmware via FTP..."));

						// Set Download Path
						char _File_Name[15];
						memset(_File_Name, '\0', sizeof(_File_Name));
						sprintf(_File_Name, "%d.hex", _Firmware_ID);

						// Print Download Size
						Terminal->Text(13, 106, _Console_CYAN_, _File_Name);

						// Declare SD File
						File FOTA_File;

						// Delay
						delay(100);

						// Control for Existing File
						if (Hardware->exists(_PostMan_Firmware_Name_)) {

							// Delete Existing File
							Hardware->remove(_PostMan_Firmware_Name_);

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Deleting Existing Firmware File..."));

							// Command Delay
							delay(100);

						} else {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Creating Firmware File..."));

						}

						// Open File for Write
						FOTA_File = Hardware->open(_PostMan_Firmware_Name_, O_WRITE | O_CREAT);

						// Command Delay
						delay(100);

						// Control for File Open
						if (FOTA_File) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Connecting FOTA FTP Server..."));

							// Set Download Start Time
							FOTA.Download_Start_Time = millis();

							// Print Command
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#FTPTO=600"));

							// FTP Timeout Configuration
							if (LE910C1_EUX::FTPTO(600)) {

								// Print Command State
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(true);

								// Set Status
								FOTA.Download_Status = FOTA_Download_OK;

							} else {

								// Print Command State
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(false);

								// Set Status
								FOTA.Download_Status = FOTA_FTP_Config_Error;

							}

							// FTP Open
							if (FOTA.Download_Status == FOTA_Download_OK) {

								// Print Command
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#FTPOPEN='**','**','**',1"));

								// Open FTP Connection
								if (LE910C1_EUX::FTPOPEN(_PostMan_FTP_Server_, _PostMan_FTP_Username_, _PostMan_FTP_Password_, 0)) {

									// Print Command State
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(true);

								} else {

									// Print Command State
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(false);

									// Set Status
									FOTA.Download_Status = FOTA_FTP_Connect_Error;

								}

							}

							// Set FTP Transfer Type
							if (FOTA.Download_Status == FOTA_Download_OK) {

								// Print Command
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#FTPTYPE=0"));

								// Set FTP Transfer Type
								if (LE910C1_EUX::FTPTYPE(0)) {

									// Print Command State
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(true);

								} else {

									// Print Command State
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(false);

									// Set Status
									FOTA.Download_Status = FOTA_FTP_Config_Error;

								}

							} 

							// Get File Size
							if (FOTA.Download_Status == FOTA_Download_OK) {

								// Print Command
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#FTPFSIZE='**'"));

								// Get FTP File Size
								if (LE910C1_EUX::FTPFSIZE(_File_Name, FOTA.Server_File_Size)) {

									// Print Command State
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(true);

									// Declare Download Size Char Buffer
									char _Server_File_Size_Buffer[8];
									memset(_Server_File_Size_Buffer, '\0', sizeof(_Server_File_Size_Buffer));

									// Set Buffer
									sprintf(_Server_File_Size_Buffer, "%07lu", FOTA.Server_File_Size);

									// Print Download Size
									Terminal->Text(15, 112, _Console_CYAN_, _Server_File_Size_Buffer);

								} else {

									// Print Command State
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(false);

									// Set Status
									FOTA.Download_Status = FOTA_FTP_File_Not_Found;

								}

							} 

							// Get File
							if (FOTA.Download_Status == FOTA_Download_OK) {

								// Print Command
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#FTPGETPKT='**',0"));

								// Get FTP File
								if (LE910C1_EUX::FTPGETPKT(_File_Name, 0)) {

									// Print Command State
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(true);

								} else {

									// Print Command State
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(false);

									// Set Status
									FOTA.Download_Status = FOTA_FTP_Connect_Error;

								}

							} 

							// Command Delay
							delay(100);

							// Recieve File
							if (FOTA.Download_Status == FOTA_Download_OK) {

								// Download File
								while (FOTA.Download_Size < FOTA.Server_File_Size) {

									// Declare Buffer
									char _FOTA_File_Buffer[765];
									memset(_FOTA_File_Buffer, '\0', sizeof(_FOTA_File_Buffer));

									// Declare Data Buffer
									char _FOTA_Data_Buffer[700];
									memset(_FOTA_Data_Buffer, '\0', sizeof(_FOTA_Data_Buffer));

									// Calculate Download Time
									FOTA.Download_Time = (millis() - FOTA.Download_Start_Time) / 1000;

									// Recieve File
									if (LE910C1_EUX::FTPRECV(700, _FOTA_File_Buffer)) {

										// Get Pack Size
										if (_FOTA_File_Buffer[0] == '\r' && _FOTA_File_Buffer[1] == '\n' && _FOTA_File_Buffer[15] == '\r' && _FOTA_File_Buffer[16] == '\n') {

											// Calculate Pack Size
											uint16_t _Data_Size = ((_FOTA_File_Buffer[12] - 48) * 100) + ((_FOTA_File_Buffer[13] - 48) * 10) + (_FOTA_File_Buffer[14] - 48);

											// Copy Data
											memcpy(_FOTA_Data_Buffer, _FOTA_File_Buffer + 17, _Data_Size);

											// Calculate Download Size
											FOTA.Download_Size += _Data_Size;

										} else if (_FOTA_File_Buffer[0] == '\r' && _FOTA_File_Buffer[1] == '\n' && _FOTA_File_Buffer[14] == '\r' && _FOTA_File_Buffer[15] == '\n') {

											// Calculate Pack Size
											uint16_t _Data_Size = ((_FOTA_File_Buffer[12] - 48) * 10) + (_FOTA_File_Buffer[13] - 48);

											// Copy Data
											memcpy(_FOTA_Data_Buffer, _FOTA_File_Buffer + 16, _Data_Size);

											// Calculate Download Size
											FOTA.Download_Size += _Data_Size;

										} else if (_FOTA_File_Buffer[0] == '\r' && _FOTA_File_Buffer[1] == '\n' && _FOTA_File_Buffer[13] == '\r' && _FOTA_File_Buffer[14] == '\n') {

											// Calculate Pack Size
											uint16_t _Data_Size = (_FOTA_File_Buffer[12] - 48);

											// Copy Data
											memcpy(_FOTA_Data_Buffer, _FOTA_File_Buffer + 15, _Data_Size);

											// Calculate Download Size
											FOTA.Download_Size += _Data_Size;

										}

										// Write Data
										FOTA_File.write(_FOTA_Data_Buffer, Length(_FOTA_Data_Buffer));

										// Print Message
										if (bitRead(this->Status, PostMan_Status_Terminal)) {

											// Declare Download Size Char Buffer
											char _Download_Size_Buffer[8];
											memset(_Download_Size_Buffer, '\0', sizeof(_Download_Size_Buffer));

											// Set Buffer
											sprintf(_Download_Size_Buffer, "%07lu", FOTA.Download_Size);

											// Print Download Size
											Terminal->Text(16, 112, _Console_CYAN_, _Download_Size_Buffer);

											// Declare Percent Char Buffer
											char _Download_Percent_Buffer[8];
											memset(_Download_Percent_Buffer, '\0', sizeof(_Download_Percent_Buffer));

											// Calculate Percent
											uint8_t _Percent = (uint8_t)((FOTA.Download_Size * 100) / FOTA.Server_File_Size);

											// Set Buffer for Percent (2 Digit Float)
											sprintf(_Download_Percent_Buffer, "%hu", _Percent);

											// Print Download Percent
											Terminal->Text(17, 114, _Console_CYAN_, _Download_Percent_Buffer);

											// Declare Percent Char Buffer
											char _Download_Time_Buffer[10];
											memset(_Download_Time_Buffer, '\0', sizeof(_Download_Time_Buffer));

											// Set Buffer
											sprintf(_Download_Time_Buffer, "%04u", FOTA.Download_Time);

											// Print Download Time
											Terminal->Text(18, 111, _Console_CYAN_, String(_Download_Time_Buffer));

										}

									} else {

										// Set Status
										FOTA.Download_Status = FOTA_Server_Error;

										// End While
										break;

									}

									// Control for TimeOut
									if (FOTA.Download_Time > 600) {

										// Set Status
										FOTA.Download_Status = FOTA_Download_TimeOut;

										// End While
										break;

									}

									// Heart Beat
									Hardware->Heartbeat();

								}

							}

							// Print Command
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#FTPCLOSE"));

							// Close FTP
							if (LE910C1_EUX::FTPCLOSE()) {

								// Print Command State
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(true);

							} else {

								// Print Command State
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(false);

							}								

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Controling File Size..."));

							// Close SD File
							FOTA_File.close();

							// Control for File Size
							if (FOTA.Download_Status == FOTA_Download_OK && FOTA.Download_Size == FOTA.Server_File_Size) {

								// Print Message
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Firmware File Size OK..."));

							} else {

								// Print Message
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Firmware File Size Error..."));

								// Set Status
								FOTA.Download_Status = FOTA_FTP_File_Size_Error;

							}

							// Clear MD5 Hash
							memset(this->MD5_Hash, '\0', sizeof(this->MD5_Hash));

							// Calculate MD5 Hash
							this->Calculate_MD5(MD5_Hash);

							// Add Variable
							this->Payload->Add(9003, FOTA.Download_Time);

						} else {

							// Set Status
							FOTA.Download_Status = FOTA_SD_Error;

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("Firmware File Open Failed!!"));

							// Disable SD Multiplexer
							Hardware->SD_Multiplexer(false);

							// Add Variable
							this->Payload->Add(9002, FOTA.Download_Status);

							// End Function
							return(false);

						}

						// Clear LED Blue
						PORT_MCU_LED_BLUE &= ~(1 << PIN_MCU_LED_BLUE);

						// Close SD File
						FOTA_File.close();

						// Add Variable
						this->Payload->Add(9002, FOTA.Download_Status);

					}

					// Disable SD Multiplexer
					Hardware->SD_Multiplexer(false);

					// End Function
					return (FOTA.Download_Status == FOTA_Download_OK);

				}

				// Disable SD Multiplexer
				Hardware->SD_Multiplexer(false);

				// End Function
				return(false);

			}

			// PCMSK1 Mask Handler Function
			void Interrupt_Handler(void) {

				// Set Interrupt Handler
				bitSet(this->Status, PostMan_Status_Ring);

			}

		// Public Context
		public:

			// Define Time Structure
			struct Struct_Time {
				uint8_t 	Year				= 0;
				uint8_t 	Month				= 0;
				uint8_t 	Day					= 0;
				uint8_t 	Hour				= 0;
				uint8_t 	Minute				= 0;
				uint8_t 	Second				= 0;
				uint8_t		Time_Zone			= 0;
				char		TimeStamp[20];
			} Time;

			// Define Pack Type Variable
			uint8_t Pack_Type = Pack_None;

			// PCMSK1 Mask Handler Function
			static void Interrupt_Handler_Static(void) {

				// Set Interrupt Handler
				if (Instance) Instance->Interrupt_Handler();

			}

			// JSON Parser Handler Function
			static void JSON_Parser_Static(uint8_t _Pack_Type) {

				// Set Interrupt Handler
				if (Instance) Instance->JSON_Parser(_Pack_Type);

			}

			// PostMan Constructor
			Postman(Stream &_Serial, B107AA* _B107AA, Variable<float>* _Variable, PowerStat_Console* _Terminal) : LE910C1_EUX(_Serial), Hardware(_B107AA), Payload(_Variable), Terminal(_Terminal) {

				// Control for Terminal
				#ifdef _DEBUG_

					// Control for Terminal
					if (Terminal != nullptr) {bitSet(this->Status, PostMan_Status_Terminal);} else {bitClear(this->Status, PostMan_Status_Terminal);}

				#else

					// Clear Terminal Status
					bitClear(this->Status, PostMan_Status_Terminal);

				#endif

				// Set Instance
				Instance = this;

				// Initialize Variables
				this->Clear_Variables();

			}
			Postman(Stream &_Serial, B107AA* _B107AA, Variable<float>* _Variable) : LE910C1_EUX(_Serial), Hardware(_B107AA), Payload(_Variable), Terminal(nullptr) {

				// Control for Terminal
				#ifdef _DEBUG_

					// Control for Terminal
					if (Terminal != nullptr) {bitSet(this->Status, PostMan_Status_Terminal);} else {bitClear(this->Status, PostMan_Status_Terminal);}

				#else

					// Clear Terminal Status
					bitClear(this->Status, PostMan_Status_Terminal);

				#endif

				// Set Instance
				Instance = this;

				// Initialize Variables
				this->Clear_Variables();

			}

			// Modem Power Functions
			bool Modem(const uint8_t _State = _Modem_STATUS_) {

				// Get Modem Status
				if (_State == _Modem_STATUS_) {

					// Return Status
					return(Hardware->PowerMonitor());

				} else if (_State == _Modem_ON_) {

					// Power ON GSM Module
					Hardware->ON();

				} else if (_State == _Modem_OFF_) {

					// Power OFF GSM Module
					Hardware->OFF();

				}

				// End Function
				return(false);

			}

			// Cloud Connection Function
			bool Connect(void) {

				// Heart Beat
				Hardware->Heartbeat();

				// Set Connection Start Time
				this->Operator.Connection_Start = millis();

				// Print Message
				if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Initializing GSM Module..."));

				// Initialize GSM Module
				if (this->Initialize()) {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Connecting GSM Module..."));

					// Connect GSM Module
					if (this->Connection()) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Connection Success!!"));

						// Set Firewall Configuration
						if (this->Firewall()) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Firewall Configuration Success!!"));

						}

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Syncronizing Time..."));

						// Update Time
						if (this->Syncronize_Time()) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Time Syncronized!!"));

						}

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Socket Listen..."));

						// Set Listen Configuration
						if (this->Listen(true)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Socket Listen Success!!"));

						}

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Text(21, 84, _Console_WHITE_, F("                                    "));

						// Disable Interrupts
						cli();

						// Enable GSM Interrupt
						Hardware->Set_GSM_Interrupt();

						// Enable Interrupts
						sei();

						// Publish On Status
						this->Pack_Type = Pack_Online;

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("PostMan Ready!!"));

						// End Function
						return(true);

					} else {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("GSM Connection Failed!!"));

					}

				} else {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("GSM Initializing Failed!!"));

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// Cloud Disconnection Function
			void Disconnect(void) {

				// Print Message
				if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, F("Disconnecting GSM Module..."));

				// Disconnect GSM Module
				LE910C1_EUX::SHDN();

			}

			// Configure Socket
			bool Listen(const bool _State) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Reset Socket Variable
					this->Socket_Status = _SOCKET_CLOSED_;

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SS=2"));

					// Send Command
					bool _Socket_State = LE910C1_EUX::SS(_PostMan_CID_, this->Socket_Status);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_Socket_State);

					// Activate Socket
					if (_State and this->Socket_Status != _SOCKET_LISTENING_) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SL=*,1,80,255"));

						// Activate Socket for Listen
						bool _SL_Command = LE910C1_EUX::SL(_PostMan_CID_, 1, 80, 255);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_SL_Command);

						// Command Delay
						delay(50);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SS=*"));

						// Get Socket Status
						bool _SS_Command = LE910C1_EUX::SS(_PostMan_CID_, this->Socket_Status);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_SS_Command);

						// Print Socket State
						if (this->Socket_Status == _SOCKET_CLOSED_) Terminal->Text(18, 64, _Console_RED_, F("    Closed     "));
						else if (this->Socket_Status == _SOCKET_ACTIVE_) Terminal->Text(18, 64, _Console_GRAY_, F("Active Transfer"));
						else if (this->Socket_Status == _SOCKET_SUSPENDED_) Terminal->Text(18, 64, _Console_GRAY_, F("   Suspended   "));
						else if (this->Socket_Status == _SOCKET_PENDING_) Terminal->Text(18, 64, _Console_GRAY_, F("  Pending Data "));
						else if (this->Socket_Status == _SOCKET_LISTENING_) Terminal->Text(18, 64, _Console_GRAY_, F("   Listening   "));

						// Clear Terminal Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Space(21, 84, 36);

						// End Function
						return (this->Socket_Status == _SOCKET_LISTENING_);

					}

					// DeActivate Socket
					if (!_State and this->Socket_Status != _SOCKET_CLOSED_) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SL=*,0,80,255"));

						// Activate Socket for Listen
						bool _SL_Command = LE910C1_EUX::SL(_PostMan_CID_, 0, 80, 255);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_SL_Command);

						// Command Delay
						delay(50);

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#SS=*"));

						// Get Socket Status
						bool _SS_Command = LE910C1_EUX::SS(_PostMan_CID_, this->Socket_Status);

						// Control for Terminal State
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_SS_Command);

						// Print Socket State
						if (this->Socket_Status == _SOCKET_CLOSED_) Terminal->Text(18, 64, _Console_RED_, F("    Closed     "));
						else if (this->Socket_Status == _SOCKET_ACTIVE_) Terminal->Text(18, 64, _Console_GRAY_, F("Active Transfer"));
						else if (this->Socket_Status == _SOCKET_SUSPENDED_) Terminal->Text(18, 64, _Console_GRAY_, F("   Suspended   "));
						else if (this->Socket_Status == _SOCKET_PENDING_) Terminal->Text(18, 64, _Console_GRAY_, F("  Pending Data "));
						else if (this->Socket_Status == _SOCKET_LISTENING_) Terminal->Text(18, 64, _Console_GRAY_, F("   Listening   "));

						// Clear Terminal Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Space(21, 84, 36);

						// End Function
						return (this->Socket_Status == _SOCKET_CLOSED_);

					}

				}

				// End Function
				return(false);

			}

			// Set Firewall Configuration
			bool Firewall(void) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Declare Variable
					bool _Firewall = false;

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT#FRWL=2"));

					// FRWL Clear (Firewall Configuration)
					_Firewall = LE910C1_EUX::FRWL(_AT_SET_, 2, _PostMan_Firewall_1_);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// AT Command
						Terminal->AT_Command(F("AT#FRWL=1,***"));

					}

					// FRWL Command 1 (Firewall Configuration)
					_Firewall = LE910C1_EUX::FRWL(_AT_SET_, 1, _PostMan_Firewall_1_);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// AT Command
						Terminal->AT_Command(F("AT#FRWL=1,***"));

					}

					// FRWL Command 2 (Firewall Configuration)
					_Firewall = LE910C1_EUX::FRWL(_AT_SET_, 1, _PostMan_Firewall_2_);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// AT Command
						Terminal->AT_Command(F("AT#FRWL=1,***"));

					}

					// FRWL Command 3 (Firewall Configuration)
					_Firewall = LE910C1_EUX::FRWL(_AT_SET_, 1, _PostMan_Firewall_3_);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// AT Command
						Terminal->AT_Command(F("AT#FRWL=1,***"));

					}

					// ICMP Command (Ping Configuration)
					_Firewall = LE910C1_EUX::ICMP(1);

					// Calculate Connection Time
					this->Operator.Connection_Time = (float)(millis() - this->Operator.Connection_Start) / 1000;

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) {

						// Print Message
						Terminal->OK(_Firewall);

						// Print Connection Time
						Terminal->Text(13,74, _Console_GRAY_, this->Operator.Connection_Time);

						// Clear Terminal Message
						Terminal->Space(21, 84, 36);

					}

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Set Clock Configuration
			bool Syncronize_Time(void) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->AT_Command(F("AT+CCLK"));

					// CCLK Command (Real Time Clock Configuration)
					bool _Clock_State = LE910C1_EUX::CCLK(this->Time.Year, this->Time.Month, this->Time.Day, this->Time.Hour, this->Time.Minute, this->Time.Second, this->Time.Time_Zone);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->OK(_Clock_State);

					// Control for Terminal State
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Syncronizing Time..."));

					// Clear Terminal Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Space(21, 84, 36);

					// End Function
					return(true);

				}

				// End Function
				return(false);

			}

			// Get Server Command Function
			uint16_t Get(void) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Control for RING
					if (bitRead(this->Status, PostMan_Status_Ring)) {

						// Print Message
						if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Beep();

						// Declare Event Variable
						uint16_t _Event = Command_None;

						// Clear Ring Status
						bitClear(this->Status, PostMan_Status_Ring);

						// Wait for SRING
						if (LE910C1_EUX::SRING()) {

							// Declare JSON Variable
							char _Buffer[_PostMan_Recieve_JSON_Size_];

							// Clear JSON Data
							memset(_Buffer, '\0', _PostMan_Recieve_JSON_Size_);

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Recieving Message..."));

							// Declare Request Pack Length
							uint16_t _Length = 0;

							// Answer Socket
							LE910C1_EUX::SA(_PostMan_CID_, 1, _Length);

							// Handle Length
							if (_Length == 0 or _Length > _PostMan_Recieve_JSON_Size_) _Length = _PostMan_Recieve_JSON_Size_;

							// Get Request Data
							LE910C1_EUX::SRECV(_PostMan_CID_, _Length, _Buffer);

								// Declare JSON Object
								JSON _JSON_Data(_Buffer);

								// Control for JSON Data
								if (_JSON_Data.isValid()) {

									// Print Message
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_CYAN_, _JSON_Data.Buffer());

									// Get Event
									_Event = _JSON_Data[F("Event")];

									// Control for Event Type
									if (_Event == Command_FOTA_Download_GET || _Event == Command_FOTA_Download_FTP) {

										// {"Response":200}

										// Declare Response Code
										uint16_t _Response_Code = _HTTP_OK_;

										// Declare Buffer Array
										char _Response_Buffer[17];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Get Firmware ID
										uint16_t _FW_ID = _JSON_Data[F("FW_ID")];

										// Decide Response Code
										if (_FW_ID != 0) _Response_Code = _HTTP_OK_; else _Response_Code = _HTTP_BadRequest_;

										// Generate Response
										sprintf(_Response_Buffer, "{\"Response\":%03u}", _Response_Code);

										// Send Response
										this->Response(_Response_Buffer);

										// Stop Socket Listen
										this->Listen(false);

										// Download Firmware
										if (_Event == Command_FOTA_Download_GET) {

											// Set Socket Configuration for Download
											LE910C1_EUX::SCFGEXT(_PostMan_CID_, 0, 0, 0, 0, 0);

											// Download File
											this->Download(_FOTA_GET_, _FW_ID);

											// Set Socket Configuration for Stream
											LE910C1_EUX::SCFGEXT(_PostMan_CID_, 1, 0, 0, 0, 0);

										} else if (_Event == Command_FOTA_Download_FTP) {

											// Download File
											this->Download(_FOTA_FTP_, _FW_ID);
										
										} 

										// Listen Incoming Socket
										this->Listen(true);

									} else if (_Event == Command_FOTA_Burn) {

										// {"Response":200}

										// Declare Buffer Array
										char _Response_Buffer[17];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Declare Response Code
										uint16_t _Response_Code = _HTTP_OK_;

										// Generate Response
										sprintf(_Response_Buffer, "{\"Response\":%03u}", _Response_Code);

										// Send Response
										this->Response(_Response_Buffer);

										// Print Message
										if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("Starting Firmware Burn..."));

										// Burn Firmware
										Hardware->Burn_Firmware();

									} else if (_Event == Command_Start) {

										// {"Response":200}

										// Declare Buffer Array
										char _Response_Buffer[17];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Declare Response Code
										uint16_t _Response_Code = _HTTP_OK_;

										// Generate Response
										sprintf(_Response_Buffer, "{\"Response\":%03u}", _Response_Code);

										// Send Response
										this->Response(_Response_Buffer);

										// Start Relay
										Hardware->Relay(RELAY_START, RELAY_UNLOCK, 500);

									} else if (_Event == Command_Stop) {

										// {"Response":200}

										// Declare Buffer Array
										char _Response_Buffer[17];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Declare Response Code
										uint16_t _Response_Code = _HTTP_OK_;

										// Generate Response
										sprintf(_Response_Buffer, "{\"Response\":%03u}", _Response_Code);

										// Send Response
										this->Response(_Response_Buffer);

										// Stop Relay
										Hardware->Relay(RELAY_STOP, RELAY_UNLOCK, 500);

									} else if (_Event == Command_Environment) {

										// {"Response":200,"Environment":{"T":10.00,"H":22.22}}

										// Declare Buffer Array
										char _Response_Buffer[60];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Set Variables
										//Payload->Add(_Data_PCB_T_, Hardware->Temperature());
										//Payload->Add(_Data_PCB_H_, Hardware->Humidity());

										// Generate Temperature Buffer
										char _Response_Buffer_T[6];
										memset(_Response_Buffer_T, '\0', sizeof(_Response_Buffer_T));
										Payload->Get(_Data_PCB_T_, _Response_Buffer_T);

										// Generate Humidity Buffer
										char _Response_Buffer_H[6];
										memset(_Response_Buffer_H, '\0', sizeof(_Response_Buffer_H));
										Payload->Get(_Data_PCB_H_, _Response_Buffer_H);

										// Declare Response Code
										uint16_t _Response_Code = _HTTP_OK_;

										// Generate Response
										sprintf(_Response_Buffer, "{\"Response\":%03u,\"Environment\":{\"T\":%s,\"H\":%s}}", _Response_Code, _Response_Buffer_T, _Response_Buffer_H);

										// Send Response
										this->Response(_Response_Buffer);

									} else if (_Event == Command_Power) {

										// Declare Power Buffer
										char _Response_Buffer[100];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Parse Power
										this->JSON_Power_Segment(_Response_Buffer, Pack_Update);

										// Send Response
										this->Response(_Response_Buffer);

									} else if (_Event == Command_GSM) {

										// Update Connection Status
										this->Update_Connection_Status();

										// Declare IoT Buffer
										char _Response_Buffer[150];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Parse IoT
										this->JSON_IoT_Segment(_Response_Buffer, Pack_Update);

										// Send Response
										this->Response(_Response_Buffer);

									} else if (_Event == Command_Energy) {

										// Declare IoT Buffer
										char _Response_Buffer[300];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Parse IoT
										this->JSON_Energy_Segment(_Response_Buffer);

										// Send Response
										this->Response(_Response_Buffer);

									} else if (_Event == Command_Version) {

										// {"Response":200,"Version":"01.00.00"}

										// Declare Buffer Array
										char _Response_Buffer[38];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Generate Response
										sprintf(_Response_Buffer, "{\"Response\":200,\"Version\":\"%s\"}", _FIRMWARE_);

										// Send Response
										this->Response(_Response_Buffer);

									} else if (_Event == Command_Ping) {

										// {"Response":200,"Pong":100}

										// Declare Buffer Array
										char _Response_Buffer[35];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Declare Response Code
										uint16_t _Response_Code = _HTTP_OK_;

										// Declare Ping Time
										uint16_t _Ping_Time = 0;
										
										// Get Ping Value
										LE910C1_EUX::Ping("ditsa.cn", _Ping_Time);

										// Generate Response
										sprintf(_Response_Buffer, "{\"Response\":%u,\"Pong\":%u}", _Response_Code, _Ping_Time * 100);

										// Send Response
										this->Response(_Response_Buffer);

									} else {

										// {"Response":200}

										// Declare Buffer Array
										char _Response_Buffer[17];
										memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

										// Declare Response Code
										uint16_t _Response_Code = _HTTP_NotAcceptable_;

										// Generate Response
										sprintf(_Response_Buffer, "{\"Response\":%03u}", _Response_Code);

										// Send Response
										this->Response(_Response_Buffer);

									}

								} else {

									// {"Response":200}

									// Declare Buffer Array
									char _Response_Buffer[17];
									memset(_Response_Buffer, '\0', sizeof(_Response_Buffer));

									// Declare Response Code
									uint16_t _Response_Code = _HTTP_BadRequest_;

									// Generate Response
									sprintf(_Response_Buffer, "{\"Response\":%03u}", _Response_Code);

									// Send Response
									this->Response(_Response_Buffer);

								}

							// Port Control
							this->Listen(true);

						}

						// End Function
						return(_Event);

					}

				}

				// End Function
				return(Command_None);

			}

			// Send Data Batch Function
			bool Publish(const uint8_t _Pack_Type = Pack_Update) {

				// Control for Modem Connection
				if (bitRead(this->Status, PostMan_Status_Connection)) {

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Beep();

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Closing Listen Socket..."));

					// Stop Socket Listen
					this->Listen(false);

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Updating Connection Status..."));

					// Update Connection Status
					this->Update_Connection_Status();

					// Print Message
					if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Connecting to Server..."));

					// Open Connection
					if (LE910C1_EUX::ATSD(_PostMan_CID_, _AT_TCP_, _PostMan_Server_, _PostMan_Port_, _CONNECTION_MANUAL_CLOSE_, 88, _CONNECTION_COMMAND_)) {

						// Sending Data
						if (LE910C1_EUX::SSEND(this->JSON_Parser_Static, _Pack_Type)) {

							// Print Message
							if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Waiting for Response..."));

							// Declare Ring Length
							uint16_t _Length;

							// Get Ring Port
							if (LE910C1_EUX::SRING(_Length)) {

								// Control for Length
								if (_Length > _PostMan_Response_JSON_Size_) _Length = _PostMan_Response_JSON_Size_;

								// Declare Response Variable
								char _Response[_PostMan_Response_JSON_Size_];

								// Clear Response
								memset(_Response, '\0', _PostMan_Response_JSON_Size_);

								// Command Delay
								delay(10);

								// Print Message
								if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Getting Response..."));

								// Handle Length
								if (_Length > _PostMan_Response_JSON_Size_) _Length = _PostMan_Response_JSON_Size_;

								// Get Request Data
								if (LE910C1_EUX::SRECV(_PostMan_CID_, _Length, _Response)) {

									// Command Delay
									delay(50);

									// Print Message
									if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_BLUE_, F("Closing Connection..."));

									// Closing Socket
									if (LE910C1_EUX::SH(_PostMan_CID_)) {

										// Control for Incoming Call
										this->Listen(true);

										// Declare JSON Object
										JSON _JSON_Data(_Response);

										// Get Event
										uint16_t _Event = _JSON_Data[F("Event")];

										// Control for Response
										if (_Event == Command_OK) {

											// Print Message
											if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_GREEN_, F("Message Sent Success!!"));

											// Clear Payload
											Payload->Clear();

											// Clear Pack Type
											this->Pack_Type = Pack_None;

											// End Function
											return(true);

										} else {

											// Print Message
											if (bitRead(this->Status, PostMan_Status_Terminal)) Terminal->Show_Message(_Console_RED_, F("Message Sent Failed!!"));

											// End Function
											return(false);

										}

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

	};

	// Set Static Instance
	Postman* Postman::Instance = nullptr;

	// Interrupt Routine PCMSK1 [PCINT11]
	ISR(PCINT1_vect) {

		// PCMSK1 Handler
		Postman::Interrupt_Handler_Static();

	}

#endif /* defined(__PostMan__) */