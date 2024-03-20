# Changelog

All notable changes to this project will be documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Planned

- Making terminal coordinates as define. May be on next release.
- Firmware download functions are still in testing.
- Get request control function may be changed.
- Namespace may be added to the library.

---

## [1.15.0]

- MD5 control is added to FTP file download function.

## [1.14.1]

- Download function improved. GET and FTP methods are used for downloading the firmware.
- New commands added. Download is now support for GET and FTP together.
- Bug fixed. Download function is now working properly.

## [1.14.0]

- JSON_Payload_Segment function updated.
- JSON_Energy_Segment function added.
- Download function updated. GET and FTP methods are used for downloading the firmware.

## [1.13.0]

- TAC and CELLID variables are added to Operator structure.
- IoT JSON parser function is updated. TAC and CELLID variables are added to the JSON parser function.
- RFSTS functions are updated. TAC and CELLID variables are added to the RFSTS function.
- _IoT_Buffer variable size (JSON) is updated from 150 to 200.

## [1.12.0]

- JSON_Payload_Segment function updated. FOTA data types are added.
- RFSTS command text fixed in connection function.
- Update_Connection_Status function added. This function is used to update the connection status.
- Payload variable parameters added in download function.
- SCFGEXT (to make normal) function is moved from download function to get function.
- Update_Connection_Status function is added on publish function.
- Update_Connection_Status function is added on Get function (update IoT parameters command).
- SKTTO Parameter is set to 0 (no timeout) in connection function.

## [1.11.0]

- No aditional library is used anymore. All used dependencies are writen by me.
- JSON functions are added. These functions are used to parse the JSON data.
- Variables are now controlled by the library. This is used to control the variables in the library.
- Downloaded firmware is now MD5 checked. This is used for checking the downloaded firmware.
- Modem function added. This function is used to ON/OFF the modem.
- Modem function is alse used for controling the modem's power.
- Disconnect function added. This function is used to disconnect the device from the cloud.

- Cloud function name is changed to Connect.
- Listen function is moved to public function.
- Firewall function is moved to public function.
- Syncronize_Time function is moved to public function.
- Interrupt_Handler and JSON_Parser functions are moved to protected functions.
- Status variable is moved to private variable.

---

## [1.10.0] - 2024-03-13

    * Initial updates and first stable version. Detailed documentation is start from these release.

---
