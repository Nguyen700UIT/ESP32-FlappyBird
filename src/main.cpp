#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define BUTTON 4
#define BUZZER 5

#define BIRD_X 30

unsigned long long lastTime = 0;

struct Tube{
  int tubeX[3];
  int upperTubeHeight[3];
  int lowerTubeHeight[3];

//Tube constructor
  Tube(){
    tubeX[0] = 128;
    tubeX[1] = 183;
    tubeX[2] = 238;
  }
};

//Bird fly 
volatile bool flew = false;
bool buzzerOn = false;

//Bird
float birdY = 32;
int birdWidth = 5;
int birdHeight = 5;
float birdVelocity = 0; 
const float GRAVITY = 0.25;
float jumpForce = -2.35;

//Tube
int tubeGap = 25;
int TubeWidth = 15;
Tube tube;

int velocity = 3;

void IRAM_ATTR handleButton()
{
  flew = true;
}

void initTube()
{

  for (int i = 0; i < 3; ++i)
  {
    tube.upperTubeHeight[i] = random(5, SCREEN_HEIGHT - tubeGap - 5);
    tube.lowerTubeHeight[i] = SCREEN_HEIGHT - tube.upperTubeHeight[i] - tubeGap;
  }


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
  initTube();
}


void drawTube()
{
  for(int i = 0; i < 3; ++i)
  {
    //Draw upperTube
    display.fillRect(tube.tubeX[i], 0, TubeWidth, tube.upperTubeHeight[i], SSD1306_WHITE);
    int lowerTubeY = tube.upperTubeHeight[i] + tubeGap;
    //Draw lowerTube
    display.fillRect(tube.tubeX[i], lowerTubeY, TubeWidth, tube.lowerTubeHeight[i], SSD1306_WHITE);
  }

}

void drawBird()
{
  display.drawRect(BIRD_X, birdY, birdWidth, birdHeight, SSD1306_WHITE);
}

void birdDropLogic()
{
  birdY += birdVelocity;
  birdVelocity += GRAVITY;
}

void birdJumpLogic()
{
  birdVelocity = jumpForce;
}

void loop() {
  birdDropLogic();
  if(flew)
  {
    birdJumpLogic();
    digitalWrite(BUZZER, HIGH);
    lastTime = millis();
    buzzerOn = true;
    flew = false;
  }  
  if(buzzerOn && millis() - lastTime >= 100)
  {
    digitalWrite(BUZZER, LOW);
    buzzerOn = false;
  }
  display.clearDisplay();
  drawTube();
  drawBird();
  display.display();
  for (int i = 0; i < 3; ++i)
  {
    tube.tubeX[i] -= 1;
    //Spawn new tube
    if(tube.tubeX[i] + TubeWidth < 0)
    {
     int maxX = tube.tubeX[0];
      for (int j = 0;j < 3; ++j)
      {
        maxX = max(maxX, tube.tubeX[j]);
      }
      tube.tubeX[i] = maxX + 55;
      tube.upperTubeHeight[i] = random(5, SCREEN_HEIGHT - tubeGap - 5);
      tube.lowerTubeHeight[i] = SCREEN_HEIGHT - tube.upperTubeHeight[i] - tubeGap;
    }
  }
}

