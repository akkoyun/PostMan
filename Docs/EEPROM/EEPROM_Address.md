# RTC EEPROM Address Table

| Address | Decimal | Description             | Default Value | Constant Name           |
|---------|---------|-------------------------|---------------|-------------------------|
| 0x00    | 0       | Online Interval         | 600 Sn        | EEPROM_Online_Interval  |
| 0x02    | 2       | Offline Interval        | 1800 Sn       | EEPROM_Offline_Interval |
| 0x04    | 4       | Alarm Duration          | 30 Sn         | EEPROM_Alarm_Duration   |
| 0x06    | 6       |                         |               |                         |
| 0x08    | 8       |                         |               |                         |
| 0x0A    | 10      | Voltage Min Limit       | 190 V         | EEPROM_V_Min            |
| 0x0C    | 12      | Voltage Max Limit       | 253 V         | EEPROM_V_Max            |
| 0x0E    | 14      | Current Max Limit       | 5 A           | EEPROM_I_Max            |
| 0x10    | 16      | Frequency Min Limit     | 47 Hz         | EEPROM_FQ_Min           |
| 0x12    | 18      | Frequency Max Limit     | 52 Hz         | EEPROM_FQ_Max           |
| 0x14    | 20      | Voltage Imbalance Limit | 6 %           | EEPROM_VIMB_Max         |
| 0x16    | 22      | Current Imbalance Limit | 6 %           | EEPROM_IIMB_Max         |
| 0x18    | 24      |                         |               |                         |
| 0x1A    | 26      |                         |               |                         |
| 0x1C    | 28      |                         |               |                         |
| 0x1E    | 30      | Regression Limit        | 10 %          | EEPROM_P_Regression     |
| 0x20    | 32      | Pmax Limit              | 10000 mBar    | EEPROM_PMAX             |
| 0x22    | 34      |                         |               |                         |
| 0x24    | 36      |                         |               |                         |
| 0x26    | 38      | CT Multiplier           | 40 x          | EEPROM_Current_Ratio    |
| 0x28    | 40      | Fault Defaults          | ----          | EEPROM_Fault_Control    |

