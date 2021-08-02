/*
This is the code for an electronic dice simulator mainly for the board game "Catan".
several modes are usable :
-Classis catan (same probabilities as with 2 six sided dices) 
-6&8 mode : all numbers have the same probability to show up, only 6 and 8 have double odds to appear
-flat mode : all numbers have the same probability to show up
-1 to 12 mode : a random number between 1 and 12 will show up, same probability for all. Not for Catan
during the setup phase,you can use the main button to pick your mode : the dimming state of the numbers
is proportionnal to the probability they show up. Validate with the secondary button.
Once you have validate, you can reset the device with the secondary button

the code is ready to go for a attiny 85
the reset function won't work on a non attiny based arduino, you will have to use the resetFunc
that is just before the setup instead.
*/
#include "FastLED.h"
#include <avr/wdt.h>

#define NUM_LEDS 12
#define DATA_PIN 0 // for the LED strip
#define CLOCK_PIN 1 // for the LED strip
#define button 2 // main button, make it big and obvious!
#define mode_butt 3 // secondary button to validate or reset, make it small!

CRGB leds[NUM_LEDS];
int ONLED;
int ONLEDrot;
int secondCount; //counter no2 for mode 0 (the second dice)
int COLOR;
int Sat;
long timer = 0;
int OLDbut;
bool pressed = false;

int MODE = 0;
//mode 0 : normal catan, like 2x dices with 6 faces. Same probabilities as with dices. 1 is YELLOW
//mode 1 : catan with double odds for 6 and 8, flat for the others (included 7). 1 is PURPLE
//mode 2 : same odds for all numbers, 1 excluded from showing up. 1 is BLUE
//mode 3 : same as mode 2, but double odds only for number 7. 1 is RED
//mode 4 : flat odds from 1 to 12 (not for catan because 1 can show up). 1 is not lit

//void(* resetFunc) (void) = 0; //doesn't work with attiny, this is for uno, nano type.

void setup() {
  // Serial.begin(57600);
  //	Serial.println("resetting");
  //	LEDS.addLeds<WS2812,DATA_PIN,GRB>(leds,NUM_LEDS);
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  LEDS.setBrightness(255);

  pinMode(button, INPUT_PULLUP);
  pinMode(mode_butt, INPUT_PULLUP);

  wdt_disable();

  for (int i = 0; i < NUM_LEDS * 10 / 2 ; i++) {
    leds[i / 10] = CHSV(0, 0, 80 + i * 3);
    leds[11 - i / 10] = CHSV(0, 0, 80 + i * 3);
    leds[0] = CHSV(map(i, 0, 60, 0, 40), map(i, 0, 60, 0, 255), map(i, 0, 60, 0, 100) );
    FastLED.show();
    fadeall();
    delay(5);
  }
  //choose your mode:
  while (digitalRead(mode_butt) == HIGH) {
    if (digitalRead(button) == LOW) { //main button is pressed
      MODE > 3 ? MODE = 0 : MODE++;
      delay(350);
    }
    if (MODE == 0) {
      leds[1] = CHSV(0, 0, 1 );
      leds[11] = CHSV(0, 0, 1 );
      leds[2] = CHSV(0, 0, 25 );
      leds[10] = CHSV(0, 0, 25 );
      leds[3] = CHSV(0, 0, 55 );
      leds[9] = CHSV(0, 0, 55 );
      leds[4] = CHSV(0, 0, 90 );
      leds[8] = CHSV(0, 0, 90 );
      leds[5] = CHSV(0, 0, 150 );
      leds[7] = CHSV(0, 0, 150 );
      leds[6] = CHSV(0, 0, 250 );
      leds[0] = CHSV(40, 255, 100 ); //the yellow led
    }
    if (MODE == 1) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(0, 0, 250 / 8 );
      }
      leds[5] = CHSV(0, 0, 250 );
      leds[7] = CHSV(0, 0, 250 );
      leds[0] = CHSV(200, 255, 100 ); //the purple led
    }
    if (MODE == 2) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(0, 0, 50 );
      }
      leds[0] = CHSV(155, 255, 100 ); //the blue led
    }
    if (MODE == 3) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(0, 0, 250 / 8 );
      }
      leds[6] = CHSV(0, 0, 250 );
      leds[0] = CHSV(0, 255, 100 ); //the red led
    }
    if (MODE == 4) {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(0, 0, 100 );
      }
    }
    FastLED.show();
  }

  for (int l = 0 ; l < 150 ; l++) {

    for (int i = 1 ; i < NUM_LEDS ; i++) {
      leds[i].nscale8( 251);
    }
    FastLED.show();
    delay(6);
  }
}

