# Data Send Procedure

## 1. Introduction

This document describes the procedure to send data to the server using the GSM module. We use 2 different socket connections for this procedure. One of them is used for sending data to the server, the other is used for incoming data listening. The data is sent to the server in JSON format. The data is sent to the server every t minutes. The data is sent to the server in the following format:

```C++
{
    "Command":"Demo:PostOffice.Timed",
    "Device":{
        "Info":{
            "ID":"B0000011D14B670"
        },
        "Power":{
            "Battery":{
                "IV":4.104219,
                "AC":3.75,
                "SOC":99,
                "Charge":3
            }
        },
        "IoT":{
            "GSM":{
                "Operator":{
                    "Code":28601,
                    "dBm":97,
                    "LAC":34142,
                    "Cell_ID":19217
                }
            }
        }
    },
    "Payload":{
        "TimeStamp":"2023-07-13 16:06:12"
    }
}
```

Data pack structure is described in definations repository.

## 2. Data Send Procedure

### 2.1. Socket Connection

Every t minutes, the GSM module sends the data to the server. The data is sent in the following format:

```C++
--> AT#MONIZIP\r\n
<-- \r\n#MONIZIP: 28601,52,6,855E,4B11,6,-97,4\r\n\r\nOK\r\n
```

After this command we handle some connection variables.

    #MONIZIP: <cc><nc>,<bsic>,<qual>,<lac>,<id>,<arfcn>, <dBm>, <timadv>

* 286 is the country code.
* 01 is the network operator code.
* 52 is the base station identity code.
* 6 is the signal reception quality (0-7).
* 855E is the location area code.
* 4B11 is the cell identity.
* 6 is the absolute radio frequency channel number.
* -97 is the received signal strength indication.
* 4 is the timing advance.

On these variables we handle some connection variables to add on JSON pack.

* Operator Name (cc-nc)
* Location Area Code (lac)
* Cell Identity (id)
* Signal strength (dBm)

### 2.2. Socket Connection

After handling these variables, we open a socket connection to the server.

```C++
--> AT#SD=3,0,80,"165.227.154.147",255,88,1\r\n
<-- \r\nOK\r\n
```

### 2.3. Data Send

When the connection is established, we send the data to the server. (Socket ID = 3)

```C++
--> AT#SSEND=3\r\n
<-- \r\n>
--> 
    POST / HTTP/1.1\r\n
    Host: 165.227.154.147\r\n
    Content-Length: 269\r\n
    Content-Type: application/json\r\n
    User-Agent: PostOffice\r\n\r\n
    {"Command":"Demo:PostOffice.Timed","Device":{"Info":{"ID":"B0000011D14B670"},"Power":
    {"Battery":{"IV":4.104219,"AC":3.75,"SOC":99,"Charge":3}},"IoT":{"GSM":{"Operator":
    {"Code":28601,"dBm":97,"LAC":34142,"Cell_ID":19217}}}},"Payload":{"TimeStamp":"2023-07-13 
    16:06:12"}}
    \x1A
<-- \r\nOK\r\n
```

### 2.4. Ring Response

After sending the data, server response ring is received.

```C++
<-- \r\nSRING: 3,120\r\n
```

### 2.5. Data Recieve

Then we recieve the response from the server.

```C++
--> AT#SRECV=3,120\r\n
<-- \r\n#SRECV: 3,120\r\n
    HTTP/1.1 406 Not Acceptable\r\n
    Content-Type: application/json\r\nContent-Length: 13\r\n
    Connection: keep-alive\r\n\r\n
    {"Event":201}\r\n\r\nOK\r\n
```

### 2.6. Socket Close

After recieving the response, we close the socket connection. (Socket ID = 3)

```C++
--> AT#SH=3\r\n
<-- \r\nOK\r\n
```

### 2.7. Incoming Socket Control

Data send procedure is completed. But we make sure for incoming connection still open. (Socket ID = 2)

```C++
--> AT#SS=2\r\n
<-- \r\n#SS: 2,4,5.26.173.230,80\r\n\r\nOK\r\n
```

We handle the port status for control.

    #SS: <connId>,<state>,<locIP>,<locPort>,<remIP>,<remPort>

* 2 is the socket identifier.
* 4 is the socket state. (0: Closed, 1: Listening, 2: Connection pending, 3: Connected, 4: Closing)
* "5.26.173.230" is the local IP address.
* 80 is the local port number.

If the socket state is 2 (listening), data send procedure is completed. If the socket state is different from 2, we reopen the socket connection.
