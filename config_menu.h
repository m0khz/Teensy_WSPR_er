/*
 * ***********************************************************************************
 * Copyright (c) 2020  M0KHZ - Kevin Wheatley
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * ***********************************************************************************
*/


// WSPR Tx frequency data - index used below (TxFlag table) to set Tx sequence slot(s)
// Do not alter these values unless the international WSPR frequencies change!
// Data correct on October 2020
unsigned long band[15] ={
                137456UL, // index 0
                475656UL, // index 1
               1838056UL, // index 2
               3570056UL, // index 3
               5288656UL, // index 4
               7040056UL, // index 5 
              10140156UL, // index 6
              14097056UL, // index 7
              18106056UL, // index 8
              21096056UL, // index 9
              24926056UL, // index 10
              28126056UL, // index 11
              50693456UL, // index 12
              70293456UL, // index 13
             144793456UL, // index 14
};

// Pick the nearest value to your Tx output power
// The WSPR system doesn't recognise any other values
// Do not alter these values
uint8_t TxPower [18] ={
            00, // dBm = 1mW    - index 0 
            03, // dBm = 2mW    - index 1
            07, // dBm = 5mW    - index 2
            10, // dBm = 10mW   - index 3
            13, // dBm = 20mW   - index 4
            17, // dBm = 50mW   - index 5
            20, // dBm = 100mW  - index 6
            23, // dBm = 200mW  - index 7
            27, // dBm = 500mW  - index 8
            30, // dBm = 1W     - index 9
            33, // dBm = 2W     - index 10
            37, // dBm = 5W     - index 11
            40, // dBm = 10W    - index 12
            43, // dBm = 20W    - index 13
            47, // dBm = 50W    - index 14
            50, // dBm = 100W   - index 15
            53, // dBm = 200W   - index 16
            57  // dBm = 500W   - index 17 
};


// Future plans are to have additional plug-in hardware, this will include bandpass filter switching
// The plan is to use a I2C controlled switch (PCF8574A) followed by a buffer (ULN2803A) to drive relays.
// This control bit array will define the hex values to control the PCF8574A.
// This array will grow over time but at present these control bits are NOT used
// Additional hardware to follow.

int ControlBit [4] ={
            64,   // index 0   - 0100 0000 
            128,  // index 1   - 1000 0000
            32,   // index 2   - 0010 0000
            160,  // index 3   - 1010 0000   
};


// Do not alter these values, ony used for tft display
String TSlot [15] ={
        "xx:00:01 - xx:30:01",  // Time slot 0    - index 0
        "xx:02:01 - xx:32:01",  // Time slot 1    - index 1
        "xx:04:01 - xx:34:01",  // Time slot 2    - index 2
        "xx:06:01 - xx:36:01",  // Time slot 3    - index 3
        "xx:08:01 - xx:38:01",  // Time slot 4    - index 4
        "xx:10:01 - xx:40:01",  // Time slot 5    - index 5
        "xx:12:01 - xx:42:01",  // Time slot 6    - index 6
        "xx:14:01 - xx:44:01",  // Time slot 7    - index 7
        "xx:16:01 - xx:46:01",  // Time slot 8    - index 8
        "xx:18:01 - xx:48:01",  // Time slot 9    - index 9
        "xx:20:01 - xx:50:01",  // Time slot 10   - index 10
        "xx:22:01 - xx:52:01",  // Time slot 11   - index 11
        "xx:24:01 - xx:54:01",  // Time slot 12   - index 12
        "xx:26:01 - xx:56:01",  // Time slot 13   - index 13
        "xx:28:01 - xx:58:01",  // Time slot 14   - index 14
  
};

// System config data
// change to suit yuor specific requirements.
  int Config_Index [15] [5] ={
            // Time slot index | Band index | ControlBit index  | Power index | Time slot active    |
            //   Do NOT change |     0 - 14 |     0 - 3         |     0 - 17  |   0 or 1 - 1 active |
                {    0           ,     5     ,       1           ,     10  ,            1           },  // Time slot 0  "xx:00:01 - xx:30:01"
                {    1           ,     5     ,       2           ,     10  ,            0           },  // Time slot 1  "xx:02:01 - xx:32:01"
                {    2           ,     5     ,       0           ,     10  ,            1           },  // Time slot 2  "xx:04:01 - xx:34:01"
                {    3           ,     5     ,       3           ,     10  ,            0           },  // Time slot 3  "xx:06:01 - xx:36:01"
                {    4           ,     5     ,       0           ,     10  ,            1           },  // Time slot 4  "xx:08:01 - xx:38:01"
                {    5           ,     5     ,       2           ,     10  ,            0           },  // Time slot 5  "xx:10:01 - xx:40:01"
                {    6           ,     5     ,       1           ,     10  ,            1           },  // Time slot 6  "xx:12:01 - xx:42:01"
                {    7           ,     5     ,       0           ,     10  ,            0           },  // Time slot 7  "xx:14:01 - xx:44:01"
                {    8           ,     5     ,       1           ,     10  ,            1           },  // Time slot 8  "xx:16:01 - xx:46:01"
                {    9           ,     5     ,       2           ,     10  ,            0           },  // Time slot 9  "xx:18:01 - xx:48:01"
                {    10          ,     5     ,       0           ,     10  ,            1           },  // Time slot 10 "xx:20:01 - xx:50:01"
                {    11          ,     5     ,       3           ,     10  ,            0           },  // Time slot 11 "xx:22:01 - xx:52:01"
                {    12          ,     5     ,       0           ,     10  ,            1           },  // Time slot 12 "xx:24:01 - xx:54:01"
                {    13          ,     5     ,       2           ,     10  ,            0           },  // Time slot 13 "xx:26:01 - xx:56:01"
                {    14          ,     5     ,       1           ,     10  ,            1           }   // Time slot 14 "xx:28:01 - xx:58:01"
                     
};

// ****** End of File
