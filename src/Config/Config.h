#ifndef __PostMan_Config__
#define __PostMan_Config__

	// Hardware Definitions
//	#define GSM_Power_Switch
//	#define GSM_LED_Switch
	#define GSM_Comm_Switch

	// Define Terminal Defaults
	#ifndef GSM_Debug
		#define CONSOLE_TEMPLATE_GE910
	#endif

	// Define Operator Defaults
	#ifndef _PDP_APN_
		#define _PDP_APN_				"mgbs"
	#endif

	// Define Version Defaults
	#ifndef __Hardware__
		#define __Hardware__ 			"00.00.00"
	#endif
	#ifndef __Firmware__
		#define __Firmware__ 			"00.00.00"
	#endif

	// Define Device Defaults
	#ifndef __Company__
		#define __Company__ 			"Demo"
	#endif
	#ifndef __Device__
		#define __Device__ 				"PostOffice"
	#endif

	// Define Cloud Parameters
	#ifndef _BackEnd_Server_
		#define _BackEnd_Server_ 		"165.227.154.147"
	#endif
	#ifndef _BackEnd_EndPoint_
		#define _BackEnd_EndPoint_ 	"/"
	#endif

	// Firewall Parameters
	#ifndef _AT_FRWL_1_IP_
		#define _AT_FRWL_1_IP_			"213.14.250.214"
	#endif

	#ifndef _AT_FRWL_2_IP_
		#define _AT_FRWL_2_IP_			"167.99.137.254"
	#endif

	#ifndef _AT_FRWL_3_IP_
		#define _AT_FRWL_3_IP_			"176.240.98.205"
	#endif

	// Define Server Address
	#ifndef _FOTA_Server_
		#define FOTA_Server 			"165.227.154.147"
	#endif

	// Define Server Folder
	#ifndef _FOTA_Folder_
		#define FOTA_Folder 			"firmware"
	#endif

	// Define Server UserName
	#ifndef _FOTA_UserName_
		#define FOTA_UserName 			"fota"
	#endif

	// Define Server Password
	#ifndef _FOTA_PassWord_
		#define FOTA_PassWord 			"123456"
	#endif

	// Define FTP SD Parameters
	#ifndef _FOTA_SD_FileName_
		#define FOTA_SD_FileName		"firmware.hex"
	#endif

	// Define LOG SD Parameters
	#ifndef _LOG_SD_File_Name_
		#define _LOG_SD_File_Name_		"LOG.txt"
	#endif

#endif