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


 // Colours to choose from:
 // ILI9341_BLACK       
 // LI9341_NAVY        
 // ILI9341_DARKGREEN    
 // ILI9341_DARKCYAN     
 // ILI9341_MAROON      
 // ILI9341_PURPLE      
 // ILI9341_OLIVE       
 // ILI9341_LIGHTGREY   
 // ILI9341_DARKGREY   
 // ILI9341_BLUE       
 // ILI9341_GREEN        
 // ILI9341_CYAN       
 // ILI9341_RED         
 // ILI9341_MAGENTA    
 // ILI9341_YELLOW      
 // ILI9341_WHITE      
 // ILI9341_ORANGE      
 // ILI9341_GREENYELLOW 
 // ILI9341_PINK       



// ---------------------------------------------------
// Structure to make handling program control 'flags' easier to work with
struct flags
{
  bool ReDraw_Screen      = true;      // this will force the screen to be updated on first loop
  bool refreshPageOne     = false;
  bool gpsHardware        = false;
  bool ReDrawgpsHardware  = false;
  bool gpsSerialData      = false;
  bool ReDrawSerialData   = false;
  bool gps_valid          = false;     // initially set to false - flag set when gps valid established
  bool page_one           = false;     // main WSPR info page
  bool page_two           = false;     // configuration page
  bool ReDrawConfScreen   = false;
  bool txValid            = false;
  bool qrgTx              = false;
  bool qrgWait            = true;
  bool gpsValidTx         = false;
  bool gpsNotValidTx      = true;
  
};
struct flags flag;
// ---------------------------------------------------

// variables for screen layout
// outline boarder
int outline_boarder_x = 0;        // position for top left  
int outline_boarder_y = 0;        // position for top left 
int outline_boarder_w = 320;      // width of boarder
int outline_boarder_d = 240;      // depth of boarder
int outline_boarder_colour = ILI9341_YELLOW;

//when we clear the display to re-write anything we don't want to re-draw the outline boarder
// so these variables define the are we'll call upon in the code, when required.
int clearscreen_x = 5;
int clearscreen_y = 5;             // position for top left 
int clearscreen_w = 315;           // width of boarder
int clearscreen_d = 215;           // depth of boarder
int clearscreen_colour = ILI9341_BLACK;

int main_boarder_x = 5;        // position for top left 
int main_boarder_y = 55;       // position for top left
int main_boarder_w = 310;      // width of boarder
int main_boarder_d = 50;       // depth of boarder
int main_boarder_colour = ILI9341_GREEN;

int info_cursor_pos_x = 35;         // position for top left
int info_cursor_pos_y = 180;        // position for top left
int info_text_colour = ILI9341_YELLOW;

int info_boarder_x = 5;             // position for top left 
int info_boarder_y = 160;           // position for top left
int info_boarder_w = 310;           // width of boarder
int info_boarder_d = 75;            // depth of boarder
int info_boarder_colour = ILI9341_GREEN;

int line_one_pos_x = 25;       // position for top left
int line_one_pos_y = 13;       // position for top left
int line_one_text_colour = ILI9341_LIGHTGREY;

int line_two_pos_x = 30;       // position for top left
int line_two_pos_y = 45;       // position for top left
int line_two_text_colour = ILI9341_YELLOW;

int line_three_pos_x = 25;       // position for top left
int line_three_pos_y = 80;       // position for top left
int line_three_text_colour = ILI9341_YELLOW;

int line_four_pos_x = 25;       // position for top left
int line_four_pos_y = 105;       // position for top left
int line_four_text_colour = ILI9341_YELLOW;

int line_five_pos_x = 25;       // position for top left
int line_five_pos_y = 130;       // position for top left
int line_five_text_colour = ILI9341_YELLOW;

int line_six_pos_x = 25;       // position for top left
int line_six_pos_y = 155;       // position for top left
int line_six_text_colour = ILI9341_YELLOW;

int line_seven_pos_x = 25;       // position for top left
int line_seven_pos_y = 180;       // position for top left
int line_seven_text_colour = ILI9341_YELLOW;

int line_eight_pos_x = 25;       // position for top left
int line_eight_pos_y = 205;       // position for top left 
int line_eight_text_colour = ILI9341_YELLOW;

// ****** End of File
