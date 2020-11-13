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

/*
 * 
 * Bounce bouncer_CM_L = Bounce(CM_L, 50);   // Config menu left

#define CM_R              16  // Right          - Config menu right
// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce bouncer_CM_R = Bounce(CM_R, 50);   // Config menu right

#define CM_S              17   // Enter          - Config - confirm selection 
// Instantiate a Bounce object with a 5 millisecond debounce time    
Bounce bouncer_CM_S = Bounce(CM_S,50 );   // Config confirm selection

#define TS_U              14  // Up             - Config timeslot up  
// Instantiate a Bounce object with a 5 millisecond debounce time   
Bounce bouncer_TS_U = Bounce(TS_U,50 );   // Config timeslot up

#define TS_D            23  // Down           - Config timeslot down
// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce bouncer_TS_D = Bounce(TS_D,50 );   // Config timeslot down

#define MODE                 2  //  MODE exchange - WSPR / Config menu
// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce bouncer_MODE = Bounce(MODE,50 );   // WSPR / Config menu
 * 
 * 
 */
//---------switch debounce update
void switch_bouncer(){
  //int menu_index = 0;
  
  while(1){
    
    bouncer_CM_D.update ( );                // Config menu Down
    bouncer_CM_U.update ( );                // Config menu UP
    bouncer_CM_S.update ( );                // Config confirm selection
    bouncer_TS_U.update ( );                // Config timeslot Up
    bouncer_TS_D.update ( );                // Config timeslot Down
    bouncer_MODE.update ( );                // WSPR / Config menu

   
 
  
  if (bouncer_CM_D.fallingEdge()) { // not yet used

    }

  if (bouncer_CM_U.fallingEdge()) { // not yet used
 
    } 

  if (bouncer_CM_S.fallingEdge()) { // not yet used
    
    }

  if (bouncer_TS_U.fallingEdge()) {
        time_index++;
        if (time_index > 14){
            time_index = 14;
          }
          menu_index = 0;
          flag.ReDrawConfScreen=true;
    } 

  if (bouncer_TS_D.fallingEdge()) {
        time_index--;
        if (time_index < 0){
                time_index = 0;
         }
         menu_index = 0;
         flag.ReDrawConfScreen=true;
    }

  if (bouncer_MODE.fallingEdge()) {
 
       flag.page_two = !flag.page_two;
       flag.refreshPageOne = !flag.refreshPageOne;
        
    }
  }
}

// ****** End of File
