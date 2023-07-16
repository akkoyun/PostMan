// Include Arduino Library
#ifndef __Arduino__
	#include <Arduino.h>
#endif

// Terminal Parameters
#define DEBUG
#define GSM_Debug
#define CONSOLE_TEMPLATE_GE910

// Version Parameters
#define	__Hardware__			"00.00.01"
#define	__Firmware__			"00.00.20"

// Device Parameters
#define	__Company__				"Demo"
#define	__Device__				"PostOffice"

// Operator Parameters
#define _PDP_APN_				"mgbs"

// BackEnd Parameters
#define	_BackEnd_Server_		"165.227.154.147"
#define	_BackEnd_EndPoint_		"/"

// FireWall Parameters
#define	_AT_FRWL_1_IP_			"213.14.250.214"
#define	_AT_FRWL_2_IP_			"167.99.137.254"
#define	_AT_FRWL_3_IP_			"45.133.37.13"

// FOTA Parameters
#define	_FOTA_Server_		   	"165.227.154.147"
#define	_FOTA_Folder_		   	"firmware"
#define	_FOTA_UserName_			"fota"
#define	_FOTA_PassWord_			"123456"
#define	_FOTA_SD_FileName_		"STF.hex"

// LOG File Parameters
#define	_LOG_SD_File_Name_		"LOG.txt"