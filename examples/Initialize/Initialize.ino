#define __Firmware__ "00.00.02"

// Define Libraries
#include <Arduino.h>
#include "Terminal_Variables.h"
#include <PostMan.h>
#include <ArduinoJson.h>
#include <Console.h>

// Define CallBack Functions
void CallBack_PackData(uint8_t);
void CallBack_Send_Response(uint16_t, uint8_t);
void CallBack_Command(uint16_t, char*);

// Define Console
Console Terminal(Serial_Terminal);

// Set PostOffice Cloud API
PostMan Postman(Serial3);
FOTA Firmware(Serial3);






// Declare Global Variable
uint32_t Timer_Counter = 0;
bool Timer_Display = false;
bool Firmware_Download = false;
uint16_t _Firmware_ID = 0;

// Timer Functions
void Timer_Count(void) {

	// Set Timer Counter
	Timer_Counter += 1;

	// Handle Max
	if (Timer_Counter == 65534) Timer_Counter = 0;

}
bool Timer_Control(const uint16_t _Interval) {

	// Timer Counter
	if (Timer_Counter % _Interval == 0) {

		// End Function
		return(true);

	} else {

		// End Function
		return(false);

	}

}

// AVR 1sec Timer
void AVR_Timer_1sn(void) {

	// Clear Registers
	TCCR5A = 0x00;
	TCCR5B = 0x00;

	// Clear Counter
	TCNT5 = 0;

	// Set Counter Value
	OCR5A = (F_CPU / (1024)) - 1;

	// Set CTC Mod
	TCCR5B |= (1 << WGM52);

	// Set Rescale (1024)
	TCCR5B |= (1 << CS52) | (1 << CS50);

	// Start Timer
	TIMSK5 |= (1 << OCIE5A);

}

// PostOffice Call Back Functions
void CallBack_PackData(uint8_t _PackType) { 

	// Print Text
	Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, YELLOW, "Device Data Updated");

	// Set Device Data
	Postman.Environment(22.22, 33.33);
	Postman.Battery(1, 2, 3, 3, 11, 1200, 1000);

	// Set Payload Data
	Postman.TimeStamp("2022-03-23 14:18:30");

	if (_PackType == 1)	Postman.SetStatus(240, 500);

	// Print Text
	Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_X, YELLOW, "                      ");

}
void CallBack_Send_Response(uint16_t _Response, uint8_t _Error) {

	// Terminal Beep
	Terminal.Beep();

	// Control for Error
	if (_Error == 0) {

		// Control for Command
		if (_Response == 200) Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, GREEN, "Pack Sended");
		if (_Response != 200) Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, "Pack Send Failed");
		Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_X, YELLOW, "                      ");

	} else {

		if (_Error == 1) Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, "Dial Fail");
		if (_Error == 2) Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, "Response Fail");
		if (_Error == 3) Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, "Dial1 Fail");
		Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_X, YELLOW, "                      ");

	}

}
void CallBack_Command(uint16_t _Command, char * _Pack) {

	// Terminal Beep
	Terminal.Beep();

	Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_X, YELLOW, String(_Command));
	Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_X, GREEN, "                         ");

	if (_Command == 999) {

		// Declare JSON Object
		StaticJsonDocument<64> Incoming_JSON;

		// Deserialize the JSON document
		DeserializationError Error = deserializeJson(Incoming_JSON, _Pack);

		// Handle JSON
		if (!Error) _Firmware_ID = Incoming_JSON["Request"]["Firmware"];

		// Set Interrupt
		Firmware_Download = true;

	}

	if (_Command == 262) Postman.Interrupt.Send = true;






	// Declare Response JSON Variable
	String _Response_JSON;

	// Declare JSON Object
	StaticJsonDocument<32> Response_JSON;

	// Declare JSON Data
	Response_JSON[F("Response")] = 200;

	// Clear Unused Data
	Response_JSON.garbageCollect();

	// Serialize JSON	
	uint8_t _JSON_Length = serializeJson(Response_JSON, _Response_JSON) + 1;

	// String to Char Convert
	char JSON[_JSON_Length];
	_Response_JSON.toCharArray(JSON, _JSON_Length);

	// Send Response
	Postman.Response(200, JSON);

	// Print JSON
	Terminal.Text(26, 4, CYAN, String(_Response_JSON));
	delay(1000);
	Terminal.Text(26, 4, CYAN, "                                                  ");

}

