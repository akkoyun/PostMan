// Include Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Include Definitions
#include "AT_Definitions.h"
#include "Definitions/Modem.h"
#include "Definitions/SIM.h"
#include "Definitions/Response.h"

// Modem AT Command Set Class
class AT_Command_Set_LE910C1_EUX {

	// Private Context
	private:

		// GSM Serial Stream Definition
		Stream * GSM_Serial;

		// Serial Buffer Variable Structure Definition
		struct Serial_Buffer {
			uint8_t 			Response;
			uint16_t 			Read_Order;
			uint16_t 			Data_Order;
			const uint32_t 		Time_Out;
			uint16_t 			Size;
		};

		// Clear Serial Buffer Function
		void Clear_UART_Buffer(void) {

			// Clear UART Buffer
			while (GSM_Serial->available()) {
				
				// Read GSM Buffer
				GSM_Serial->read();

				// Read Delay
				delay(1);

			}

		}

		// Read Serial Buffer Function
		bool Read_UART_Buffer(Serial_Buffer * _Buffer, char * _Buffer_Variable) {

			// Response Wait Delay
			delay(5);

			// Set Read Order
			_Buffer->Read_Order = 0;
			_Buffer->Response = _AT_TIMEOUT_;

			// Read Current Time
			const uint32_t _Current_Time = millis();

			// Read UART Response
			while (!_Buffer->Response) {

				// Read Serial Char
				_Buffer_Variable[_Buffer->Read_Order] = GSM_Serial->read();

				// Control for Response
				if (this->Find(_AT_OK_, _Buffer_Variable, _Buffer->Read_Order)) _Buffer->Response = _AT_OK_;
				if (this->Find(_AT_ERROR_, _Buffer_Variable, _Buffer->Read_Order)) _Buffer->Response = _AT_ERROR_;
				if (this->Find(_AT_CME_, _Buffer_Variable, _Buffer->Read_Order)) _Buffer->Response = _AT_CME_;
				if (this->Find(_AT_SD_PROMPT_, _Buffer_Variable, _Buffer->Read_Order)) _Buffer->Response = _AT_SD_PROMPT_;

				// Increase Read Order
				if (isAscii(_Buffer_Variable[_Buffer->Read_Order])) _Buffer->Read_Order++;

				// Handle for timeout
				if (millis() - _Current_Time >= _Buffer->Time_Out) break;

			}

			// Control for Response
			if (_Buffer->Response != _AT_TIMEOUT_) return(true);

			// End Function
			return(false);

		}

		// OK Find Function
		bool Find(const uint8_t _Type, char * _Buffer, uint16_t _Size) {

			// Select Find Type
			if (_Type == _AT_OK_ and _Size > 4) {

				// \r\nOK\r\n
				// Control for <\r\nOK\r\n> Response
				if (
					(_Buffer[_Size - 5] == 13) && 	// \r
					(_Buffer[_Size - 4] == 10) && 	// \n
					(_Buffer[_Size - 3] == 79) && 	// O
					(_Buffer[_Size - 2] == 75) && 	// K
					(_Buffer[_Size - 1] == 13) && 	// \r
					(_Buffer[_Size - 0] == 10)		// \n
				) return(true);

			} else if (_Type == _AT_ERROR_ and _Size > 7) {

				// \r\nERROR\r\n
				// Control for <\r\nERROR\r\n> Response
				if (
					(_Buffer[_Size - 8] == 13) && 	// \r
					(_Buffer[_Size - 7] == 10) && 	// \n
					(_Buffer[_Size - 6] == 69) && 	// E
					(_Buffer[_Size - 5] == 82) && 	// R
					(_Buffer[_Size - 4] == 82) && 	// R
					(_Buffer[_Size - 3] == 79) && 	// O
					(_Buffer[_Size - 2] == 82) && 	// R
					(_Buffer[_Size - 1] == 13) && 	// \r
					(_Buffer[_Size - 0] == 10)		// \n
				) return(true);

			} else if (_Type == _AT_CME_ and _Size > 15) {

				// \r\n+CME ERROR: 614\r\n
				// Control for <\r\n+CME> Response
				if (
					(_Buffer[_Size - 18] == 13) && 	// \r
					(_Buffer[_Size - 17] == 10) && 	// \r
					(_Buffer[_Size - 16] == 43) && 	// +
					(_Buffer[_Size - 15] == 67) &&	// C 
					(_Buffer[_Size - 14] == 77) && 	// M
					(_Buffer[_Size - 13] == 69) && 	// E
					(_Buffer[_Size - 1] == 13) && 	// \r
					(_Buffer[_Size - 0] == 10)		// \r
				) return(true);

			} else if (_Type == _AT_SD_PROMPT_ and _Size > 2) {

				// \r\n> 
				// Control for <\r\n> > Response
				if (
					(_Buffer[_Size - 3] == 13) && 
					(_Buffer[_Size - 2] == 10) && 
					(_Buffer[_Size - 1] == 62) && 
					(_Buffer[_Size - 0] == 32)
				) return(true);

			} 

			// End Function
			return(false);

		}

