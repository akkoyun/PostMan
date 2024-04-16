// Define Modem Power Function Definitions
#define _Modem_STATUS_								(uint8_t)0
#define _Modem_ON_									(uint8_t)1
#define _Modem_OFF_									(uint8_t)2

// Define Status Definitions
#define PostMan_Status_Terminal						(uint8_t)0
#define PostMan_Status_Initialize					(uint8_t)1
#define PostMan_Status_SIM_Inserted					(uint8_t)2
#define PostMan_Status_Connection					(uint8_t)3
#define PostMan_Status_Firewall						(uint8_t)4
#define PostMan_Status_Ring							(uint8_t)5

// Define PowerStat Commands
#define Command_None								(uint16_t)0
#define Command_Version 							(uint16_t)100
#define Command_Ping								(uint16_t)101
#define Command_OK									(uint16_t)200
#define Command_NOK									(uint16_t)201
#define Command_Start								(uint16_t)256
#define Command_Stop								(uint16_t)257
#define Command_Update								(uint16_t)262
#define Command_Parameter							(uint16_t)500
#define Command_Environment							(uint16_t)501
#define Command_Power								(uint16_t)502
#define Command_GSM	    							(uint16_t)503
#define Command_Energy								(uint16_t)800
#define Command_FOTA_Download_GET					(uint16_t)900
#define Command_FOTA_Download_FTP					(uint16_t)901
#define Command_FOTA_Burn							(uint16_t)950
#define Command_Reset								(uint16_t)999

// Define Pack Types
#define Pack_None									(uint8_t)0
#define Pack_Online									(uint8_t)1
#define Pack_Update									(uint8_t)2
#define Pack_Timed									(uint8_t)3
#define Pack_Interrupt								(uint8_t)4
#define Pack_Alarm									(uint8_t)5
#define Pack_Offline								(uint8_t)6
#define Pack_FOTA_Info								(uint8_t)90
#define Pack_FOTA_Download							(uint8_t)91
#define Pack_FOTA_Burn								(uint8_t)99