// Hardware Functions
inline void Set_Pinout(void) {

	/*  PORT A
		PA0 - Output / Pull Down [NC] 			- AD0		- [22]
		PA1 - Output / Pull Down [NC] 			- AD1		- [23]
		PA2 - Output / Pull Down [NC] 			- AD2		- [24]
		PA3 - Output / Pull Down [NC] 			- AD3		- [25]
		PA4 - Output / Pull Down [NC] 			- AD4		- [26]
		PA5 - Output / Pull Down [NC] 			- AD5		- [27]
		PA6 - Output / Pull Down [NC] 			- AD6		- [28]
		PA7 - Output / Pull Down [NC] 			- AD7		- [29]
	*/
	DDRA = 0b11111111; PORTA = 0b00000000;

	/*  PORT B
		PB0 - SS
		PB1 - SCK
		PB2 - MOSI
		PB3 - MISO
		PB4 - Input / Pull Up [RTC Interrupt] 	- PCINT4 	- [10]
		PB5 - Output / Pull Down [NC] 			- PCINT5 	- [11]
		PB6 - Output / Pull Down [NC] 			- PCINT6 	- [12]
		PB7 - Output / Pull Down [NC] 			- PCINT7 	- [13]
	*/
	DDRB |= 0b11100000; PORTB &= 0b00011111;
	DDRB &= 0b11101111; PORTB |= 0b00010000;

	/*  PORT C
		PC0 - Output / Pull Down [SD Mux Dir.]	- AD8		- [37]
		PC1 - Input / Pull Down [SD Card Det.]	- AD9		- [36]
		PC2 - Output / Pull Down [NC] 			- AD10		- [35]
		PC3 - Output / Pull Down [NC] 			- AD11		- [34]
		PC4 - Output / Pull Down [NC] 			- AD12		- [33]
		PC5 - Output / Pull Down [NC] 			- AD13		- [32]
		PC6 - Output / Pull Down [NC] 			- AD14		- [31]
		PC7 - Output / Pull Down [NC] 			- AD15		- [30]
	*/
	DDRC |= 0b00000001;	PORTC &= 0b11111110;
	DDRC &= 0b11111101;	PORTC &= 0b11111101;
	DDRC |= 0b11111100;	PORTC &= 0b00000011;

	/*  PORT D
		PD0 - SCL
		PD1 - SDA
		PD2 - RXD1
		PD3 - TXD1
		PD4 - Output / Pull Down [NC] 			- ICP1		- [85]
		PD5 - Output / Pull Down [MCU LED Blue]	- XCK1		- [84]
		PD6 - Output / Pull Down [MCU LED Green]- T1		- [83]
		PD7 - Output / Pull Down [MCU LED Red]	- T0		- [82]
	*/
	DDRD |= 0b11110000; PORTD &= 0b00001111;

	/*  PORT E
		PE0 - RXD0
		PE1 - TXD0
		PE2 - Output / Pull Down [NC]			- AIN0		- [-]
		PE3 - Output / Pull Down [Buzzer EN1]	- AIN1		- [5]
		PE4 - Output / Pull Down [Buzzer EN2]	- INT4		- [2]
		PE5 - Output / Pull Down [PWM Buzzer]	- INT5		- [3]
		PE6 - Output / Pull Down [NC]			- INT6		- [-]
		PE7 - Output / Pull Down [NC]			- INT7		- [-]
	*/
	DDRE |= 0b11111100; PORTE &= 0b11000000;

	/*  PORT F
		PF0 - Input / Pull Down [Sns 1 Sense]	- ADC0		- [A0]
		PF1 - Sensor Data 1						- ADC1		- [A1]
		PF2 - Sensor Data 2						- ADC2		- [A2]
		PF3 - Input / Pull Down [Sns 2 Sense]	- ADC3		- [A3]
		PF4 - Output / Pull Down [NC]			- ADC4		- [A4]
		PF5 - Output / Pull Down [NC]			- ADC5		- [A5]
		PF6 - Output / Pull Down [NC]			- ADC6		- [A6]
		PF7 - Output / Pull Down [NC]			- ADC7		- [A7]
	*/
	DDRF &= 0b11110000; PORTF &= 0b11110110;
	DDRF |= 0b11110000; PORTF &= 0b00001111;

	/*  PORT G
		PG0 - Output / Pull Down [FOTA Pwr En]	- WR		- [41]
		PG1 - Output / Pull Down [NC] 			- RD		- [40]
		PG2 - Output / Pull Down [NC] 			- ALE		- [39]
		PG3 - Output / Pull Down [NC] 			- TOSC2		- [-]
		PG4 - Output / Pull Down [NC] 			- TOSC1		- [-]
		PG5 - Output / Pull Down [NC] 			- OC0B		- [4]
	*/
	DDRG |= 0b00111111; PORTG &= 0b11000000;

	/*  PORT J
		PJ0 - RXD3
		PJ1 - TXD3
		PJ2 - Input / Pull Down [GSM Ring]		- PCINT11	- [-]
		PJ3 - Input / Pull Down [GSM PwrMon]	- PCINT12	- [-]
		PJ4 - Output / Pull Down [GSM Communication Enable]	- PCINT13	- [-]
		PJ5 - Output / Pull Down [GSM ShtDwn]	- PCINT14	- [-]
		PJ6 - Output / Pull Down [GSM On Off]	- PCINT15	- [-]
		PJ7 - Output / Pull Down [NC]			-  			- [-]
	*/
	DDRJ &= 0b11110011; PORTJ |= 0b00000100;
	DDRJ |= 0b11110000; PORTJ &= 0b00000111;

	/*  PORT K
		PK0 - Input / Pull Down [220V Sense 1]  - PCINT16	- [89]
		PK1 - Input / Pull Down [220V Sense 2]  - PCINT17	- [88]
		PK2 - Input / Pull Down [220V Sense 3]  - PCINT18	- [87]
		PK3 - Input / Pull Down [220V Sense 4]  - PCINT19	- [86]
		PK4 - Input / Pull Down [220V Sense 5]  - PCINT20	- [85]
		PK5 - Input / Pull Down [220V Sense 6]  - PCINT21	- [84]
		PK6 - Input / Pull Down [220V Sense 7]  - PCINT22	- [83]
		PK7 - Input / Pull Down [220V Sense 8]  - PCINT23	- [82]
	*/
	DDRK = 0b00000000; PORTK = 0b00000000;

	/*  PORT L
		PL0 - Output / Pull Down [Start Relay]	- ICP4		- [49]
		PL1 - Output / Pull Down [Stop Relay]	- ICP5		- [48]
		PL2 - Output / Pull Down [Alarm Relay]	- T5		- [47]
		PL3 - Output / Pull Down [Res. Relay]	- OC5A		- [46]
		PL4 - Output / Pull Down [NC]			- OC5B		- [45]
		PL5 - Output / Pull Down [NC]			- OC5C		- [44]
		PL6 - Output / Pull Down [NC]			- 			- [43]
		PL7 - Output / Pull Down [NC]			- 			- [42]
	*/
	DDRL |= 0b11111111; PORTL &= 0b00000000;

}

