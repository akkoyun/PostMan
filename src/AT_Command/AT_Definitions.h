// Library include guard
#pragma once

// Include JSON Library
#include <ArduinoJson.h>

// Function Mode Definitions
#define SET						    true
#define GET						    false

// Define HTTP Method
#define HTTP_RESPONSE			    0
#define HTTP_GET				    1
#define HTTP_POST				    2
#define HTTP_PUT				    3
#define HTTP_DELETE				    4

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
#define FOTA_Download_TimeOut		20
#define FOTA_Download_Size_Error	21
#define FOTA_Download_Not_Save		21
#define FOTA_SD_Error				30
