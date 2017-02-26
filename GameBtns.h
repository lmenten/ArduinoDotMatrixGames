#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

enum {
  DirUp,
  DirDown,
  DirLeft,
  DirRight
};

class GameBtns {
  public:
    unsigned short m_uRadius;
    unsigned short m_x, m_y;
    Adafruit_ILI9341& m_tft;
    Adafruit_STMPE610& m_ts;
    GameBtns(Adafruit_ILI9341& tft, Adafruit_STMPE610& ts, unsigned short x, unsigned short y): m_tft(tft), m_ts(ts) {
      m_x = x;
      m_y = y;
    };
    GameBtns(Adafruit_ILI9341& tft, Adafruit_STMPE610& ts): m_tft(tft), m_ts(ts) {
      m_x = 120;
      //m_y = 60;
      m_y = 275;
    };
    void draw(void);
    int getDir(int direction);
};
