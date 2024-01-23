// Library include guard
#pragma once

// Include Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Include Definitions
#include "AT_Definitions.h"
#include "Definitions/Modem.h"
#include "Definitions/SIM.h"
#include "Definitions/Response.h"

// Define Response Max Length
#define Max_AT_OK_Response_Length 20

// Modem AT Command Set Class
class AT_Command_Set_LE910S1_EAG {

	// Private Context
	private:

		// GSM Serial Stream Definition
		Stream * GSM_Serial;

		// Serial Buffer Variable Structure Definition
		struct Serial_Buffer {
			bool 				Response;
			uint8_t 			Read_Order;
			uint8_t 			Data_Order;
			const uint32_t 		Time_Out;
			uint8_t 			Size;
		};

		// Clear Serial Buffer Function
		void Clear_UART_Buffer(void) {

			// Clear UART Buffer
			while (GSM_Serial->available() > 0) {
				
				// Read GSM Buffer
				GSM_Serial->read();

				// Read Delay
				delay(1);

			}

		}

		// Read Serial Buffer Function
		uint8_t Read_UART_Buffer(Serial_Buffer * _Buffer, char * _Buffer_Variable) {

			// Response Wait Delay
			delay(2);

			// Read Current Time
			const uint32_t Current_Time = millis();

			// Read UART Response
			while (!_Buffer->Response) {

				// Read Serial Char
				_Buffer_Variable[_Buffer->Read_Order] = GSM_Serial->read();

				// Control for Response
				if (this->Find(_AT_OK_, _Buffer_Variable, _Buffer->Read_Order)) {

					// End Function
					return(_AT_OK_);

				} else if (this->Find(_AT_ERROR_, _Buffer_Variable, _Buffer->Read_Order)) {

					// End Function
					return(_AT_ERROR_);

				} else if (this->Find(_AT_CME_, _Buffer_Variable, _Buffer->Read_Order)) {

					// End Function
					return(_AT_CME_);

				}

				// Increase Read Order
				if (isAscii(_Buffer_Variable[_Buffer->Read_Order])) _Buffer->Read_Order++;

				// Handle for Memory Overflow
				if (_Buffer->Read_Order >= _Buffer->Size) break;

				// Handle for timeout
				if (millis() - Current_Time >= _Buffer->Time_Out) break;

			}

			// End Function
			return(_AT_TIMEOUT_);

		}

		// OK Find Function
		bool Find(const uint8_t _Type, char * _Buffer, uint8_t _Size) {

			// Select Find Type
			if (_Type == _AT_OK_) {

				// \r\nOK\r\n

				// Control for <\r\nOK\r\n> Response
				if (_Buffer[_Size - 5] == 13 and _Buffer[_Size - 4] == 10 and _Buffer[_Size - 3] == 79 and _Buffer[_Size - 2] == 75 and _Buffer[_Size - 1] == 13 and _Buffer[_Size - 0] == 10) return(true);

			} else if (_Type == _AT_ERROR_) {

				// \r\nERROR\r\n

				// Control for <\r\nERROR\r\n> Response
				if (_Buffer[_Size - 8] == 13 and _Buffer[_Size - 7] == 10 and _Buffer[_Size - 6] == 69 and _Buffer[_Size - 5] == 82 and _Buffer[_Size - 4] == 82 and _Buffer[_Size - 3] == 79 and _Buffer[_Size - 2] == 82 and _Buffer[_Size - 1] == 13 and _Buffer[_Size - 0] == 10) return(true);

			} else if (_Type == _AT_CME_) {

				// \r\n+CME ERROR: 614\r\n
				// \r\n+CME ERROR: 4\r\n

				// Find <\r\n+CME ERROR:> Response
				if (_Buffer[_Size - 16] == '\r' and _Buffer[_Size - 15] == '\n' and _Buffer[_Size - 14] == '+' and _Buffer[_Size - 13] == 'C' and _Buffer[_Size - 12] == 'M' and _Buffer[_Size - 11] == 'E' and _Buffer[_Size - 10] == ' ' and _Buffer[_Size - 9] == 'E' and _Buffer[_Size - 8] == 'R' and _Buffer[_Size - 7] == 'R' and _Buffer[_Size - 6] == 'O' and _Buffer[_Size - 5] == 'R' and _Buffer[_Size - 4] == ':' and _Buffer[_Size - 1] == '\r' and _Buffer[_Size - 0] == '\n') return(true);
				else if (_Buffer[_Size - 17] == '\r' and _Buffer[_Size - 16] == '\n' and _Buffer[_Size - 15] == '+' and _Buffer[_Size - 14] == 'C' and _Buffer[_Size - 13] == 'M' and _Buffer[_Size - 12] == 'E' and _Buffer[_Size - 11] == ' ' and _Buffer[_Size - 10] == 'E' and _Buffer[_Size - 9] == 'R' and _Buffer[_Size - 8] == 'R' and _Buffer[_Size - 7] == 'O' and _Buffer[_Size - 6] == 'R' and _Buffer[_Size - 5] == ':' and _Buffer[_Size - 1] == '\r' and _Buffer[_Size - 0] == '\n') return(true);
				else if (_Buffer[_Size - 18] == '\r' and _Buffer[_Size - 17] == '\n' and _Buffer[_Size - 16] == '+' and _Buffer[_Size - 15] == 'C' and _Buffer[_Size - 14] == 'M' and _Buffer[_Size - 13] == 'E' and _Buffer[_Size - 12] == ' ' and _Buffer[_Size - 11] == 'E' and _Buffer[_Size - 10] == 'R' and _Buffer[_Size - 9] == 'R' and _Buffer[_Size - 8] == 'O' and _Buffer[_Size - 7] == 'R' and _Buffer[_Size - 6] == ':' and _Buffer[_Size - 1] == '\r' and _Buffer[_Size - 0] == '\n') return(true);

			}

			// End Function
			return(false);

		}

