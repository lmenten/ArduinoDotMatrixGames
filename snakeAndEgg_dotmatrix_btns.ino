
#include "LedSimulation.h"
#include <SPI.h>
#include <Wire.h>
#include "GameBtns.h"

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

LedSimulation *led = new LedSimulation(tft);
GameBtns btns(tft, ts);

#define MAX_SNAKE  64
int x[MAX_SNAKE], y[MAX_SNAKE], xEgg, yEgg, lenSnake;

void setup(void)
{
  Serial.begin(9600);
  Serial.println(F("Snake"));
  tft.begin();

  if (!ts.begin()) {
    Serial.println("Unable to start touchscreen.");
  } else {
    Serial.println("Touchscreen started.");
  }

  Serial.println(F("Game!"));
  tft.fillScreen(BLACK);
  // if using hardware SPI on an Uno #10 must be an output, remove line
  // if using software SPI or I2C
  pinMode(10, OUTPUT);

  if (!ts.begin()) {
    Serial.println("Unable to start touchscreen.");
  } else {
    Serial.println("Touchscreen started.");
  }

  led->clear();
  btns.draw();
  randomSeed(0);

  lenSnake = 4;
  for (int i = 0; i <= lenSnake; i++) {
    //Set starting coordinates of snake
    x[i] = i + 2;
    y[i] = 8;
  }
  xEgg = 10;
  yEgg = 10;
  drawSnake(x, y);
}

void drawSnake(int x[], int y[])
{
  //Serial.print("Snake head: ("); Serial.print(x[0]); Serial.print(","); Serial.print(y[0]); Serial.println(")");
  led->setLedBlue(x[0], y[0]);
  for (int i = 1; i < lenSnake; i++) {
    led->setLedRed(x[i], y[i]);
  }
  led->setLedGreen(xEgg, yEgg);
  led->draw();
}

void placeEgg(void)
{
  while (true) {
    int a = random(0, 16);
    int b = random(0, 16);
    if (led->getLedStateCur(a, b) == 1) {
      led->setLedGreen(a, b);
      xEgg = a;
      yEgg = b;
      break;
    }
  }
}

bool collision(int x[], int y[], int a, int b)
{
  bool bCollision = false;
  int eState = led->getLedStateCur(a, b);
  bCollision = (eState != 1) && (eState != 3);
  if (bCollision) {
    //Serial.println("Collision!");
  }
  return bCollision;
}

bool moveS(int x[], int y[], int eDir)
{
  bool bSuccess = false;
  //Serial.print("random: ");
  //Serial.println(eDir);
  switch (eDir) {
    case DirUp:
      if ((y[1] > 0) && (!collision(x, y, x[1], y[1] - 1))) {
        x[0] = x[1];
        y[0] = y[1] - 1;
        //Serial.println("DirUp");
        bSuccess = true;
      }
      break;
    case DirDown:
      if ((y[1] < 15) && (!collision(x, y, x[1], y[1] + 1))) {
        x[0] = x[1];
        y[0] = y[1] + 1;
        //Serial.println("DirDown");
        bSuccess = true;
      }
      break;
    case DirLeft:
      if ((x[1] > 0) && (!collision(x, y, x[1] - 1, y[1]))) {
        x[0] = x[1] - 1;
        y[0] = y[1];
        //Serial.println("DirLeft");
        bSuccess = true;
      }
      break;
    case DirRight:
      if ((x[1] < 15) && (!collision(x, y, x[1] + 1, y[1]))) {
        x[0] = x[1] + 1;
        y[0] = y[1];
        //Serial.println("DirRight");
        bSuccess = true;
      }
      break;
  }
  //Serial.print("moveS: eDir = ");Serial.print(eDir);Serial.print(", bSuccess = "); Serial.println(bSuccess);
  return bSuccess;
}

int moveIt(int x[], int y[], int eDir)
{
  //Serial.println("moveSnake");
  int a = x[lenSnake - 1];
  int b = y[lenSnake - 1];
  for (int i = lenSnake - 1; i > 0; i--) {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }
  int nTried = 0;
  bool aTried[4];
  for (int i = 0; i < 4; i++) {
    aTried[i] = false;
  }
  bool bSuccess = false;
  while (!bSuccess) {
    //Serial.println("loop1");
    while ((nTried < 4) && (!bSuccess)) {
      //Serial.print("nTried: ");Serial.println(nTried);
      if (!aTried[eDir]) {
        aTried[eDir] = true;
        nTried += 1;
        bSuccess = moveS(x, y, eDir);
        //Serial.print("New candidate: "); Serial.print(eDir); Serial.print(" result: "); Serial.println(bSuccess);      
      } else {
        eDir = random(0, 4);
      }
    }
    if (bSuccess) {
      if (lenSnake < MAX_SNAKE) {
        // Did we eat an egg?
        if ((x[0] == xEgg) && (y[0] == yEgg)) {
          placeEgg();
          lenSnake++;
          x[lenSnake - 1] = a;
          y[lenSnake - 1] = b;
        }
      }
    } else {
      // No forward movement possible.  Reverse snake.
      Serial.println("Reverse");
      for (int i = 1; i < lenSnake / 2; i++) {
        int xt = x[i];
        x[i] = x[lenSnake - i];
        x[lenSnake - i] = xt;
      }
      x[0] = a;
      y[0] = b;
      bSuccess = true;
      break;
    }
  }
  //Serial.print("Returning: "); Serial.println(eDir);
  return eDir;
}

bool bEgg = false;
int eDir = DirLeft;
int nTimer = 0;

void loop(void)
{
  nTimer++;
  delay(10);
  eDir = btns.getDir(eDir);
  if (nTimer > 100) {
    eDir = moveIt(x, y, eDir);
    drawSnake(x, y);
    nTimer = 0;
  }
}






