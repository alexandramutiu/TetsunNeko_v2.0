#include <Sabertooth.h>

//DOAR UN MODEL

//PIN 2 -senzor oponent stanga
//PIN 3 -senzor oponent dreapta

//Senzori linie alba
//A0 - Fata
//A1 - Spate
//A3 - Dreapta
//A4 - Stanga

//Altele
//PIN13 - Modul Start/Stop
//TX1 - Sabertooth TX


#define PIN_SENZ_STG 2
#define PIN_SENZ_DR 3
#define PIN_LIA_FATA A0
#define PIN_LIA_SPATE A1
#define PIN_LIA_DRT A3
#define PIN_LIA_STG A4
#define PIN_START_STOP 13

#define PRAG_LINIE 200
#define TIMP_SPATE 80
#define TIMP_INTOARCERE 180

#define VITEZA_MAX       127
#define VITEZA_CAUTARE    80
#define TIMEOUT_CAUTARE  400

typedef enum{
  STATE_READY = 0,
  STATE_START,
  STATE_FIGHT,
  STATE_STOPPED
}TetsunNekoState;


//Global var
Sabertooth ST(128);
TetsunNekoState state;
uint8_t mascaSensori;
int liaFata, liaSpate, liaDr, liaStg;
bool onLine;
bool offLine;
unsigned long timpATAC;  
unsigned long timpQTR;
void setup() {
  pinMode(PIN_SENZ_STG, INPUT);
  pinMode(PIN_SENZ_STG, INPUT);
  pinMode(PIN_START_STOP, INPUT);
  #ifndef DEBUG_MODE
      SabertoothTXPinSerial.begin(115200);
  #else
      Serial.begin(9600);
  #endif

  timpAtac = 0;
  timpLIA = 0;
  onLine = fales;
#ifndef DEBUG_MODE
    robotState = STATE_READY;
    setMotoare(0, 0);
#else
    robotState = STATE_DEBUGGING;
#endifun
}

void loop() {
  // put your main code here, to run repeatedly:

}