		// RSSI to Signal Quality Function
		uint8_t RSSI_to_Signal_Quality(int8_t _RSSI) {

			// Calculate Signal Level
			if (_RSSI >= -65) return(5);
			if (_RSSI >= -75 and _RSSI < -65) return(4);
			if (_RSSI >= -85 and _RSSI < -75) return(3);
			if (_RSSI >= -95 and _RSSI < -85) return(2);
			if (_RSSI < -95) return(1);

			// End Function
			return(0);

		}

		// Convert Coordinate
		double convertLatitude(double rawLatitude) {
			int degrees = (int)rawLatitude / 100;
			double minutes = (int)(rawLatitude - (degrees * 100));
			double seconds = (rawLatitude - (degrees * 100) - minutes) * 60;
			double latitude = degrees + (minutes / 60.0) + (seconds / 3600.0);

			return latitude;
		}
		double convertLongitude(double rawLongitude) {
			int degrees = (int)rawLongitude / 100;
			double minutes = rawLongitude - (degrees * 100);
			double longitude = degrees + (minutes / 60.0);

			return longitude;
		}

	// Public Context
	public:

		// PostOffice Constructor
		AT_Command_Set_LE910S1_EAG(Stream &_Serial) {

			// Set Serial Port
			GSM_Serial = & _Serial;

		}

		// AT Command
		bool AT(void) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Set Connection Mode Function
		bool FCLASS(const uint8_t _FCLASS) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+FCLASS="));
			GSM_Serial->print(_FCLASS);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Get Manufacturer Function
		bool CGMI(uint8_t & _Manufacturer) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				35		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CGMI"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// AT+CGMI\r\n
				// \r\nTelit\r\n\r\nOK\r\n

				// Declare Variable
				char _Manufacturer_Name[10];

				// Clear Variable
				memset(_Manufacturer_Name, '\0', 10);

				// Handle Manufacturer Name
				sscanf(Buffer_Variable, "\r\n%s\r\n\r\nOK\r\n", _Manufacturer_Name);

				// Set No Manufacturer
				_Manufacturer = _MODEM_MANUFACTURER_UNKNOWN_;

				// Control for Manufacturer Name
				if (strstr(_Manufacturer_Name, "Telit") != NULL) _Manufacturer = _MODEM_MANUFACTURER_TELIT_;

				// End Function
				if (_Manufacturer != _MODEM_MANUFACTURER_UNKNOWN_) return(true);

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// Get Model Function
		bool CGMM(uint8_t & _Model) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				35		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CGMM"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// AT+CGMM\r\n
				// \r\nLE910C1-EUX\r\n\r\nOK\r\n

				// Declare Variable
				char _Model_Name[15];

				// Clear Variable
				memset(_Model_Name, '\0', 15);

				// Handle Model Name
				sscanf(Buffer_Variable, "\r\n%s\r\n\r\nOK\r\n", _Model_Name);

				// Set No Model
				_Model = _MODEM_MODEL_UNKNOWN_;

				// Control for Model Name
				if (strstr(_Model_Name, "GE910-QUAD") != NULL) _Model = _MODEM_MODEL_GE910_QUAD_;
				else if (strstr(_Model_Name, "GE910-QUAD-V3") != NULL) _Model = _MODEM_MODEL_GE910_QUAD_V3_;
				else if (strstr(_Model_Name, "LE910S1-EA") != NULL) _Model = _MODEM_MODEL_LE910S1_EA_;
				else if (strstr(_Model_Name, "LE910S1-EAG") != NULL) _Model = _MODEM_MODEL_LE910S1_EAG_;
				else if (strstr(_Model_Name, "LE910R1-EU") != NULL) _Model = _MODEM_MODEL_LE910R1_EU_;
				else if (strstr(_Model_Name, "LE910C1-EUX") != NULL) _Model = _MODEM_MODEL_LE910C1_EUX_;

				// End Function
				if (_Model != _MODEM_MODEL_UNKNOWN_) return(true);

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// Get Firmware Function
		bool SWPKGV(char * _Firmware) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				70		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SWPKGV"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// AT#SWPKGV\r\n
				// \r\n25.30.226-P0F.225200\r\nM0F.223006\r\nP0F.225200\r\nA0F.223006\r\n\r\nOK\r\n

				// Clear Variable
				memset(_Firmware, '\0', 15);

				// Handle Firmware
				_Firmware[0] = Buffer_Variable[2];
				_Firmware[1] = Buffer_Variable[3];
				_Firmware[2] = Buffer_Variable[4];
				_Firmware[3] = Buffer_Variable[5];
				_Firmware[4] = Buffer_Variable[6];
				_Firmware[5] = Buffer_Variable[7];
				_Firmware[6] = Buffer_Variable[8];
				_Firmware[7] = Buffer_Variable[9];
				_Firmware[8] = Buffer_Variable[10];

