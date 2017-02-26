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

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

void GameBtns::draw(void) {
  m_tft.fillCircle(m_x + 25, m_y, 10, YELLOW);
  m_tft.fillCircle(m_x - 25, m_y, 10, YELLOW);
  m_tft.fillCircle(m_x, m_y + 25, 10, BLUE);
  m_tft.fillCircle(m_x, m_y - 25, 10, BLUE);
}

int GameBtns::getDir(int direction)
{
  uint16_t x, y;
  uint8_t z;
  // See if there's any  touch data for us
  if (m_ts.touched()) {
    while (!m_ts.bufferEmpty()) {
      //Serial.print(m_ts.bufferSize());
      m_ts.readData(&x, &y, &z);
      /*
      Serial.print("->(");
      Serial.print(x); Serial.print(", ");
      Serial.print(y); Serial.print(", ");
      Serial.print(z);
      Serial.println(")");
      */
    }
    m_ts.writeRegister8(STMPE_INT_STA, 0xFF);
    x = map(x, TS_MINX, TS_MAXX, 0, m_tft.width());
    y = map(y, TS_MINY, TS_MAXY, m_tft.height(), 0);
    ///*
    Serial.print("mapped->(");
    Serial.print(x); Serial.print(", ");
    Serial.print(y);
    Serial.println(")");
    //*/
    if (y < 100) {
      //Serial.println("A palpable hit!");
      if (y > 75) {
        Serial.print("Up! y: ");
        Serial.println(y);
        direction = DirUp;
      } else if (y < 40) {
        Serial.print("Down! y: ");
        Serial.println(y);
        direction = DirDown;
      } else if (x < 110) {
        Serial.print("Left! x: ");
        Serial.println(x);
        direction = DirLeft;
      } else if (x > 125) {
        Serial.println("Right! y: ");
        Serial.println(y);
        direction = DirRight;
      }
    }
  }
  return direction;
}

