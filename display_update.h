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

#include "_fonts.h"

// Things to make the display look pretty :)
#include "display_co_ordinates.h"


void splashscreen(void)
{
   tft.writeRect(0,0,320,240,(uint16_t*)WSPR_Pic); // Draw a pretty picture on screen
}

void rectangle(int x, int y, int w, int d, uint16_t color) 
{
  tft.drawRect(x, y, w, d, color);
}

void updateDisplay(){

  char currentTime7 = 1;
  char previousTime7 = 0;
  char currentTime8 = 1;
  char previousTime8 = 0;
  char currentTime10 = 1;
  char previousTime10 = 0;
  char currentTime11 = 1;
  char previousTime11 = 0;
  char currentTime13 = 1;
  char previousTime13 = 0;
  char currentTime14 = 1;
  char previousTime14 = 0;

  uint8_t currentdBm = 0;
  uint8_t previousdBm = 1;

  uint8_t currentnum_sats = 0;
  uint8_t previousnum_sats = 1;

  float currenthdopvalue = 0.00;
  float previoushdopvalue = 0.10;
  
  // this is run as a seperate thread so needs to loop continually hence the while(1) statement
  while(1){

    if(flag.ReDraw_Screen==true)
    {
  
      if(flag.gpsHardware==false){
          if(flag.ReDrawgpsHardware==false){
              tft.fillScreen(ILI9341_BLACK);
              // draw boarder around full display
              rectangle(outline_boarder_x, outline_boarder_y, outline_boarder_w, outline_boarder_d, outline_boarder_colour);
              tft.fillRect((main_boarder_x +1),(main_boarder_y + 1), (main_boarder_w - 3), (main_boarder_d - 2), ILI9341_BLACK);
              tft.setCursor(main_boarder_x + 7,main_boarder_y + 10); 
              tft.setTextColor(info_text_colour);
              tft.setFont(DroidSansMono_12);
              tft.print("No GPS detected...");
              tft.setCursor(main_boarder_x + 7,main_boarder_y + 30);
              tft.print("Checking again...");
              flag.ReDrawgpsHardware=true;
          }
      } 
  
      if(flag.gpsHardware==true && flag.gpsSerialData==true){
        if(flag.ReDrawSerialData==false){
              tft.fillScreen(ILI9341_BLACK);
              // draw boarder around full display
              rectangle(outline_boarder_x, outline_boarder_y, outline_boarder_w, outline_boarder_d, outline_boarder_colour);
              tft.fillRect((main_boarder_x +1),(main_boarder_y + 1), (main_boarder_w - 3), (main_boarder_d - 2), ILI9341_BLACK);
              tft.setCursor(main_boarder_x + 7,main_boarder_y + 10); 
              tft.setTextColor(info_text_colour);
              tft.setFont(DroidSansMono_12);
              tft.print("Hardware detected...");
              tft.setCursor(main_boarder_x + 7,main_boarder_y + 30);
              tft.print("Synchronising data stream...");
              flag.ReDrawSerialData=true;
        }
      }
      
      if(flag.gps_valid==true){
        flag.ReDraw_Screen=false;
      }  
    }

    if(!flag.page_two){
      
        if((flag.gps_valid==true)&&(flag.page_one==false)){
             tft.fillScreen(ILI9341_BLACK);
             // draw boarder around full display
             rectangle(outline_boarder_x, outline_boarder_y, outline_boarder_w, outline_boarder_d, outline_boarder_colour);
            //threads.delay(100); // To stop TFT weired stuff
            if(flag.page_two==false){
                 flag.page_one=true;
                    
          }  
       }   
    } 

     // if something clear screen for main screen text flag.refreshPageOne==true
        if(flag.refreshPageOne==true && flag.page_two==false){
             tft.fillScreen(ILI9341_BLACK);
             // draw boarder around full display
             rectangle(outline_boarder_x, outline_boarder_y, outline_boarder_w, outline_boarder_d, outline_boarder_colour);
            flag.refreshPageOne=false;
            Serial.println("clearing screen after config menu");      
        }

    if(flag.page_one==true){
      flag.page_two=false;
      flag.ReDrawConfScreen=true;
      
// Line one **********************************************************************************

      tft.setCursor(line_one_pos_x,line_one_pos_y); 
      tft.setTextColor(line_one_text_colour);
      tft.setFont(DroidSansMono_24);

      currentTime7 = Time[7];
      if(currentTime7 != previousTime7) {
          previousTime7 = currentTime7;
           tft.fillRect((line_one_pos_x + 10),(line_one_pos_y), (19), (24), ILI9341_BLACK);
           tft.setCursor(line_one_pos_x + 10,line_one_pos_y);
           tft.print(Time[7]);
      }

      currentTime8 = Time[8];
      if(currentTime8 != previousTime8) {
          previousTime8 = currentTime8;
           tft.fillRect((line_one_pos_x + 30),(line_one_pos_y), (19), (24), ILI9341_BLACK);
           tft.setCursor(line_one_pos_x + 30,line_one_pos_y);
           tft.print(Time[8]);
      }

      tft.setCursor(line_one_pos_x + 45,line_one_pos_y);
      tft.print(":");

      currentTime10 = Time[10];
      if(currentTime10 != previousTime10) {
          previousTime10 = currentTime10;
           tft.fillRect((line_one_pos_x + 65),(line_one_pos_y), (19), (24), ILI9341_BLACK);
           tft.setCursor(line_one_pos_x + 65,line_one_pos_y);
           tft.print(Time[10]);
      }

      currentTime11 = Time[11];
      if(currentTime11 != previousTime11) {
          previousTime11 = currentTime11;
           tft.fillRect((line_one_pos_x + 85),(line_one_pos_y), (19), (24), ILI9341_BLACK);
           tft.setCursor(line_one_pos_x + 85,line_one_pos_y);
           tft.print(Time[11]);
      }

      tft.setCursor(line_one_pos_x + 100,line_one_pos_y);
      tft.print(":");

      currentTime13 = Time[13];
      if(currentTime13 != previousTime13) {
          previousTime13 = currentTime13;
           tft.fillRect((line_one_pos_x + 120),(line_one_pos_y), (19), (24), ILI9341_BLACK);
           tft.setCursor(line_one_pos_x + 120,line_one_pos_y);
           tft.print(Time[13]);
      }
      currentTime14 = Time[14];
      if(currentTime14 != previousTime14) {
          previousTime14 = currentTime14;
           tft.fillRect((line_one_pos_x + 140),(line_one_pos_y), (19), (24), ILI9341_BLACK);
           tft.setCursor(line_one_pos_x + 140,line_one_pos_y);
           tft.print(Time[14]);
           tft.setFont(DroidSansMono_16);
           tft.setCursor(line_one_pos_x + 185,line_one_pos_y +8);
           tft.print("   UTC");
      }
      
// Line two **********************************************************************************
      
      // change text colour t suit transmit state
      tft.setCursor(line_two_pos_x,line_two_pos_y);
      if(flag.txValid == true){
        tft.setTextColor(ILI9341_RED);
      }
      if(flag.txValid == false){
         tft.setTextColor(line_two_text_colour);
      }
      
      tft.setFont(DroidSansMono_24);
      tft.print(call);
      tft.print(" ");
      tft.print(loc);

      tft.setCursor(line_two_pos_x + 205,line_two_pos_y);
      tft.print(" ");
      tft.print(dbm);
      
      // if dBm changed, clear screen and rewrite value 

      currentdBm = dbm;
      if(currentdBm != previousdBm){
          previousdBm = currentdBm;
          // clear text area and display current dBm valse
          tft.fillRect((line_two_pos_x + 225),(line_two_pos_y), (40), (24), ILI9341_BLACK);

          tft.setCursor(line_two_pos_x + 205,line_two_pos_y);
          tft.print(" ");
          tft.print(dbm);
        
      }
      
      
// Line three **********************************************************************************

      tft.setCursor(line_three_pos_x,line_three_pos_y); 
      tft.setTextColor(line_three_text_colour);
      // set frequency readout font size
      tft.setFont(DroidSansMono_14);
  
      // change text message as required
      if(flag.qrgTx == true){
          tft.setTextColor(ILI9341_RED);
          tft.fillRect((line_three_pos_x),(line_three_pos_y-2), (290), (22), ILI9341_BLACK);
          // display Tx frequency !!!!
          // unsigned long freq = 7040056UL; 
          //tft.print("QRG : Transmitting");
          tft.print("QRG : ");
          tft.print(freq);
          tft.print(" Hz");
          flag.qrgTx = false;
        }
        
        if(flag.qrgWait == true){
          tft.setTextColor(ILI9341_MAGENTA);
          tft.fillRect((line_three_pos_x),(line_three_pos_y-2), (290), (22), ILI9341_BLACK);
          tft.print("QRG : Waiting     ");
          flag.qrgWait = false;
        }

// Line four **********************************************************************************
    
      // set frequency readout font size
      tft.setFont(DroidSansMono_14);

       if(flag.gpsNotValidTx==true){
          tft.setCursor(line_four_pos_x,line_four_pos_y); 
          tft.fillRect((line_four_pos_x),(line_four_pos_y-2), (290), (22), ILI9341_BLACK);
          tft.setTextColor(ILI9341_MAGENTA);
          tft.print("GPS : Not Valid ");
          flag.gpsNotValidTx=false;
        }
        
      if((flag.gpsValidTx==false)&&(flag.gps_valid==true)){
         tft.setCursor(line_four_pos_x,line_four_pos_y); 
          tft.fillRect((line_four_pos_x),(line_four_pos_y-2), (290), (22), ILI9341_BLACK);
          tft.setTextColor(line_four_text_colour);
          tft.print("GPS : Valid ");
          flag.gpsValidTx=true;
        }
      
// Line five **********************************************************************************

      // flag.satNumUpdate
      
      tft.setCursor(line_five_pos_x,line_five_pos_y); 
      tft.setTextColor(line_five_text_colour);
      // set frequency readout font size
      tft.setFont(DroidSansMono_14);
      tft.print("GPS : Satellite no. ");
      tft.setCursor(line_five_pos_x + 205,line_five_pos_y);
      tft.print(num_sats);

      // if number sats change clear screen and rewrite value
     
       currentnum_sats = num_sats;
       if(currentnum_sats != previousnum_sats){
            previousnum_sats = currentnum_sats;
            tft.fillRect((line_five_pos_x + 205),(line_five_pos_y), (35), (15), ILI9341_BLACK);
            tft.setCursor(line_five_pos_x + 205,line_five_pos_y);
            tft.print(num_sats);
        
       }

// Line six **********************************************************************************

      tft.setCursor(line_six_pos_x,line_six_pos_y); 
      tft.setTextColor(line_six_text_colour);
      // set frequency readout font size
      tft.setFont(DroidSansMono_14);
      tft.print("GPS : HDOP");
      tft.setCursor(line_six_pos_x + 125,line_six_pos_y);
      tft.print(hdopvalue,2);

      currenthdopvalue = hdopvalue;
      if(currenthdopvalue != previoushdopvalue){
          previoushdopvalue = currenthdopvalue;
          tft.fillRect((line_six_pos_x + 125),(line_six_pos_y), (70), (15), ILI9341_BLACK);
          tft.setCursor(line_six_pos_x + 125,line_six_pos_y);
          tft.print(hdopvalue,2);
      }
      
      
// Line seven **********************************************************************************

      tft.setCursor(line_seven_pos_x,line_seven_pos_y); 
      tft.setTextColor(line_seven_text_colour);
      // set frequency readout font size
      tft.setFont(DroidSansMono_14);
      if(lat != oldLat) {
          tft.fillRect((line_seven_pos_x),(line_seven_pos_y-2), (290), (22), ILI9341_BLACK);
          tft.print("Lat : ");
          oldLat = lat;
          tft.print(lat,6);
      }
      

// Line eight **********************************************************************************

      tft.setCursor(line_eight_pos_x,line_eight_pos_y); 
      tft.setTextColor(line_eight_text_colour);
      // set frequency readout font size
      tft.setFont(DroidSansMono_14);
    
      if(lon != oldLon) {
        tft.fillRect((line_eight_pos_x),(line_eight_pos_y-2), (290), (22), ILI9341_BLACK);
          tft.print("Lon : ");
          oldLon = lon;
          tft.print(lon,6);

      } 
      
    }

   if(flag.page_two==true){
      flag.page_one=false;

      // turn off Si5351 and reset display flags
      // Turn off the output
      si5351.output_enable(SI5351_CLK0, 0);

      
      flag.txValid = false;
      flag.qrgTx = false;
      flag.qrgWait = true;
      
          if(flag.ReDrawConfScreen==true){
            
              // clear screen and display config menu
              tft.fillScreen(ILI9341_BLACK);
              
              // draw boarder around full display
              rectangle(outline_boarder_x, outline_boarder_y, outline_boarder_w, outline_boarder_d, outline_boarder_colour);
              tft.fillRect((main_boarder_x +1),(main_boarder_y + 1), (main_boarder_w - 3), (main_boarder_d - 2), ILI9341_BLACK);

// Line one **********************************************************************************
              // Config menu banner text
              tft.setCursor(line_one_pos_x + 7,line_one_pos_y + 10); 
              tft.setTextColor(line_one_text_colour);
              tft.setFont(DroidSansMono_16);
              tft.print("Configuration menu");

// Line two **********************************************************************************
            
              tft.setCursor(line_two_pos_x - 25,line_two_pos_y + 10);  
              tft.setTextColor(info_text_colour);
              tft.setFont(DroidSansMono_12);
              tft.print("Time slot - ");
              //tft.print(TSlot[Config_Index[1][0]]);
              tft.print(TSlot[Config_Index[time_index][0]]);
              
// Line three **********************************************************************************
             
              int config_index = Config_Index[time_index][4];
              
              //config_index = Config_Index[1][4];
              if(config_index == 0){
                  tft.setCursor(line_three_pos_x - 20,line_three_pos_y);  
                  tft.setTextColor(info_text_colour);
                  tft.setFont(DroidSansMono_12);
                  tft.print("Time slot NOT active");
              }
              else if(config_index == 1){
                  tft.setCursor(line_three_pos_x - 20,line_three_pos_y);  
                  tft.setTextColor(ILI9341_GREEN);
                  tft.setFont(DroidSansMono_12);
                  tft.print("Time slot ACTIVE");
              }

// Line four **********************************************************************************
              
              tft.setCursor(line_four_pos_x - 20,line_four_pos_y);  
              tft.setTextColor(info_text_colour);
              tft.setFont(DroidSansMono_12);
              tft.print("Tx frequency - ");
              tft.print(band[Config_Index[time_index][1]]);
              tft.print(" Hz");

// Line five **********************************************************************************
              
              tft.setCursor(line_five_pos_x - 20,line_five_pos_y);  
              tft.setTextColor(info_text_colour);
              tft.setFont(DroidSansMono_12);
              tft.print("Control byte -");
              tft.print(" 0x");
              tft.print(ControlBit[Config_Index[time_index][2]],HEX);

// Line six **********************************************************************************
              
              tft.setCursor(line_six_pos_x - 20,line_six_pos_y);  
              tft.setTextColor(info_text_colour);
              tft.setFont(DroidSansMono_12);
              tft.print("Output power - ");
              //tft.print("0x");
              tft.print(TxPower[Config_Index[time_index][3]]);
              tft.print(" dBm");
              
// Line eight **********************************************************************************

              tft.setCursor(line_eight_pos_x - 20,line_eight_pos_y);
              tft.setTextColor(ILI9341_OLIVE);
              tft.print("Software version:  ");
              tft.print(VERSION);
              
// Now reset appropriate flags etc. ************************************************************

              // set flag so we only update display once to stop flicker
              flag.ReDrawConfScreen=false;
                       
              // set flag below for when we return
              flag.gpsValidTx=false;
              //flag.refreshPageOne=true;
              
              // reset store prevous times so display updates on return to main screen
              previousTime7 = 0;
              previousTime8 = 0;
              previousTime10 = 0;
              previousTime11 = 0;
              previousTime13 = 0;
              previousTime14 = 0;
              // and the same for Lat / Lon
              oldLat = 0.0;
              oldLon = 0.0;
              
            }
             // update all six lines    
        }
  // end of while(1) - below 
    }
}

// ****** End of File
