// Define Libraries
#include <Arduino.h>
#include "Definitions.h"
#include "B100BC.h"
#include <PostMan.h>
#include <ArduinoJson.h>

// Define Hardware
B100BC B100_BC;

// Define Console
Console Terminal(Serial_Terminal);

// Set PostOffice Cloud API
PostMan Postman(Serial3);

// Define CallBack Functions
void CallBack_PackData(uint8_t);
void CallBack_Send_Response(uint16_t, uint8_t);
void CallBack_Command(uint16_t, char*);

// Declare Global Variable
bool Timer_Measure_Pressure = false;
bool Timer_Display = false;

// PostOffice Call Back Functions
void CallBack_PackData(uint8_t _PackType) { 

	Postman.JSON_Data.JSON_Status.Device_State = 200;

	// Print Status
	B100_BC.Terminal_Text(14, 111, Terminal_CYAN, String(Postman.JSON_Data.JSON_Status.Device_State, HEX));

}
void CallBack_Send_Response(uint16_t _Response, uint8_t _Error) {

	// Control for Error
	if (_Error == 0) {

		// Buzzer Beep
		B100_BC.Buzzer(S_MODE2);

		// Control for Command
		if (_Response == 200) {

			// Print Command State
			B100_BC.Terminal_Text(14, 44, Terminal_CYAN, F("                                    "));
			B100_BC.Terminal_Text(14, 44, Terminal_CYAN, F("Pack Sended"));

		}
		if (_Response != 200) {

			// Print Command State
			B100_BC.Terminal_Text(14, 44, Terminal_CYAN, F("                                    "));
			B100_BC.Terminal_Text(14, 44, Terminal_CYAN, F("Pack Send Failed"));

		}

		// Print Command State
		B100_BC.Terminal_Text(14, 44, Terminal_CYAN, F("                                    "));

	} else {

		// Buzzer Beep
		B100_BC.Buzzer(S_SAD);

		if (_Error == 1) Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, "Dial Fail");
		if (_Error == 2) Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, "Response Fail");
		if (_Error == 3) Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_Y, RED, "Dial1 Fail");
		Terminal.Text(GSM_PostOfficeStatus_X, GSM_PostOfficeStatus_X, YELLOW, "                      ");

		if (_Error != 1) Postman.LOG();

	}

	// Set RTC Timer
	B100_BC.Set_Timer(600);

}
void CallBack_Command(uint16_t _Command, char * _Pack) {

	// Terminal Beep
	Terminal.Beep();

	// Print Command State
	B100_BC.Terminal_Text(14, 44, Terminal_CYAN, F("                                    "));
	B100_BC.Terminal_Text(14, 44, Terminal_CYAN, String(_Command));

	// Send Response
	Postman.Response(200, 200);

	// Set RTC Timer
	B100_BC.Set_Timer(180);

}

// PostOffice Interrupt Routine
void Interrupt_Routine(void) {

	// Incoming Pack
	if (Postman.Interrupt.Ring) Postman.Get();

	// Send Pack Routine
	if (Postman.Interrupt.Online) Postman.Publish(Online);
	if (Postman.Interrupt.Update) Postman.Publish(Update);
	if (Postman.Interrupt.Timed) Postman.Publish(Timed);
	if (Postman.Interrupt.Interrupt) Postman.Publish(Interrupt);
	if (Postman.Interrupt.Alarm) Postman.Publish(Alarm);
	if (Postman.Interrupt.Offline) Postman.Publish(Offline);
	if (Postman.Interrupt.FOTA_Info) Postman.Publish(FOTA_Info);

	// FOTA Routine
	if (Postman.Interrupt.FOTA_Download) {

		// Terminal Beep
		Terminal.Beep();

		// Download Firmware
		Postman.Download(Postman.FOTA_Variables.File_ID);

		// Publish Download Status
		Postman.Interrupt.FOTA_Download = false;
		Postman.Interrupt.Timed = false;
		Postman.Interrupt.FOTA_Info = true;

	}
	if (Postman.Interrupt.FOTA_Burn) {

		// Terminal Beep
		Terminal.Beep();

		// Turn FOTA Power Off
		PORTG |= 0b00000001;

	}

}

void setup() {

	// Define Hardware
	B100_BC.Begin();

	// Activate Mux
	DDRC |= 0b00000001; PORTC |= 0b00000001;
	delay(200);

	// Start SD Card
	SD.begin(53);

	// Turn SD MUX Enable LOW
	PORTC &= 0b11111110;

	// Print Version
	B100_BC.Terminal_Text(6, 71, Terminal_CYAN, String(__Firmware__));
	B100_BC.Terminal_Text(7, 71, Terminal_CYAN, String(__Hardware__));
	B100_BC.Terminal_Text(10, 72, Terminal_CYAN, String(B100_BC.Variables.Interval.Online / 60));
	B100_BC.Terminal_Text(11, 72, Terminal_CYAN, String(B100_BC.Variables.Interval.Offline / 60));

	// Set CallBacks
	Postman.Event_PackData(CallBack_PackData);
	Postman.Event_PackSend_Response(CallBack_Send_Response);
	Postman.Event_Request(CallBack_Command);

	// Connect to Cloud
	Postman.Online();

	// Enable Interrupts
	B100_BC.AVR_Enable_Interrupt();

}

void loop() {

	// Interrupt Routine
	Interrupt_Routine();

	// Update Timer
	if (Timer_Display) {

		// Update Timer
		Terminal.Text(2, 13, BLUE, String(B100_BC.Variables.Timer_Counter));

		// Release Interrupt
		Timer_Display = false;

	}

	// Pressure Measurement
	if (Timer_Measure_Pressure) {

		// Blink
		B100_BC.LED(__BLUE__, 1, 200);

		// Measure Pressure
		B100_BC.Measure_Pressure();

		// Print Version
		Terminal.Text(25, 110, CYAN, String(B100_BC.Variables.Pressure.Value, 3));
		Terminal.Text(26, 110, CYAN, String(B100_BC.Variables.Pressure.Min, 3));
		Terminal.Text(27, 110, CYAN, String(B100_BC.Variables.Pressure.Max, 3));
		Terminal.Text(28, 110, CYAN, String(B100_BC.Variables.Pressure.Average, 3));
		Terminal.Text(29, 110, BLUE, String(B100_BC.Variables.Pressure.Deviation, 3));
		Terminal.Text(31, 116, CYAN, String(B100_BC.Variables.Pressure.Data_Count));

		// Release Interrupt
		Timer_Measure_Pressure = false;

	}

}

// Timer Interrupt
ISR(TIMER5_COMPA_vect) {

	// Set Timer Counter
	B100_BC.Timer_Count();

	// Activate Timer Interrupt
	if (B100_BC.Timer_Control(1)) Timer_Display = true;

	// Measure Pressure
	if (B100_BC.Timer_Control(3)) Timer_Measure_Pressure = true;

}

// Pin Interrupt
ISR(PCINT2_vect) {

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

// RTC Timer Interrupt
ISR(PCINT0_vect) {

	// Control RTC Interrupt [PB4]
	if ((PINB & (1 << PINB4)) == (1 << PINB4)) Postman.Interrupt.Timed = true;

	// Interrupt Delay
	delay(50);

}