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

String currentLat;
String previousLat;
long previousMillis = 0;        // will store last time LED was updated

// Maidenhead Locator not currently used
char mh[10] = "";

void findMH(char *dst, float fa, float fo)
{
  int a1, a2, a3;
  int o1, o2, o3;
  double rd;

  // Latitude
  rd = fa + 90.0;
  a1 = (int)(rd / 10.0);
  rd = rd - (double)a1 * 10.0;
  a2 = (int)(rd);
  rd = rd - (double)a2;
  a3 = (int)(24.0 * rd);

  // Longitude
  rd = fo + 180.0;
  o1 = (int)(rd / 20.0);
  rd = rd - (double)o1 * 20.0;
  o2 = (int)(rd / 2.0);
  rd = rd - 2.0 * (double)o2;
  o3 = (int)(12.0 * rd);

  dst[0] = (char)o1 + 'A';
  dst[1] = (char)a1 + 'A';
  dst[2] = (char)o2 + '0';
  dst[3] = (char)a2 + '0';
  dst[4] = (char)o3 + 'A';
  dst[5] = (char)a3 + 'A';
  dst[6] = '\0';
}



void fetchGpsData()
{
  // and comms to gps module
  gpsSerial.begin(9600);
  
while(1){
  while(gpsSerial.available() > 0) {// check for gps data
      flag.gpsHardware=true;
      if(gps.encode(gpsSerial.read())){
        flag.gpsSerialData=true;
            if (gps.location.isValid())
            {  
              flag.gps_valid=true; 
            }
            
             if (gps.date.isUpdated())
             {
              if (gps.time.isValid()) {
                      Time[7]   = gps.time.hour()   / 10 + 48;
                      Time[8]   = gps.time.hour()   % 10 + 48;
                      Time[10]  = gps.time.minute() / 10 + 48;
                      Time[11]  = gps.time.minute() % 10 + 48;
                      Time[13]  = gps.time.second() / 10 + 48;
                      Time[14]  = gps.time.second() % 10 + 48;
                     }
             }
             
             if (gps.location.isValid()){
                    lat = (gps.location.lat());
                    lon = (gps.location.lng());
                    /*
                    // now find Maidenhead
                    findMH(mh, lat, lon);
                    // now shift calculated Maidenhead locator into array for WSPR encoding
                    loc[0]=mh[0];
                    loc[1]=mh[1];
                    loc[2]=mh[2];
                    loc[3]=mh[3];  
                    */
             }

              if(gps.satellites.value()){
                  num_sats = (gps.satellites.value());     
              }
              
              if(gps.hdop.hdop()){
                hdopvalue = (gps.hdop.hdop());
              }
              
          else
            {
               #ifdef DEBUG
                  Serial.println("GPS not locked ...");
               #endif //DEBUG 
              
             }
          }
      }
      gpsSerial.flush();
      //if (((millis() > 9000 && gps.charsProcessed() < 1000) && (flag.gpsHardware == false)));
      if (millis() > 9000 && flag.gpsHardware==false)
        {
          #ifdef DEBUG
            Serial.println(F("No GPS detected: check wiring."));
            while(true);
          #endif //DEBUG 
        }
         threads.yield();
    }
}

// ****** End of File
