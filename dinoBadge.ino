// enable or disable GxEPD2_GFX base class
#define ENABLE_GxEPD2_GFX 0
#define BUTTON_PIN 34

#include <BluetoothSerial.h>
#include <WiFi.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

#include "qr_code_122x122.h"
#include "black_dino.h"
#include "red_dino.h"

// Enable 3-color EPD
GxEPD2_3C<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT> display(GxEPD2_213_Z98c(/*CS=5*/ 22, /*DC=*/ 21, /*RES=*/ 19, /*BUSY=*/ 5)); // SCK (Clock): GPIO 18 MOSI (Data): GPIO 23; GDEY0213Z98 122x250, SSD1680

const int ledPin = 2;

const char name[] = "Virgil";
const char divider[] = "__________";
const char position1[] = "Poet";
const char position2[] = "Cisalpine";
const char position3[] = "Gaul";

const int buttonPin = 34;
const int buttonThreshold = 500;
bool dinoFlag = false;
int buttonState;
int lastButtonState = LOW;
unsigned long pressTime = 0;
unsigned long releaseTime = 0;
bool buttonPressed = false;
const int sleepDuration = 300000; // five minutes deep sleep timer

void setup()
{
  WiFi.mode(WIFI_OFF);
  btStop();

  pinMode(BUTTON_PIN, INPUT);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, HIGH);

  Serial.begin(115200);
  display.init(115200, true, 50, false);
  display.setRotation(1);
  showBusiness();
  delay(1000);
}

void makeDino()
{
  display.setPartialWindow(0, 0, 250, 122);
  display.firstPage();
  do
  {
    display.drawBitmap(0, 0, black_dino, 250, 122, GxEPD_BLACK);
    display.drawBitmap(0, 0, red_dino, 250, 122, GxEPD_RED);
  } 
  while (display.nextPage());
}


void showBusiness()
{
  display.setPartialWindow(0, 0, 250, 122);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, qr_code_122x122, 122, 122, GxEPD_BLACK);

    display.setTextColor(GxEPD_BLACK);

    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(140, 20);
    display.print(name);

    display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK); //red if available
    display.setCursor(133, 35);
    display.print(divider);

    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMono9pt7b);
    display.setCursor(140, 60);
    display.print(position1);
    display.setCursor(140, 95);
    display.print(position2);
    display.setCursor(140, 110);
    display.print(position3);
  }
  while (display.nextPage());
}

void loop() {
  buttonState = digitalRead(buttonPin); //gpio button

  if (buttonState == HIGH && lastButtonState == LOW) {
    pressTime = millis();
    buttonPressed = true;
  }

  if (buttonState == LOW && lastButtonState == HIGH && buttonPressed) {
    releaseTime = millis();
    unsigned long duration = releaseTime - pressTime;

    if (duration < buttonThreshold) {
      dinoFlag = !dinoFlag;
      if (dinoFlag) {
        Serial.println("makeDino");
        makeDino();
      } else {
        Serial.println("showBusiness");
        showBusiness();
      }
      delay(100);
    }
    buttonPressed = false;
  }
  lastButtonState = buttonState;

  if (millis() - releaseTime >= sleepDuration) {
    Serial.println("Going to sleep...");
    display.hibernate();
    esp_deep_sleep_start();
  }
}