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
 * 
 * 
 * Simple WSPR transmitter 
 * Code based on this code: http://appnotes.etherkit.com/2016/04/10-milliwatt-si5351a-wspr-beacon/
 * 
 * Libraries:
 *            Modified version of etherkit si5351.h to use the Teensy enhanced i2c_t3.h library from Teensy Forum memebr Brian (nox771).
 *            (this should be included with the distribution zip file, along with the accociated si5351.cpp file).
 *            
 *            Modified version of ILI9341_t3.h to use the Teensy enhanced i2c_t3.h library from Teensy Forum memebr Brian (nox771).
 *            (this should be included with the distribution zip file, along with the accociated ILI9341_t3.cpp & glcfont.c files).
 *            
 * 
 * *********************************************************************************************
 * 
 * Hardware notes:
 *            - Teensy 3.2 module available from https://www.pjrc.com/store/teensy32.html 
 *            - I bought mine from eBay
 *                                               
 *            - tft module uses the ILI9341 driver there are different versions out there and the pinout changes,
 *            - the boards have been designed for this type:
 *            - https://www.elektor.com/2-2-spi-tft-display-module-240x320 
 *            - I bought mine from eBay
 *                            
 *            - Processor board version 1.1
 *            - Display board version 1.0
 *            - Button board version 1.0
 *                        
 *            - VFO output implemented on SI5351_CLK0
 *                            
 *                            
 * *********************************************************************************************
 *                            
 * Development environment:                           
 * 
 *            Teensyduino   version - 1.51  For instalation details see: https://www.pjrc.com/teensy/td_download.html           
 * 
 * *********************************************************************************************
 * 
 *  Version   - 0.1   - 25.09.2020 - Project start basic functionality running using threads
 *            - 0.2   - 26.09.2020 - Code tidy and restructure
 *            - 0.3   - 30.09.2020 - WSPR encoding and transmitting fine, display started
 *            - 0.5   - 02.10.2020 - Started to implement using i2c_t3.h
 *            - 0.6   - 03.10.2020 - Code tidy & gps serial sync issue Plus Maidenhead locator now
 *                                 - dynamically set.
 *            - 0.7   - 04.10.2020 - Buttons and encoder incorporated, inpreparation for config UI
 *                                 - Encoder button switchs between main WSPR display and config display.
 *            - 0.8   - 04.10.2020 - Bug Fix - Maidenhead display overwrite with bad data
 *                                 - Started config UI, config arrays defined.
 *                                 - Basic place holder config screen established.
 *                                 - Encoder switch enabled - page 1 (main) & page 2 (config) screens now switch
 *                                 - Si5351 output disabled when in config mode, when returning to main screen
 *                                 - transmission resumes on next timeslot.
 *            - 0.9   - 07.10.2020 - Config menu development continures
 *            - 0.10  - 08.10.2020 - Encoder interrupt issues so in this version I'm trying a different approach
 *                                 - MaidenHead locator now hard coded.
 *            - 0.11  - 08.10.2020 - Lat/Lon display issue resolved.
 *            - 0.12  - 08.10.2020 - Removed the encoder from the menu UI, now just using the buttons...
 *                                 - Mode now 'Entre' button
 *            - 0.13  - 09.10.2020 - Big cody rationalisation, removed encoder routines
 *                                 - Time slot and config array syncronised,
 *                                 - System basically complete.
 *                                 - To enter view configuration push encoder button
 *                                 - To scroll through the configuration for each time slot use the Up/Down buttons
 *                                 - To return to main screen push encoder button.
 *                                 - all other buttons do NOTHING, the config aray is hard coded.
 *                                 - Moved version number to Config screen
 *                                 - Bug fix for power display (screen artifacts cleared).
 *            - 0.14  - 10.10.2020 - Satallite number is now reporting correctly
 *                                 - HDOP value added to line 6.
 *                                 - Debug messages to serial port now via #define DEBUG
 *            - 1.0   - 10.10.2020 - Initial release
 *            
 * 
 * Issues:
 *        
 *     
 *       
 */

