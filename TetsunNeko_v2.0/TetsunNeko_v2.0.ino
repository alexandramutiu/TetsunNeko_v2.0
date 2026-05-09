#include <Sabertooth.h>

//DOAR UN MODEL

// PIN 2  - senzor oponent stanga
// PIN 3  - senzor oponent dreapta
// PIN 4  - senzor oponent centru
// A0     - senzor linie fata
// A1     - senzor linie spate
// A2     - senzor linie dreapta (sau stanga)
// PIN 13 - Modul Start/Stop
// TX1    - Sabertooth TX


#define PIN_SENZ_STG 2
#define PIN_SENZ_DR 3
#define PIN_SENZ_FATA 4

#define PIN_LIA_STG A0
#define PIN_LIA_CENTRU A1
#define PIN_LIA_DR A2

#define PIN_START_STOP 13

//initializarea ca sa lucrez pe biti
#define SENZ_OP_STG 0x01
#define SENZ_OP_DR 0x02
#define LINIE_FATA 0x04
//#define LINIE_SPATE 0x08
#define LINIE_STG 0x10
#define LINIE_CTR 0x20
#define LINIE_DR 0x40


#define TIMP_SPATE 500
#define TIMP_INTOARCERE 300
#define TIMP_LIA_INAINTE 80
#define TIMP_ROT_LINIE   180

#define VITEZA_MAX       127
#define VITEZA_CAUTARE    80
#define VITEZA_JUMATATE    60



typedef enum{
  STATE_READY = 0,
  STATE_FIGHT,
  STATE_STOPPED

}TetsunNekoState;

typedef enum{
  DIST_NONE = 0,
  DIST_FAR, // >10 CM
  DIST_CLOSE //in front of us <10 cm
}OpponentDist;

//Global var
Sabertooth ST(128);
TetsunNekoState state;

uint8_t mascaSensori;
uint8_t senzori;

bool irStg, irCentru, irDr;
bool liaStg, liaCentru, liaDr;

bool onLine, offLine;
int parteLinie;

unsigned long timpAtac;  
unsigned long timpQTR;
int PRAG_LINIE = 400;

void setup() {
  pinMode(PIN_SENZ_STG,    INPUT);
  pinMode(PIN_SENZ_DR,     INPUT);
  //pinMode(PIN_SENZ_CENTRU, INPUT);
  pinMode(PIN_START_STOP,  INPUT);

  pinMode(PIN_LIA_STG,    INPUT);
  pinMode(PIN_LIA_CENTRU, INPUT);
  pinMode(PIN_LIA_DR,     INPUT);

 SabertoothTXPinSerial.begin(115200);

  state      = STATE_READY;
  onLine     = false;
  parteLinie = 0;
  timpAtac   = 0;
  timpQTR    = 0;

setMotoare(0, 0);
}

void loop() {
  citesteSenzori();

  switch(state)
  {
    case STATE_READY:
        setMotoare(0,0);
        timpAtac=millis();
        if(digitalRead(PIN_START_STOP)==HIGH)
        {
          state=STATE_FIGHT;
          onLine=false;
        }
        break;
    case STATE_FIGHT:
        if (digitalRead(PIN_START_STOP) == LOW) {
        state = STATE_STOPPED;
        break;
      }
      if (onLine) linieAlba();
      else        opponentSearch();
      break;
    case STATE_STOPPED:
        setMotoare(0,0);
        break;
  }
}


void citesteSenzori()
{
   senzori = 0;

  bool opStg = digitalRead(PIN_SENZ_STG);
  bool opDr= digitalRead(PIN_SENZ_DR);

  int liaFata = analogRead(PIN_LIA_CENTRU);
  int liaDr = analogRead(PIN_LIA_DR);
  int liaStg = analogRead(PIN_LIA_STG);

  if (opStg)
  { senzori |= SENZ_OP_STG;}

  if (opDr)
  {senzori |= SENZ_OP_DR;}

  //if (liaSpate < PRAG_LINIE) { senzori |= LINIE_SPATE;}
  if (liaFata < PRAG_LINIE) {senzori |= LINIE_FATA;}
  if (liaDr < PRAG_LINIE)   {senzori |= LINIE_DR;}
  if (liaStg < PRAG_LINIE)  {senzori |= LINIE_STG;}

  mascaSensori= senzori;
  onLine = (senzori & (LINIE_FATA | LINIE_DR | LINIE_STG));
  if (onLine && timpQTR == 0) { 
    if      ((senzori & LINIE_STG) && !(senzori & LINIE_DR)) parteLinie = -1;
    else if ((senzori & LINIE_DR)  && !(senzori & LINIE_STG)) parteLinie =  1;
    else                                                        parteLinie =  0;
    timpQTR = millis();  
}
  offLine = !onLine;

}

void linieAlba() {
  unsigned long t = millis() - timpQTR;

  if (t < TIMP_LIA_INAINTE) {
    setMotoare(VITEZA_MAX, VITEZA_MAX);

  } else if (t < (unsigned long)(TIMP_LIA_INAINTE + TIMP_ROT_LINIE)) {
    if (parteLinie == -1) setMotoare( VITEZA_MAX, -VITEZA_MAX);
    else                  setMotoare(-VITEZA_MAX,  VITEZA_MAX);

  } else {
    onLine     = false;
    parteLinie = 0;
    timpQTR    = 0;  
  }
}

void setMotoare(int8_t vm1, int8_t vm2)
{
  #ifndef DEBUG_MODE
    ST.motor(1,vm1);
    ST.motor(2,vm2);
  #else
  Serial.print("Motor1: "); Serial.print(vm1);
  Serial.print(" | Motor2: "); Serial.println(vm2);
#endif  
}

void opponentSearch()
{
  
  bool opStg = senzori & SENZ_OP_STG;
  bool opDr = senzori & SENZ_OP_DR;

  if (opStg && opDr)
  {// daca ii in fata
  setMotoare(VITEZA_MAX, VITEZA_MAX);
  timpAtac= millis();
  }
  else if (opStg)
  {//oponent in stanga
  setMotoare(-VITEZA_CAUTARE, VITEZA_CAUTARE);
  timpAtac = millis();
  }
  else if (opDr)
  {//oponent in dreapta
  setMotoare(VITEZA_CAUTARE, -VITEZA_CAUTARE);
  timpAtac = millis();
  }
  else{// daca a iesit din raza de vedere a senzorului, rotire
    setMotoare(VITEZA_CAUTARE, -VITEZA_CAUTARE);
  }
}
