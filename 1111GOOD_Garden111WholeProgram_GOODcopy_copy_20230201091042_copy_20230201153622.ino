// Start of program
#include "Adafruit_seesaw.h"
Adafruit_seesaw ss;  //refer to seesaw sensor tools import as 'ss' for short

// General setup
void setup() {
  Serial.begin(115200);  //open serial monitor (115200) to see stuff happen!
  if (!ss.begin(0x36)) {
    Serial.println("ERROR! seesaw not found");
    while (1) delay(1);
  } else {
    Serial.print("seesaw started! version: ");
    Serial.println(ss.getVersion(), HEX);
  }
  
  pinMode(0,INPUT);
  pinMode(9, OUTPUT);
  pinMode(2,OUTPUT);
}


// Define constants for main loop
const int LIGHT_SENSOR_PIN = 0;
const int LED_PIN = 9;
const int PUMP_PIN = 2;
const int SEC_0_25 = 250; //this gives 5 mL
const int SEC_0_50 = 500; //this gives 10 mL
const int MIN_LIGHT = 300;
const uint16_t CRITICAL_SOIL_VAL = 500;
const uint16_t NORMAL_SOIL_VAL = 900;
const int WAIT_DURATION_MS = 1000;//wait for 1 sec before looping


// Define variables for main loop
long t = 0;              //overall loop counter
long pump_on_time = 0;   //How long has pump been on?
long pump_run_time = 0;  //How long to run the pump based on soil moisture
bool pumpIsOn = false; 
long pump_limit_time = 0;
uint16_t capread = 0;
float tempC = 0;
int lightVal = 0;


// Sensor functions
// This gets the photoresistor value
int lightGetter() {
  lightVal = analogRead(LIGHT_SENSOR_PIN);
  return lightVal;
}
// This gets capacitive reading, indicates soil moisture
uint16_t soilGetter() {
  capread = ss.touchRead(0);
  return capread;
}
// This gets soil temperature
//float soilTempGetter() {
//  tempC = ss.getTemp();
//  return tempC;
//}
// This turns pump on for specific duration
void turn_pump_on(long run_time) {
  digitalWrite(PUMP_PIN, HIGH);
  pumpIsOn = true;
  pump_limit_time = run_time;
  Serial.println("---- ON -----");
}
// This turns pump off
void turn_pump_off() {
  digitalWrite(PUMP_PIN, LOW);
  pumpIsOn = false;
  pump_run_time = 0;
  pump_limit_time = 0;
  Serial.println("---- PUMP OFFFFFFFFFF -----");
}


// This is the main loop that executes constantly
void loop() {
  // ***** Light *******/
  lightVal = lightGetter();

  //TESTING
  Serial.print("lightval=");
  Serial.println(lightVal);
  Serial.print("t=");
  Serial.println(t);

  //Check lights every ten minutes
  if (pumpIsOn==false){ 
    Serial.println("LIGHTTTT");                         //only have lights on if pump is not on                //do we want constant check for demo purpose?
    if (lightVal < MIN_LIGHT) {
      digitalWrite(LED_PIN, HIGH);          //lights on if dark out
    } 
    else {
      digitalWrite(LED_PIN, LOW);          //else if it's day and light enough, keep lights off
    }
    
  }

  // ***** Pump *******
  capread = soilGetter();

  //TESTING
  Serial.print("capread=");
  Serial.println(capread);
  Serial.print("t=");
  Serial.println(t);
  Serial.print("pump_run_time=");
  Serial.println(pump_run_time);
  Serial.print("is_pump on=");
  Serial.println(pumpIsOn);

  // if pump is already on for MAX time (set previously), turn it off and reset time counter
  if (pumpIsOn && (pump_run_time%pump_limit_time == 0)) {
    turn_pump_off();
  }
  // if pump is off, and for demo purpose no time(24hrs) has passed, check all levels of moisture and water accordingly
  if (capread < CRITICAL_SOIL_VAL) {
    Serial.println("*** criticalVal ****");
    turn_pump_on(SEC_0_50);
  } 
  else if (capread < NORMAL_SOIL_VAL) {
    Serial.println("*** normalVal ****");
    turn_pump_on(SEC_0_25);
  } 
  else {
    Serial.println("*** WET ****");
    turn_pump_off();
  }

  // Check moisture every six hours, only water if critically low
  //else if ((t % HRS_6) == 0) {  
  //  if (capread < CRITICAL_SOIL_VAL) {
  //    turn_pump_on(MINS_2);
  //  } else {
  //    turn_pump_off();
  //  }
  //}
  
  // delay before next loop for 1 second
  delay(WAIT_DURATION_MS);

  // increment timers by length of delay
  t += WAIT_DURATION_MS;

  if (pumpIsOn == true) {
    pump_run_time += WAIT_DURATION_MS;
  }
}