				// End Function
				return(true);

			}

			// End Function
			return(false);

		}

		// Get IMEI Function
		bool CGSN(char * _IMEI) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				35		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CGSN=1"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// AT+CGSN=1\r\n
				// \r\n+CGSN: 354485417617003\r\n\r\nOK\r\n

				// Clear Buffer Variable
				memset(_IMEI, '\0', 17);

				// Handle for Response
				for (uint8_t i = 0; i < Buffer.Size; i++) {

					// Handle IMEI
					if (Buffer_Variable[i] > 47 and Buffer_Variable[i] < 58) {

						// Set IMEI Variable
						_IMEI[Buffer.Data_Order] = Buffer_Variable[i];

						// Set Data Order
						Buffer.Data_Order++;

					}

				}

				// End Function
				return(true);

			}

			// End Function
			return(false);

		}

		// Set Power Mode Function
		bool CFUN(const uint8_t _Fun) {

			// Control for Parameter
			if (_Fun != 0 and _Fun != 1 and _Fun != 4 and _Fun != 5 and _Fun != 12) return(false);

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CFUN="));
			GSM_Serial->print(_Fun);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Set CMEE Function
		bool CMEE(const uint8_t _CMEE) {

			// Control for Parameter
			if (_CMEE != 0 and _CMEE != 1 and _CMEE != 2) return(false);

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CMEE="));
			GSM_Serial->print(_CMEE);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Get Error Code Function
		bool CEER(uint16_t & _Code) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				25		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#CEER"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {
				
				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n#CEER: %03d\r\n\r\nOK\r\n", &_Code);

				// Control for Variable
				if (_Variable_Count == 1) return(true);

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// ATE Command
		bool ATE(const bool _ECHO) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				13		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("ATE"));
			GSM_Serial->print(_ECHO);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Flow Control Function
		bool K(const uint8_t _K) {

			// Control for Parameter
			if (_K != 0 and _K != 3) return(false);

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT&K"));
			GSM_Serial->print(_K);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Get SIM PIN Status Function
		bool CPIN(uint8_t & _Code) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				30000, 	// Time Out
				30		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CPIN?"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// \r\n+CPIN: READY\r\n\r\nOK\r\n

				// Declare Handle Variables
				char _PIN_Response[15];

				// Clear Handle Variables
				memset(_PIN_Response, '\0', 15);

				// Handle Variables
				sscanf(Buffer_Variable, "\r\n+CPIN: %s\r\n\r\nOK\r\n", _PIN_Response);

				// Declare Variable
				_Code = _SIM_UNKNOWN_;

				// Control for Manufacturer Name
				if (strstr(_PIN_Response, "READY")) _Code = _SIM_READY_;
				else if (strstr(_PIN_Response, "SIM PIN")) _Code = _SIM_PIN_;
				else if (strstr(_PIN_Response, "SIM PUK")) _Code = _SIM_PUK_;

				// Handle Status
				if (_Code != _SIM_UNKNOWN_) return(true);

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// Get ICCID Function
		bool CCID(char * _ICCID) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				37		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#CCID"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// Handle for Response
				for (size_t i = 0; i < Buffer.Size; i++) {

					// Handle IMEI
					if (Buffer_Variable[i] > 47 and Buffer_Variable[i] < 58) {

						// Set IMEI Variable
						_ICCID[Buffer.Data_Order] = Buffer_Variable[i];

						// Set Data Order
						Buffer.Data_Order++;

					}

				}

				// End Function
				return(true);

			}

			// End Function
			return(false);

		}

		// SIMDET Function
		bool SIMDET(const bool _Function_Type, uint8_t _Mode, bool & _SIM_in_Pin_State) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				23		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// SET Function
			if (_Function_Type == SET) {

				// Send UART Command
				GSM_Serial->print(F("AT#SIMDET="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Response
				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) return(true);

				// End Function
				return(false);

			} 
			
			// GET Function
			if (_Function_Type == GET) {

				// Send UART Command
				GSM_Serial->print(F("AT#SIMDET?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Response is 22 byte
				// \r\n#SIMDET: 2,1\r\n\r\nOK\r\n

				// Declare Response
				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) {

					// Define Handle Variable
					int _SIMDET_Mode = 0;
					int _SIMDET_State = 0;

					// Handle Query Answer
					uint8_t _Parsed_Variable = sscanf(Buffer_Variable, "\r\n#SIMDET: %d,%d\r\n\r\nOK\r\n", &_SIMDET_Mode, &_SIMDET_State);

					// Clear Variables
					_Mode = 0;
					_SIM_in_Pin_State = false;

					// Assign SIM State
					if (_SIMDET_State == 1) _SIM_in_Pin_State = true;

					// End Function
					if (_Parsed_Variable == 2) return(true);

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// Set GPIO Function
		bool GPIO(const bool _Function_Type, const uint8_t _Pin, const uint8_t _Mode = 0, const uint8_t _Direction = 0) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// SET Function
			if (_Function_Type == SET) {

				// Send UART Command
				GSM_Serial->print(F("AT#GPIO="));
				GSM_Serial->print(_Pin);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Mode);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Direction);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Response
				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) return(true);

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// Set SLED Function
		bool SLED(const uint8_t _SLED) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SLED="));
			GSM_Serial->print(_SLED);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Execution command saves STAT_LED setting in NVM.
		bool SLEDSAV(void) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SLEDSAV"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Set Socket Listen Ring Indicator Function
		bool E2SLRI(const uint16_t _Pulse_Duration) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer
			char Buffer_Variable[Buffer.Size];
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#E2SLRI="));
			GSM_Serial->print(_Pulse_Duration);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// CREG Function
		bool CREG(const bool _Function_Type, uint8_t & _Mode, uint8_t & _Stat) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				25		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// SET Function
			if (_Function_Type == SET) {

				// Send UART Command
				GSM_Serial->print(F("AT+CREG="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) return(true);

				// End Function
				return(false);

			}

			// GET Function
			if (_Function_Type == GET) {

				// Send UART Command
				GSM_Serial->print(F("AT+CREG?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) {
					
					// Handle Variables
					sscanf(Buffer_Variable, "\r\n+CREG: %hhu,%hhu\r\n\r\nOK\r\n", &_Mode, &_Stat);

					// Handle Response
					if (_Stat == 0) return(true);
					else if (_Stat == 1) return(true);
					else if (_Stat == 2) return(true);
					else if (_Stat == 3) return(true);
					else if (_Stat == 4) return(true);
					else if (_Stat == 5) return(true);
					else if (_Stat == 6) return(true);
					else if (_Stat == 7) return(true);
					else if (_Stat == 8) return(true);
					else if (_Stat == 9) return(true);
					else if (_Stat == 10) return(true);
					else if (_Stat == 11) return(true);

					// End Function
					return (false);

				}

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// CGREG Function
		bool CGREG(const bool _Function_Type, uint8_t & _Mode, uint8_t & _Stat) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				25		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// SET Function
			if (_Function_Type == SET) {

				// Send UART Command
				GSM_Serial->print(F("AT+CGREG="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) return(true);

				// End Function
				return(false);

			}

			// GET Function
			if (_Function_Type == GET) {

				// Send UART Command
				GSM_Serial->print(F("AT+CGREG?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) {
					
					// Handle Variables
					sscanf(Buffer_Variable, "\r\n+CGREG: %hhu,%hhu\r\n\r\nOK\r\n", &_Mode, &_Stat);

					// Handle Response
					if (_Stat == 0) return(true);
					else if (_Stat == 1) return(true);
					else if (_Stat == 2) return(true);
					else if (_Stat == 3) return(true);
					else if (_Stat == 4) return(true);
					else if (_Stat == 5) return(true);
					else if (_Stat == 6) return(true);
					else if (_Stat == 7) return(true);
					else if (_Stat == 8) return(true);
					else if (_Stat == 9) return(true);
					else if (_Stat == 10) return(true);
					else if (_Stat == 11) return(true);

					// End Function
					return (false);

				}

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// Set CGDCONT Function
		bool CGDCONT(const uint8_t _Cid, const char * _PDP_Type, const char * _APN) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CGDCONT="));
			GSM_Serial->print(_Cid);
			GSM_Serial->print(F(",\""));
			GSM_Serial->print(_PDP_Type);
			GSM_Serial->print(F("\",\""));
			GSM_Serial->print(_APN);
			GSM_Serial->print(F("\""));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Set SGACT Function
		bool SGACT(const uint8_t _Cid, const bool _Stat, char * _IP_Address) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				60000, 	// Time Out
				35		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SGACT="));
			GSM_Serial->print(_Cid);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Stat);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// \r\n#SGACT: 000.000.000.000\r\n\r\nOK\r\n

				// Declare IP Segments
				uint16_t	Segment_1 = 0;
				uint16_t	Segment_2 = 0;
				uint16_t	Segment_3 = 0;
				uint16_t	Segment_4 = 0;

				// Handle IP 
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n#SGACT: %d.%d.%d.%d\r\n\r\nOK\r\n", &Segment_1, &Segment_2, &Segment_3, &Segment_4);
	
				// Parse IP Address
				sprintf(_IP_Address, "%03d.%03d.%03d.%03d", Segment_1, Segment_2, Segment_3, Segment_4);

				// Control for IP
				if (_Variable_Count == 4) return (true);

				// End Function
				return (false);
				
			}

			// End Function
			return(false);

		}

		// WS46 Function
		bool WS46(const bool _Function_Type, uint16_t & _Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				20		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);
			
			// GET Function
			if (_Function_Type == GET) {

				// Send UART Command
				GSM_Serial->print(F("AT+WS46?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Response
				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) {

					// AT+WS46?\r\n
					// \r\n+WS46: 30\r\n\r\nOK\r\n

					// Handle Variables
					uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n+WS46: %02d\r\n\r\nOK\r\n", &_Mode);

					// Control for Variable
					if (_Variable_Count == 1) return(true);

					// End Function
					return(false);

				}

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// MONI Function
		bool MONI(uint16_t & _MCC, uint16_t & _MNC, uint16_t & _RSSI, uint8_t & _Signal_Level) {

			// Declare Variable Structure
			struct Operator_Structure {
				int		Temp_Numeric;
				char	Temp_String[50];
			} Handle_Buffer;

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				2000, 	// Time Out
				120		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);
			
			// Send UART Command
			GSM_Serial->print(F("AT#MONI"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// \r\n#MONI:TR TURKCELL RSRP:-110 RSRQ:15 TAC:2242 Id:B5D125 EARFCN:100 PWR:-95 DRX:128 pci:335 QRxlevMin:-124\r\n\r\nOK\r\n
				// \r\n#MONI:TR TURKCELL RSRP:-99 RSRQ:-16 TAC:2242 Id:859315 EARFCN:6400 PWR:-66 DRX:128 pci:438 QRxlevMin:-124\r\n\r\nOK\r\n

				// Declare Variables
				char _Country_Code[3];
				char _Operator_Code[10];

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n#MONI:%s %s RSRP:-%s RSRQ:%s TAC:%s Id:%s EARFCN:%s PWR:-%03d DRX:%s pci:%s QRxlevMin:-%s\r\n\r\nOK\r\n", _Country_Code, _Operator_Code, Handle_Buffer.Temp_String, Handle_Buffer.Temp_String, Handle_Buffer.Temp_String, Handle_Buffer.Temp_String, Handle_Buffer.Temp_String, &_RSSI, Handle_Buffer.Temp_String, Handle_Buffer.Temp_String, Handle_Buffer.Temp_String);

				// Handle for Variable
				if (strstr(_Country_Code, "TR") != NULL) _MCC = 286;
				if (strstr(_Operator_Code, "TURKCELL") != NULL) _MNC = 1;

				// Calculate Signal Level
				_Signal_Level = this->RSSI_to_Signal_Quality(_RSSI * -1);

				// Control for Variable
				if (_Variable_Count == 11) return(true);

				// End Function
				return(false);

			}

			// End Function
			return(false);

		}

		// Socket Configuration Function
		bool SCFG(const uint8_t _Conn_ID, const uint8_t _Cid, const uint16_t _Pkt_Sz, const uint16_t _Max_To, const uint16_t _Conn_To, const uint8_t _TX_To) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SCFG="));
			GSM_Serial->print(_Conn_ID);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Cid);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Pkt_Sz);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Max_To);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Conn_To);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_TX_To);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Extended Socket Configuration Function
		bool SCFGEXT(const uint8_t _Conn_ID, const uint8_t _Sr_Mode, const uint8_t _Recv_Data_Mode, const uint8_t _Keep_Alive, const uint8_t _Listen_Auto_Rsp, const uint8_t _Send_Data_Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SCFGEXT="));
			GSM_Serial->print(_Conn_ID);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Sr_Mode);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Recv_Data_Mode);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Keep_Alive);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Listen_Auto_Rsp);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Send_Data_Mode);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Extended 2 Socket Configuration Function
		bool SCFGEXT2(const uint8_t _Conn_ID, const uint8_t _Buffer_Start, const uint8_t _Abort_Conn_Attempt, const uint8_t _SRing_Len, const uint8_t _SRing_To, const uint8_t _No_Carrier_Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SCFGEXT2="));
			GSM_Serial->print(_Conn_ID);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Buffer_Start);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Abort_Conn_Attempt);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_SRing_Len);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_SRing_To);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_No_Carrier_Mode);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Firewall Add/Remove Function
		bool FRWL(const bool _Function_Type, const uint8_t _Action, const char *_IP_Addr) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// SET Function
			if (_Function_Type == SET) {

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 					// Response State
					0, 						// Read Order
					0, 						// Data Order
					1000, 					// Time Out
					Max_AT_OK_Response_Length		// Buffer Size
				};

				// Declare Buffer Variable
				char Buffer_Variable[Buffer.Size];

				// Clear Buffer Variable
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FRWL="));
				GSM_Serial->print(_Action);
				if (_Action != 2) GSM_Serial->print(F(",\""));
				if (_Action != 2) GSM_Serial->print(_IP_Addr);
				if (_Action != 2) GSM_Serial->print(F("\",\"255.255.255.255\""));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Response
				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) return(true);

				// End Function
				return(false);

			}

			// GET Function
			if (_Function_Type == GET) {

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					200		// Buffer Size
				};

				// Declare Buffer Variable
				char Buffer_Variable[Buffer.Size];

				// Clear Buffer Variable
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FRWL?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Response
				uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (_Response == _AT_OK_) return(true);

				// #define	_AT_FRWL_1_IP_			"213.014.250.214"
				// #define	_AT_FRWL_2_IP_			"167.099.137.254"
				// #define	_AT_FRWL_3_IP_			"045.133.037.013"

				// AT#FRWL?\r\n\r\n#FRWL: "213.12.250.214","255.255.255.0"\r\n\r\n#FRWL: "167.81.137.254","255.255.255.0"\r\n\r\n#FRWL: "37.133.31.11","255.255.255.0"\r\n\r\nOK\r\n

				// End Function
				return (true);

			}

			// End Function
			return (false);

		}

		// Ping Enable/Disable Function
		bool ICMP(const uint8_t _Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#ICMP="));
			GSM_Serial->print(_Mode);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Get Clock Function
		bool CCLK(uint16_t & _Year, uint16_t & _Month, uint16_t & _Day, uint16_t & _Hour, uint16_t & _Minute, uint16_t & _Second, uint16_t & _Time_Zone) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				40		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];
			
			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CCLK?"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				/*
				--> AT+CCLK?\r\n
				<-- \r\n+CCLK: "23/08/02,11:18:16+12"\r\n\r\nOK\r\n
				*/

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n+CCLK: \"%02d/%02d/%02d,%02d:%02d:%02d+%02d\"\r\n\r\nOK\r\n", &_Year, &_Month, &_Day, &_Hour, &_Minute, &_Second, &_Time_Zone);

				// Control for Variables
				if (_Year > 24 and _Year < 22) return(false);	
				if (_Month > 12 and _Month < 0) return(false);	
				if (_Day > 31 and _Day < 0) return(false);	
				if (_Hour > 24 and _Hour < 0) return(false);	
				if (_Minute > 59 and _Minute < 0) return(false);	
				if (_Second > 59 and _Second < 0) return(false);

				// Control for Variable
				if (_Variable_Count == 7) return(true);

				// End Function
				return (false);

			}

			// End Function
			return(false);

		}

		// Enable or Disable Automatic TimeZone Update Function
		bool CTZU(const bool _State) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT+CTZU="));
			GSM_Serial->print(String(_State));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Enable or Disable Network Time UnSolicited Function
		bool NITZ(const bool _State) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#NITZ="));
			GSM_Serial->print(_State);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Enable the local time or the UTC time.
		bool CCLKMODE(const bool _Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#CCLKMODE="));
			GSM_Serial->print(_Mode);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Socket Dial Function
		bool ATSD(const uint8_t _Cid, const uint8_t _Protocol, const uint8_t _Port, const uint8_t _Closure_Type, uint16_t _IPort, const bool _Conn_Mode, const char *_IP) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				140000, 				// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SD="));
			GSM_Serial->print(_Cid);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Protocol);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Port);
			GSM_Serial->print(F(",\""));
			GSM_Serial->print(_IP);
			GSM_Serial->print(F("\","));
			GSM_Serial->print(_Closure_Type);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_IPort);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Conn_Mode);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Close Socket Function
		bool SH(const uint8_t _ConnID) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				5000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SH="));
			GSM_Serial->print(_ConnID);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Socket Listen Function
		bool SL(const uint8_t _ConnID, const bool _Listen_State, const uint16_t _Listen_Port, const uint8_t _Closure_Type) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SL="));
			GSM_Serial->print(_ConnID);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Listen_State);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Listen_Port);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_Closure_Type);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Socket Answer Function
		bool SA(const uint8_t _ConnID, const uint8_t _ConnMode, uint16_t & _Length) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				15000, 	// Time Out
				30		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SA="));
			GSM_Serial->print(_ConnID);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_ConnMode);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {
				

				// End Function
				return (true);

			}

			// End Function
			return(false);

		}

		// Socket Status Function
		bool SS(const uint8_t _ConnID, uint8_t & _State) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				60		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SS="));
			GSM_Serial->print(String(_ConnID));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// \r\n#SS: 2,4,5.26.173.230,80\r\n\r\nOK\r\n
				// \r\n#SS: 2,2,5.26.173.230,80,213.14.250.214,54883\r\n\r\nOK\r\n
				// \r\n#SS: 2,2,000.000.000.000,80,000.000.000.000,54883\r\n\r\nOK\r\n

				// Handle State
				_State = (Buffer_Variable[9] - 48);

				// End Function
				return (true);

			}

			// End Function
			return(false);

		}

		// ReOpen Socket Function
		bool SO(const uint8_t _ConnID) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {false, 0, 0, 5000};

			// Declare Buffer Variable
			char Buffer_Variable[255];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', 255);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SO="));
			GSM_Serial->print(String(_ConnID));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Socket Pack Send Function
		bool SSEND(const uint8_t _ConnID, const uint8_t _Header_Type, const uint16_t _Response_Code, const char * _IP, const char * _URL, const char * _Data_Pack) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer_Set = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				2000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer_Set.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer_Set.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SSEND="));
			GSM_Serial->print(_ConnID);
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
			this->Header(_Header_Type, _Response_Code, _IP, _URL, _Data_Pack);

			// Send Data Pack
			GSM_Serial->print(_Data_Pack);

			// Print End Char
			GSM_Serial->print((char)26);

			// Declare Buffer Object
			Serial_Buffer Buffer_Get = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				2000, 	// Time Out
				7		// Buffer Size
			};

			// Declare Buffer
			memset(Buffer_Variable, '\0', Buffer_Get.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Read Current Time
			Current_Time = millis();

			// Read UART Response
			while (!Buffer_Get.Response) {

				// Read Serial Char
				Buffer_Variable[Buffer_Get.Read_Order] = GSM_Serial->read();

				// Control for <OK> Response
				Buffer_Get.Response = this->Find(_AT_OK_, Buffer_Variable, Buffer_Get.Read_Order);

				// Increase Read Order
				if (isAscii(Buffer_Variable[Buffer_Get.Read_Order])) Buffer_Get.Read_Order++;

				// Handle for timeout
				if (millis() - Current_Time >= Buffer_Get.Time_Out) return(false);

			}

			// End Function
			return (true);

		}

		// Socket Recieve Function
		bool SRECV(const uint8_t _ConnID, const uint16_t _MaxByte, char * _Data) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				5000, 	// Time Out
				255		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SRECV="));
			GSM_Serial->print(_ConnID);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_MaxByte);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Read Current Time
			const uint32_t Current_Time = millis();

			// Response Wait Delay
			delay(2);

			// Read UART Response
			while (!Buffer.Response) {

				// Read Serial Char
				Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

				// Control for <OK> Response
				Buffer.Response = this->Find(_AT_OK_, Buffer_Variable, Buffer.Read_Order);

				// Increase Read Order
				if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

				// Handle for timeout
				if (millis() - Current_Time >= Buffer.Time_Out) return(false);

			}

			// Declare Data Handle Variable
			bool Data_Handle = false;

			// Declare Data Order
			int Data_Order = 0;

			// \r\n#SRECV: 2,129\r\nPOST / HTTP/1.1\r\nHost: 5.26.173.230\r\nContent-Length: 70\r\n\r\n{\n    "Request": {\n        "Event": 900,\n        "Firmware": 6\n    }\n}\r\n\r\nOK\r\n

			// Control for Buffer
			for (uint16_t i = 10; i <= Buffer.Read_Order; i++) {

				// Handle JSON Data
				if (Buffer_Variable[i] == '{') Data_Handle = true;

				// Get Data
				if (Data_Handle) {

					// Handle for Space
					if (Buffer_Variable[i] != ' ' and Buffer_Variable[i] != '\n' and Buffer_Variable[i] != '\r') {

						// Set Data
						_Data[Data_Order] = Buffer_Variable[i];
						
						// Increase Data Order
						Data_Order += 1;

					}

				}

				// Handle JSON Data
				if (Buffer_Variable[i-2] == '}' and Buffer_Variable[i-1] == '\r' and Buffer_Variable[i] == '\n') Data_Handle = false;

			}

			// End Function
			return (true);

		}

		// Detect SRING Response.
		bool SRING(uint16_t &_Length) {

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				50000, 	// Time Out
				20		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Read Current Time
			const uint32_t Current_Time = millis();

			// \r\nSRING: 3,108\r\n

			// Read UART Response
			while (!Buffer.Response) {

				// Read Serial Char
				Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

				// Handle for Message End
				if (Buffer.Read_Order > 5 and Buffer_Variable[Buffer.Read_Order - 1] == '\r' and Buffer_Variable[Buffer.Read_Order] == '\n') Buffer.Response = true;

				// Increase Read Order
				if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;
				if (Buffer_Variable[Buffer.Read_Order] == '\r') Buffer.Read_Order += 1;
				if (Buffer_Variable[Buffer.Read_Order] == '\n') Buffer.Read_Order += 1;

				// Handle for timeout
				if (millis() - Current_Time >= Buffer.Time_Out) return(false);

			}

			// Declare Buffer Variable
			uint16_t _Buffer;

			// Handle Variables
			uint8_t _Parsed = sscanf(Buffer_Variable, "\r\nSRING: %d,%d\r\n", &_Buffer, &_Length);

			// Handle for Parse Count
			if (_Parsed == 2) return(true);

			// End Function
			return(false);

		}

		// Send HTTP Pack Header Function
		bool Header(const uint8_t _Header_Type, const uint16_t _Response_Code, const char *_IP, const char *_URL, const char *_Data) {

			// Handle Type
			if (_Header_Type == 1) {

				// Select Response Code
				if (_Response_Code == 200) GSM_Serial->print(F("HTTP/1.1 200 OK\r\n"));
				if (_Response_Code == 202) GSM_Serial->print(F("HTTP/1.1 202 Accepted\r\n"));
				if (_Response_Code == 400) GSM_Serial->print(F("HTTP/1.1 400 Bad Request\r\n"));
				if (_Response_Code == 405) GSM_Serial->print(F("HTTP/1.1 405 Method Not Allowed\r\n"));
				if (_Response_Code == 406) GSM_Serial->print(F("HTTP/1.1 406 Not Acceptable\r\n"));

				// Print Connection Header
				GSM_Serial->print(F("Connection: close\r\n"));

				// Print Content Type
				GSM_Serial->print(F("Content-Type: application/json\r\n"));

				// Print User Agent
				GSM_Serial->print(F("User-Agent: STF-PowerStat\r\n"));

				// End of Header
				GSM_Serial->print(F("\r\n"));

				// End Function
				return(true);

			} else if (_Header_Type == 2) {

				// Print HTTP Header
				GSM_Serial->print(F("POST ")); GSM_Serial->print(_URL); GSM_Serial->print(F(" HTTP/1.1\r\n"));

				// Print Host
				GSM_Serial->print(F("Host: ")); GSM_Serial->print(_IP); GSM_Serial->print(F("\r\n"));

				// Print Content Length
				GSM_Serial->print(F("Content-Length: ")); GSM_Serial->print(String(_Data).length()); GSM_Serial->print(F("\r\n"));

				// Print Content Type
				GSM_Serial->print(F("Content-Type: application/json\r\n"));

				// Print User Agent
				GSM_Serial->print(F("User-Agent: PowerStat\r\n"));

				// End of Header
				GSM_Serial->print(F("\r\n"));

				// End Function
				return(true);

			}

			// End Function
			return(false);

		}

		// Close FTP Connection Function
		bool FTPCLOSE(void) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				500000, 				// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#FTPCLOSE"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Change FTP Folder Function
		bool FTPCWD(const char * _Folder) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				500000,	// Time Out
				20		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#FTPCWD="));
			GSM_Serial->print(F("\""));
			GSM_Serial->print(_Folder);
			GSM_Serial->print(F("\""));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Get FTP File Size Function
		bool FTPFSIZE(const uint16_t _FileID, uint32_t & _Length) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				500000, // Time Out
				30		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#FTPFSIZE="));
			GSM_Serial->print(F("\""));
			GSM_Serial->print(_FileID);
			GSM_Serial->print(F(".hex"));
			GSM_Serial->print(F("\""));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// \r\n#FTPFSIZE: 174945\r\n\r\nOK\r\n

				// Handle Variables
				sscanf(Buffer_Variable, "\r\n#FTPFSIZE:%lu\r\n\r\nOK\r\n", &_Length);

				// End Function
				return (true);

			}

			// End Function
			return(false);

		}

		// Get FTP File to Buffer function
		bool FTPGETPKT(const uint16_t _FileID, const uint8_t _ViewMode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				100000,	// Time Out
				20		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#FTPGETPKT="));
			GSM_Serial->print(F("\""));
			GSM_Serial->print(_FileID);
			GSM_Serial->print(F(".hex"));
			GSM_Serial->print(F("\","));
			GSM_Serial->print(_ViewMode);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Open FTP Connection Function
		bool FTPOPEN(const char * _Server, const char * _Username, const char * _Password, const uint8_t _Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				100000,	// Time Out
				20		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#FTPOPEN="));
			GSM_Serial->print(F("\""));
			GSM_Serial->print(_Server);
			GSM_Serial->print(F("\",\""));
			GSM_Serial->print(_Username);
			GSM_Serial->print(F("\",\""));
			GSM_Serial->print(_Password);
			GSM_Serial->print(F("\","));
			GSM_Serial->print(_Mode);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);

			// End Function
			return(false);

		}

		// Get FTP File From Buffer Function
		bool FTPRECV(const uint16_t _Size, uint16_t & _ReadSize, uint8_t & _State, char * _Data) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				15000,	// Time Out
				255		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#FTPRECV="));
			GSM_Serial->print(_Size);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {
				
				// \r\n#FTPRECV: 200\r\n20202055\r\n:100BA00020202020000D0A002C002C002C00415495\r\n:100BB00023534C3D000D0A004154234532534C52FF\r\n:100BC000493D000D0A00415423534C4544534156BE\r\n:100BD000000D0A00415423534C45443D000D0A00CA\r\n:100BE0004\r\n\r\nOK\r\n
				// \r\n+CME ERROR: 614\r\n	

				// Clear Size Variable
				_ReadSize = 0;

				// Control for Response
				if (Buffer.Response) {

					// Parse Size
					sscanf(Buffer_Variable, "\r\n#FTPRECV: %03d\r\n", &_ReadSize);

					// Calculate Header Length
					const int _Start = 14 + (int)log10(_ReadSize) + 1;

					// Parse Data
					for (uint16_t i = _Start; i < (_Start + _ReadSize); i++) {

						// Get Data
						_Data[Buffer.Data_Order] = Buffer_Variable[i];

						// Increase Data Order
						Buffer.Data_Order += 1;

					}

				} else {

					// \r\n+CME ERROR: 614\r\n

					// Declare CMEE Variable
					int _CME = 0;

					// Control for CME Error
					sscanf(Buffer_Variable, "\r\n+CME ERROR: %03d\r\n", &_CME);

					// Handle CME Error
					if (_CME == 606) _State = 1;
					if (_CME == 614) _State = 2;

				}

				// Control for Timeout
				if (!Buffer.Response) return(false);

				// End Function
				if (_ReadSize == Buffer.Data_Order and _ReadSize != 0) {

					// Return Function
					return (true);

				} else {

					// Wait Delay
					delay(1000);

					// Return Function
					return (false);

				}

				// End Function
				return(false);


			}

			// End Function
			return(false);

		}

		// Set FTP Time Out Function
		bool FTPTO(const uint16_t _TOut) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				20		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#FTPTO="));
			GSM_Serial->print(_TOut);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}

		// Set FTP File Transfer Type Function
		bool FTPTYPE(const uint8_t _Type) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				500000,					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#FTPTYPE="));
			GSM_Serial->print(_Type);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}

		// Soft Reset Function
		bool Z(const uint8_t _Z) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				5000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("ATZ"));
			GSM_Serial->print(_Z);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}

		// Detech From Network and Shut Down Modem Function
		bool SHDN(void) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				5000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer
			char Buffer_Variable[Buffer.Size];
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SHDN"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}




		// GNSS Power Control Function
		bool GPSP(const bool _Status) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				2000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT$GPSP="));
			GSM_Serial->print(_Status);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}

		// GNSS Update Rate Function
		bool GPSNHZ(const uint8_t _Update_Rate) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT$GPSNHZ="));
			GSM_Serial->print(_Update_Rate);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}

		// GNSS Configuration Function
		bool GPSCFG(const uint8_t _Value) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT$GPSCFG=2,"));
			GSM_Serial->print(_Value);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}

		// GNSS Configuration Function
		bool GPSNMUNEX(void) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 					// Response State
				0, 						// Read Order
				0, 						// Data Order
				1000, 					// Time Out
				Max_AT_OK_Response_Length		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT$GPSNMUNEX=0,1,1,0,0,1,0,1,0"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) return(true);
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}

		// GNSS Get Position Function
		bool GPSACP(bool & _Position_OK, double & _Latitude, double & _Longitude) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				false, 	// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				100		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT$GPSACP"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			uint8_t _Response = this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (_Response == _AT_OK_) {

				// \r\n$GPSACP: ,,,,,1,,,,,,\r\n\r\nOK\r\n
				// \r\n\r\n$GPSACP: 121313.00,3757.84237N,03235.96966E,2.91,1044.4,3,,0.693,0.374,180823,06\r\n\r\nOK\r\n
				// \r\n\r\n$GPSACP: 134944.13,3757.87023N,03235.96638E,16.25,1086.6,3,,3.054,1.649,,04\r\n\r\nOK\r\n
				// \r\n\r\n$GPSACP: 140320.44,3757.87827N,03235.96748E,1.25,1114.1,3,,0.713,0.385,,06\r\n\r\nOK\r\n
				// \r\n\r\n$GPSACP: 140322.00,3757.87771N,03235.96699E,1.25,1102.6,3,,0.148,0.080,180823,06\r\n\r\nOK\r\n

				// 3757.87771N,03235.96699E




				// $GPSACP: <UTC>,<latitude>,<longitude>,<hdop>,<altitude>,<fix>,<cog>,<spkm>,<spkn>,<date>,<nsat_tot>
				// $GPSACP:      ,          ,           ,      ,          ,1    ,     ,      ,      ,      ,,\r\n\r\nOK\r\n


				// \r\n\r\n$GPSACP: 140322.00,3757.87771N,03235.96699E,1.25 ,1102.6,3,,0.148,0.080,180823,06\r\n\r\nOK\r\n
				// \r\n\r\n$GPSACP: 134944.13,3757.87023N,03235.96638E,16.25,1086.6,3,,3.054,1.649,      ,04\r\n\r\nOK\r\n
				// \r\n\r\n$GPSACP: 140320.44,3757.87827N,03235.96748E,1.25 ,1114.1,3,,0.713,0.385,      ,06\r\n\r\nOK\r\n
				// \r\n\r\n$GPSACP: 140322.00,3757.87771N,03235.96699E,1.25 ,1102.6,3,,0.148,0.080,180823,06\r\n\r\nOK\r\n
				// \r\n    $GPSACP:          ,           ,            ,     ,      ,1,,     ,     ,      ,,\r\n\r\nOK\r\n


				//                  1         2           3            4      5          6     7     8      9     10     11
				//                  |         |           |            |      |          |     |     |      |      |      | 
				// \r\n\r\n$GPSACP: <UTC>    ,<latitude> ,<longitude> ,<hdop>,<altitude>,<fix>,<cog>,<spkm>,<spkn>,<date>,<nsat_tot>\r\n\r\nOK\r\n
				// \r\n\r\n$GPSACP: hhmmss.ss,ddmm.mmmmma,dddmm.mmmmmb,x     ,x         ,x    ,     ,x     ,x     ,ddmmyy,x         \r\n\r\nOK\r\n





				// Control for length
				if (Buffer.Read_Order < 40) return(true);

				// Declare Trimmed Array
				char _Trimmed_Array[25];

				// Clear Array
				memset(_Trimmed_Array, '\0', 25);

				// Trim Array
				for (uint8_t i = 0; i < 24; i++) _Trimmed_Array[i] = Buffer_Variable[i + 23];

				// 3757.8469000000,3235.9658000000

				// Clear Variables
				_Latitude = 0;
				_Longitude = 0;

				// Handle for Latitude
				_Latitude = ((_Trimmed_Array[0] - 48) * 1000) + ((_Trimmed_Array[1] - 48) * 100) + ((_Trimmed_Array[2] - 48) * 10) +	((_Trimmed_Array[3] - 48) * 1) + ((_Trimmed_Array[5] - 48) * 0.1) + ((_Trimmed_Array[6] - 48) * 0.01) + ((_Trimmed_Array[7] - 48) * 0.001) + ((_Trimmed_Array[8] - 48) * 0.0001) + ((_Trimmed_Array[9] - 48) * 0.00001);
				_Longitude = ((_Trimmed_Array[12] - 48) * 10000) + ((_Trimmed_Array[13] - 48) * 1000) + ((_Trimmed_Array[14] - 48) * 100) + ((_Trimmed_Array[15] - 48) * 10) + ((_Trimmed_Array[16] - 48) * 1) + ((_Trimmed_Array[18] - 48) * 0.1) +	((_Trimmed_Array[19] - 48) * 0.01) + ((_Trimmed_Array[20] - 48) * 0.001) + ((_Trimmed_Array[21] - 48) * 0.0001) + ((_Trimmed_Array[22] - 48) * 0.00001);

				// Convert Coordinates
				_Latitude = this->convertLatitude(_Latitude);
				_Longitude = this->convertLongitude(_Longitude);

				// End Function
				return(true);

			}
			else if (_Response == _AT_ERROR_) return(false);
			else if (_Response == _AT_CME_) return(false);
			else if (_Response == _AT_TIMEOUT_) return(false);
			else return(false);

		}

};
