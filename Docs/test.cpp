	// Define Arduino Library
	#ifndef __Arduino__
		#include <Arduino.h>
	#endif



// PinOut Definitions
inline void PORT_A_PinOut(void) {

	// PORT A
	// 	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	// 	| PA0 | Output	| Pull Down 	|   			    | 
	// 	| PA1 | Output  | Pull Down 	|   			    | 
	// 	| PA2 | Output  | Pull Down 	|   			    | 
	// 	| PA3 | Output	| Pull Down 	|   			    | 
	// 	| PA4 | Output	| Pull Down 	|   			    | 
	// 	| PA5 | Output	| Pull Down 	|   			    | 
	// 	| PA6 | Output	| Pull Down 	|   			    | 
	// 	| PA7 | Output	| Pull Down 	|   			    | 
	// 	---------------------------------------------------------------------
	DDRA 	= 0b11111111;					PORTA 	= 0b00000000;					// All Pins Set Output / Pull Down

}
inline void PORT_B_PinOut(void) {

	// Port B
	#define B100_RTC_Int		PCINT4
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PB0 | -		| -				| 				    | SS
	//	| PB1 | -		| -				| 				    | SCK
	//	| PB2 | -		| -				| 				    | MOSI
	//	| PB3 | -		| -				| 				    | MISO
	//	| PB4 | Input 	| Pull Up 		| B100_RTC_Int	    | PCINT4
	//	| PB5 | Output	| Pull Down 	|   			    | 
	//	| PB6 | Output	| Pull Down 	|   			    | 
	//	| PB7 | Output	| Pull Down 	|   			    | 
	// 	---------------------------------------------------------------------
	DDRB 	&= ~(1 << B100_RTC_Int);		PORTB 	|= (1 << B100_RTC_Int);			// Set PB4 ad Input / Pull Up
	DDRB 	|= (1 << PB5);					PORTB 	&= ~(1 << PB5);					// Set PB5 as Output / Pull Down
	DDRB 	|= (1 << PB6);					PORTB 	&= ~(1 << PB6);					// Set PB6 as Output / Pull Down
	DDRB 	|= (1 << PB7);					PORTB 	&= ~(1 << PB7);					// Set PB7 as Output / Pull Down

}
inline void PORT_C_PinOut(void) {

	// Port C
	#define B100_SD_Mux_En		PC0
	#define B100_SD_Detect		PC1
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PC0 | Output	| Pull Down		| B100_SD_Mux_En    | PC0
	//	| PC1 | Input	| Pull Down		| B100_SD_Detect    | PC1
	//	| PC2 | Output	| Pull Down		|   			    | 
	//	| PC3 | Output	| Pull Down		|   			    | 
	//	| PC4 | Output	| Pull Down		|   			    | 
	//	| PC5 | Output	| Pull Down		|   			    | 
	//	| PC6 | Output	| Pull Down		|   			    | 
	//	| PC7 | Output	| Pull Down		|   			    | 
	// 	---------------------------------------------------------------------
	DDRC 	|= (1 << B100_SD_Mux_En);		PORTC 	&= ~(1 << B100_SD_Mux_En);		// Set PC0 as Output / Pull Down
	DDRC 	&= ~(1 << B100_SD_Detect);		PORTC 	&= ~(1 << B100_SD_Detect);		// Set PC1 as Input / Pull Down
	DDRC 	|= (1 << PC2);					PORTC 	&= ~(1 << PC2);					// Set PC2 as Output / Pull Down
	DDRC 	|= (1 << PC3);					PORTC 	&= ~(1 << PC3);					// Set PC3 as Output / Pull Down
	DDRC 	|= (1 << PC4);					PORTC 	&= ~(1 << PC4);					// Set PC4 as Output / Pull Down
	DDRC 	|= (1 << PC5);					PORTC 	&= ~(1 << PC5);					// Set PC5 as Output / Pull Down
	DDRC 	|= (1 << PC6);					PORTC 	&= ~(1 << PC6);					// Set PC6 as Output / Pull Down
	DDRC 	|= (1 << PC7);					PORTC 	&= ~(1 << PC7);					// Set PC7 as Output / Pull Down

}
inline void PORT_D_PinOut(void) {

	// Port D
	#define B100_LED_Blue		PD5
	#define B100_LED_Green		PD6
	#define B100_LED_Red		PD7
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PD0 | -		| -				|				    | SCL
	//	| PD1 | -		| -				|				    | SDA
	//	| PD2 | -		| -				|				    | RXD1
	//	| PD3 | -		| -				|				    | TXD1
	//	| PD4 | Output	| Pull Down 	|   			    | 
	//	| PD5 | Output	| Pull Down 	| B100_LED_Blue     | PD5
	//	| PD6 | Output	| Pull Down 	| B100_LED_Green    | PD6
	//	| PD7 | Output	| Pull Down 	| B100_LED_Red	    | PD7
	// 	---------------------------------------------------------------------
	DDRD 	|= (1 << PD4);					PORTD 	&= ~(1 << PD4);					// Set PD4 as Output / Pull Down
	DDRD 	|= (1 << B100_LED_Blue);		PORTD 	&= ~(1 << B100_LED_Blue);		// Set PD5 as Output / Pull Down
	DDRD 	|= (1 << B100_LED_Green);		PORTD 	&= ~(1 << B100_LED_Green);		// Set PD6 as Output / Pull Down
	DDRD 	|= (1 << B100_LED_Red);			PORTD 	&= ~(1 << B100_LED_Red);		// Set PD7 as Output / Pull Down

}
inline void PORT_E_PinOut(void) {

	// Port E
	#define B100_Buzzer_En1		PE3
	#define B100_Buzzer_En2		PE4
	#define B100_Buzzer_Data	PE5
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PE0 | -		| -				|				    | RXD0
	//	| PE1 | -		| -				|				    | TXD0
	//	| PE2 | Output	| Pull Down 	|   			    | 
	//	| PE3 | Output	| Pull Down		| B100_Buzzer_En1   | PE3
	//	| PE4 | Output	| Pull Down		| B100_Buzzer_En2   | PE4
	//	| PE5 | Output	| Pull Down		| B100_Buzzer_Data  | PE5
	//	| PE6 | Output	| Pull Down		|   			    | 
	//	| PE7 | Output	| Pull Down		|   			    | 
	// 	---------------------------------------------------------------------
	DDRE 	|= (1 << PE2);					PORTE 	&= ~(1 << PE2);					// Set PE2 as Output / Pull Down
	DDRE 	|= (1 << B100_Buzzer_En1);		PORTE 	&= ~(1 << B100_Buzzer_En1);		// Set PE3 as Output / Pull Down
	DDRE 	|= (1 << B100_Buzzer_En2);		PORTE 	&= ~(1 << B100_Buzzer_En2);		// Set PE4 as Output / Pull Down
	DDRE 	|= (1 << B100_Buzzer_Data);		PORTE 	&= ~(1 << B100_Buzzer_Data);	// Set PE5 as Output / Pull Down
	DDRE 	|= (1 << PE6);					PORTE 	&= ~(1 << PE6);					// Set PE6 as Output / Pull Down
	DDRE 	|= (1 << PE7);					PORTE 	&= ~(1 << PE7);					// Set PE7 as Output / Pull Down

}
inline void PORT_F_PinOut(void) {

	// Port F
	#define B100_P1_Sense		PF0
	#define B100_P1_Data		PF1
	#define B100_P2_Data		PF2
	#define B100_P2_Sense		PF3
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PF0 | Input	| Pull Down		| B100_P1_Sense	    | PF0
	//	| PF1 | 		|				| B100_P1_Data	    | ADC1
	//	| PF2 | 		|				| B100_P2_Data	    | ADC2
	//	| PF3 | Input	| Pull Down		| B100_P2_Sense	    | PF3
	//	| PF4 | Output	| Pull Down		|   			    | 
	//	| PF5 | Output	| Pull Down		|   			    | 
	//	| PF6 | Output	| Pull Down		|   			    | 
	//	| PF7 | Output	| Pull Down		|   			    | 
	// 	---------------------------------------------------------------------
	DDRF 	&= ~(1 << B100_P1_Sense);		PORTF 	&= ~(1 << B100_P1_Sense);		// Set PF0 as Input / Pull Down
	DDRF 	&= ~(1 << B100_P1_Data);		PORTF 	&= ~(1 << B100_P1_Data);		// Set PF1 as Input / Pull Down
	DDRF 	&= ~(1 << B100_P2_Data);		PORTF 	&= ~(1 << B100_P2_Data);		// Set PF2 as Input / Pull Down
	DDRF 	&= ~(1 << B100_P2_Sense);		PORTF 	&= ~(1 << B100_P2_Sense);		// Set PF3 as Input / Pull Down
	DDRF 	|= (1 << PF4);					PORTF 	&= ~(1 << PF4);					// Set PF4 as Output / Pull Down
	DDRF 	|= (1 << PF5);					PORTF 	&= ~(1 << PF5);					// Set PF5 as Output / Pull Down
	DDRF 	|= (1 << PF6);					PORTF 	&= ~(1 << PF6);					// Set PF6 as Output / Pull Down
	DDRF 	|= (1 << PF7);					PORTF 	&= ~(1 << PF7);					// Set PF7 as Output / Pull Down

}
inline void PORT_G_PinOut(void) {

	// Port G
	#define B100_Fota_En		PG0
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PG0 | Output	| Pull Down		| B100_Fota_En	    | PG0
	//	| PG1 | Output	| Pull Down		|   			    | 
	//	| PG2 | Output	| Pull Down		|   			    | 
	//	| PG3 | Output	| Pull Down		|   			    | 
	//	| PG4 | Output	| Pull Down		|   			    | 
	//	| PG5 | Output	| Pull Down		|   			    | 
	// 	---------------------------------------------------------------------
	DDRG 	|= (1 << B100_Fota_En);			PORTG 	&= ~(1 << B100_Fota_En);		// Set PG0 as Output / Pull Down
	DDRG 	|= (1 << PG1);					PORTG 	&= ~(1 << PG1);					// Set PG1 as Output / Pull Down
	DDRG 	|= (1 << PG2);					PORTG 	&= ~(1 << PG2);					// Set PG2 as Output / Pull Down
	DDRG 	|= (1 << PG3);					PORTG 	&= ~(1 << PG3);					// Set PG3 as Output / Pull Down
	DDRG 	|= (1 << PG4);					PORTG 	&= ~(1 << PG4);					// Set PG4 as Output / Pull Down
	DDRG 	|= (1 << PG5);					PORTG 	&= ~(1 << PG5);					// Set PG5 as Output / Pull Down

}
inline void PORT_J_PinOut(void) {

	// Port J
	#define B100_GSM_Ring		PCINT11
	#define B100_GSM_PMon		PCINT12
	#define B100_GSM_Comm_En	PCINT13
	#define B100_GSM_SDown		PCINT14
	#define B100_GSM_OnOff		PCINT15
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PJ0 | -		| -				|				    | RXD3
	//	| PJ1 | -		| -				|				    | TXD3
	//	| PJ2 | Input	| Pull Down		| B100_GSM_Ring	    | PCINT11
	//	| PJ3 | Input	| Pull Down		| B100_GSM_PMon	    | PCINT12
	//	| PJ4 | Output	| Pull Down		| B100_GSM_Comm_En  | PCINT13
	//	| PJ5 | Output	| Pull Down		| B100_GSM_SDown    | PCINT14
	//	| PJ6 | Output	| Pull Down		| B100_GSM_OnOff    | PCINT15
	//	| PJ7 | Output	| Pull Down		|   			    | 
	// 	---------------------------------------------------------------------
	DDRJ 	&= ~(1 << B100_GSM_Ring);		PORTJ 	&= ~(1 << B100_GSM_Ring);		// Set PJ2 as Input / Pull Down
	DDRJ 	&= ~(1 << B100_GSM_PMon);		PORTJ 	&= ~(1 << B100_GSM_PMon);		// Set PJ3 as Input / Pull Down
	DDRJ 	|= (1 << B100_GSM_Comm_En);		PORTJ 	&= ~(1 << B100_GSM_Comm_En);	// Set PJ4 as Output / Pull Down
	DDRJ 	|= (1 << B100_GSM_SDown);		PORTJ 	&= ~(1 << B100_GSM_SDown);		// Set PJ5 as Output / Pull Down
	DDRJ 	|= (1 << B100_GSM_OnOff);		PORTJ 	&= ~(1 << B100_GSM_OnOff);		// Set PJ6 as Output / Pull Down
	DDRJ 	|= (1 << PJ7);					PORTJ 	&= ~(1 << PJ7);					// Set PJ7 as Output / Pull Down

}
inline void PORT_K_PinOut(void) {

	// Port K
	#define B100_220_IO_1		PCINT16
	#define B100_220_IO_2		PCINT17
	#define B100_220_IO_3		PCINT18
	#define B100_220_IO_4		PCINT19
	#define B100_220_IO_5		PCINT20
	#define B100_220_IO_6		PCINT21
	#define B100_220_IO_7		PCINT22
	#define B100_220_IO_8		PCINT23
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PK0 | Input	| Pull Down		| B100_220_IO_1     | PCINT16
	//	| PK1 | Input	| Pull Down		| B100_220_IO_2     | PCINT17
	//	| PK2 | Input	| Pull Down		| B100_220_IO_3     | PCINT18
	//	| PK3 | Input	| Pull Down		| B100_220_IO_4     | PCINT19
	//	| PK4 | Input	| Pull Down		| B100_220_IO_5     | PCINT20
	//	| PK5 | Input	| Pull Down		| B100_220_IO_6     | PCINT21
	//	| PK6 | Input	| Pull Down		| B100_220_IO_7     | PCINT22
	//	| PK7 | Input	| Pull Down		| B100_220_IO_8     | PCINT23
	// 	---------------------------------------------------------------------
	DDRK 	&= ~(1 << B100_220_IO_1);		PORTK 	&= ~(1 << B100_220_IO_1);		// Set PK0 as Input / Pull Down
	DDRK 	&= ~(1 << B100_220_IO_2);		PORTK 	&= ~(1 << B100_220_IO_2);		// Set PK1 as Input / Pull Down
	DDRK 	&= ~(1 << B100_220_IO_3);		PORTK 	&= ~(1 << B100_220_IO_3);		// Set PK2 as Input / Pull Down
	DDRK 	&= ~(1 << B100_220_IO_4);		PORTK 	&= ~(1 << B100_220_IO_4);		// Set PK3 as Input / Pull Down
	DDRK 	&= ~(1 << B100_220_IO_5);		PORTK 	&= ~(1 << B100_220_IO_5);		// Set PK4 as Input / Pull Down
	DDRK 	&= ~(1 << B100_220_IO_6);		PORTK 	&= ~(1 << B100_220_IO_6);		// Set PK5 as Input / Pull Down
	DDRK 	&= ~(1 << B100_220_IO_7);		PORTK 	&= ~(1 << B100_220_IO_7);		// Set PK6 as Input / Pull Down
	DDRK 	&= ~(1 << B100_220_IO_8);		PORTK 	&= ~(1 << B100_220_IO_8);		// Set PK7 as Input / Pull Down

}
inline void PORT_L_PinOut(void) {

	// Port L
	#define B100_Relay_On		PL0
	#define B100_Relay_Off		PL1
	#define B100_Relay_Alarm	PL2
	#define B100_Relay_Res		PL3
	//	---------------------------------------------------------------------
	// 	| Pin |	DDRn    | Default       | B100 Name	   	    | MCU Name
	// 	|-----|---------|---------------|-------------------|---------------
	//	| PL0 | Output	| Pull Down		| B100_Relay_On	    | PL0
	//	| PL1 | Output	| Pull Down		| B100_Relay_Off	| PL1
	//	| PL2 | Output	| Pull Down		| B100_Relay_Alarm	| PL2
	//	| PL3 | Output	| Pull Down		| B100_Relay_Res	| PL3
	//	| PL4 | Output	| Pull Down		|   			    | 
	//	| PL5 | Output	| Pull Down		|   			    | 
	//	| PL6 | Output	| Pull Down		|   			    | 
	//	| PL7 | Output	| Pull Down		|   			    | 
	// 	---------------------------------------------------------------------
	DDRL 	|= (1 << B100_Relay_On);		PORTL 	&= ~(1 << B100_Relay_On);		// Set PL0 as Output / Pull Down
	DDRL 	|= (1 << B100_Relay_Off);		PORTL 	&= ~(1 << B100_Relay_Off);		// Set PL1 as Output / Pull Down
	DDRL 	|= (1 << B100_Relay_Alarm);		PORTL 	&= ~(1 << B100_Relay_Alarm);	// Set PL2 as Output / Pull Down
	DDRL 	|= (1 << B100_Relay_Res);		PORTL 	&= ~(1 << B100_Relay_Res);		// Set PL3 as Output / Pull Down
	DDRL 	|= (1 << PL4);					PORTL 	&= ~(1 << PL4);					// Set PL4 as Output / Pull Down
	DDRL 	|= (1 << PL5);					PORTL 	&= ~(1 << PL5);					// Set PL5 as Output / Pull Down
	DDRL 	|= (1 << PL6);					PORTL 	&= ~(1 << PL6);					// Set PL6 as Output / Pull Down
	DDRL 	|= (1 << PL7);					PORTL 	&= ~(1 << PL7);					// Set PL7 as Output / Pull Down

}
inline void Pinout(void) {

	// Set PinOut Definitions
	PORT_A_PinOut();
	PORT_B_PinOut();
	PORT_C_PinOut();
	PORT_D_PinOut();
	PORT_E_PinOut();
	PORT_F_PinOut();
	PORT_G_PinOut();
	PORT_J_PinOut();
	PORT_K_PinOut();
	PORT_L_PinOut();

}
