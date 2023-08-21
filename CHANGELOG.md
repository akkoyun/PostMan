# Release Notes

## 0.17.2 (2023-08-21)

* Read Serial ID function deleted. [WeatherStat]
* DS28C library added. [WeatherStat]

## 0.17.0 (2023-08-17)

* Environment measurement library added. [WeatherStat]
* Measurement structure deleted. [WeatherStat]
* Get_Serial_ID function added. [WeatherStat]
* Environment measurement function added. [WeatherStat]
* Pack type decide updated. [WeatherStat]
* Battery measurements added. [WeatherStat]
* JSON payload segment updated. [WeatherStat]

## 0.16.4 (2023-08-15)

* Response_JSON_Size definition updated. [WeatherStat]
* AT response fail control updated. [WeatherStat]
* Measurement 0 value control updated. [WeatherStat]
* Publish function updated. [WeatherStat]

## 0.16.3 (2023-08-15)

* Time structure moved to public.
* Pack type definitions added.
* Parse_JSON function updated.
* Measurement structure updated.

## 0.16.2 (2023-08-15)

* Hardware class updated.
* GNSS power order changed.

## 0.16.1 (2023-08-14)

* Callback functions removed from WeatherStat class.
* RTC functions removed from WeatherStat class.
* Device variables structure added.
* JSON parse function updated.
* LE910S1-EAG model added.
* MONI function updated.

## 0.16.0 (2023-08-14)

* Project updates.
* AT Command memory leak fixed.
* General updates.

## 0.15.6 (2023-08-11)

* Progma once protections added to all header files.

## 0.15.5 (2023-08-11)

* GNSS_EN function added.
* Definition choise functions updated.
* Console includes moved to product class.

## 0.15.4 (2023-08-11)

* LE910S1-EAG AT Command added.
* B108AA hardware added.

## 0.15.3 (2023-08-11)

* WeatherStat device added.

## 0.15.2 (2023-08-11)

* GE910-GNSS model added.

## 0.15.1 (2023-08-11)

* Library updated for platformio.

## 0.15.0 (2023-08-11)

* Library updated for platformio.
* AT Command library separeted for module type.
* AT Command library updated.
* Hardware library updated.

## 0.14.6 (2023-08-04)

* Socket ID constants defined in config file.
* Macros deleted.
* Buffer structure added.
* WDS type print functions updated.
* Firewall batch function updated.
* Update_Connection_Variables function deleted.
* Parse JSON batch updated.
* PowerMonitor function deleted.
* SWReady function deleted.
* Find_Error function added to SD function.
* Send_Header function renamed to Header.
* FRWL function updated.
* SRING function rewritten.

## 0.14.5 (2023-08-04)

* TimeZone variable added.
* Read clock procedure updated.

## 0.14.4 (2023-08-04)

* Time variable structure moved to private.
* Connection time procedure updated.
* Firewall procedure moved to function.
* Close procedure moved to function.
* Update_Connection_Variables function updated.
* Online procedure updated.

## 0.14.3 (2023-08-03)

* IoT_Operator variables updated.
* Initialize procedure updated.
* Connection procedure updated.
* RSSI_to_Signal_Quality function added.
* GPIO function updated.
* WS46 function added.
* RFSTS function added.

## 0.14.2 (2023-08-02)

* NTP function handle updated.

## 0.14.1 (2023-08-02)

* Firmware variable size updated.
* PowerMonitor control function added.
* LE910R1-EU model update.
* Initialisation procedure updated.
* Connection procedure updated.
* Find ERROR function added.
* AT command response delay updated.
* AT commands updated (multi).

## 0.13.7 (2023-07-24)

* No DEBUG version updated.
* Pressure callback function added.

## 0.13.6 (2023-07-24)

* Pack type definitions added.
* Includes file updated.
* Start and Stop callback functions added.
* ON and OFF functions moved to public.
* Get_Serial_ID function deleted.
* Mask read function deleted.
* Pressure variables deleted.
* Fake ring clear function added to Online procedure.
* GET function updated.
* Max and Min pressure limit control updated.

## 0.13.4 (2023-07-21)

* Environment and Battery libraries removed from library.
* Environment callback function added.
* Battery callback function added.
* JSON_Data structure deleted.
* JSON parse function updated.

