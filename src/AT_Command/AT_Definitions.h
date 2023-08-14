// Library include guard
#pragma once

// Include JSON Library
#include <ArduinoJson.h>

// Include RTC Library
#include <RV3028.h>

// Function Mode Definitions
#define SET						    true
#define GET						    false

// AT Command Output Definitions
#define _TIMEOUT_				    0
#define _OK_					    1
#define _ERROR_					    2
#define _CME_					    3

// SIM Code Definitions
#define _SIM_UNKNOWN_	    	    0
#define _SIM_READY_			        1
#define _SIM_PIN_			        2
#define _SIM_PUK_			        3
#define _SIM_NOT_INSERTED_	        false
#define _SIM_INSERTED_		        true

// Manufacturer Definitions
#define _UNKNOWN_MANUFACTURER_	    0
#define _TELIT_					    1

// Telit Model Definitions
#define _UNKNOWN_MODEL_			    0
#define _GE910_QUAD_			    10
#define _GE910_QUAD_V3_			    11
#define _GE910_GNSS_			    12
#define _LE910S1_EA_			    20
#define _LE910S1_EAG_			    21
#define _LE910R1_EU_			    30
#define _LE910C1_EUX_			    40

// Modem Status Definitions
#define _MODEM_INITIALIZED_		    true
#define _MODEM_NOT_INITIALIZED_	    false
#define _MODEM_CONNECTED_		    true
#define _MODEM_NOT_CONNECTED_	    false

// Define HTTP Headers
#define HTTP_OK					    200
#define HTTP_Accepted			    202
#define HTTP_NoContent			    204
#define HTTP_BadRequest			    400
#define HTTP_NotFound			    404
#define HTTP_MethodNotAllowed	    405
#define HTTP_NotAcceptable		    406

// Define FOTA Status
#define FOTA_Download_OK			1
#define FOTA_FTP_Open_Error			10
#define FOTA_FTP_Type_Error			11
#define FOTA_FTP_Folder_Error		12
#define FOTA_FTP_File_Size_Error	13
#define FOTA_FTP_Get_Error			14
#define FOTA_FTP_Close_Error		15
#define FOTA_Download_TimeOut		20
#define FOTA_Download_Size_Error	21
#define FOTA_Download_Not_Save		21
#define FOTA_SD_Error				30
