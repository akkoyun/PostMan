# Initialize Batch

GSM connection prosedure has two step. In first step system initialize GSM modem and get some definition parameters. Initialize prosedure strated in specified function;

```C++
// Initialize GSM Modem
bool Initialize(void);
```

Function returns ```true``` when everything is ok and ```false``` when an error accured.

```C++
// Set Control Variable
Status.Initialize;
```

Function also sets the init global function in GSM class.

***

## 1. At Test

```C++
--> AT // Command
<-- AT // Command Echo
<-- OK // Response
```
                          
***

## 2. Set Band Search Mode

```C++
--> AT#SEARCHLIM=100,100 // Command
<-- AT#SEARCHLIM=100,100 // Command Echo
<-- OK // Response
```

***

## 3. Set Modem Power Mode

    0 - Minimum functionality, in this mode, the AT interface is not accessible.
    1 - Mobile full functionality with power saving disabled. 
    2 - Disable TX
    4 - Disable either TX and RX
    5 - Mobile full functionality with power saving enabled

```C++
--> AT+CFUN=1 // Command
<-- AT+CFUN=1 // Command Echo
<-- OK // Response
```

***

## 4. Set Command Echo Mode

    0 - Disables command echo
    1 - Enables command echo

```C++
--> ATE0 // Command
<-- ATE0 // Command Echo
<-- OK // Response
```

***

## 5. Set Numeric Error Response

    0 - disable +CME ERROR:<err> reports, use only ERROR report.
    1 - enable +CME ERROR:<err> reports, with <err> in numeric format 
    2 - enable +CME ERROR: <err> reports, with <err> in verbose format

```C++
--> AT+CMEE=1 // Command
<-- OK // Response
```

***

## 6. Set Connection Mode

    0 - Data
    1 - Fax class 1 
    8 - Voice

```C++
--> AT+FCLASS=0 // Command
<-- OK // Response
```

***

## 7. Set Flow Control

    0 - No flow control
    1 - Hardware mono-directional flow control (only CTS active)
    2 - Software mono-directional flow control (XON/XOFF)
    3 - Hardware bi-directional flow control (both RTS/CTS active) (factory default) 
    4 - Software bi-directional with filtering (XON/XOFF)
    5 - Pass through: software bi-directional without filtering (XON/XOFF)
    6 - Both hardware bi-directional flow control (both RTS/CTS active) and software
    bi-directional flow control (XON/XOFF) with filtering

```C++
--> AT&K0 // Command
<-- OK // Response
```

***

## 8. Control SIM PIN Status

```C++
--> AT+CPIN? // Command
<-- +CPIN: READY // Response
<-- OK // Response
```

    Control for "READY" response

***

## 9. Get Modem IMEI Number

```C++
--> AT+CGSN // Command
<-- 355412119877544 // Response
<-- OK // Response
```

    Get modem IMEI number to variable.

***

## 10. Get Modem Serial Number

```C++
--> AT+GSN // Command
<-- 0001409405 // Response
<-- OK // Response
```

    Get modem serial number to variable.

***

## 11. Get the SIM ICCID Number

```C++
--> AT#CCID // Command
<-- #CCID: 8990011916180288209 // Response
<-- OK // Response
```

    Get SIM ICCID number to variable.

***

## 12. Get Modem Manufacturer Name

```C++
--> AT+GMI // Command
<-- Telit // Response
<-- OK // Response
```

    Get modem manufacturer name and convert to numeric variable
    
    1- Telit

***

## 13. Get Modem Moldel Name

```C++
--> AT+GMM // Command
<-- GE910-QUAD-V3// Response
<-- OK // Response
```

    Get modem model name and convert to numeric variable.

    1- GE910 QUAD
    2- GE910 QUAD V3

***

## 14. Get Modem Firmware Release

```C++
--> AT+GMR // Command
<-- 16.01.305// Response
<-- OK // Response
```

    Get modem firmware version to variable.

***

## 15. Set Modem LED Status

    0 - LED Off
    1 - LED On
    2 - LED handled by Module Software
    3 - LED is turned on and off alternatively

```C++
--> AT#SLED=2 // Command
<-- OK // Response
```

***

## 16. Save Modem LED Status

```C++
--> AT#SLEDSAV // Command
<-- OK // Response
```

***

## 17. Set Socket Listen Ring Interrupt Signal

    0 - RI disabled for Socket Listen connect
    50..1150 - RI enabled for Socket Listen connect

```C++
--> AT#E2SLRI=50 // Command
<-- OK // Response
```
