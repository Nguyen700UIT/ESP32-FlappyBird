#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BUTTON 4
#define BUZZER 5

//Bird fly 
volatile bool flew = false;

//Tube
int tubeGap = 20;
int TubeWidth = 30;
int tubeX = 98; 
int upperTubeHeight = 30;
int lowerTubeHeight;
int velocity = 3;

void IRAM_ATTR handleButton()
{
  flew = true;
}

void initTube()
{
  upperTubeHeight = random(5, SCREEN_HEIGHT - tubeGap - 5);
  lowerTubeHeight = SCREEN_HEIGHT - upperTubeHeight - tubeGap;

}

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
  attachInterrupt(digitalPinToInterrupt(BUTTON), handleButton, FALLING);

}


void drawTube()
{
  //Draw upperTube
  display.fillRect(tubeX, 0, TubeWidth, upperTubeHeight, SSD1306_WHITE);
  
  //Draw lowerTube
  int lowerTubeY = upperTubeHeight + tubeGap;
  display.fillRect(tubeX, lowerTubeY, TubeWidth, lowerTubeHeight, SSD1306_WHITE);
}

void loop() {
  if(flew)
  {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    flew = false;
  }  
  display.clearDisplay();
  drawTube();
  display.display();
  tubeX -= 1;
  if(tubeX + TubeWidth < 0)
  {
    tubeX = SCREEN_WIDTH;
    initTube();
  }
}

