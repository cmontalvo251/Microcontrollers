#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

void loop() {
  //Read the solar panel voltage and the battery voltage
  float solar_voltage = 2.0*analogRead(A2)*5.0/1023.0; //multiply by 2 since I've intalled a voltage divider
  float battery_voltage = 1.0*analogRead(A1)*5.0/1023.0;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  int column = 0;
  int row = 0;
  lcd.setCursor(column, row);
  // print Batter Voltage
  lcd.print("Batt(V): ");
  lcd.setCursor(column+9,row);
  lcd.print(battery_voltage);
  lcd.setCursor(column,1);
  lcd.print("Solar(V): ");
  lcd.setCursor(column+10,1);
  lcd.print(solar_voltage);
  delay(2000);
}


