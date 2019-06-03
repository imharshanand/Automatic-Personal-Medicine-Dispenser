#include "PinChangeInterrupt.h"
#include <Wire.h>
#include <Time.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <gprs.h>
#include <LiquidCrystal.h>

#define SOFT_BAUD 9600
#define MOBILE "+91"

#define TIME_SET 0

//#define BUTTON 8
//#define LCD_CONTRAST 9

#define GPRS_E 1
#define BUZZER A0

#if GPRS_E
#define GPS_RX 11
#define GPS_TX 12
GPRS gprs(GPS_RX, GPS_TX);
#endif

#define SENSOR1 8
#define SENSOR2 9
#define SENSOR3 10

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

char* message = "Medicine not taken! please take medicine!";
tmElements_t tm;
byte nottaken;

void smartDelay(int d){
  while(d--){
    delay(1);
    if(nottaken){
      if(!(!digitalRead(SENSOR1) && !digitalRead(SENSOR2) && !digitalRead(SENSOR3))){
        Serial.println("medicine taken!");
        lcd.setCursor(0, 1);
        lcd.print("Medicine Taken!");
        nottaken = 0;
      }
    }
  }
}

unsigned long lastTaken;

void setup() {
  //pinMode(LCD_CONTRAST, OUTPUT);
  //analogWrite(LCD_CONTRAST, 90);
  pinMode(BUZZER, OUTPUT);
  
  Serial.begin(115200);
  lcd.begin(16, 2);

#if TIME_SET

  tm.Hour = 14;
  tm.Minute = 24;
    
  tm.Day = 27;
  tm.Month = 3;
  tm.Year = CalendarYrToTm(2018);  
  lcd.print("Time Set Mode!");
  Serial.println("Time Set Mode");
  RTC.write(tm);
  while(1);
#endif
  
  lcd.print("Medicine Dispencer!");
  Serial.println("Medicine Dispencer");

#if GPRS_E
  gprs.preInit(SOFT_BAUD);
  lcd.setCursor(0, 1);
  lcd.print("Searching Modem!");
  while(0 != gprs.init()) {
      delay(1000);
      Serial.println("GSM ERROR");
  }
  lcd.setCursor(0, 1);
  lcd.print("Modem Found!");
  gprs.deleteAllSMS();
#endif

  //pinMode(BUTTON, INPUT_PULLUP);
  lcd.setCursor(0, 1);
  lcd.print("!!! ALL OK  !!!");

  lastTaken = millis();
  nottaken = 1;

  tmElements_t tm;
  
  if (RTC.read(tm)) {
    Serial.print("Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  }else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
}

void loop() {
char buff[100];
    
    unsigned long current = millis();
    current = current - lastTaken;
    current = current / 1000;
    RTC.read(tm);
   //if () {
      //if(((tm.Hour == 8 || tm.Hour == 14 || tm.Hour == 21) && tm.Minute == 59 && tm.Second < 10) || (current > 10  && nottaken)){
      if(tm.Hour == 14 && tm.Minute == 26 && nottaken){
        lcd.clear();
        lcd.print("Time for Medicine!");
        Serial.println("TIME FOR MEDICINE!");
        int trycount = 15;
        
        Serial.print("medicine not taken!");
        while(trycount && nottaken){
          if(trycount % 5 == 0)
            Serial.print("!");
          digitalWrite(BUZZER, HIGH);
          smartDelay(500);
          digitalWrite(BUZZER, LOW);
          smartDelay(500);
          trycount--;
        }
        Serial.println();

        if(!trycount){
          Serial.println("Message Sent");
#if GPRS_E
          gprs.sendSMS("7376468351", message);
#endif
          nottaken = 0;
        }
      }
//   }else{
//    Serial.println("RTC Error");
//   }
  lcd.setCursor(0, 1);
  lcd.print("               ");
  lcd.setCursor(0, 1);
  
  tmElements_t tm;
  RTC.read(tm);
  lcd.print(tm.Hour);
  lcd.print(':');
  lcd.print(tm.Minute);
  lcd.print(' ');
  lcd.print(tm.Day);
  lcd.print('/');
  lcd.print(tm.Month);
  lcd.print('/');
  lcd.print(tmYearToCalendar(tm.Year));
  delay(1000);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
