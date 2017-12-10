/*
 * Emergency Button Z-wave Program 
 * Run on Z-Uno plate forme
 * Author : Jean Perardel
 */


#include <ZUNO_NeoPixel.h>

// 13 pin - user LED of Z-Uno board
#define LED_PIN     13

// button pin number
// 18 pin - button(BTN) of Z-Uno board
#define BTN_PIN     18

// channel number
#define ZUNO_CHANNEL_NUMBER_ONE   1

#define MAX_PIXELS 1  // Number of pixel in strip or module that we use
// ! The maximum supported number is 25
#define PIXEL_SIZE 3   // Number of color components. Do not modify. Must be 3
#define BUFF_SIZE     (MAX_PIXELS * PIXEL_SIZE)

#define CPT_VALUE_BTW_BLINK 15000
#define CPT_VALUE_MESSAGE 50000

byte pixel_buff[BUFF_SIZE]; // Pixel buffer

// Make object to control neo-pixel module/strip
NeoPixel pixels(pixel_buff, BUFF_SIZE);

// variable to store current button state
byte lastButtonState;
long cpt = 0;

//ZUNO_SETUP_CHANNELS(ZUNO_SENSOR_BINARY(ZUNO_SENSOR_BINARY_TYPE_GENERAL_PURPOSE, getter));
ZUNO_SETUP_CHANNELS(ZUNO_SENSOR_MULTILEVEL_GENERAL_PURPOSE(getter));

// the setup routine runs once when you press reset:
void setup() {
  pixels.begin();
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT); // set LED pin as output
  pinMode(BTN_PIN, INPUT_PULLUP); // set button pin as input
  pixels.setPixelColor(0, 0xFFFFFFFF); // Set the LED to OFF
  pixels.show();
}

// the loop routine runs over and over again forever:
void loop() {
  byte currenButtonState = digitalRead(BTN_PIN);

  if (currenButtonState == LOW) { // if button is pressed
    cpt = CPT_VALUE_MESSAGE;
    lastButtonState = 0; 

    for (int i = 0; i < 8; i++) {
      zunoSendReport(ZUNO_CHANNEL_NUMBER_ONE); // send report over the Z-Wave to the controller
      pixels.setPixelColor(0, 0xFFFF00FF); // Turn Red LED on
      delay(1);
      pixels.show();
      delay(400);
      pixels.setPixelColor(0, 0xFFFFFFFF);
      delay(1);
      pixels.show();
      delay(400);
    }
  }

  if (cpt > 0) {
    cpt --;
  }
  else if (cpt == 0) {
    cpt = CPT_VALUE_BTW_BLINK;
    lastButtonState = 1; // save new state
    pixels.setPixelColor(0, 0xFF00FFFF); // Blink Green
    pixels.show();
    delay(50);
    pixels.setPixelColor(0, 0xFFFFFFFF);
    pixels.show();
  }
}

byte getter() {

  if (lastButtonState == 0) { // if button is pressed
    return 0x64;              // return "Triggered" state to the controller. 0x64 is 100 in decimal
  } else {                    // if button is released
    return 0x01;                 // return "Idle" state to the controller
  }
}