## 0.13.3 (2023-07-21)

* Terminal Console moved from includes to class.
* HTTP headers added to definitions.
* Send_JSON_Size definition added.
* Hardware class updated and moved to postman class.
* Power pin macros added.
* IoT_Status, IoT_Module, IoT_Operator and IoT_FOTA classes moved to postman class.
* On and Off functions added to postman class.
* On and Off functions updated.
* IoT_Status.Power variable removed.
* Console library GSM_Command function adepted.
* Some function ends updated.
* Command variables added to class.
* GET and Publish functions updated.
* EEPROM get variables limit constant added.
* FOTA download function moved to class internal.
* Hardware.h file removed from library.
* FTPRECV timeout updated.

## 0.13.2 (2023-07-20)

* Delete EEPROM adres on definition.
* Read_Masks function added.
* Read masks on online procedure added.
* Command update JSON object updated.
* Command update function updated.
* SS function buffer size updated.

## 0.13.1 (2023-07-19)

* Terminal_Defaults file deleted.
* Register definitions removed from library.
* FOTA_Info interrupt removed.
* LAC and Cell_ID variables changed to HEX string.
* Start and stop commands added.
* Send time calculation added.
* Communication function changed to macro.
* Power_Switch function changed to macro.
* LED functions changed to macro.

## 0.13.0 (2023-07-18)

* MONIZIP bug fixed.
* Status variable changed to IoT_Status.
* Power variables changed to IoT_Power.
* Module variables changed to IoT_Module.
* Operator variables changed to IoT_Operator.
* FOTA_Variables changed to IoT_FOTA.
* PowerStat variable added.
* JSON_Parse function updated.
* uint64ToString function moved to public.
* Status bit definitions added.

## 0.12.16 (2023-07-17)

* Device status and fault variables updated.

## 0.12.15 (2023-07-17)

* Interrupt clear function updated.

## 0.12.14 (2023-07-17)

* FOTA Status variables added.
* Recieve JSON size variable updated.
* GET function updated.
* Download function updated.
* Find CME error function updated.
* FTPRECV function updated.

## 0.12.13 (2023-07-17)

* Connection procedure updated. CGREG query command writen wrong in procedure.
* JSON Parse function updated.
* JSON LOG function added when not send data.
* FOTA download command updated.
* FOTA download procedure updated.
* LOG FUnction name changed to JSON_LOG.
* FTP commands timeout variable updated.

## 0.12.12 (2023-07-17)

* Library update for platformio.

## 0.12.11 (2023-07-17)

* FTP Commands timeout variable updated.
* FTPRECV function updated.
* JSON Parse function updated.
* Handle JSON request function deleted.
* GET function updated.
* Download function updated.

## 0.12.10 (2023-07-16)

* AT_Definitions file removed from library.
* FOTA class removed, download function moved to Postman class.
* All FOTA variables moved to postman class.
* Send_Response function deleted.
* Response function updated.
* Recieve command and send response JSON size are defined in constant.
* Download function updated.
* Set_status function deleted.
* FOTO function deleted in postman class.
* Config file updated.

## 0.12.9 (2023-07-16)

* GSM clas removed.
* All GSM class function moved to Postman class.
* Serial_ID function updated.
* GSM_Debug definitions changed to DEBUG

## 0.12.8 (2023-07-16)

* SIMDET function updated.
* CPIN function updated.
* CGSN function updated.
* GSN function updated.
* CCID function updated.
* GMI function updated.
* GMM function updated.
* GMR function updated.
* CREG functions updated.
* CGREG functions updated.
* SGACT function updated.
* MONIZIP function updated.
* State, Modem and Operator variables moved to GSM class.
* FTPGETPKT function updated.
* Initialize procedure updated.
* Connection procedure updated.
* Listen procedure updated.

## 0.12.6 (2023-07-14)

* Initialisation procedure updated.
* Connection setup procedure updated.
* Single connection mode added.
* Power functions updated.
* Publish functions updated.
* Header functions updated.
* Documentation update.

## 0.12.5 (2023-07-13)

* Publish parameters are tuned.
* Documentation update.

## 0.12.4 (2023-07-13)

* FTP firmware download system updated.
* #FTPRECV command updated.
* FTP firmware download CME error handling added.
* FTP_INFO pack updated.
