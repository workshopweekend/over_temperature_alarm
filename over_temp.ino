//**************************************************
/*
  Author: Malcolm Knapp
  Project: Over Temperature Alarm
  Date: 2/3/14
  Version: 1.1
  Description: This program is a over temperature alarm start code. To extend this code into
               the full over temperature alarm please follow the Over Temperature Alarm Tutorial 1V0. 
               This code uses thermistor and button as inputs and a LCD output.
  Note: This source code is licensed under a Creative Commons License, CC-by-nc-sa.
        (attribution, non-commercial, share-alike)
   see http://creativecommons.org/licenses/by-nc-sa/3.0/ for details.
  Disclaimer: This code is distributed in the hope that it will be useful, but WITHOUT ANY 
             WARRANTY;without even the implied warranty of MERCHANTABILITY or FITNESS FOR 
             A PARTICULAR PURPOSE
*/ 
//***************************************************
// ---------- included libraries ------------ 
#include <Button.h>
#include <LiquidCrystal.h>

// ---------- pin defines  ----------- 
#define TEMP_PIN A0
#define BUTTON_PIN 6

// ---------- variable initialization  ----------- 
unsigned long BASE_TIMEOUT = 1000; // timer tick for the whole system set at 1s.  Set to 1000 for release
unsigned long baseTimedOut = 1000;

enum {
  // States
  NORMAL = 1,
  ALARM,
  NO_STATE
} STATES;

enum {
  button_pressed = 0,
  over_temp,
  ENTRY,
  NONE
} EVENTS;

unsigned char state = NORMAL;       // Tracks current state
unsigned char prevState = NO_STATE;
unsigned char event = NONE;        // Triggers a particular action within current state

int adc = 0;
double tempF = 0.0;
double tempC = 0.0;
//-6-
double OVER_TEMP_LIMIT = 80;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Button button = Button(BUTTON_PIN, BUTTON_PULLUP);

//-2-

// ---------- Conversion Functions -----------  
double ReadThermistor(int adc) {

  double resistance = ((1024.0/adc) - 1);    //calculate from voltage divider, for 10k resistor
  double Temp = log(resistance);

  // calculate the temperature, in K, using 4 thermistor model/material specific parameters A, B, C, D
  // here we use the values for the Sparkfun/Hactronics version of the Vishay 10k NTC thermistor
  Temp = 1 / (0.003354016 + 0.0002569850 * Temp + 0.000002620131 * Temp * Temp + 0.00000006383091 * Temp * Temp * Temp);
  Temp = Temp - 273.15;            // Convert Kelvin to Celsius
  return Temp;
}

void setup () {
  Serial.begin (9600);
  Serial.println("Starting Over Temperature Alarm");
  
  pinMode (BUTTON_PIN, INPUT);
  pinMode (TEMP_PIN, INPUT);
  
  lcd.begin(16,2);

}

void loop () {
  
  // ---------- event polling  ----------- 
  // poll temp sensor 
  adc = analogRead(TEMP_PIN);
  if (adc > 1 && adc < 1022) {    // catch an "open" or a "short"
    tempC = ReadThermistor(adc);
    tempF = (tempC * 9.0)/ 5.0 + 32.0; // Convert Celsius to Fahrenheit
  } else {
    Serial.println("Thermistor is open or shorted!");
  }
  
  // -5-
  
    // over temp event check
  if (tempF > OVER_TEMP_LIMIT) {
    event = over_temp;
  }
  
  if (button.uniquePress()) {  // poll button and check for button event 
    Serial.println("button pressed!");
    event = button_pressed;
  }

  // ---------- state machine ----------- 
  switch (state) {
    
  case NORMAL:  // Normal State
  
      // Entry Code
      if (state != prevState) {
        Serial.println ("In NORMAL state, display 'Normal'");
        lcd.clear();
        lcd.print ("Normal");
        prevState = state;
      }
      
      // -1-
      
      // -4-
      
      if (event == over_temp) {
        Serial.println ("over_temp event!");
        state = ALARM;
        event = NONE;
      }
    
    break; 
 
    //-3-
 
    case ALARM:// Alarm State
    
      if (state != prevState) {
        Serial.println ("in ALARM state, display 'Alarm'");
        lcd.clear();
        lcd.print("ALARM");
        prevState = state;
      }
           
      // Event Capture    
      if (event == button_pressed) {  // button Pressed transition
        Serial.println ("end alarm");
        lcd.clear();
        lcd.print ("Ending alarm");
        delay (1000);
        state = NORMAL;
        event = NONE;
      }  
    break;
    } // -------------- end state machine ---------------
    
   // Base Timer
  if(millis() - baseTimedOut > BASE_TIMEOUT) { 
    baseTimedOut = millis();
    
    // update temperature display 
    Serial.print("The temp is: ");Serial.println(tempF, 1);
    lcd.setCursor(0,1);
    lcd.print ("Temp is:");lcd.print (tempF); lcd.print("F");

 }
}
