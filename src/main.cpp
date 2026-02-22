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

const unsigned char bird1 [] PROGMEM = {
0x18,
0x3C,
0x7E,
0xDB,
0xFF,
0x7E,
0x3C,
0x18
};

const unsigned char bird2 [] PROGMEM = {
0x18,
0x3C,
0x7E,
0xFF,
0xFF,
0x7E,
0x3C,
0x18
};

const unsigned char bird3 [] PROGMEM = {
0x18,
0x3C,
0x7E,
0xDB,
0xFF,
0x3C,
0x18,
0x00
};

unsigned long long lastTime = 0;
unsigned long long lastInterruptTime = 0;
struct Tube{
  bool passed[3];
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


//Debounce
const int debounce = 150;

//Bird fly 
volatile bool flew = false;
bool buzzerOn = false;

//Bird
int birdFrame = 0;
unsigned int lastFrameTime = 0;
float birdY = 32;
int birdWidth = 8;
int birdHeight = 8;
float birdVelocity = 0; 
const float GRAVITY = 0.25;
float jumpForce = -2.35;

//Score 
int score = 0;
int bestScore = 0;
//Game
bool gameOver = false;
bool lost = false;
//Tube
int tubeGap = 30;
int TubeWidth = 15;
Tube tube;

int velocity = 3;

void IRAM_ATTR handleButton()
{
  unsigned long long curr = millis();
  if(curr - lastInterruptTime > 150) 
  {
    flew = true;
    lastInterruptTime = curr;
  }
}

void initTube()
{

  for (int i = 0; i < 3; ++i)
  {
    tube.upperTubeHeight[i] = random(5, SCREEN_HEIGHT - tubeGap - 5);
    tube.lowerTubeHeight[i] = SCREEN_HEIGHT - tube.upperTubeHeight[i] - tubeGap;
    tube.passed[i] = false;
  }

}

void updateAnimation()
{
  if (millis() - lastFrameTime > 120)
  {
    birdFrame++;
    if (birdFrame > 2)
    {
      birdFrame = 0;
    }
    lastFrameTime = millis();
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
    const unsigned char* frames[] = {bird1, bird2, bird3};

    display.drawBitmap(
        BIRD_X,
        (int)birdY,
        frames[birdFrame],
        8,
        8,
        SSD1306_WHITE
    );
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

void collisionLogic()
{
  for (int i = 0; i < 3; ++i)
  {
    if(BIRD_X + birdWidth > tube.tubeX[i] && tube.tubeX[i] + TubeWidth > BIRD_X)
    {
      int top = tube.upperTubeHeight[i];
      int bot = tube.upperTubeHeight[i] + tubeGap;
      if(birdY < top || birdY + birdHeight > bot)
        gameOver = true;
    }
  }
  if(birdY <= 0 || birdY + birdHeight >= SCREEN_HEIGHT)
{
    gameOver = true;
}
}

void scored()
{
  for (int i = 0; i < 3; ++i)
  {
    if(BIRD_X >= tube.tubeX[i] + TubeWidth && tube.passed[i] == false)
    {
      
      score++;
      tube.passed[i] = true;
      bestScore = max(bestScore, score);
    }
  }
}

void loop() {
//Logic
if(!gameOver)
{
  if(flew)
    {
      birdJumpLogic();
      digitalWrite(BUZZER, HIGH);
      lastTime = millis();
      buzzerOn = true;
      flew = false;
    }  
    if(buzzerOn && millis() - lastTime >= 10)
    {
      digitalWrite(BUZZER, LOW);
      buzzerOn = false;
    }
    birdDropLogic();
    collisionLogic();
    scored();
}
else //Draw game over 
{
  if(lost == false)
  {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    lost = true;
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 20);
  display.println("GAME OVER");
        
  display.setTextSize(1);
  display.setCursor(30, 40);
  display.print("Score: ");
  display.println(score);
  display.setTextSize(1);
  display.setCursor(30, 50);
  display.print("Best score: ");
  display.println(bestScore);
  display.display();
  
}

if (gameOver && digitalRead(BUTTON) == LOW)
{
  gameOver = false;
  score = 0;
  tube = Tube();
  initTube();
  birdY = 32;
  birdVelocity = 0;
  lost = false;
  delay(1000);
}

if(!gameOver)
{
  //Drawing
  display.clearDisplay();
  drawTube();
  updateAnimation();
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
      tube.passed[i] = false;
    }
  }
}
}

