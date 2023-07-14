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

			// Define Module Structure
			struct Struct_Module {

				// SIM State
				bool 		SIM_State = false;

				// Manufacturer and Model
				uint8_t 	Manufacturer = 0;
				uint8_t 	Model = 0;

				// Modem IMEI
				char 		IMEI[17];

				// GSM Serial ID
				uint32_t	Serial_ID = 0;

				// IP Address Struct
				struct Struct_Firmware {
					uint16_t	Segment_1 = 0;
					uint16_t	Segment_2 = 0;
					uint16_t	Segment_3 = 0;
				} Firmware;

			} GSM_Module;

			// Define Network Structure
			struct Struct_Network {

				// PIN State
				uint8_t 	PIN;

				// SIM ICCID
				char 		ICCID[21];

				// IP Address Struct
				struct Struct_IP {
					uint16_t	Segment_1;
					uint16_t	Segment_2;
					uint16_t	Segment_3;
					uint16_t	Segment_4;
				} IP_Address;

				// Connection Time
				uint8_t 	Connection_Time;

				// Operator
				uint16_t 	Operator;

				// Location
				uint16_t	LAC;
				uint16_t	Cell_ID;

				// Signal Level
				uint8_t 	dBm;
				uint8_t		Signal;

			} GSM_Operator;

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

			// CME Find Function
			bool Find_CME(char * _Buffer, uint8_t _Size) {

				// Control for <\r\n+CME> Response
				if (_Buffer[_Size - 3] == '+' and _Buffer[_Size - 2] == 'C' and _Buffer[_Size - 1] == 'M' and _Buffer[_Size] == 'E') return(true);

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

			// Set configuration parameters to default values.
			bool ATF(const uint8_t _F) {

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
				GSM_Serial->print(F("AT&F"));
				GSM_Serial->print(_F);
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

			// Soft Reset Function
			bool Z(const uint8_t _Z) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					5000, 	// Time Out
					30		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("ATZ"));
				GSM_Serial->print(_Z);
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

			// Detech From Network and Shut Down Modem Function
			bool SHDN(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					5000, 	// Time Out
					30		// Buffer Size
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

			// Modem Hard Reset Function
			bool CMAR(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					35000, 	// Time Out
					30		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CMAR=\"00000000\""));
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

			// Get Model Function
			bool SIMDET_QUERY(void) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SIMDET?"));
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

				// \r\n#SIMDET: 2,1\r\n\r\nOK\r\n

				// Handle Model Name
				int _Sim_State = 0;
				sscanf(Buffer_Variable, "\r\n#SIMDET: 2,%d\r\n\r\nOK\r\n", &_Sim_State);

				// Control for SIM State
				if (_Sim_State == 1) {

					// Set SIM State
					this->GSM_Module.SIM_State = true;

					// End Function
					return(true);

				}
				
				// End Function
				return(false);

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

			// Get SIM PIN Status Function
			bool CPIN(void) {

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
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CPIN?"));
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

				// \r\n+CPIN: READY\r\n\r\nOK\r\n

				// Handle Model Name
				char _PIN_Response[15];
				memset(_PIN_Response, '\0', 15);
				sscanf(Buffer_Variable, "\r\n+CPIN: %s\r\n\r\nOK\r\n", _PIN_Response);

				// Declare Variable
				this->GSM_Operator.PIN = 0;

				// Control for Manufacturer Name
				if (strstr(_PIN_Response, "READY")) this->GSM_Operator.PIN = 1;
				if (strstr(_PIN_Response, "SIM PIN")) this->GSM_Operator.PIN = 2;
				if (strstr(_PIN_Response, "SIM PUK")) this->GSM_Operator.PIN = 3;

				// Handle Status
				if (this->GSM_Operator.PIN == 1) return(true);

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
				memset(this->GSM_Module.IMEI, '\0', 17);

				// Handle for Response
				for (size_t i = 0; i < Buffer.Size; i++) {

					// Handle IMEI
					if (Buffer_Variable[i] > 47 and Buffer_Variable[i] < 58) {

						// Set IMEI Variable
						this->GSM_Module.IMEI[Buffer.Data_Order] = Buffer_Variable[i];

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
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "%10luOK", &this->GSM_Module.Serial_ID);

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
					37		// Buffer Size
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
				memset(this->GSM_Operator.ICCID, '\0', 21);

				// Handle for Response
				for (size_t i = 0; i < Buffer.Size; i++) {

					// Handle IMEI
					if (Buffer_Variable[i] > 47 and Buffer_Variable[i] < 58) {

						// Set IMEI Variable
						this->GSM_Operator.ICCID[Buffer.Data_Order] = Buffer_Variable[i];

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

				// Set No Manufacturer
				this->GSM_Module.Manufacturer = 0;

				// Control for Manufacturer Name
				if (strstr(_Manufacruer_Name, "Telit")) this->GSM_Module.Manufacturer = 1;

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

				// Set No Model
				this->GSM_Module.Model = 0;

				// Control for Manufacturer Name
				if (strstr(_Model_Name, "GE910-QUAD")) this->GSM_Module.Model = 1;
				if (strstr(_Model_Name, "GE910-QUAD-V3")) this->GSM_Module.Model = 2;

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
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n%d.%d.%d\r\n\r\nOK\r\n", &this->GSM_Module.Firmware.Segment_1, &this->GSM_Module.Firmware.Segment_2, &this->GSM_Module.Firmware.Segment_3);

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

			// Set Event to Ring Indicator Function
			bool E2RI(const uint8_t _Event_Mask, const uint16_t _Pulse_Duration) {

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
				GSM_Serial->print(F("AT#E2RI="));
				GSM_Serial->print(_Event_Mask, HEX);
				GSM_Serial->print(F(","));
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

			// Set RegMode Function
			bool REGMODE(const uint8_t _REGMODE) {

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
				GSM_Serial->print(F("AT#REGMODE="));
				GSM_Serial->print(_REGMODE);
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

			// Set Operator Selection Function
			bool COPS(const uint8_t _Mode) {

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
				GSM_Serial->print(F("AT+COPS="));
				GSM_Serial->print(_Mode);
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

			// Set AutoBnd Function
			bool AUTOBND(const uint8_t _Mode) {

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
				GSM_Serial->print(F("AT#AUTOBND="));
				GSM_Serial->print(_Mode);
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

			// Set CREG Function
			bool Set_CREG(const bool _Mode) {

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
				GSM_Serial->print(F("AT+CREG="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

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

			// Get CREG Function
			bool Get_CREG(uint8_t & _Mode, uint8_t & _Stat) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CREG?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

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

				// Handle Variables
				sscanf(Buffer_Variable, "\r\n+CREG: %hhu,%hhu\r\n\r\nOK\r\n", &_Mode, &_Stat);

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

			// Set CGREG Function
			bool Set_CGREG(const bool _Mode) {

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
				GSM_Serial->print(F("AT+CGREG="));
				GSM_Serial->print(_Mode);
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

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

			// Get CGREG Function
			bool Get_CGREG(uint8_t & _Mode, uint8_t & _Stat) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CGREG?"));
				GSM_Serial->write(0x0D);
				GSM_Serial->write(0x0A);

				// Response Wait Delay
				delay(10);

				// Read Current Time
				const uint32_t Current_Time = millis();

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

				// Handle Variables
				sscanf(Buffer_Variable, "\r\n+CGREG: %hhu,%hhu\r\n\r\nOK\r\n", &_Mode, &_Stat);

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#CEER"));
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

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n#CEER: %03d\r\n\r\nOK\r\n", &_Code);

				// Control for Variable
				if (_Variable_Count == 1) return(true);

				// End Function
				return(false);

			}

			// Set CGDCONT Function
			bool CGDCONT(const uint8_t _Cid, const char * _PDP_Type, const char * _APN) {

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
				GSM_Serial->print(F("AT+CGDCONT="));
				GSM_Serial->print(_Cid);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_PDP_Type);
				GSM_Serial->print(F("\",\""));
				GSM_Serial->print(_APN);
				GSM_Serial->print(F("\""));
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

			// Set SGACT Function
			bool Set_SGACT(const uint8_t _Cid, const bool _Stat) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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

				// \r\n#SGACT: 000.000.000.000\r\n\r\nOK\r\n

				// Handle IP 
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n#SGACT: %d.%d.%d.%d\r\n\r\nOK\r\n", &this->GSM_Operator.IP_Address.Segment_1, &this->GSM_Operator.IP_Address.Segment_2, &this->GSM_Operator.IP_Address.Segment_3, &this->GSM_Operator.IP_Address.Segment_4);
	
				// Control for IP
				if (_Variable_Count == 4) return (true);

				// End Function
				return (false);

			}

			// Socket Configuration Function
			bool SCFG(const uint8_t _Conn_ID, const uint8_t _Cid, const uint16_t _Pkt_Sz, const uint16_t _Max_To, const uint16_t _Conn_To, const uint8_t _TX_To) {

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

			// Extended Socket Configuration Function
			bool SCFGEXT(const uint8_t _Conn_ID, const uint8_t _Sr_Mode, const uint8_t _Recv_Data_Mode, const uint8_t _Keep_Alive, const uint8_t _Listen_Auto_Rsp, const uint8_t _Send_Data_Mode) {

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

			// Extended 2 Socket Configuration Function
			bool SCFGEXT2(const uint8_t _Conn_ID, const uint8_t _Buffer_Start, const uint8_t _Abort_Conn_Attempt, const uint8_t _SRing_Len, const uint8_t _SRing_To, const uint8_t _No_Carrier_Mode) {

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

			// Firewall Add/Remove Function
			bool FRWL(const uint8_t _Action, const char *_IP_Addr) {

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
				GSM_Serial->print(F("AT#FRWL="));
				GSM_Serial->print(_Action);
				GSM_Serial->print(F(",\""));
				GSM_Serial->print(_IP_Addr);
				GSM_Serial->print(F("\",\"255.255.255.0\""));
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

			// Ping Enable/Disable Function
			bool ICMP(const uint8_t _Mode) {

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
				GSM_Serial->print(F("AT#ICMP="));
				GSM_Serial->print(_Mode);
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
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					1000, 	// Time Out
					60		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#MONIZIP"));
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

				// #MONIZIP: 28601,23,0,855E,CCF3,60,-92,3OK
				// #MONIZIP: 28601,23,0,855E,CCF3,60,-93,0OK
				// #MONIZIP: 28601,23,0,855E,CCF3,60,-91,3OK
				// #MONIZIP: 28601,23,0,855E,CCF3,60,-92,0OK
				// \r\n#MONIZIP: 28602,65,0,D3D6,5DAA,119,-73,3\r\n\r\nOK\r\n

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n#MONIZIP: %05d,%02d,%01d,%04X,%04X,%03d,-%hhu,%01d\r\n\r\nOK\r\n", &this->GSM_Operator.Operator, &Operator.BSIC, &Operator.QUAL, &this->GSM_Operator.LAC, &this->GSM_Operator.Cell_ID, &Operator.ARFCN, &this->GSM_Operator.dBm, &Operator.TIMADV);

				// Calculate Signal Level
				if (this->GSM_Operator.dBm >= 51 and this->GSM_Operator.dBm < 65) this->GSM_Operator.Signal = 5;
				if (this->GSM_Operator.dBm >= 66 and this->GSM_Operator.dBm < 77) this->GSM_Operator.Signal = 4;
				if (this->GSM_Operator.dBm >= 78 and this->GSM_Operator.dBm < 89) this->GSM_Operator.Signal = 3;
				if (this->GSM_Operator.dBm >= 90 and this->GSM_Operator.dBm < 101) this->GSM_Operator.Signal = 2;
				if (this->GSM_Operator.dBm >= 102 and this->GSM_Operator.dBm < 111) this->GSM_Operator.Signal = 1;
				if (this->GSM_Operator.dBm >= 112 and this->GSM_Operator.dBm < 113) this->GSM_Operator.Signal = 0;

				// Control for Variable
				if (_Variable_Count == 8) return(true);

				// End Function
				return(false);

			}

			// Get Clock Function
			bool CCLK(uint16_t & _Year, uint16_t & _Month, uint16_t & _Day, uint16_t & _Hour, uint16_t & _Minute, uint16_t & _Second) {

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT+CCLK?"));
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

				// \r\n+CCLK: "22/05/31,13:06:06+00"\r\n\r\nOK\r\n

				// Handle Variables
				uint8_t _Variable_Count = sscanf(Buffer_Variable, "\r\n+CCLK: \"%02d/%02d/%02d,%02d:%02d:%02d+00\"\r\n\r\nOK\r\n", &_Year, &_Month, &_Day, &_Hour, &_Minute, &_Second);

				// Control for Variables
				if (_Year > 24 and _Year < 22) return(false);	
				if (_Month > 12 and _Month < 0) return(false);	
				if (_Day > 31 and _Day < 0) return(false);	
				if (_Hour > 24 and _Hour < 0) return(false);	
				if (_Minute > 59 and _Minute < 0) return(false);	
				if (_Second > 59 and _Second < 0) return(false);

				// Control for Variable
				if (_Variable_Count == 6) return(true);

				// End Function
				return (false);

			}

			// Enable or Disable Automatic TimeZone Update Function
			bool CTZU(const bool _State) {

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
				GSM_Serial->print(F("AT+CTZU="));
				GSM_Serial->print(String(_State));
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

			// Enable or Disable Network Time UnSolicited Function
			bool NITZ(const bool _State) {

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
				GSM_Serial->print(F("AT#NITZ="));
				GSM_Serial->print(_State);
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

			// Set DNS Function
			bool DNS(void) {

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
				GSM_Serial->print(F("AT#DNS=1,\"8.8.8.8\",\"8.8.4.4\""));
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

				// End Function
				return(true);

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
					40		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SS="));
				GSM_Serial->print(String(_ConnID));
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

				// \r\n#SS: 2,4,5.26.173.230,80\r\n\r\nOK\r\n

				// Handle State
				_State = (Buffer_Variable[9] - 48);

				// End Function
				return (true);

			}

			// Socket Listen Function
			bool SL(const uint8_t _ConnID, const bool _Listen_State, const uint16_t _Listen_Port, const uint8_t _Closure_Type) {

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

			// Close Socket Function
			bool SH(const uint8_t _ConnID) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					5000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#SH="));
				GSM_Serial->print(String(_ConnID));
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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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
				uint32_t Current_Time = millis();

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

				// Read Current Time
				Current_Time = millis();

				// Declare Buffer
				memset(Buffer_Variable, '\0', Buffer.Size);

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

			// Socket Dial Function
			bool ATSD(const uint8_t _Cid, const uint8_t _Protocol, const uint8_t _Port, const uint8_t _Closure_Type, uint16_t _IPort, const bool _Conn_Mode, const char *_IP) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					140000, // Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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

				// Read Current Time
				uint32_t Current_Time = millis();

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

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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

			// Socket Pack Send Function
			bool SSEND(const uint8_t _ConnID, const uint8_t _Header_Type, const uint16_t _Response_Code, const char * _IP, const char * _URL, const char * _Data_Pack) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer_Set = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					2000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer_Set.Size];
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
				this->Send_Header(_Header_Type, _Response_Code, _IP, _URL, _Data_Pack);

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
					Buffer_Get.Response = this->Find_OK(Buffer_Variable, Buffer_Get.Read_Order);

					// Increase Read Order
					if (isAscii(Buffer_Variable[Buffer_Get.Read_Order])) Buffer_Get.Read_Order++;

					// Handle for timeout
					if (millis() - Current_Time >= Buffer_Get.Time_Out) return(false);

				}

				// End Function
				return (true);

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
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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

			// Close FTP Connection Function
			bool FTPCLOSE(void) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					10000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPCLOSE"));
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
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPTO="));
				GSM_Serial->print(_TOut);
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

			// Change FTP Folder Function
			bool FTPCWD(const char * _Folder) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					5000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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

			// Get FTP File to Buffer function
			bool FTPGETPKT(const uint16_t _FileID, const uint8_t _ViewMode) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					5000, 	// Time Out
					20		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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

			// Get FTP File From Buffer Function
			bool FTPRECV(const uint16_t _Size, uint16_t & _ReadSize, uint8_t & _State, char * _Data) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					5000,	// Time Out
					255		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPRECV="));
				GSM_Serial->print(_Size);
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
					if (millis() - Current_Time >= Buffer.Time_Out) break;;

				}

				// \r\n#FTPRECV: 200\r\n20202055\r\n:100BA00020202020000D0A002C002C002C00415495\r\n:100BB00023534C3D000D0A004154234532534C52FF\r\n:100BC000493D000D0A00415423534C4544534156BE\r\n:100BD000000D0A00415423534C45443D000D0A00CA\r\n:100BE0004\r\n\r\nOK\r\n
				// \r\n+CME ERROR: 614\r\n	

				// Control for CME Error
				if (this->Find_CME(Buffer_Variable, Buffer.Read_Order)) {

					// Set State
					_State = 1;

					// End Function
					return (false);

				}

				// Clear Size Variable
				_ReadSize = 0;

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

			// Set FTP File Transfer Type Function
			bool FTPTYPE(const uint8_t _Type) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					5000, 	// Time Out
					7		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
				memset(Buffer_Variable, '\0', Buffer.Size);

				// Command Chain Delay (Advice by Telit)
				delay(20);

				// Send UART Command
				GSM_Serial->print(F("AT#FTPTYPE="));
				GSM_Serial->print(_Type);
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

			// Get FTP File Size Function
			bool FTPFSIZE(const uint16_t _FileID, uint32_t & _Length) {

				// Clear UART Buffer
				this->Clear_UART_Buffer();

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					5000, 	// Time Out
					30		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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

				// Read Current Time
				const uint32_t Current_Time = millis();

				// \r\n#FTPFSIZE: 174945\r\n\r\nOK\r\n

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

				// \r\n#FTPFSIZE: 174945\r\n\r\nOK\r\n

				// Handle Variables
				sscanf(Buffer_Variable, "\r\n#FTPFSIZE:%lu\r\n\r\nOK\r\n", &_Length);

				// End Function
				return (true);

			}

			// Detect SRING Response.
			bool Send_SRING(uint16_t &_Length) {

				// Declare Buffer Object
				Serial_Buffer Buffer = {
					false, 	// Response State
					0, 		// Read Order
					0, 		// Data Order
					50000, 	// Time Out
					30		// Buffer Size
				};

				// Declare Buffer
				char Buffer_Variable[Buffer.Size];
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

				// Handle Variables
				uint8_t _Parsed = sscanf(Buffer_Variable, "\r\nSRING: 3,%d\r\n", &_Length);

				// End Function
				if (_Parsed ==1) return(true);
				return(false);

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

			// ******************** Operator Commands ********************

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


			// ******************** RTC Commands ********************

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

			// ******************** Socket Commands ********************

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
//					GSM_Serial->print(F("Connection: close\r\n"));

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