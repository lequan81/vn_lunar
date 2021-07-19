/* Using algorithm to converted from Solar (Gregorian calendar) to Lunar (Vietnamese lunar calendar) by Ho Ngoc Duc.
   Links: https://www.informatik.uni-leipzig.de/~duc/amlich/calrules_v02.html

   Fixing some buggy to work with Arduino (Arduino Uno R3, Arduino Nano, ESP8266 tested) by me ^.^
   Last modified: 08/06/2021 by lequanruby@gmail.com

   For more informations please contact me via Gmail: lequanruby@gmail.com
*/

// Include the libary
#include <vn_lunar.h>

uint32_t dd, mm, yy;

void setup() {
  // Start serial port
  Serial.begin(9600);

  // Day = 07; Month = 06; Year = 2021
  dd = 8; mm = 6; yy = 2021;

  vn_lunar solar;  //  You can change "solar" to "s2l" or something else.

  // convert from Solar (Gregorian calendar) to Lunar (Vietnamese lunar calendar).
  solar.convertSolar2Lunar(dd, mm, yy);

  // Get the lunar day (type: int)
  int lunar_dd = solar.get_lunar_dd();
  
  // Get the lunar month (type: int)
  int lunar_mm = solar.get_lunar_mm();
  
  // Get the lunar year (type: int)
  int lunar_yy = solar.get_lunar_yy();

  // Print to the Serial monitor lunar day.
  Serial.print("\n");
  Serial.print(lunar_dd);

  // Print to the Serial monitor lunar month.
  Serial.print("\n");
  Serial.print(lunar_mm);

  // Print to the Serial monitor lunar year.
  Serial.print("\n");
  Serial.print(lunar_yy);

}

void loop() {}