		// RSSI to Signal Quality Function
		uint8_t RSSI_to_Signal_Quality(int8_t _RSSI) {

			// Handle for RSSI
			if (_RSSI >= -65) {
				
				// Return Signal Quality
				return 5;

			} else if (_RSSI >= -75) {

				// Return Signal Quality
				return 4;

			} else if (_RSSI >= -85) {

				// Return Signal Quality
				return 3;

			} else if (_RSSI >= -95) {

				// Return Signal Quality
				return 2;

			} else {

				// Return Signal Quality
				return 1;

			}

		}

	// Public Context
	public:

		// PostOffice Constructor
		AT_Command_Set_LE910C1_EUX(Stream &_Serial) {

			// Set Serial Port
			GSM_Serial = & _Serial;

		}

		// AT Command
		bool AT(void) {
			
			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				11      // Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Set Connection Mode Function
		bool FCLASS(const uint8_t _FCLASS) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Get Manufacturer Function
		bool CGMI(uint8_t& _Manufacturer) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				35      // Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// AT+CGMI\r\n
				// \r\nTelit\r\n\r\nOK\r\n

				// Declare Variable
				char _Manufacturer_Name[10];

				// Clear Variable
				memset(_Manufacturer_Name, '\0', 10);

				// Handle Manufacturer Name
				sscanf(Buffer_Variable, "\r\n%9s\r\n\r\nOK\r\n", _Manufacturer_Name);

				// Set No Manufacturer
				_Manufacturer = _MODEM_MANUFACTURER_UNKNOWN_;

				// Control for Manufacturer Name
				if (strstr(_Manufacturer_Name, "Telit") != NULL) {
					_Manufacturer = _MODEM_MANUFACTURER_TELIT_;
					return true;
				}

			}

