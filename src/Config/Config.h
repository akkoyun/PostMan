#ifndef __PostMan_Config__
#define __PostMan_Config__

	// Hardware Definitions
//	#define GSM_Power_Switch
//	#define GSM_LED_Switch
	#define GSM_Comm_Switch

	// Define Terminal Defaults
	#define GSM_Debug
	#ifdef GSM_Debug
		#define CONSOLE_TEMPLATE_GE910
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
	#ifndef PostOffice_Server
		#define PostOffice_Server 		"165.227.154.147"
	#endif
	#ifndef PostOffice_EndPoint
		#define PostOffice_EndPoint 	"/"
	#endif

	// Firewall Parameters
	#define _AT_FRWL_1_
	#ifdef _AT_FRWL_1_
		#define _AT_FRWL_1_IP_			"213.14.250.214"
	#endif

	#define _AT_FRWL_2_
	#ifdef _AT_FRWL_2_
		#define _AT_FRWL_2_IP_			"167.99.137.254"
	#endif

	#define _AT_FRWL_3_
	#ifdef _AT_FRWL_3_
		#define _AT_FRWL_3_IP_			"176.240.98.205"
	#endif

	// Define Server Address
	#ifndef FOTA_Server
		#define FOTA_Server 			"165.227.154.147"
	#endif

	// Define Server Folder
	#ifndef FOTA_Folder
		#define FOTA_Folder 			"firmware"
	#endif

	// Define Server UserName
	#ifndef FOTA_UserName
		#define FOTA_UserName 			"fota"
	#endif

	// Define Server Password
	#ifndef FOTA_PassWord
		#define FOTA_PassWord 			"123456"
	#endif

	// Define FTP SD Parameters
	#ifndef FOTA_SD_FileName
		#define FOTA_SD_FileName		"STF.hex"
	#endif


#endif