// Timer Interrupt
ISR(TIMER5_COMPA_vect) {

	// Set Timer Counter
	Timer_Count();

	// Activate Timer Interrupt
	if (Timer_Control(1)) Timer_Display = true;

	// Data Send Timer Interrupt
	if (Timer_Control(120)) Postman.Interrupt.Send = true;

}

// GSM Ring Interrupt
ISR(PCINT1_vect) {

	// Control Ring Interrupt [PJ2]
	if ((PINJ & (1 << PINJ2)) == (1 << PINJ2)) {
		
		// Set Interrupt Variable
		Postman.Interrupt.Ring = true;

		// Interrupt Delay
		delay(75);

	} else {
		
		// Set Interrupt Variable
		Postman.Interrupt.Ring = false;

	}

}

void setup() {

	// Pin Definitions
	Set_Pinout();

	// Turn FOTA Power Off
	PORTG &= 0b11111110;

	// Start Terminal
	Serial.begin(115200);

	// Start GSM Serial
	Serial3.begin(115200);





	// Start Console
	Terminal.Begin();
	Terminal.Telit_xE910();

	// Print Version
	Terminal.Text(2, 111, CYAN, String(__Firmware__));

	// Print Version
//	Terminal.Text(2, 85, CYAN, String(SERIAL_RX_BUFFER_SIZE));
//	Terminal.Text(2, 30, CYAN, String(___Firmware___));






	// Power OFF GSM Modem
	Postman.Power(false);

	// Set CallBacks
	Postman.Event_PackData(CallBack_PackData);
	Postman.Event_PackSend_Response(CallBack_Send_Response);
	Postman.Event_Request(CallBack_Command);

	// Initialize Modem
	Postman.Initialize();

	// Connect to Cloud
	Postman.Connect();

	// Set Postman
	Postman.Subscribe("70A11D1D01000099");





	// Set Pin Change Interrupt Mask 1
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT11) | (1 << PCINT12);

	// Set 1sec Timer
	AVR_Timer_1sn();

	// Start Interrupts
	sei();

}

void loop() {

	// Firmware Download
	if (Firmware_Download) {

		Terminal.Beep();

			Postman.Interrupt.Send = false;

			bool _Download = Firmware.Download(_Firmware_ID);

			// Send Download Status
			Postman.SetStatus(999, 500);
			Postman.FOTA(_Firmware_ID, _Download, Firmware.Variables.File_Size, Firmware.Variables.SD_File_Size, Firmware.Variables.Download_Time);
			Postman.Publish(99);


			Firmware_Download = false;
			_Firmware_ID = 0;

	}

	// Update Timer
	if (Timer_Display) {

		// Update Timer
		Terminal.Text(2, 13, BLUE, String(Timer_Counter));

		// Release Interrupt
		Timer_Display = false;

	}

	// Send Data Pack
	if (Postman.Interrupt.Send) Postman.Publish(1);

	// Incoming Pack
	if (Postman.Interrupt.Ring) Postman.Get();

}
