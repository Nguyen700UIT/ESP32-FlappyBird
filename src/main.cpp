#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BUTTON 4
#define BUZZER 5


void setup() {
  Serial.begin(9600);
  //Init pins
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  //Init OLED
  Wire.begin(21, 22);
  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("Display allocated");
  }
  else 
    Serial.println("Display allocation failed");
  
  display.clearDisplay();
  display.display();
  
}

void loop() {
  
}