char VERSION[9] = "1.0";
// comment out if not required
//#define DEBUG

//******************************** Change to suit your station ************************************************
// Global variables
char call[7] = "M0KHZ";                             // Change this

// Eventually the Maidenhead locator will be automatically provided with the GPS but at the moment it's hard coded,
// So change this to suit your locator - use only the first 4 characters / 6 digit precision not allowed/recommended.
char loc[5] = "IO84";                               // Change this IO84

// The transmit frequency, power, timeslot etc. 
// are configured in an array located at the bottom of config_menu.h file

//*************************************************************************************************************





// add the libraries to support multitasking
#include <Arduino.h>
#include "TeensyThreads.h"

// add the libraries to support the display
#include "SPI.h"
#include "ILI9341_t3.h"

// tft display definitions
#define TFT_DC 20
#define TFT_CS 21
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

// used for button debounce etc.
#include <Bounce.h>

// add the library for the interrupt handler timer
#include <TimerOne.h>

// Button assignments:
#define CM_D              17  // down           -  left was 17
// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce bouncer_CM_D = Bounce(CM_D, 50);   // Config menu down

#define CM_U              14  // Up          -  right was 14
// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce bouncer_CM_U = Bounce(CM_U, 50);   // Config menu up

#define CM_S              23   // Enter          - Config - confirm selection   23 
// Instantiate a Bounce object with a 5 millisecond debounce time    
Bounce bouncer_CM_S = Bounce(CM_S,50 );   // Config confirm selection

#define TS_U              16  // Up             - Config timeslot up   was 15
// Instantiate a Bounce object with a 5 millisecond debounce time   
Bounce bouncer_TS_U = Bounce(TS_U,50 );   // Config timeslot up

#define TS_D            15  // Down           - Config timeslot down   was 16       
// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce bouncer_TS_D = Bounce(TS_D,50 );   // Config timeslot down

#define MODE                 2  //  MODE exchange - WSPR / Config menu
// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce bouncer_MODE = Bounce(MODE,50 );   // WSPR / Config menu

// Idex used with encoder and config updates
volatile int time_index = 0;
//volatile int old_time_index = 8;
volatile int menu_index = 0;

// Config menu 'stuff'
#include  "config_menu.h"

// #include <int.h>
// use this utility to create new picture content:
// http://www.rinkydinkelectronics.com/_t_doimageconverter565.php
//
#include "image_240_320.c"
#include "si5351.h"
//#include "Wire.h"
#include <i2c_t3.h>
#include <JTEncode.h>

#define TONE_SPACING            146                 // ~1.46 Hz
#define SYMBOL_COUNT            WSPR_SYMBOL_COUNT   //162

// Change this for your ref osc
// all testing with my hardware needed no correction factor
#define CORRECTION              0                  

// This is now the default value, is now changed in the configuration array
uint8_t dbm = 33;                             

uint8_t tx_buffer[SYMBOL_COUNT];              // for WSPR this value is 162

// This is now the default value, is now changed in the configuration array
unsigned long freq = 7040056UL; 
               
float lat = 28.5458;
float lon = 77.1703;
float oldLat;
float oldLon;
volatile bool proceed = false; // use in symbol timing loop
uint8_t num_sats;
float hdopvalue;

//volatile byte last_second;
byte last_second;
char Time[]  = "Time  :00:00:00";
char Date[]  = "Date  :00.00.2000";

// Create an IntervalTimer object 
IntervalTimer WSPR_SymbolPeriod;

Si5351 si5351;
JTEncode jtencode;

// Display drawing / refresh functions
#include "display_update.h"

// Button functions
#include "Button_functions.h"

// GPS functions
#define gpsSerial Serial3
#include <TinyGPS++.h>
TinyGPSPlus gps;
#include "gps_functions.h" 
 
