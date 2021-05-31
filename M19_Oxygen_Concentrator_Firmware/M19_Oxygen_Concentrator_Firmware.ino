//****************************************************************
#include <EEPROM.h>

#include "uptime.h"
#include "uptime_formatter.h"

void runCycle();
void updateTime();

int currentTimeSeconds;
int noOfhours;
int noOfDays;
bool DaysWritten;

// RELAY PIN ASSIGNMENT
//**************************************************************************
#define SIEVE_A_PIN 5
#define SIEVE_B_PIN 6
#define EQUILISATION_PIN 7


//**************************************************************************
void setup()
{
  Serial.begin(9600);
  EEPROM.begin();
  noOfDays = EEPROM.read(5);
  noOfhours = EEPROM.read(1);

  // STARTUP
  //**************************************************************************
  // Serial Port initialization
  Serial.begin(9600);


  // SET PIN MODE FOR PINS IN PROGRAM
  //**************************************************************************
  pinMode(SIEVE_A_PIN, OUTPUT);
  pinMode(SIEVE_B_PIN, OUTPUT);
  pinMode(EQUILISATION_PIN, OUTPUT);


  //  SET DELAY TIMING HERE
  //**************************************************************************

  #define PRODUCTION_TIME  8000
  #define FLUSH_TIME 200
  #define PRECHARGE_TIME 1000

}


unsigned long prevCycleTime = millis();
int prevCycle = 6;
unsigned long waitTime = 0;

void loop()
{
  runCycle();
  updateTime();
}


void updateTime() {
  DaysWritten = false;
  uptime::calculateUptime();

  if (uptime::getHours() != currentTimeSeconds) {
    noOfhours += 1;
    EEPROM.write(1, noOfhours);
    currentTimeSeconds = uptime::getHours();
    Serial.println(noOfhours);

    Serial.print(uptime::getSeconds());
    Serial.println('s');
    Serial.print(uptime::getMinutes());
    Serial.println('m');
    Serial.print(uptime::getHours());
    Serial.println('h');
    Serial.print(uptime::getDays());
    Serial.println('d');

    if (noOfhours == 24 && DaysWritten == false) {
      noOfDays += 1;
      EEPROM.write(5, noOfDays);
      Serial.println("no of days = ");
      Serial.println(noOfDays);
      noOfhours = 0;
      DaysWritten = true;
    }
  }
}

void runCycle() {
  //CYCLE 1
  //**************************************************************************
  if ((millis() - prevCycleTime >= waitTime) && (prevCycle == 6)) {
    Serial.println("Precharge");
    digitalWrite(SIEVE_A_PIN, LOW);
    digitalWrite(SIEVE_B_PIN, LOW);
    digitalWrite(EQUILISATION_PIN, LOW);
    prevCycleTime = millis();
    prevCycle = 1;
    waitTime = PRECHARGE_TIME;
  }

  //CYCLE 2
  //**************************************************************************
  if ((millis() - prevCycleTime >= waitTime) && (prevCycle == 1)) {
      Serial.println("Sieve A Charge / Sieve B Purge ");
      digitalWrite(SIEVE_A_PIN, LOW);
      digitalWrite(SIEVE_B_PIN, HIGH);
      digitalWrite(EQUILISATION_PIN, HIGH);
      prevCycleTime = millis();
      prevCycle = 2;
      waitTime = PRODUCTION_TIME;
  }

  //CYCLE 3
  //**************************************************************************
  if ((millis() - prevCycleTime >= waitTime) && (prevCycle == 2)) {
      Serial.println("Sieve A Charge / Sieve B Flush");
      digitalWrite(SIEVE_A_PIN, LOW);
      digitalWrite(SIEVE_B_PIN, HIGH);
      digitalWrite(EQUILISATION_PIN, LOW);
      prevCycleTime = millis();
      prevCycle = 3;
      waitTime = FLUSH_TIME;
  }

  //CYCLE 4
  //**************************************************************************
  if ((millis() - prevCycleTime >= waitTime) && (prevCycle == 3)) {
      Serial.println("Precharge");
      digitalWrite(SIEVE_A_PIN, LOW);
      digitalWrite(SIEVE_B_PIN, LOW);
      digitalWrite(EQUILISATION_PIN, LOW);
      prevCycleTime = millis();
      prevCycle = 4;
      waitTime = PRECHARGE_TIME;
  }

  //CYCLE 5
  //**************************************************************************
  if ((millis() - prevCycleTime >= waitTime) && (prevCycle == 4)) {
      Serial.println("Sieve A Purge / Sieve B Charge");
      digitalWrite(SIEVE_A_PIN, HIGH);
      digitalWrite(SIEVE_B_PIN, LOW);
      digitalWrite(EQUILISATION_PIN, HIGH);
      //delay(Flush_Delay);
      prevCycleTime = millis();
      prevCycle = 5;
      waitTime = PRODUCTION_TIME;
  }

  //CYCLE 6
  //**************************************************************************
  if ((millis() - prevCycleTime >= waitTime) && (prevCycle == 5)) {
      Serial.println("Sieve A Flush / Sieve B Charge");
      digitalWrite(SIEVE_A_PIN, HIGH);
      digitalWrite(SIEVE_B_PIN, LOW);
      digitalWrite(EQUILISATION_PIN, LOW);
      prevCycleTime = millis();
      prevCycle = 6;
      waitTime = FLUSH_TIME;
  }
}