void reboot() {

  cli();
  WDTCR = 0xD8 | WDTO_1S;
  sei();

  wdt_reset();
  while (true) {}

} //reboot for Attiny

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(200);
  }
}

void loop() {

  if (digitalRead(mode_butt) == LOW) {
    delay(500);
    reboot();
    //  resetFunc(); //reset
  }

  if (MODE == 0) {
    ONLEDrot < 6 ? ONLEDrot ++ : ONLEDrot = 1 ;
  }
  if (MODE == 1) {
    ONLEDrot < 12 ? ONLEDrot ++ : ONLEDrot = 0 ;
  }
  if (MODE == 2) {
    ONLEDrot < 10 ? ONLEDrot ++ : ONLEDrot = 0 ;
  }
  if (MODE == 3) {
    ONLEDrot < 11 ? ONLEDrot ++ : ONLEDrot = 0 ;
  }
  if (MODE == 4) {
    ONLEDrot < 11 ? ONLEDrot ++ : ONLEDrot = 0 ;
  }



  while (digitalRead(button) == LOW && pressed == true ) { //button pressed at the previous loop and hold pressed
    //blocker
  }

  if (pressed == true && digitalRead(button) == HIGH) {
    delay(15);
    pressed = false;
  }

  if (digitalRead(button) == LOW && pressed == false ) { //button pressed
    while (digitalRead(button) == LOW) { //button pressed and hold pressed
      secondCount < 6 ? secondCount++ : secondCount = 1; //randomizer for the second dice
    }

    pressed = true;
    //set the lighting up LED
    ONLED = ONLEDrot;
    if (MODE == 0) {
      ONLED += secondCount; //result should be between 1 and 11 (from 2 to 12 to be displayed)
      ONLED -= 1;
    }
    if (MODE == 1) {
      if (ONLED == 0) {
        ONLED = 5;
      }
      if (ONLED == 12) {
        ONLED = 7;
      }
    }
    if (MODE == 2) {
      ONLED += 1;
    }
    if (MODE == 3) {
      ONLED += 1;
      if (ONLED == 12) {
        ONLED = 6;
      }
    }
    if (MODE == 4) {
      //nothing to do
    }



    // Serial.print("Result : ");
    // Serial.println(ONLED);

    for (int n = 0 ; n < 3 ; n++) {
      for (byte i = 1; i < NUM_LEDS; i++) {
        leds[i] = CHSV(i * 15, 255 - (i + n * i) * 10, 70 + (i + n * i) * 3);
        if (MODE == 0) {
          leds[0] = CHSV(40, 255, 65 ); //the yellow led
        }
        if (MODE == 1) {
          leds[0] = CHSV(200, 255, 65 ); //the purple led
        }
        if (MODE == 2) {
          leds[0] = CHSV(155, 255, 65 ); //the blue led
        }
        if (MODE == 3) {
          leds[0] = CHSV(0, 255, 65 ); //the red led
        }
        FastLED.show();
        fadeall();
        //      delay(40-(n-1)*20);
        delay((2 * NUM_LEDS + NUM_LEDS) - (i + n * i) + 20 - n * 10);
      }
    }

    for (int a = 0 ; a < 100 ; a++) {
      ONLED == 6 ? COLOR = 0 : COLOR = 100;

      if (ONLED == 5 || ONLED == 7) {
        COLOR = 200;
      }
      if (MODE == 4) {
        COLOR = 145;
        Sat = 190;
      }
      else {
        Sat = 255;
      }

      leds[ONLED] = CHSV(COLOR, Sat, 2.5 * a);
      delay(5);
      fadeall();

      if (MODE == 0) {
        leds[0] = CHSV(40, 255, 65 ); //the yellow led
      }
      if (MODE == 1) {
        leds[0] = CHSV(200, 255, 65 ); //the purple led
      }
      if (MODE == 2) {
        leds[0] = CHSV(155, 255, 65 ); //the blue led
      }
      if (MODE == 3) {
        leds[0] = CHSV(0, 255, 65 ); //the red led
      }
      FastLED.show();
    }

    // Serial.print("Count = ");
    // Serial.println( ONLEDrot);
    // Serial.print("secondCount = ");
    // Serial.println(secondCount);
    // Serial.println("--------------");
  }

}
