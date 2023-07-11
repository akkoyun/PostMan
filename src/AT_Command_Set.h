#ifndef __Telit_AT_Command_Set__
#define __Telit_AT_Command_Set__

	// Define Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif

	// Define AT Command Defaults	
	#include "Config/AT_Definitions.h"

	// Modem AT Command Set Class
	class AT_Command_Set {

		private:

			// Digit Counter Function
			uint8_t CountDigits(uint16_t _Number) {
			
				// Calculate Digit
				return(1 + log10(_Number)) ;
		
			}

		public:

            // GSM Serial Stream Definition
            Stream * GSM_Serial;

			/**
			 * @brief GSM Serial Buffer Object Definition.
			 */
			struct Serial_Buffer {
				bool Response;
				uint8_t Read_Order;
				uint8_t Data_Order;
				const uint32_t Time_Out;
				uint8_t Size;
			};

			// Define Modem Structure
			struct Struct_Modem {
				uint8_t 	PIN;
				uint8_t 	Connection_Time;
				char 		IMEI[17];
				char 		ICCID[21];

				// Signal Level
				uint8_t 	dBm;
				uint8_t		Signal;

				// Manufacturer and Model
				uint8_t 	Manufacturer;
				uint8_t 	Model;

				// GSM Serial ID
				uint32_t	Serial_ID;

				// IP Address Struct
				struct Struct_IP {
					uint16_t	Segment_1;
					uint16_t	Segment_2;
					uint16_t	Segment_3;
					uint16_t	Segment_4;
				} IP_Address;
				
				// IP Address Struct
				struct Struct_Firmware {
					uint16_t	Segment_1;
					uint16_t	Segment_2;
					uint16_t	Segment_3;
					uint16_t	Segment_4;
				} Firmware;

				// Operator
				uint16_t 	Operator;

				// Location
				uint16_t	LAC;
				uint16_t	Cell_ID;

			} Modem;

			// PostOffice Constructor
			AT_Command_Set(Stream &_Serial) {

				// Set Serial Port
				GSM_Serial = & _Serial;

			}

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

			// OK Find Function
			bool Find_OK(char * _Buffer, uint8_t _Size) {

				// Control for <\r\nOK\r\n> Response
				if (_Buffer[_Size - 5] == 13 and _Buffer[_Size - 4] == 10 and _Buffer[_Size - 3] == 79 and _Buffer[_Size - 2] == 75 and _Buffer[_Size - 1] == 13 and _Buffer[_Size - 0] == 10) return(true);

				// End Function
				return(false);

			}

			// ******************** General Control and Config Commands ********************

			// AT Command
			bool AT(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					11		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("ATE"));
				GSM_Serial->print(_ECHO);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// AT SEARCHLIM Command
			bool SEARCHLIM(const uint8_t _GSMSearchLim, const uint8_t _DCSPCSSearchLim) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SEARCHLIM="));
				GSM_Serial->print(_GSMSearchLim);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_DCSPCSSearchLim);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set Power Mode Function
			bool CFUN(const uint8_t _Fun) {

				// Control for Parameter
				if (_Fun > 1) return(false);

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CFUN="));
				GSM_Serial->print(_Fun);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set CMEE Function
			bool CMEE(const uint8_t _CMEE) {

				// Control for Parameter
				if (_CMEE > 2) return(false);

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CMEE="));
				GSM_Serial->print(_CMEE);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set Connection Mode Function
			bool FCLASS(const uint8_t _FCLASS) {

				// Control for Parameter
				if (_FCLASS != 0 and _FCLASS != 8) return(false);

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+FCLASS="));
				GSM_Serial->print(_FCLASS);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Set command controls the RS232 flow control behavior.
			 * @param _K Parameter. 
			 * 0 : No flow control
			 * 1 : Hardware mono-directional flow control
			 * 2 : Software mono-directional flow control
			 * 3 : Hardware bi-directional flow control
			 * 4 : Software bi-directional with filtering
			 * 5 : Pass through: software bi-directional without filtering
			 * 6 : Both hardware bi-directional flow control
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool K(const uint8_t _K) {

				// Control for Parameter
				if (_K > 6) return(false);

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT&K"));
				GSM_Serial->print(_K);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// TODO: Düzenlenecek
			// Get SIM PIN Status Function
			bool CPIN(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CPIN?"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(20);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Declare Variable
				this->Modem.PIN = 0;

				// Handle for Response
				for (size_t i = 0; i < 255; i++) {

					// Control for SIM
					if (
						Buffer_Variable[i - 12] == 'C' and
						Buffer_Variable[i - 11] == 'M' and
						Buffer_Variable[i - 10] == 'E' and
						Buffer_Variable[i - 9] == ' ' and
						Buffer_Variable[i - 8] == 'E' and
						Buffer_Variable[i - 7] == 'R' and
						Buffer_Variable[i - 6] == 'R' and
						Buffer_Variable[i - 5] == 'O' and
						Buffer_Variable[i - 4] == 'R' and
						Buffer_Variable[i - 3] == ':' and
						Buffer_Variable[i - 2] == ' ' and
						Buffer_Variable[i - 1] == '1' and
						Buffer_Variable[i - 0] == '0'
					) this->Modem.PIN = 0;

					// Control for READY
					if (
						Buffer_Variable[i - 4] == 'R' and
						Buffer_Variable[i - 3] == 'E' and
						Buffer_Variable[i - 2] == 'A' and
						Buffer_Variable[i - 1] == 'D' and
						Buffer_Variable[i - 0] == 'Y'
					) this->Modem.PIN = 1;

					// Control for SIM PIN
					if (
						Buffer_Variable[i - 6] == 'S' and
						Buffer_Variable[i - 5] == 'I' and
						Buffer_Variable[i - 4] == 'M' and
						Buffer_Variable[i - 3] == ' ' and
						Buffer_Variable[i - 2] == 'P' and
						Buffer_Variable[i - 1] == 'I' and
						Buffer_Variable[i - 0] == 'N'
					) this->Modem.PIN = 2;

					// Control for SIM PUK
					if (
						Buffer_Variable[i - 6] == 'S' and
						Buffer_Variable[i - 5] == 'I' and
						Buffer_Variable[i - 4] == 'M' and
						Buffer_Variable[i - 3] == ' ' and
						Buffer_Variable[i - 2] == 'P' and
						Buffer_Variable[i - 1] == 'U' and
						Buffer_Variable[i - 0] == 'K'
					) this->Modem.PIN = 3;

				}

				// Handle Status
				if (this->Modem.PIN == 1) return(true);

				// End Function
				return(false);

			}

			// Get IMEI Function
			bool CGSN(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					30		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(10);

				// Send UART Command
				GSM_Serial->print(F("AT+CGSN"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(20);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Clear Variable
				memset(this->Modem.IMEI, '\0', 17);

				// Handle for Response
				for (size_t i = 0; i < Buffer.Size; i++) {

					// Handle IMEI
					if (Buffer_Variable[i] > 47 and Buffer_Variable[i] < 58) {

						// Set IMEI Variable
						this->Modem.IMEI[Buffer.Data_Order] = Buffer_Variable[i];

						// Set Data Order
						Buffer.Data_Order++;

					}

				}

				// End Function
				return(true);

			}

			// Get Modem Serial ID Function
			bool GSN(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					21		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+GSN"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// \r\n0001409405\r\n\r\nOK\r\n

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// 0001409405OK

				// Handle Firmware
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "%10luOK", &this->Modem.Serial_ID);

				// Control for IP
				if (_Variable_Count == 1) return (true);

				// End Function
				return (false);

			}

			// Get ICCID Function
			bool CCID(void) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#CCID"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Clear Variable
				memset(this->Modem.ICCID, '\0', 21);

				// Handle for Response
				for (size_t i = 0; i < Buffer.Size; i++) {

					// Handle IMEI
					if (Buffer_Variable[i] > 47 and Buffer_Variable[i] < 58) {

						// Set IMEI Variable
						this->Modem.ICCID[Buffer.Data_Order] = Buffer_Variable[i];

						// Set Data Order
						Buffer.Data_Order++;

					}

				}

				// End Function
				return(true);

			}

			// Get Manufacturer Function
			bool GMI(void) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+GMI"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Handle Manufacturer Name
				char _Manufacruer_Name[10];
				memset(_Manufacruer_Name, '\0', 10);
				sscanf(Buffer_Variable, "\r\n%s\r\n\r\nOK\r\n", _Manufacruer_Name);

				// Control for Manufacturer Name
				if (strstr(_Manufacruer_Name, "Telit")) this->Modem.Manufacturer = 1;

				// End Function
				return(true);

			}

			// Get Model Function
			bool GMM(void) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+GMM"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Handle Model Name
				char _Model_Name[15];
				memset(_Model_Name, '\0', 15);
				sscanf(Buffer_Variable, "\r\n%s\r\n\r\nOK\r\n", _Model_Name);

				// Control for Manufacturer Name
				if (strstr(_Model_Name, "GE910-QUAD")) this->Modem.Model = 1;
				if (strstr(_Model_Name, "GE910-QUAD-V3")) this->Modem.Model = 2;

				// End Function
				return(true);

			}

			// Get Firmware Function
			bool GMR(void) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+GMR"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// \r\n16.01.305\r\n\r\nOK\r\n

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// 16.01.305OK

				// Handle Firmware
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n%d.%d.%d\r\n\r\nOK\r\n", &this->Modem.Firmware.Segment_1, &this->Modem.Firmware.Segment_2, &this->Modem.Firmware.Segment_3);

				// Control for IP
				if (_Variable_Count == 3) return (true);

				// End Function
				return (false);

			}

			// Set SLED Function
			bool SLED(const uint8_t _SLED) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SLED="));
				GSM_Serial->print(_SLED);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Execution command saves STAT_LED setting in NVM.
			bool SLEDSAV(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SLEDSAV"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set Socket Listen Ring Indicator Function
			bool E2SLRI(const uint16_t _Pulse_Duration) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
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

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set TxMonMode Function
			bool TXMONMODE(const uint8_t _TXMONMODE) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#TXMONMODE="));
				GSM_Serial->print(_TXMONMODE);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					Buffer.Response = this->Find_OK(Buffer_Variable, Buffer.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer.Read_Order])) Buffer.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}




























			/**
			 * @brief Set configuration parameters to default values.
			 * @param _F Parameter. 
			 * 0 : only the factory profile base section parameters are considered
			 * 1 : either the factory profile base section and the extended section are considered (full factory profile)
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool AT_F(const uint8_t _F) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT&F"));
				GSM_Serial->print(_F);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Soft Reset
			 * @param _Z Parameter. 
			 * 0,1 : user profile number
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Z(const uint8_t _Z) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("ATZ"));
				GSM_Serial->print(_Z);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Define the basic profile that is loaded on startup.
			 * @param _Y Parameter. 
			 * 0,1 : profile number: the wireless module can store 2 complete configurations (see command &W)
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Y(const uint8_t _Y) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT&Y="));
				GSM_Serial->print(_Y);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Define which full profile is loaded at startup.
			 * @param _P Parameter. 
			 * 0,1 : profile number: the wireless module can store 2 complete configurations (see command &W)
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool P(const uint8_t _P) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT&P="));
				GSM_Serial->print(_P);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Stores the complete configuration of the device.
			 * @param _W Parameter. 
			 * 0,1 : profile identifiers
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool W(const uint8_t _W) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT&W"));
				GSM_Serial->print(_W);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			bool AT_Test(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 50000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SEARCHLIM=100,100"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Print Command State
				#ifdef GSM_Debug
					Terminal_GSM.Text(27, 5, CYAN, F("                                                           "));
					Terminal_GSM.Text(27, 5, CYAN, String(Buffer_Variable));
				#endif

				// End Function
				return (true);

			}

			/**
			 * @brief Execution command sets the configuration parameters to default values specified by manufacturer; 
			 * it takes in consideration hardware configuration switches and other manufacturer-defined criteria.
			 * @param _Value 
			 * 0 - just the factory profile base section parameters are considered.
			 * 1 - either the factory profile base section and the extended section are considered
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool ATF(const bool _Value) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT&F"));
				GSM_Serial->print(_Value);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Query SIM Status unsolicited indication
			 * @param _Mode 
			 * @param _Status 
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool QSS(uint8_t & _Mode, uint8_t & _Status) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#QSS?"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// #QSS: 0,1OK

				// Handle Response
				_Mode = Buffer_Variable[6];
				_Status = Buffer_Variable[8];

				// End Function
				return(false);

			}

			/**
			 * @brief Set command enables/disables the Ring Indicator pin response to one or more events. 
			 * If an event has been enabled, a negative going pulse is generated when event happens. 
			 * The duration of this pulse is determined by the value of <duration>.
			 * @param _Event_Mask Parameter
			 * 0 – disables all events hexadecimal number representing the list of events: Mode (same as AT#PSMRI=<duration>)
			 * 1 – Power Saving
			 * 2 – Socket Listen (same as AT#E2SLRI=<duration>)
			 * 4 – OTA firmware upgrade (same as AT#OTASETRI=<duration>)
			 * 8 – MT SMS has been received (same as AT#E2SMSRI=<duration>) 10 – +CREG will change status
			 * 20 – +CGREG will change status
			 * 40 – #QSS become 2 (SIM INSERTED and PIN UNLOCKED)
			 * 80 – MO SMS has been delivered
			 * 100 – Jamming Detection & Reporting (JDR)
			 * @param _Pulse_Duration Parameter
			 * 50 - 1150 : RI enabled for Socket Listen connect
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool E2RI(const uint8_t _Event_Mask, const uint16_t _Pulse_Duration) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#E2RI="));
				GSM_Serial->print(_Event_Mask);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Pulse_Duration);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Set command forces an attempt to select and register the GSM network operator.
			 * @param _Mode Parameter
			 * 0 - automatic choice (the parameter <oper> will be ignored) (factory default)
			 * 1 - manual choice unlocked (network is kept as long as available, then it can be changed with some other suited networks to guarantee the service)
			 * 2 - deregister from GSM network; the MODULE is kept unregistered until a +COPS with <mode>=0, 1, 4 or 5 is issued 
			 * 3 - set only <format> parameter (the parameter <oper> will be ignored)
			 * 4 - manual/automatic (<oper> field shall be present); if manual selection fails, automatic mode (<mode>=0) is entered
			 * 5 - manual choice locked (network is kept fixed, if the chosen network is not available, then the mobile has no service)
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool COPS(const uint8_t _Mode, const uint8_t _Format, const uint16_t _Operator) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 30000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+COPS="));
				GSM_Serial->print(_Mode);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Format);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_Operator);
				GSM_Serial->print(F("\""));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Set command enables/disables network registration reports depending on the parameter <mode>.
			 * @param _Mode Parameter
			 * 0 - Disable network registration unsolicited result code (factory default) 
			 * 1 - Enable network registration unsolicited result code
			 * 2 - Enable network registration unsolicited result code with network Cell identification data
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Set_CREG(const bool _Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CREG="));
				GSM_Serial->print(_Mode);
				GSM_Serial->print(F("\r\n"));

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Get command registration reports depending on the parameter <stat>.
			 * @param _Stat Parameter
			 * 0 - Not registered, ME is not currently searching a new operator to register to 
			 * 1 - Registered, home network
			 * 2 - Not registered, but ME is currently searching a new operator to register to 
			 * 3 - Registration denied
			 * 4 - Unknown
			 * 5 - Registered, roaming
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Get_CREG(uint8_t & _Mode, uint8_t & _Stat) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CREG?"));
				GSM_Serial->print(F("\r\n"));

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Handle Variables
				sscanf(Buffer_Variable, "+CREG: %hhu,%hhuOK", &_Mode, &_Stat);

				// Handle Response
				if (_Stat == 0) return(true);
				if (_Stat == 1) return(true);
				if (_Stat == 2) return(true);
				if (_Stat == 3) return(true);
				if (_Stat == 4) return(true);
				if (_Stat == 5) return(true);

				// End Function
				return (false);

			}

			/**
			 * @brief Set command controls the presentation of an unsolicited result code
			 * @param _Mode Parameter
			 * 0 - disable network registration unsolicited result code
			 * 1 - enable network registration unsolicited result code; if there is a change in the terminal GPRS network registration status, it is issued the unsolicited result
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Set_CGREG(const bool _Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CGREG="));
				GSM_Serial->print(_Mode);
				GSM_Serial->print(F("\r\n"));

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Get command controls the presentation of an unsolicited result code.
			 * @param _Stat Parameter
			 * 0 - Not registered, ME is not currently searching a new operator to register to 
			 * 1 - Registered, home network
			 * 2 - Not registered, but ME is currently searching a new operator to register to 
			 * 3 - Registration denied
			 * 4 - Unknown
			 * 5 - Registered, roaming
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Get_CGREG(uint8_t & _Mode, uint8_t & _Stat) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CGREG?"));
				GSM_Serial->print(F("\r\n"));

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Handle Variables
				sscanf(Buffer_Variable, "+CGREG: %hhu,%hhuOK", &_Mode, &_Stat);

				// Handle Response
				if (_Stat == 0) return(true);
				if (_Stat == 1) return(true);
				if (_Stat == 2) return(true);
				if (_Stat == 3) return(true);
				if (_Stat == 4) return(true);
				if (_Stat == 5) return(true);

				// End Function
				return (false);

			}

			/**
			 * @brief Set command specifies PDP context parameter values for a PDP context identified by the (local) context identification parameter, <cid>
			 * @param _Cid Parameter
			 * (PDP Context Identifier) numeric parameter which specifies a particular PDP context definition.
			 * @param _PDP_Type Parameter
			 * (Packet Data Protocol type) a string parameter which specifies the type of packet data protocol 
			 * "IP" - Internet Protocol
			 * @param _APN Parameter
			 * (Access Point Name) a string parameter which is a logical name that is used to select the GGSN or the external packet data network. If the value is empty (“”) or omitted, then the subscription value will be requested.
			 * @param _PDP_Addr Parameter
			 * a string parameter that identifies the terminal in the address space applicable to the PDP. The allocated address may be read using the +CGPADDR command.
			 * @param _D_Comp Parameter
			 * numeric parameter that controls PDP data compression
			 * 0 - off (default if value is omitted)
			 * 1 - on
			 * @param _H_Comp Parameter
			 * numeric parameter that controls PDP header compression
			 * 0 - off (default if value is omitted)
			 * 1 - on
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool CGDCONT(const uint8_t _Cid, const char * _PDP_Type, const char * _APN, const char * _PDP_Addr, const bool _D_Comp, const bool _H_Comp) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CGDCONT="));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_PDP_Type);
				GSM_Serial->print(F("\",\""));
				GSM_Serial->print(_APN);
				GSM_Serial->print(F("\",\""));
				GSM_Serial->print(_PDP_Addr);
				GSM_Serial->print(F("\","));
				GSM_Serial->print(_D_Comp);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_H_Comp);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Execution command is used to activate or deactivate either the GSM context or the specified PDP context.
			 * @version 01.01.00
			 * @param _Cid Parameter
			 * PDP context identifier
			 * 0 - specifies the GSM context
			 * 1..5 - numeric parameter which specifies a particular PDP context definition
			 * @param _Stat Parameter
			 * 0 - deactivate the context
			 * 1 - activate the context
			 * @param _IP Parameter
			 * IP address obtained from the network.
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Set_SGACT(const uint8_t _Cid, const bool _Stat) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 60000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SGACT="));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Stat);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(30);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// #SGACT: 178.242.9.4OK

				// Handle IP 
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "#SGACT: %d.%d.%d.%dOK", &this->Modem.IP_Address.Segment_1, &this->Modem.IP_Address.Segment_2, &this->Modem.IP_Address.Segment_3, &this->Modem.IP_Address.Segment_4);
	
				// Control for IP
				if (_Variable_Count == 4) return (true);

				// End Function
				return (false);

			}

			/**
			 * @brief Execution command is used to get either the GSM context or the specified PDP context.
			 * @param _Cid Parameter
			 * PDP context identifier
			 * 0 - specifies the GSM context
			 * 1..5 - numeric parameter which specifies a particular PDP context definition
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Get_SGACT(const uint8_t _Cid) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SGACT?"));
				GSM_Serial->print(F("\r\n"));

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Declare Variable
				char _Stat_Response;

				// Handle Stat Response
				if (_Cid == 1) _Stat_Response = Buffer_Variable[10];
				if (_Cid == 2) _Stat_Response = Buffer_Variable[23];
				if (_Cid == 3) _Stat_Response = Buffer_Variable[36];

				// End Function
				if (_Stat_Response == '0') return(false);
				if (_Stat_Response == '1') return(true);

				// End Function
				return(false);

			}

			/**
			 * @brief Execution command is used to activate or deactivate the specified PDP context(s)
			 * @param _Cid Parameter
			 * a numeric parameter which specifies a particular PDP context definition
			 * 0 - specifies the GSM context
			 * 1..5 - numeric parameter which specifies a particular PDP context definition
			 * @param _Stat Parameter
			 * indicates the state of PDP context activation
			 * 0 - deactivate
			 * 1 - activate
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Set_CGACT(const uint8_t _Cid, const bool _State) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 150000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CGACT="));
				GSM_Serial->print(_State);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			/**
			 * @brief Execution command is used to activate or deactivate the specified PDP context(s)
			 * @param _Cid Parameter
			 * a numeric parameter which specifies a particular PDP context definition
			 * 0 - specifies the GSM context
			 * 1..5 - numeric parameter which specifies a particular PDP context definition
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Get_CGACT(const uint8_t _Cid) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CGACT?"));
				GSM_Serial->print(F("\r\n"));

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Declare Variable
				char _Stat_Response;

				// Handle Stat Response
				if (_Cid == 1) _Stat_Response = Buffer_Variable[10];
				if (_Cid == 2) _Stat_Response = Buffer_Variable[23];
				if (_Cid == 3) _Stat_Response = Buffer_Variable[36];

				// End Function
				if (_Stat_Response == '0') return(false);
				if (_Stat_Response == '1') return(true);

				// End Function
				return(false);

			}

			/**
			 * @brief Execution command returns a list of PDP addresses for the specified context identifiers in the format:
			 * @param _Cid Parameter
			 * a numeric parameter which specifies a particular PDP context definition
			 * 0 - specifies the GSM context
			 * 1..5 - numeric parameter which specifies a particular PDP context definition
			 * @param _IP Parameter 
			 * a string that identifies the terminal in the address space applicable to the PDP
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool CGPADDR(const uint8_t _Cid,  char * _IP) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CGPADDR="));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Clear Variables
				memset(_IP, '\0', 16);

				// Declare Handle Variable
				bool _Handle = false;

				// Control for Buffer
				for (uint8_t i = 0; i < Buffer.Read_Order; i++) {

					// Handle Data
					if (((Buffer_Variable[i] < 58 and Buffer_Variable[i] > 47) or Buffer_Variable[i] == 46) and _Handle == true) {

						// Get Data
						_IP[Buffer.Data_Order] = Buffer_Variable[i];

						// Increase Data Order
						Buffer.Data_Order++;

					}

					// Set Handle
					if (Buffer_Variable[i] == 34) _Handle = true;

				}

				// End Function
				return(true);

			}

			// ******************** Hardware Commands ********************

			// Detech From Network and Shut Down Modem Function
			bool SHDN(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SHDN"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Modem Hard Reset Function
			bool CMAR(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 35000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CMAR=\"00000000\""));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// ******************** Info Commands ********************

			// Get Error Code Function
			bool CEER(uint16_t & _Code) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#CEER"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Serial.println(Buffer_Variable);

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "#CEER: %03dOK", &_Code);

				// Control for Variable
				if (_Variable_Count == 1) return(true);

				// End Function
				return(false);


			}

			// ******************** Network Commands ********************

			// Ping Enable/Disable Function
			bool ICMP(const uint8_t _Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#ICMP="));
				GSM_Serial->print(_Mode);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Firewall Add/Remove Function
			bool FRWL(const uint8_t _Action, const char *_IP_Addr) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FRWL="));
				GSM_Serial->print(_Action);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_IP_Addr);
				GSM_Serial->print(F("\",\"255.255.255.0\""));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// ******************** Operator Commands ********************

			// Set RegMode Function
			bool REGMODE(const uint8_t _REGMODE) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#REGMODE="));
				GSM_Serial->print(_REGMODE);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set AutoBnd Function
			bool AUTOBND(const uint8_t _Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#AUTOBND="));
				GSM_Serial->print(_Mode);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Get Signal Quality Function
			bool CSQ(uint8_t & _RSSI) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CSQ"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Clear Variables
				_RSSI = 0;

				// Declare Response Data
				char _CSQ[2]; 
				memset(_CSQ, '\0', 2);

				// Declare Handle Data
				bool _Handle = true;

				// Control for Buffer
				for (uint8_t i = 0; i < Buffer.Read_Order; i++) {

					// Handle Data
					if (Buffer_Variable[i] <= '9' and Buffer_Variable[i] >= '0' and _Handle == true) {

						// Get Data
						_CSQ[Buffer.Data_Order] = Buffer_Variable[i];

						// Increase Data Order
						Buffer.Data_Order++;

					}

					// Set Handle Pointer
					if (Buffer_Variable[i] == ',') _Handle = false;

				}

				// Set Signal Variable
				_RSSI = atoi(_CSQ);

				// End Function
				return (true);

			}

			// Get Operator Info Function
			bool SERVINFO(uint16_t & _Operator, uint16_t & _BARFCN, uint16_t & _dBM, uint16_t & _BSIC, uint16_t & _TA, uint16_t & _GPRS, char * _LAC) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SERVINFO"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// #SERVINFO: 3,-101,"Turkcell","28601",52,855E,03,1,,"II",01,6OK
				// 3,101,,28601,52,855,03,1,,,01,6

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "#SERVINFO: %d,-%d,\"Turkcell\",\"%d\",%d,%4c,%d,%d,,\"II\",01,6OK", &_BARFCN, &_dBM, &_Operator, &_BSIC, _LAC, &_TA, &_GPRS);

				// #SERVINFO: 3,-101,"Turkcell","28601",52,855E,03,1,,"II",01,6OK
				// #SERVINFO: <B-ARFCN>,<dBM>,<NetNameAsc>,<NetCode>,<BSIC>,<LAC>,<TA>,<GPRS>[,[<PB-ARFCN>],[<NOM>],<RAC>[,<PAT>]]

				// Handle Operator
				if (_Variable_Count == 7) {

					// End Function
					return (true);

				} else {

					// End Function
					return (false);

				}

			}

			// Get Cell Info Function
			bool MONI(char * _LAC, char * _Cell_ID) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#MONI"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Define Variables
				int _BSIC;
				int _RxQual;

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "#MONI: Turkcell BSIC:%d RxQual:%d LAC:%4c Id:%4c ARFCN:3 PWR:-94dbm TA:3OK", &_BSIC, &_RxQual, _LAC, _Cell_ID);

				// Control for Variable
				if (_Variable_Count == 4) {

					// End Function
					return(true);

				} else {

					// End Function
					return(false);

				}

			}

			// Get Cell Info Function
			bool MONIZIP(void) {

				// Declare Variable Structure
				struct Operator_Structure {
					int 	BSIC			= 0;
					int 	QUAL			= 0;
					int		ARFCN			= 0;
					int		TIMADV			= 0;
				} Operator;

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 10000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#MONIZIP"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// #MONIZIP: 28601,23,0,855E,CCF3,60,-92,3OK
				// #MONIZIP: 28601,23,0,855E,CCF3,60,-93,0OK
				// #MONIZIP: 28601,23,0,855E,CCF3,60,-91,3OK
				// #MONIZIP: 28601,23,0,855E,CCF3,60,-92,0OK
				// #MONIZIP: 28602,65,0,D3D6,5DAA,119,-73,3OK

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "#MONIZIP: %05d,%02d,%01d,%04X,%04X,%03d,-%hhu,%01d", &this->Modem.Operator, &Operator.BSIC, &Operator.QUAL, &this->Modem.LAC, &this->Modem.Cell_ID, &Operator.ARFCN, &this->Modem.dBm, &Operator.TIMADV);

				// Calculate Signal Level
				if (this->Modem.dBm >= 51 and this->Modem.dBm < 65) this->Modem.Signal = 5;
				if (this->Modem.dBm >= 66 and this->Modem.dBm < 77) this->Modem.Signal = 4;
				if (this->Modem.dBm >= 78 and this->Modem.dBm < 89) this->Modem.Signal = 3;
				if (this->Modem.dBm >= 90 and this->Modem.dBm < 101) this->Modem.Signal = 2;
				if (this->Modem.dBm >= 102 and this->Modem.dBm < 111) this->Modem.Signal = 1;
				if (this->Modem.dBm >= 112 and this->Modem.dBm < 113) this->Modem.Signal = 0;

				// Control for Variable
				if (_Variable_Count == 8) return(true);

				// End Function
				return(false);

			}

			// ******************** RTC Commands ********************

			// Enable or Disable Automatic TimeZone Update Function
			bool CTZU(const bool _State) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CTZU="));
				GSM_Serial->print(String(_State));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Enable or Disable Network Time UnSolicited Function
			bool NITZ(const bool _State) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#NITZ="));
				GSM_Serial->print(_State);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set NTP Server Function
			bool NTP(const char *_NTP_Addr, const uint8_t _NTP_Port, const bool _Update_Module_Clock, const uint8_t _Time_Out, uint8_t & _Year, uint8_t & _Month, uint8_t & _Day, uint8_t & _Hour, uint8_t & _Minute, uint8_t & _Second) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#NTP=\""));
				GSM_Serial->print(_NTP_Addr);
				GSM_Serial->print(F("\","));
				GSM_Serial->print(_NTP_Port);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Update_Module_Clock);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Time_Out);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// #NTP: 22/05/31,13:30:00OK

				// Handle Time
				_Year = (((Buffer_Variable[6] - 48) * 10) + (Buffer_Variable[7] - 48));
				_Month = (((Buffer_Variable[9] - 48) * 10) + (Buffer_Variable[10] - 48));
				_Day = (((Buffer_Variable[12] - 48) * 10) + (Buffer_Variable[13] - 48));
				_Hour = (((Buffer_Variable[15] - 48) * 10) + (Buffer_Variable[16] - 48));
				_Minute = (((Buffer_Variable[18] - 48) * 10) + (Buffer_Variable[19] - 48));
				_Second = (((Buffer_Variable[21] - 48) * 10) + (Buffer_Variable[22] - 48));

				// Control for Variables
				if (_Year > 24 and _Year < 22) return(false);	
				if (_Month > 12 and _Month < 0) return(false);	
				if (_Day > 31 and _Day < 0) return(false);	
				if (_Hour > 24 and _Hour < 0) return(false);	
				if (_Minute > 59 and _Minute < 0) return(false);	
				if (_Second > 59 and _Second < 0) return(false);	

				// End Function
				return (true);

			}

			// Get Clock Function
			bool CCLK(uint8_t & _Year, uint8_t & _Month, uint8_t & _Day, uint8_t & _Hour, uint8_t & _Minute, uint8_t & _Second) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CCLK?"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// +CCLK: "22/05/31,13:06:06+00"OK

				// Handle Time
				_Year = (((Buffer_Variable[8] - 48) * 10) + (Buffer_Variable[9] - 48));
				_Month = (((Buffer_Variable[11] - 48) * 10) + (Buffer_Variable[12] - 48));
				_Day = (((Buffer_Variable[14] - 48) * 10) + (Buffer_Variable[15] - 48));
				_Hour = (((Buffer_Variable[17] - 48) * 10) + (Buffer_Variable[18] - 48));
				_Minute = (((Buffer_Variable[20] - 48) * 10) + (Buffer_Variable[21] - 48));
				_Second = (((Buffer_Variable[23] - 48) * 10) + (Buffer_Variable[24] - 48));

				// Control for Variables
				if (_Year > 24 and _Year < 22) return(false);	
				if (_Month > 12 and _Month < 0) return(false);	
				if (_Day > 31 and _Day < 0) return(false);	
				if (_Hour > 24 and _Hour < 0) return(false);	
				if (_Minute > 59 and _Minute < 0) return(false);	
				if (_Second > 59 and _Second < 0) return(false);
				
				// End Function
				return (true);

			}

			// ******************** Socket Commands ********************

			// Socket Configuration Function
			bool SCFG(const uint8_t _Conn_ID, const uint8_t _Cid, const uint16_t _Pkt_Sz, const uint16_t _Max_To, const uint16_t _Conn_To, const uint8_t _TX_To) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

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
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Extended Socket Configuration Function
			bool SCFGEXT(const uint8_t _Conn_ID, const uint8_t _Sr_Mode, const uint8_t _Recv_Data_Mode, const uint8_t _Keep_Alive, const uint8_t _Listen_Auto_Rsp, const uint8_t _Send_Data_Mode) {

				// <connId> - socket connection identifier (1..6)
				// <srMode> - SRing unsolicited mode
				// <recvDataMode> - data view mode for received data in command mode(AT#SRECV or <srMode> = 2)
				// <keepalive> - Set the TCP Keepalive value in minutes
				// <ListenAutoRsp> - Set the listen auto-response mode, that affects the commands AT#SL and AT#SLUDP
				// <sendDataMode> - data mode for sending data in command mode(AT#SSEND)

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

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
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Extended 2 Socket Configuration Function
			bool SCFGEXT2(const uint8_t _Conn_ID, const uint8_t _Buffer_Start, const uint8_t _Abort_Conn_Attempt, const uint8_t _SRing_Len, const uint8_t _SRing_To, const uint8_t _No_Carrier_Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

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
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Extended 3 Socket Configuration Function
			bool SCFGEXT3(const uint8_t _Conn_ID, const uint8_t _Imm_Rsp, const uint8_t _Closure_Type_Cmd_Mode_Enabling) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SCFGEXT3="));
				GSM_Serial->print(_Conn_ID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Imm_Rsp);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Closure_Type_Cmd_Mode_Enabling);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Socket Status Function
			bool SS(const uint8_t _ConnID, uint8_t & _State) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SS="));
				GSM_Serial->print(String(_ConnID));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Handle State
				_State = (Buffer_Variable[7] - 48);

				// End Function
				return (true);

			}

			// Socket Listen Function
			bool SL(const uint8_t _ConnID, const bool _Listen_State, const uint16_t _Listen_Port, const uint8_t _Closure_Type) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

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
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Close Socket Function
			bool SH(const uint8_t _ConnID) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SH="));
				GSM_Serial->print(String(_ConnID));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// ReOpen Socket Function
			bool SO(const uint8_t _ConnID) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SO="));
				GSM_Serial->print(String(_ConnID));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Socket Answer Function
			bool SA(const uint8_t _ConnID, const uint8_t _ConnMode, uint16_t & _Length) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 15000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SA="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_ConnMode);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Read Current Time
				Current_Time = millis();

				// Declare Buffer
				memset(Buffer_Variable, '\0', 255);

				// Declare Buffer Object
				Serial_Buffer Buffer_Get = {false, 0, 0, 25000};

				// Declare Handle Variable
				bool Comma_Handle = false;

				// Declare Response Array
				char Length_Array[3];

				// Read UART Response
				while (!Buffer_Get.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer_Get.Read_Order] = GSM_Serial->read();

					// Get Length
					if (Comma_Handle) {

						// Get Length
						if (Buffer_Variable[Buffer_Get.Read_Order] > 47 and Buffer_Variable[Buffer_Get.Read_Order] < 58) {

							// Read Data	
							Length_Array[Buffer_Get.Data_Order] = Buffer_Variable[Buffer_Get.Read_Order];

							// Handle Data Order
							Buffer_Get.Data_Order += 1;

						}

						// Set Handle
						if (Buffer_Variable[Buffer_Get.Read_Order] == '\n') Buffer_Get.Response = true;

					}

					// Set Handle
					if (Buffer_Variable[Buffer_Get.Read_Order] == ',') Comma_Handle = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer_Get.Read_Order] > 31 and Buffer_Variable[Buffer_Get.Read_Order] < 127) Buffer_Get.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer_Get.Time_Out) return(false);

				}

				// Convert char to int
				_Length = atoi(Length_Array);

				// End Function
				return (true);

			}

			// Socket Pack Send Function
			bool SSEND(const uint8_t _ConnID, const uint8_t _Header_Type, const uint16_t _Response_Code, const char * _IP, const char * _URL, const char * _Data_Pack) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer_Set = {false, 0, 0, 2000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

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
				this->Send_Header(_Header_Type, _Response_Code, _IP, _URL, _Data_Pack);

				// Send Data Pack
				GSM_Serial->print(_Data_Pack);

				// Print End Char
				GSM_Serial->print((char)26);

				// Declare Buffer Object
				Serial_Buffer Buffer_Get = {false, 0, 0, 2000};

				// Declare Buffer
				memset(Buffer_Variable, '\0', 255);

				// Read Current Time
				Current_Time = millis();

				// Response Wait Delay
				delay(10);

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

				// End Function
				return (true);

			}

			// Socket Recieve Function
			bool SRECV(const uint8_t _ConnID, const uint16_t _MaxByte, char * _Data) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SRECV="));
				GSM_Serial->print(_ConnID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_MaxByte);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer.Read_Order > 30 and Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 32 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Declare Data Handle Variable
				bool Data_Handle = false;

				// Declare Data Order
				int Data_Order = 0;

				// Control for Buffer
				for (uint16_t i = 10; i <= Buffer.Read_Order; i++) {

					// Handle JSON Data
					if (Buffer_Variable[i] == '{') Data_Handle = true;

					// Get Data
					if (Data_Handle) {
						_Data[Data_Order] = Buffer_Variable[i];
						Data_Order += 1;
					}

					// Handle JSON Data
					if (Buffer_Variable[i-1] == '}') Data_Handle = false;

				}

				// End Function
				return (true);

			}

			// Socket Dial Function
			bool SocketDial(const uint8_t _Cid, const uint8_t _Protocol, const uint8_t _Port, const uint8_t _Closure_Type, uint16_t _IPort, const bool _Conn_Mode, const char *_IP) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 50000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

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
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				uint32_t Current_Time = millis();

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == 'C' and Buffer_Variable[Buffer.Read_Order - 1] == 'M' and Buffer_Variable[Buffer.Read_Order] == 'E') return(false);

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set HTTP Connection Function
			bool HTTPCFG(const char *_HTTP_Server, const uint8_t _Port) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 1000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#HTTPCFG=1,\""));
				GSM_Serial->print(_HTTP_Server);
				GSM_Serial->print(F("\","));
				GSM_Serial->print(_Port);
				GSM_Serial->print(F(",0,\"\",\"\",0,30,1"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// HTTP Send Function
			bool HTTPSND(const uint8_t _Prof_ID, const uint8_t _Command, const char *_URL, const char *_Data) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer_Set = {false, 0, 0, 20000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#HTTPSND="));
				GSM_Serial->print(_Prof_ID);
				GSM_Serial->print(F(","));
				GSM_Serial->print(_Command);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_URL);
				GSM_Serial->print(F("\","));
				GSM_Serial->print(String(_Data).length());
				GSM_Serial->print(F(",\"application/json\""));
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
					if (Buffer_Variable[Buffer_Set.Read_Order - 2] == '>' and Buffer_Variable[Buffer_Set.Read_Order - 1] == '>' and Buffer_Variable[Buffer_Set.Read_Order] == '>') Buffer_Set.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer_Set.Read_Order] > 31 and Buffer_Variable[Buffer_Set.Read_Order] < 127) Buffer_Set.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer_Set.Time_Out) return(false);

				}

				// Send Delay
				delay(50);

				// Print Data
				GSM_Serial->print(_Data);

				// Send Delay
				delay(50);

				// Declare Buffer Object
				Serial_Buffer Buffer_Get = {false, 0, 0, 20000};

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

				// End Function
				return (true);

			}

			// HTTP Recieve Function
			bool HTTPRCV(const uint8_t _Prof_ID, char * _Response) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 20000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#HTTPRCV="));
				GSM_Serial->print(_Prof_ID);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == '<' and Buffer_Variable[Buffer.Read_Order - 1] == '<' and Buffer_Variable[Buffer.Read_Order] == '<') Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// Command Work Delay
				delay(10);

				// Declare Buffer Object
				Serial_Buffer Data_Buffer = {false, 0, 0, 20000};

				// Read Current Time
				Current_Time = millis();

				// Read UART Response
				while (!Data_Buffer.Response) {

					// Read Serial Char
					_Response[Data_Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (_Response[Data_Buffer.Read_Order] == '\n') Data_Buffer.Response = true;

					// Increase Read Order
					if (_Response[Data_Buffer.Read_Order] > 31 and _Response[Data_Buffer.Read_Order] < 127) Data_Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Data_Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// ******************** FTP Commands ********************

			// Open FTP Connection Function
			bool FTPOPEN(const char * _Server, const char * _Username, const char * _Password, const uint8_t _Mode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 100000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

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
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Control for <CME> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == 'C' and Buffer_Variable[Buffer.Read_Order - 1] == 'M' and Buffer_Variable[Buffer.Read_Order] == 'E') return(false);

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Close FTP Connection Function
			bool FTPCLOSE(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 500000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPCLOSE"));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Control for <CME> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == 'C' and Buffer_Variable[Buffer.Read_Order - 1] == 'M' and Buffer_Variable[Buffer.Read_Order] == 'E') return(false);

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Set FTP Time Out Function
			bool FTPTO(const uint16_t _TOut) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPTO="));
				GSM_Serial->print(_TOut);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Control for <CME> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == 'C' and Buffer_Variable[Buffer.Read_Order - 1] == 'M' and Buffer_Variable[Buffer.Read_Order] == 'E') return(false);

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Change FTP Folder Function
			bool FTPCWD(const char * _Folder) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 500000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPCWD="));
				GSM_Serial->print(F("\""));
				GSM_Serial->print(_Folder);
				GSM_Serial->print(F("\""));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Control for <CME> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == 'C' and Buffer_Variable[Buffer.Read_Order - 1] == 'M' and Buffer_Variable[Buffer.Read_Order] == 'E') return(false);

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Get FTP File to Buffer function
			bool FTPGETPKT(const uint16_t _FileID, const uint8_t _ViewMode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 500000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPGETPKT="));
				GSM_Serial->print(F("\""));
				GSM_Serial->print(_FileID);
				GSM_Serial->print(F(".hex"));
				GSM_Serial->print(F("\","));
				GSM_Serial->print(_ViewMode);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Control for <CME> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == 'C' and Buffer_Variable[Buffer.Read_Order - 1] == 'M' and Buffer_Variable[Buffer.Read_Order] == 'E') return(false);

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Get FTP File From Buffer Function
			bool FTPRECV(const uint16_t _Size, uint16_t & _ReadSize, char * _Data) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 500000};

				// Clear Variable
				_ReadSize = 0;

				// Send UART Command
				GSM_Serial->print(F("AT#FTPRECV="));
				GSM_Serial->print(_Size);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Clear Variable
				Buffer.Read_Order = 0;

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 3] == 'O' and Buffer_Variable[Buffer.Read_Order - 2] == 'K') break;

					// Increase Read Order
					if ((Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) or Buffer_Variable[Buffer.Read_Order] == 13 or Buffer_Variable[Buffer.Read_Order] == 10) {

						// UART Delay
						delayMicroseconds(10);

						// Handle Data Orders
						Buffer.Read_Order += 1;

					} else {

						// UART Delay
						delayMicroseconds(50);

					}

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// \r\n#FTPRECV: 150\r\n:100000000C9423070C9454070C9454070C94540735\r\n:100010000C9454070C9454070C9454070C945407F4\r\n:100020000C9454070C9454070C945E3F0C945407A2\r\n:100030000C9454\r\n\r\nOK\r\n
				// \r\n#FTPRECV: 150\r\n0E0F701EC0F66\r\n:100A7000FD1F6491662361F0D801ED91FC91019016\r\n:100A8000F081E02DC8011995892B11F02196EECF48\r\n:100A9000CE01DF91CF911F910F91FF90EF900895BC\r\n\r\n\r\nOK\r\n
				// \r\n#FTPRECV: 150\r\n   \r\n:10007000FF2059F31836B1F11C3609F43DC01123A5\r\n:1000800021F3612F70E084EA91E00E945483892B70\r\n:10009000E1F2912C812C00FD06C0F5018080918059\r\n:1000A000C501\r\n\r\nOK\r\n

				// rn#FTPRECV: 200rn2710F931F9392rn:10E2B000CF93DF93EC010E944F718C01CE010E943Drn:10E2C0007F70800F911FDF91CF911F910F91089563rn:10E2D0009A01AB0170E060E00C944271FC01808116rn:10E2E00091810C945C79CF93DF93EC0188819981C3rn:10E2rnrnOKrn
				
				// rn#FTPRECV: 1rn
				// rn#FTPRECV: 10rn
				// rn#FTPRECV: 150rn2710F93....

				// Declare Size Array Variable
				char _Header[20];
				memset(_Header, '\0', 20);

				// Handle Pack Size
				for (uint8_t i = 0; i < 20; i++) _Header[i] = Buffer_Variable[i];

				// Handle Variables
				sscanf(_Header, "\r\n#FTPRECV: %03d\r\n", &_ReadSize);

				// Calculate Header Size
				uint16_t _HeaderSize = 14 + this->CountDigits(_ReadSize);

				// Size : 145
				// rn#FTPRECV: 120rn0E942C08C8010E9490rn:1012C0006E718FE892ECA1EBB1E48093E10490930Ern:1012D000E204A093E304B093E4048CEE91E5A5E06Ern:1012E000BrnrnOKrn
				// 17 - 120 - 8

				// Clear Variable
				memset(_Data, '\0', 250);
				Buffer.Data_Order = 0;

				// Get Data
				for (uint8_t i = _HeaderSize; i < (Buffer.Read_Order - 7); i++) {

					// Delay
					delayMicroseconds(10);

					// Set Buffer
					if ((Buffer_Variable[i] > 31 and Buffer_Variable[i] < 127) or Buffer_Variable[i] == 13 or Buffer_Variable[i] == 10) {
						
						_Data[Buffer.Data_Order] = Buffer_Variable[i];

						Buffer.Data_Order++;
				
					} else {

						Buffer.Data_Order = 0;
						i = _HeaderSize;

					}

				}

				// End Function
				if (_ReadSize > 0) return (true);
				
				// Error Delay
				delay(5000);

				// End Function
				return(false);

			}

			// Set FTP File Transfer Type Function
			bool FTPTYPE(const uint8_t _Type) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 500000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPTYPE="));
				GSM_Serial->print(_Type);
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Response Wait Delay
				delay(10);

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Control for <CME> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == 'C' and Buffer_Variable[Buffer.Read_Order - 1] == 'M' and Buffer_Variable[Buffer.Read_Order] == 'E') return(false);

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return (true);

			}

			// Get FTP File Size Function
			bool FTPFSIZE(const uint16_t _FileID, uint32_t & _Length) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 5000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPFSIZE="));
				GSM_Serial->print(F("\""));
				GSM_Serial->print(_FileID);
				GSM_Serial->print(F(".hex"));
				GSM_Serial->print(F("\""));
				GSM_Serial->print(F("\r\n"));

				// Read Current Time
				const uint32_t Current_Time = millis();

				// \r\n#FTPFSIZE: 174945\r\n\r\nOK\r\n

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (Buffer_Variable[Buffer.Read_Order - 1] == 'O' and Buffer_Variable[Buffer.Read_Order] == 'K') Buffer.Response = true;

					// Control for <CME> Response
					if (Buffer_Variable[Buffer.Read_Order - 2] == 'C' and Buffer_Variable[Buffer.Read_Order - 1] == 'M' and Buffer_Variable[Buffer.Read_Order] == 'E') return(false);

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 32 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// #FTPFSIZE:174945OK

				// Handle Variables
				sscanf(Buffer_Variable, "#FTPFSIZE:%luOK", &_Length);

				// End Function
				return (true);

			}









			// Detect SRING Response.
			bool Send_SRING(uint16_t &_Length) {

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 25000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

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

				// Handle Variables
				uint8_t _Parsed = sscanf(Buffer_Variable, "\r\nSRING: 3,%d\r\n", &_Length);

				// End Function
				if (_Parsed ==1) return(true);
				return(false);

			}

			/**
			 * @brief Detect SRING Response.
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Receive_SRING(void) {

				// Declare Buffer Object
				Serial_Buffer Buffer = {false, 0, 0, 15000};

				// Declare Buffer
				char Buffer_Variable[255];
				memset(Buffer_Variable, '\0', 255);

				// Read Current Time
				const uint32_t Current_Time = millis();

				// Read UART Response
				while (!Buffer.Response) {

					// Read Serial Char
					Buffer_Variable[Buffer.Read_Order] = GSM_Serial->read();

					// Control for <OK> Response
					if (
						Buffer_Variable[Buffer.Read_Order - 7] == 'S' and 
						Buffer_Variable[Buffer.Read_Order - 6] == 'R' and 
						Buffer_Variable[Buffer.Read_Order - 5] == 'I' and 
						Buffer_Variable[Buffer.Read_Order - 4] == 'N' and 
						Buffer_Variable[Buffer.Read_Order - 3] == 'G' and 
						Buffer_Variable[Buffer.Read_Order - 2] == ':'
						) Buffer.Response = true;

					// Increase Read Order
					if (Buffer_Variable[Buffer.Read_Order] > 31 and Buffer_Variable[Buffer.Read_Order] < 127) Buffer.Read_Order += 1;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer.Time_Out) return(false);

				}

				// End Function
				return(true);

			}

			/**
			 * @brief Send HTTP Pack Header
			 * @param _Header_Type Parameter
			 * 1 - Response
			 * 2 - Send
			 * @param _IP Parameter
			 * @param _URL Parameter
			 * @param _Data Parameter
			 * @return true Function is success.
			 * @return false Function fail.
			 */
			bool Send_Header(const uint8_t _Header_Type, const uint16_t _Response_Code, const char *_IP, const char *_URL, const char *_Data) {

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

				}

				// End Function
				return(false);

			}

	};

#endif /* defined(__Telit_AT_Command_Set__) */