// Loop through the string, transmitting one character at a time.
void WSPRencode()
{

    flag.txValid = true;
    flag.qrgTx = true;
    flag.qrgWait = false;

    uint8_t i;
  
    jtencode.wspr_encode(call, loc, dbm, tx_buffer);

    // Reset Si5351 just incase it has stalled within a thread (inturrupt issues)
    // might be able to delete this if threads.suspend/threads.restart works
    // if so need to move this statement back into setup()
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, CORRECTION);
  
    // Reset the tone to 0 and turn on the output
    si5351.output_enable(SI5351_CLK0, 1);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA); // Set for max power
       
    // Now do the rest of the message
    for(i = 0; i < SYMBOL_COUNT; i++)
    {
        si5351.set_freq((freq * 100) + (tx_buffer[i] * TONE_SPACING), SI5351_CLK0);
        proceed = false;
        while(!proceed);
    }  
    // Turn off the output
    si5351.output_enable(SI5351_CLK0, 0);
   
    flag.txValid = false;
    flag.qrgTx = false;
    flag.qrgWait = true;
}

void setup() {

// init display
  tft.begin();
  tft.setRotation(1); // will need to change this to 1 for panel mounting in case
  tft.fillScreen(ILI9341_BLACK);
  delay(50);
  // display welcome message
  splashscreen();

  // small delay before clearing screen
  delay(4000); // increasing above this value causes problems - need to understand this?
  // now clear the display
  tft.fillScreen(ILI9341_BLACK);
  delay(50);

// just incase we need some diagnostics
  Serial.begin(9600);
  // and comms to gps module

  #ifdef DEBUG
      Serial.print("Version Number: ");
      Serial.println(VERSION);
  #endif //DEBUG   
  
  // initialise physical buttons
  pinMode(CM_D, INPUT_PULLUP);
  pinMode(CM_U, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(CM_S, INPUT_PULLUP);
  pinMode(TS_U, INPUT_PULLUP);
  pinMode(TS_D, INPUT_PULLUP);

  // Initialise I2C Communications
  Wire.begin();

  WSPR_SymbolPeriod.begin(nextSymbol, 682716); // 0.682716 secs * 162 = 110.6 sec
  
  // all gps data serial comms, display functions and button functions are handled in a seperate threads, 
  // the WRSP symbols are handled in the main loop
  threads.addThread(fetchGpsData);
  threads.addThread(updateDisplay);
  threads.addThread(switch_bouncer);
}

void nextSymbol(){
  proceed = true;
}

void loop() {
  // put your main code here, to run repeatedly:
     
     if((Time[10]=='0') && (Time[11]=='0') && (Time[13]=='0') && (Time[14]=='1')){                // xx:00:01

        if((Config_Index[0][4])==1){
            
            freq=band[Config_Index[0][1]];
            dbm=TxPower[Config_Index[0][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[0][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
           
                
        
        }
           
     if((Time[10]=='0') && (Time[11]=='2') && (Time[13]=='0') && (Time[14]=='1')){                // xx:02:01
          
           if((Config_Index[1][4])==1){
            
            freq=band[Config_Index[1][1]];
            dbm=TxPower[Config_Index[1][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[1][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                  
           
        }
       
     if((Time[10]=='0') && (Time[11]=='4') && (Time[13]=='0') && (Time[14]=='1')){                // xx:04:01
           
          if((Config_Index[2][4])==1){
            
            freq=band[Config_Index[2][1]];
            dbm=TxPower[Config_Index[2][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[2][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                   
           
        }
       
     if((Time[10]=='0') && (Time[11]=='6') && (Time[13]=='0') && (Time[14]=='1')){                // xx:06:01
           
            if((Config_Index[3][4])==1){
            
            freq=band[Config_Index[3][1]];
            dbm=TxPower[Config_Index[3][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[3][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
          }
                    
           
        }
      
     if((Time[10]=='0') && (Time[11]=='8') && (Time[13]=='0') && (Time[14]=='1')){                // xx:08:01
           
            if((Config_Index[4][4])==1){
            
            freq=band[Config_Index[4][1]];
            dbm=TxPower[Config_Index[4][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[4][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
     
      if((Time[10]=='1') && (Time[11]=='0') && (Time[13]=='0') && (Time[14]=='1')){                // xx:10:01
           
            if((Config_Index[5][4])==1){
            
            freq=band[Config_Index[5][1]];
            dbm=TxPower[Config_Index[5][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[5][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                    
          
        }
      
     if((Time[10]=='1') && (Time[11]=='2') && (Time[13]=='0') && (Time[14]=='1')){                // xx:12:01
           
            if((Config_Index[6][4])==1){
            
            freq=band[Config_Index[6][1]];
            dbm=TxPower[Config_Index[6][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[6][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
      
     if((Time[10]=='1') && (Time[11]=='4') && (Time[13]=='0') && (Time[14]=='1')){                // xx:14:01
           
            if((Config_Index[7][4])==1){
            
            freq=band[Config_Index[7][1]];
            dbm=TxPower[Config_Index[7][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[7][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
      
     if((Time[10]=='1') && (Time[11]=='6') && (Time[13]=='0') && (Time[14]=='1')){                // xx:16:01
           
           if((Config_Index[8][4])==1){
            
            freq=band[Config_Index[8][1]];
            dbm=TxPower[Config_Index[8][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[8][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
      
     if((Time[10]=='1') && (Time[11]=='8') && (Time[13]=='0') && (Time[14]=='1')){                // xx:18:01
           
            if((Config_Index[9][4])==1){
            
            freq=band[Config_Index[9][1]];
            dbm=TxPower[Config_Index[9][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[9][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                    
           
        }
     
     if((Time[10]=='2') && (Time[11]=='0') && (Time[13]=='0') && (Time[14]=='1')){                // xx:20:01
           
            if((Config_Index[10][4])==1){
            
            freq=band[Config_Index[10][1]];
            dbm=TxPower[Config_Index[10][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[10][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                 
          
        }
     
     if((Time[10]=='2') && (Time[11]=='2') && (Time[13]=='0') && (Time[14]=='1')){                // xx:22:01
          
            if((Config_Index[11][4])==1){
            
            freq=band[Config_Index[11][1]];
            dbm=TxPower[Config_Index[11][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[11][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                   
           
        }
    
     if((Time[10]=='2') && (Time[11]=='4') && (Time[13]=='0') && (Time[14]=='1')){                // xx:24:01
           
            if((Config_Index[12][4])==1){
            
            freq=band[Config_Index[12][1]];
            dbm=TxPower[Config_Index[12][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[12][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                    
          
        }
    
     if((Time[10]=='2') && (Time[11]=='6') && (Time[13]=='0') && (Time[14]=='1')){                // xx:26:01
           
            if((Config_Index[13][4])==1){
            
            freq=band[Config_Index[13][1]];
            dbm=TxPower[Config_Index[13][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[13][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
    
     if((Time[10]=='2') && (Time[11]=='8') && (Time[13]=='0') && (Time[14]=='1')){                // xx:28:01
           
           if((Config_Index[14][4])==1){
            
            freq=band[Config_Index[14][1]];
            dbm=TxPower[Config_Index[14][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[14][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                    
           
        }
     
     if((Time[10]=='3') && (Time[11]=='0') && (Time[13]=='0') && (Time[14]=='1')){                // xx:30:01
           
            if((Config_Index[0][4])==1){
            
            freq=band[Config_Index[0][1]];
            dbm=TxPower[Config_Index[0][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[0][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
    
     if((Time[10]=='3') && (Time[11]=='2') && (Time[13]=='0') && (Time[14]=='1')){                // xx:32:01
          
            if((Config_Index[1][4])==1){
            
            freq=band[Config_Index[1][1]];
            dbm=TxPower[Config_Index[1][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[1][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
     
     if((Time[10]=='3') && (Time[11]=='4') && (Time[13]=='0') && (Time[14]=='1')){                // xx:34:01
          
           if((Config_Index[2][4])==1){
            
            freq=band[Config_Index[2][1]];
            dbm=TxPower[Config_Index[2][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[2][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                    
           
        }
    
     if((Time[10]=='3') && (Time[11]=='6') && (Time[13]=='0') && (Time[14]=='1')){                // xx:36:01
          
            if((Config_Index[3][4])==1){
            
            freq=band[Config_Index[3][1]];
            dbm=TxPower[Config_Index[3][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[3][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                   
           
        }
     
     if((Time[10]=='3') && (Time[11]=='8') && (Time[13]=='0') && (Time[14]=='1')){                // xx:38:01
          
           if((Config_Index[4][4])==1){
            
            freq=band[Config_Index[4][1]];
            dbm=TxPower[Config_Index[4][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[4][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                    
          
        }
     
     if((Time[10]=='4') && (Time[11]=='0') && (Time[13]=='0') && (Time[14]=='1')){                // xx:40:01
          
           if((Config_Index[5][4])==1){
            
            freq=band[Config_Index[5][1]];
            dbm=TxPower[Config_Index[5][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[5][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
     
     if((Time[10]=='4') && (Time[11]=='2') && (Time[13]=='0') && (Time[14]=='1')){                // xx:42:01
           
           if((Config_Index[6][4])==1){
            
            freq=band[Config_Index[6][1]];
            dbm=TxPower[Config_Index[6][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[6][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
     
     if((Time[10]=='4') && (Time[11]=='4') && (Time[13]=='0') && (Time[14]=='1')){                // xx:44:01
          
            if((Config_Index[7][4])==1){
            
            freq=band[Config_Index[7][1]];
            dbm=TxPower[Config_Index[7][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[7][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                    
           
        }
    
     if((Time[10]=='4') && (Time[11]=='6') && (Time[13]=='0') && (Time[14]=='1')){                // xx:46:01
          
           if((Config_Index[8][4])==1){
            
            freq=band[Config_Index[8][1]];
            dbm=TxPower[Config_Index[8][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[8][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                   
           
        }
    
     if((Time[10]=='4') && (Time[11]=='8') && (Time[13]=='0') && (Time[14]=='1')){                // xx:48:01
          
            if((Config_Index[9][4])==1){
            
            freq=band[Config_Index[9][1]];
            dbm=TxPower[Config_Index[9][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[9][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                   
           
        }
     
     if((Time[10]=='5') && (Time[11]=='0') && (Time[13]=='0') && (Time[14]=='1')){                // xx:50:01
          
            if((Config_Index[10][4])==1){
            
            freq=band[Config_Index[10][1]];
            dbm=TxPower[Config_Index[10][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[10][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
    
     if((Time[10]=='5') && (Time[11]=='2') && (Time[13]=='0') && (Time[14]=='1')){                // xx:52:01
          
            if((Config_Index[11][4])==1){
            
            freq=band[Config_Index[11][1]];
            dbm=TxPower[Config_Index[11][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[11][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                    
           
        }
     
     if((Time[10]=='5') && (Time[11]=='4') && (Time[13]=='0') && (Time[14]=='1')){                // xx:54:01
         
            if((Config_Index[12][4])==1){
            
            freq=band[Config_Index[12][1]];
            dbm=TxPower[Config_Index[12][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[12][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
       
           }           
           
        }
     
     if((Time[10]=='5') && (Time[11]=='6') && (Time[13]=='0') && (Time[14]=='1')){                // xx:56:01
          
            if((Config_Index[13][4])==1){
            
            freq=band[Config_Index[13][1]];
            dbm=TxPower[Config_Index[13][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[13][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
           
        }
     
     if((Time[10]=='5') && (Time[11]=='8') && (Time[13]=='0') && (Time[14]=='1')){                // xx:58:01
          
            if((Config_Index[14][4])==1){
            
            freq=band[Config_Index[14][1]];
            dbm=TxPower[Config_Index[14][3]];
            
            #ifdef DEBUG
                Serial.print("Slot Active - ");
                Serial.print(Time);
                Serial.print(" ");
                Serial.print(freq);
                Serial.print(" kHz  ");
                Serial.print(dbm);
                Serial.print(" dBm  ");
                Serial.print("Control byte -");
                Serial.print(" 0x");
                Serial.println(ControlBit[Config_Index[14][2]],HEX);
              #endif //DEBUG      

            WSPRencode();
        }
                     
     }   
 
}

// ****** End of File