			// End Function
			return(false);

		}

		// Get Model Function
		bool CGMM(uint8_t& _Model) {
			
			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				35      // Buffer Size
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

			// Get Response
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {
				
				// Declare Variable
				char _Model_Name[15] = {};

				// Get Model Name				
				sscanf(Buffer_Variable, "\r\n%14s\r\n\r\nOK\r\n", _Model_Name);

				// Set No Model
				_Model = _MODEM_MODEL_UNKNOWN_;

				// Control for Model Name
				if (strstr(_Model_Name, "GE910-QUAD") != NULL) {
					_Model = _MODEM_MODEL_GE910_QUAD_;
				} else if (strstr(_Model_Name, "GE910-QUAD-V3") != NULL) {
					_Model = _MODEM_MODEL_GE910_QUAD_V3_;
				} else if (strstr(_Model_Name, "LE910S1-EA") != NULL) {
					_Model = _MODEM_MODEL_LE910S1_EA_;
				} else if (strstr(_Model_Name, "LE910R1-EU") != NULL) {
					_Model = _MODEM_MODEL_LE910R1_EU_;
				} else if (strstr(_Model_Name, "LE910C1-EUX") != NULL) {
					_Model = _MODEM_MODEL_LE910C1_EUX_;
				}

				// Return Response
				return (_Model != _MODEM_MODEL_UNKNOWN_);

			}

			// End Function
			return false;

		}

		// Get Firmware Function
		bool SWPKGV(char * _Firmware) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// AT#SWPKGV\r\n
				// \r\n25.30.226-P0F.225200\r\nM0F.223006\r\nP0F.225200\r\nA0F.223006\r\n\r\nOK\r\n

				// Clear Variable
				memset(_Firmware, '\0', 15);

				// Handle Firmware
				strncpy(_Firmware, &Buffer_Variable[2], 9);

				// End Function
				return true;

			}

			// End Function
			return(false);

		}

		// Get IMEI Function
		bool CGSN(char* _IMEI) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				35      // Buffer Size
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

			// Declare Response
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// AT+CGSN=1\r\n
				// \r\n+CGSN: 354485417617003\r\n\r\nOK\r\n

				// Clear Buffer Variable
				memset(_IMEI, '\0', 17);

				// Handle for Response
				char* _IMEI_Start = strstr(Buffer_Variable, "+CGSN: ");

				// Control for IMEI
				if (_IMEI_Start != nullptr) {
					
					// Set IMEI Start
					_IMEI_Start += 7; // Skip "+CGSN: "
					
					// Get IMEI
					for (int i = 0; i < 15; i++) {
						
						// Control for Digit
						if (isdigit(_IMEI_Start[i])) {
							_IMEI[i] = _IMEI_Start[i];
						} else {
							break;
						}

					}

					// End Function
					return true;

				}

			}

			// End Function
			return false;

		}

		// Set Power Mode Function
		bool CFUN(const uint8_t _Fun) {

			// Control for Parameter
			if (_Fun != 0 and _Fun != 1 and _Fun != 4 and _Fun != 5 and _Fun != 12) {
				
				// End Function
				return(false);

			} 

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Set CMEE Function
		bool CMEE(const uint8_t _CMEE) {

			// Control for Parameter
			if (_CMEE != 0 and _CMEE != 1 and _CMEE != 2){
				
				// End Function
				return(false);

			}

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Get Error Code Function
		bool CEER(uint16_t& _Code) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response State
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				21      // Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// Handle Variables
				if (sscanf(Buffer_Variable, "\r\n#CEER: %03d\r\n\r\nOK\r\n", &_Code) == 1) return (true);

			}

			// End Function
			return (false);

		}

		// ATE Command
		bool ATE(const bool _ECHO) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Flow Control Function
		bool K(const uint8_t _K) {

			// Control for Parameter
			if (_K != 0 and _K != 3) {
				
				// End Function
				return(false);
			
			}

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Get SIM PIN Status Function
		bool CPIN(uint8_t& _Code) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				30      // Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// \r\n+CPIN: READY\r\n\r\nOK\r\n

				// Declare Handle Variables
				char _PIN_Response[15] = {};

				// Handle Variables
				sscanf(Buffer_Variable, "\r\n+CPIN: %14s\r\n\r\nOK\r\n", _PIN_Response);

				// Control for SIM State
				if (strstr(_PIN_Response, "READY")) {
					_Code = _SIM_READY_;
				} else if (strstr(_PIN_Response, "SIM PIN")) {
					_Code = _SIM_PIN_;
				} else if (strstr(_PIN_Response, "SIM PUK")) {
					_Code = _SIM_PUK_;
				} else {
					_Code = _SIM_UNKNOWN_;
				}

				// Handle Status
				return (_Code != _SIM_UNKNOWN_);

			}

			// End Function
			return false;
		}

		// Get ICCID Function
		bool CCID(char* _ICCID) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				37      // Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// Clear ICCID Variable
				memset(_ICCID, '\0', 21);

				// Handle for Response
				for (uint8_t i = 0; i < Buffer.Size; i++) {

					// Handle ICCID
					if (isdigit(Buffer_Variable[i])) {

						// Set ICCID Variable
						_ICCID[Buffer.Data_Order++] = Buffer_Variable[i];

					}

				}

				// End Function
				return true;

			}

			// End Function
			return false;

		}

		// SIMDET Function
		bool SIMDET(const bool _Function_Type, uint8_t _Mode, bool& _SIM_in_Pin_State) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				23      // Buffer Size
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
				this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				return (Buffer.Response == _AT_OK_);

			}

			// GET Function
			if (_Function_Type == GET) {

				// Send UART Command
				GSM_Serial->print(F("AT#SIMDET?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Response
				this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (Buffer.Response == _AT_OK_) {

					// Clear Variables
					_Mode = 0;
					_SIM_in_Pin_State = false;

					// \r\n#SIMDET: 2,1\r\n\r\nOK\r\n

					// Define Handle Variable
					uint8_t _SIMDET_Mode = 0;
					uint8_t _SIMDET_State = 0;

					// Handle Query Answer
					uint8_t _Parsed_Variable = sscanf(Buffer_Variable, "\r\n#SIMDET: %01hhu,%01hhu\r\n\r\nOK\r\n", &_SIMDET_Mode, &_SIMDET_State);

					// Assign SIM State
					if (_Parsed_Variable == 2) {
						
						// Assign Mode
						_Mode = _SIMDET_Mode;

						// Assign State
						if (_SIMDET_State == 0) _SIM_in_Pin_State = false;
						if (_SIMDET_State == 1) _SIM_in_Pin_State = true;

						// End Function
						return (true);

					}

					// End Function
					return (false);

				}

			}

			// End Function
			return (false);

		}

		// Set GPIO Function
		bool GPIO(const bool _Function_Type, const uint8_t _Pin, const uint8_t _Mode = 0, const uint8_t _Direction = 0) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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

				// Get Response
				this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				return (Buffer.Response == _AT_OK_);

			}

			return false;
		}

		// Set SLED Function
		bool SLED(const uint8_t _SLED) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,
				0,
				0,
				1000,
				7
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

			// Get Response
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Execution command saves STAT_LED setting in NVM.
		bool SLEDSAV(void) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response 
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Set Socket Listen Ring Indicator Function
		bool E2SLRI(const uint16_t _Pulse_Duration) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command (Pulse Duration: 0-65535)
			GSM_Serial->print(F("AT#E2SLRI="));
			GSM_Serial->print(_Pulse_Duration);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Get Response
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// CREG Function
		bool CREG(const bool _Function_Type, uint8_t& _Mode, uint8_t& _Stat) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				25      // Buffer Size
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

				// Get Response
				this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				return (Buffer.Response == _AT_OK_);

			}

			// GET Function
			if (_Function_Type == GET) {

				// Send UART Command
				GSM_Serial->print(F("AT+CREG?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Get Response
				this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (Buffer.Response == _AT_OK_) {

					// Handle Variables
					sscanf(Buffer_Variable, "\r\n+CREG: %hhu,%hhu\r\n\r\nOK\r\n", &_Mode, &_Stat);

					// Handle Response
					if (_Stat >= 0 && _Stat <= 11) {
						return true;
					}

				}

			}

			// End Function
			return false;

		}

		// Set CGDCONT Function
		bool CGDCONT(const uint8_t _Cid, const char* _PDP_Type, const char* _APN) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				7       // Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			return (Buffer.Response == _AT_OK_);

		}

		// Set SGACT Function
		bool SGACT(const uint8_t _Cid, const bool _Stat, uint8_t _IP_Segment[4]) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0,      // Read Order
				0,      // Data Order
				60000,  // Time Out
				35      // Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// \r\n#SGACT: 000.000.000.000\r\n\r\nOK\r\n

				// Handle IP
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n#SGACT: %03hhu.%03hhu.%03hhu.%03hhu\r\n\r\nOK\r\n", &_IP_Segment[0], &_IP_Segment[1], &_IP_Segment[2], &_IP_Segment[3]);

				// Control for IP
				return (_Variable_Count == 4);

			}

			// End Function
			return false;

		}

		// WS46 Function
		bool WS46(const bool _Function_Type, uint8_t & _Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,
				0,
				0,
				1000,
				20
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
				this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				if (Buffer.Response == _AT_OK_) {

					// Handle Variables
					uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n+WS46: %02hhu\r\n\r\nOK\r\n", &_Mode);

					// Handle Response
					return (_Variable_Count == 1);

				}

			}

			// SET Function
			if (_Function_Type == SET) {

				// Send UART Command
				GSM_Serial->print(F("AT+WS46="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Declare Response
				this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// Handle for Response
				return (Buffer.Response == _AT_OK_);

			}

			// End Function
			return false;

		}

		// RFSTS Function
		bool RFSTS(uint16_t & _MCC, uint16_t & _MNC, uint16_t & _RSSI, uint8_t & _Signal_Level, uint32_t & _Cell_ID, uint16_t & _TAC) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				120		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);
			
			// Send UART Command
			GSM_Serial->print(F("AT#RFSTS"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			delay(20);

			// Declare Response
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// AT#RFSTS\r\n
				// \r\n#RFSTS: "286 01",1795,-101,-67,-15,2242,,128,3,1,0B5D121,"286016339612498","Turkcell",3,3,108\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",1651,-101,-66,-15,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,121\r\n\r\nOK\r\n

				// #RFSTS:<PLMN>,<EARFCN>,<RSRP>,<RSSI>,<RSRQ>,<TAC>,<RAC>,[<TXPWR>],<DR X>, <MM>,<RRC>,<CID>,<IMSI>,[<NetNameAsc>],<SD>,<ABND>,<T3402>,<T3412>,<SI NR>
				//
				// <PLMN> 		- Country code and operator code(MCC, MNC)						+ "286 01"
				// <EARFCN> 	- E-UTRA Assigned Radio Channel									- 6400	
				// <RSRP> 		- Reference Signal Received Power								- -100
				// <RSSI> 		- Received Signal Strength Indication							+ -66
				// <RSRQ> 		- Reference Signal Received Quality								- -18
				// <TAC> 		- Tracking Area Code											+ 2242
				// <RAC> 		- Routing Area Code												- FF
				// <TXPWR> 		- Tx Power (In traffic only)									- 0
				// <DR X> 		- Discontinuous reception cycle Length (cycle length in ms)		- 128
				// <MM> 		- Mobility Management state (for debug purpose only)			- 10
				// <RRC> 		- Radio Resource state (for debug purpose only)					- 1
				// <CID> 		- Cell ID														+ 859315
				// <IMSI> 		- International Mobile Station ID								- "286016339811626"
				// <NetNameAsc> - Operator name													- "TR TURKCELL"
				// <SD> 		- Service Domain												- 3
				// <ABND> 		- Active Band													- 20
				// <T3402> 		- Timer T3402 in seconds										- 720
				// <T3412> 		- Timer T3412 in seconds										- 3240
				// <SI NR> 		- Signal-to-Interface plus Noise Ratio							- -5

				// Example Response
				// \r\n#RFSTS: "286 01",1651,-101,-66,-15,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,121\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",1651,-100,-66,-14,2242,,128,3,0,0B5D120,"286016339612498","Turkcell",3,3,126\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",1795,-101,-73,-10,2242,,128,3,0,0B5D121,"286016339612498","Turkcell",3,3,138\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",100,-98,-68,-10,2242,227,128,3,1,0B5D125,"286016339612498","Turkcell",3,1,120\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",1651,-99,-63,-14,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,101\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",1651,-99,-68,-12,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,128\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",1795,-102,-69,-15,2242,,128,3,1,0B5D121,"286016339612498","Turkcell",3,3,102\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",1651,-99,-67,-12,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,126\r\n\r\nOK\r\n
				// \r\n#RFSTS: "286 01",1651,-99,-64,-14,2242,,128,3,1,0B5D120,"286016339612498","Turkcell",3,3,107\r\n\r\nOK\r\n



				// Handle Variables
				char * _Segment_1 = strtok(Buffer_Variable, ",");	// \r\n#RFSTS: "286 01"
				strtok(NULL, ",");									// 1651
				strtok(NULL, ",");									// -101
				char * _Segment_4 = strtok(NULL, ",");				// -66
				strtok(NULL, ",");									// -15
				char * _Segment_6 = strtok(NULL, ",");				// 2242
				strtok(NULL, ",");									//	
				strtok(NULL, ",");									// 128
				strtok(NULL, ",");									// 3
				strtok(NULL, ",");									// 1
				char * _Segment_11 = strtok(NULL, ",");				// 0B5D120

				// Handle MCC, MNC
				_MCC = 0; _MNC = 0;
				sscanf(_Segment_1, "\r\n#RFSTS: \"%03u %02u\"", &_MCC, &_MNC);

				// Handle RSSI
				_RSSI = 0;
				sscanf(_Segment_4, "-%03u", &_RSSI);

				// Calculate Signal Level
				_Signal_Level = 0;
				_Signal_Level = this->RSSI_to_Signal_Quality(_RSSI * -1);

				// Handle TAC
				_TAC = 0;
				sscanf(_Segment_6, "%05u", &_TAC);

				// Handle Cell ID
				_Cell_ID = 0;
				sscanf(_Segment_11, "%lX", &_Cell_ID);

				// End Function
				return(true);

			}

			// End Function
			return(false);

		}

		// MONI Function
		bool MONI(uint16_t & _TAC, uint32_t & _Cell_ID, uint16_t & _RSSI, uint8_t & _Signal_Level, uint32_t & _PCell_ID) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				130		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// Control for GSM or LTE
				if (strstr(Buffer_Variable, "bsic") != NULL) {

					// GSM network
					// #MONI: <netname> BSIC:<bsic> RxQual:<qual> LAC:<lac> Id:<id> ARFCN:<arfcn> PWR:<dBm> dBm TA: <timadv>

				} else {

					// LTE network
					// #MONI: <netmame> RSRP:<rsrp> RSRQ:<rsrq> TAC:<tac> Id:<id> EARFCN:<earfcn> PWR:<dBm> DRX:<drx> pci:<physicalCellId> QRxLevMin:<QRxLevMin>

					// <netname>			- network name
					// <id>					- cell identifier
					// <dBm>				- received signal strength in dBm
					// <drx>				- Discontinuous reception cycle length
					// <physicalCellId>		- physical cell identifier
					// <rsrp>				- Reference Signal Received Power
					// <rsrq>				- Reference Signal Received Quality
					// <tac>				- Tracking Area Code
					// <earfcn>				- E-UTRA Absolute Radio Frequency Channel Number
					// <QRxLevMin>			- Minimum required received signal level

					// AT#MONI\r\n
					// \r\n#MONI: Turkcell RSRP:-97 RSRQ:-11 TAC:2242 Id:0B5D125 EARFCN:100 PWR:-66dbm DRX:128 pci:335 QRxLevMin:0\r\n\r\nOK\r\n

					// Handle Variables
					strtok(Buffer_Variable, " ");				// \r\n#MONI:
					strtok(NULL, " ");							// Turkcell
					strtok(NULL, " ");							// RSRP:-97
					strtok(NULL, " ");							// RSRQ:-11
					char * _Segment_5 = strtok(NULL, " ");		// TAC:2242
					char * _Segment_6 = strtok(NULL, " ");		// Id:0B5D125
					strtok(NULL, " ");							// EARFCN:100
					char * _Segment_8 = strtok(NULL, " ");		// PWR:-66dbm
					strtok(NULL, " ");							// DRX:128
					char * _Segment_10 = strtok(NULL, " ");		// pci:335
					strtok(NULL, " ");							// QRxLevMin:0\r\n\r\nOK\r\n

					// Handle TAC
					_TAC = 0;
					sscanf(_Segment_5, "TAC:%05u", &_TAC);

					// Handle Cell ID
					_Cell_ID = 0;
					sscanf(_Segment_6, "Id:%lx", &_Cell_ID);

					// Handle RSSI
					_RSSI = 0;
					sscanf(_Segment_8, "PWR:-%03udbm", &_RSSI);

					// Handle PCell ID
					_PCell_ID = 0;
					sscanf(_Segment_10, "pci:%lx", &_PCell_ID);

					// Calculate Signal Level
					_Signal_Level = 0;
					_Signal_Level = this->RSSI_to_Signal_Quality(_RSSI * -1);

					// End Function
					return(true);

				}

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
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Extended Socket Configuration Function
		bool SCFGEXT(const uint8_t _Conn_ID, const uint8_t _Sr_Mode, const uint8_t _Recv_Data_Mode, const uint8_t _Keep_Alive, const uint8_t _Listen_Auto_Rsp, const uint8_t _Send_Data_Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Extended 2 Socket Configuration Function
		bool SCFGEXT2(const uint8_t _Conn_ID, const uint8_t _Buffer_Start, const uint8_t _Abort_Conn_Attempt, const uint8_t _SRing_Len, const uint8_t _SRing_To, const uint8_t _No_Carrier_Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Firewall Add/Remove Function
		bool FRWL(const bool _Function_Type, const uint8_t _Action, const char *_IP_Addr) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// SET Function
			if (_Function_Type == SET) {

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					0, 		// Response
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					15		// Buffer Size TODO: test
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
				this->Read_UART_Buffer(&Buffer, Buffer_Variable);

				// End Function
				return(Buffer.Response == _AT_OK_);

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
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				15		// Buffer Size TODO: test
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Get Clock Function
		bool CCLK(uint8_t& _Year, uint8_t& _Month, uint8_t& _Day, uint8_t& _Hour, uint8_t& _Minute, uint8_t& _Second, uint8_t& _Time_Zone) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				/*
				--> AT+CCLK?\r\n
				<-- \r\n+CCLK: "23/08/02,11:18:16+12"\r\n\r\nOK\r\n
				*/

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n+CCLK: \"%02hhu/%02hhu/%02hhu,%02hhu:%02hhu:%02hhu+%02hhu\"\r\n\r\nOK\r\n", &_Year, &_Month, &_Day, &_Hour, &_Minute, &_Second, &_Time_Zone);

				// Control for Variables
				if (_Year > 24 || _Year < 22 || _Month > 12 || _Month < 0 || _Day > 31 || _Day < 0 || _Hour > 24 || _Hour < 0 || _Minute > 59 || _Minute < 0 || _Second > 59 || _Second < 0) return false;

				// Control for Variable
				if (_Variable_Count == 7) return true;

			}

			// End Function
			return false;

		}

		// Enable or Disable Automatic TimeZone Update Function
		bool CTZU(const bool _State) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Enable or Disable Network Time UnSolicited Function
		bool NITZ(const bool _State) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Enable the local time or the UTC time.
		bool CCLKMODE(const bool _Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Socket Dial Function
		bool ATSD(const uint8_t _Cid, const uint8_t _Protocol, const uint8_t _Port, const uint8_t _Closure_Type, uint16_t _IPort, const bool _Conn_Mode, const char *_IP) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response
				0, 		// Read Order
				0, 		// Data Order
				140000, // Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Close Socket Function
		bool SH(const uint8_t _ConnID) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				5000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Socket Listen Function
		bool SL(const uint8_t _ConnID, const bool _Listen_State, const uint16_t _Listen_Port, const uint8_t _Closure_Type) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Socket Answer Function
		bool SA(const uint8_t _ConnID, const uint8_t _ConnMode, uint16_t & _Length) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
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

			// Read Current Time
			const uint32_t Current_Time = millis();

			// \r\nOK\r\n\r\nSRING: 2,86\r\n

			// Read UART Response
			while (!Buffer.Response) {

				// Read Serial Char
				Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

				// Handle for Message End
				if (Buffer.Read_Order > 10 && Buffer_Variable[Buffer.Read_Order - 1] == '\r' && Buffer_Variable[Buffer.Read_Order] == '\n') Buffer.Response = true;

				// Increase Read Order
				if (Buffer_Variable[Buffer.Read_Order] > 31 && Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;
				if (Buffer_Variable[Buffer.Read_Order] == '\r') Buffer.Read_Order += 1;
				if (Buffer_Variable[Buffer.Read_Order] == '\n') Buffer.Read_Order += 1;

				// Handle for timeout
				if (millis() - Current_Time >= Buffer.Time_Out) return(false);

			}

			// Declare Buffer Variable
			uint16_t _Buffer;

			// Handle Variables
			uint8_t _Parsed = sscanf(Buffer_Variable, "\r\nOK\r\n\r\nSRING: %01u,%03u\r\n", &_Buffer, &_Length);

			// Handle for Parse Count
			if (_Parsed == 2) return(true);

			// End Function
			return(false);

		}

		// Socket Status Function
		bool SS(const uint8_t _ConnID, uint8_t & _State) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

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
			Serial_Buffer Buffer = {
				0, 
				0, 
				0, 
				5000,
				20
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SO="));
			GSM_Serial->print(String(_ConnID));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Socket Info Function
		bool SI(const uint8_t _ConnID, uint16_t & _Buffer) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,  	// Response State
				0,      // Read Order
				0,      // Data Order
				1000,   // Time Out
				50      // Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SI="));
			GSM_Serial->print(String(_ConnID));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

				// \r\n#SI: 2,51,0,13900,0\r\n\r\nOK\r\n

				// Handle Variables
				strtok(Buffer_Variable, ",");				// \r\n#SI: 2
				strtok(NULL, ",");							// 51
				strtok(NULL, ",");							// 0
				char * _Buffer_Size = strtok(NULL, ",");	// 13900

				// Handle Buffer Size
				_Buffer = 0;
				sscanf(_Buffer_Size, "%05u", &_Buffer);

				// End Function
				return(true);

			}

			// End Function
			return(false);

		}

		// Socket Pack Send Function
		bool SSEND(const uint8_t _ConnID, const uint8_t _Method_Type, const uint16_t _Response_Code, const char * _URL, const char * _Data_Pack) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer_Set = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				2000, 	// Time Out
				7		// Buffer Size
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
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			this->Read_UART_Buffer(&Buffer_Set, Buffer_Variable);

			// Handle for Response
			if (Buffer_Set.Response == _AT_SD_PROMPT_) {

				// Send Delay
				delay(10);

				// Print HTTP Header
				this->Header(_Method_Type, _Response_Code, _URL, _Data_Pack);

				// Send Data Pack
				GSM_Serial->print(_Data_Pack);

				// Print End Char
				GSM_Serial->print((char)26);

				// Declare Buffer Object
				Serial_Buffer Buffer_Get = {
					0, 		// Response State
					0, 		// Read Order
					0, 		// Data Order
					2000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer Variable
				char Buffer_Variable[Buffer_Get.Size];

				// Clear Buffer Variable
				memset(Buffer_Variable, '\0', Buffer_Get.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Declare Response
				this->Read_UART_Buffer(&Buffer_Get, Buffer_Variable);

				// End Function
				return(Buffer_Get.Response == _AT_OK_);

			}

			// End Function
			return (false);

		}

		// Socket Recieve Function
		bool SRECV(const uint8_t _ConnID, const uint16_t _MaxByte, char * _Data) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0,		// Response State
				0, 		// Read Order
				0, 		// Data Order
				5000,	// Time Out
				1023	// Buffer Size
			};

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SRECV="));
			GSM_Serial->print(_ConnID);
			GSM_Serial->print(F(","));
			GSM_Serial->print(_MaxByte);
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			this->Read_UART_Buffer(&Buffer, _Data);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) return (true);

			// End Function
			return (false);

		}

		// Detect SRING Response.
		bool SRING(uint16_t &_Length) {

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
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
				if (Buffer.Read_Order > 5 && Buffer_Variable[Buffer.Read_Order - 1] == '\r' && Buffer_Variable[Buffer.Read_Order] == '\n') Buffer.Response = true;

				// Increase Read Order
				if (Buffer_Variable[Buffer.Read_Order] > 31 && Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;
				if (Buffer_Variable[Buffer.Read_Order] == '\r') Buffer.Read_Order += 1;
				if (Buffer_Variable[Buffer.Read_Order] == '\n') Buffer.Read_Order += 1;

				// Handle for timeout
				if (millis() - Current_Time >= Buffer.Time_Out) return(false);

			}

			// Declare Buffer Variable
			uint16_t _Buffer;

			// Handle Variables
			uint8_t _Parsed = sscanf(Buffer_Variable, "\r\nSRING: %01u,%03u\r\n", &_Buffer, &_Length);

			// Handle for Parse Count
			if (_Parsed == 2) return(true);

			// End Function
			return(false);

		}
		bool SRING(void) {

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
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
				if (Buffer.Read_Order > 5 && Buffer_Variable[Buffer.Read_Order - 1] == '\r' && Buffer_Variable[Buffer.Read_Order] == '\n') Buffer.Response = true;

				// Increase Read Order
				if (Buffer_Variable[Buffer.Read_Order] > 31 && Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;
				if (Buffer_Variable[Buffer.Read_Order] == '\r') Buffer.Read_Order += 1;
				if (Buffer_Variable[Buffer.Read_Order] == '\n') Buffer.Read_Order += 1;

				// Handle for timeout
				if (millis() - Current_Time >= Buffer.Time_Out) return(false);

			}

			// Control for SRING
			if (strstr(Buffer_Variable, "\r\nSRING") != NULL) return(true);

			// End Function
			return(false);

		}

		// Send HTTP Pack Header Function
		bool Header(const uint8_t _Method, const uint16_t _Response_Code, const char * _URL, const char *_Data) {

			// Control for Method
			if (_Method == HTTP_RESPONSE) {

				// Select Response Code
				switch (_Response_Code) {
					case 200:
						GSM_Serial->print(F("HTTP/1.1 200 OK\r\n"));
						break;
					case 202:
						GSM_Serial->print(F("HTTP/1.1 202 Accepted\r\n"));
						break;
					case 400:
						GSM_Serial->print(F("HTTP/1.1 400 Bad Request\r\n"));
						break;
					case 405:
						GSM_Serial->print(F("HTTP/1.1 405 Method Not Allowed\r\n"));
						break;
					case 406:
						GSM_Serial->print(F("HTTP/1.1 406 Not Acceptable\r\n"));
						break;
					default:
						break;
				}

				// Print Connection Header
				GSM_Serial->print(F("Connection: close\r\n"));

				// Print Content Type
				GSM_Serial->print(F("Content-Type: application/json\r\n"));

				// Print User Agent
				GSM_Serial->print(F("User-Agent: PostOffice\r\n"));

				// End of Header
				GSM_Serial->print(F("\r\n"));

				// End Function
				return true;

			} else if (_Method == HTTP_POST) {

				// Print HTTP Header
				GSM_Serial->print(F("POST "));
				GSM_Serial->print(_URL);
				GSM_Serial->print(F(" HTTP/1.1\r\n"));

				// Print Host
				GSM_Serial->print(F("Host: "));
				GSM_Serial->print(_PostMan_Server_);
				GSM_Serial->print(F("\r\n"));

				// Print Content Length
				GSM_Serial->print(F("Content-Length: "));
				GSM_Serial->print(String(_Data).length());
				GSM_Serial->print(F("\r\n"));

				// Print Content Type
				GSM_Serial->print(F("Content-Type: application/json\r\n"));

				// Print User Agent
				GSM_Serial->print(F("User-Agent: PostOffice\r\n"));

				// End of Header
				GSM_Serial->print(F("\r\n"));

				// End Function
				return true;

			} else if (_Method == HTTP_GET) {

				// Print HTTP Header
				GSM_Serial->print(F("GET "));
				GSM_Serial->print(_URL);
				GSM_Serial->print(F(" HTTP/1.1\r\n"));

				// Print Host
				GSM_Serial->print(F("Host: "));
				GSM_Serial->print(_PostMan_Server_);
				GSM_Serial->print(F("\r\n"));

				// End of Header
				GSM_Serial->print(F("\r\n"));

				// End Function
				return true;

			}

			// End Function
			return false;

		}

		// Close FTP Connection Function
		bool FTPCLOSE(void) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				500000, // Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Change FTP Folder Function
		bool FTPCWD(const char * _Folder) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				500000,	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Get FTP File Size Function
		bool FTPFSIZE(const uint16_t _FileID, uint32_t & _Length) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {

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
				0, 		// Response State
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Open FTP Connection Function
		bool FTPOPEN(const char * _Server, const char * _Username, const char * _Password, const uint8_t _Mode) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				100000,	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Get FTP File From Buffer Function
		bool FTPRECV(const uint16_t _Size, uint16_t & _ReadSize, uint8_t & _State, char * _Data) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// Handle for Response
			if (Buffer.Response == _AT_OK_) {
				
				// \r\n#FTPRECV: 200\r\n20202055\r\n:100BA00020202020000D0A002C002C002C00415495\r\n:100BB00023534C3D000D0A004154234532534C52FF\r\n:100BC000493D000D0A00415423534C4544534156BE\r\n:100BD000000D0A00415423534C45443D000D0A00CA\r\n:100BE0004\r\n\r\nOK\r\n
				// \r\n+CME ERROR: 614\r\n	

				// Clear Size Variable
				_ReadSize = 0;

				// Control for Response
				if (Buffer.Response) {

					// Parse Size
					sscanf(Buffer_Variable, "\r\n#FTPRECV: %03u\r\n", &_ReadSize);

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
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				1000, 	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Set FTP File Transfer Type Function
		bool FTPTYPE(const uint8_t _Type) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				500000,	// Time Out
				7		// Buffer Size
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Soft Reset Function
		bool Z(const uint8_t _Z) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				5000, 	// Time Out
				7		// Buffer Size
			};

			// Declare Buffer Variable
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
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

		// Detech From Network and Shut Down Modem Function
		bool SHDN(void) {

			// Clear UART Buffer
			this->Clear_UART_Buffer();

			// Declare Buffer Object
			Serial_Buffer Buffer = {
				0, 		// Response State
				0, 		// Read Order
				0, 		// Data Order
				5000, 	// Time Out
				7		// Buffer Size
			};

			// Declare Buffer Variable
			char Buffer_Variable[Buffer.Size];

			// Clear Buffer Variable
			memset(Buffer_Variable, '\0', Buffer.Size);

			// Command Chain Delay (Advice by Telit)
			delay(20);

			// Send UART Command
			GSM_Serial->print(F("AT#SHDN"));
			GSM_Serial->write(0x0D);
			GSM_Serial->write(0x0A);

			// Declare Response
			this->Read_UART_Buffer(&Buffer, Buffer_Variable);

			// End Function
			return(Buffer.Response == _AT_OK_);

		}

};
