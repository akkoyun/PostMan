// Define Status Definitions
#define PostMan_Status_Terminal						(uint8_t)0
#define PostMan_Status_Initialize					(uint8_t)1
#define PostMan_Status_SIM_Inserted					(uint8_t)2
#define PostMan_Status_Connection					(uint8_t)3
#define PostMan_Status_Firewall						(uint8_t)4
#define PostMan_Status_Ring							(uint8_t)5






// Define PowerStat Commands
#define Command_None								0
#define Command_OK									200
#define Command_NOK									201
#define Command_Start								256
#define Command_Stop								257
#define Command_Update								262
#define Command_Reset								999
#define Command_Parameter							500
#define Command_FOTA_Download						900
#define Command_FOTA_Burn							901

// Define Pack Types
#define Pack_None									0
#define Pack_Online									1
#define Pack_Update									2
#define Pack_Timed									3
#define Pack_Interrupt								4
#define Pack_Alarm									5
#define Pack_Offline								6
#define Pack_FOTA_Info								90
#define Pack_FOTA_Download							91
#define Pack_FOTA_Burn								99