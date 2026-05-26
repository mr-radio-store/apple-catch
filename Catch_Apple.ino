/*
Catch red-like apple game with touch screen

Wire Connection
📺 TFT Display (Parallel 8-bit Interface)
TFT Pin	Arduino UNO Pin	Description
DB0–DB7	D8–D0	Data bus (8-bit)
RD	A0	Read (Active Low)
WR	A1	Write (Active Low)
RS (DC)	A2	Register Select
CS	A3	Chip Select
RESET	A4	Reset (Active Low)
VCC	5V	Power
GND	GND	Ground
LED	3.3V/5V (via 100Ω)	Backlight (optional)

⚠️ You can adjust control pins in the MCUFRIEND_kbv library if using a breakout, but it's designed for shields.
*/


#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

#define YP A1
#define XM A2
#define YM 7
#define XP 6

#define TS_MINX 120
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 900

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define MINPRESSURE 200
#define MAXPRESSURE 1000

const int NUM_APPLES = 4;
int appleX[NUM_APPLES];
int appleY[NUM_APPLES];
int appleSize = 10;

int screenW, screenH;

int basketW = 60;
int basketH = 10;
int basketX;
int basketY;

int score = 0;
int speed = 5;

void drawBasket() {
  tft.fillRect(basketX, basketY, basketW, basketH, 0x07E0); // green
}

void eraseBasket() {
  tft.fillRect(basketX, basketY, basketW, basketH, 0xFFFF);
}

void drawApple(int i) {
  tft.fillCircle(appleX[i], appleY[i], appleSize, 0xF800); // red
}

void eraseApple(int i) {
  tft.fillCircle(appleX[i], appleY[i], appleSize, 0xFFFF);
}

void resetApple(int i) {
  appleX[i] = random(appleSize, screenW - appleSize);
  appleY[i] = random(-100, -10); // start above screen
}

void updateScore() {
  tft.fillRect(0, 0, screenW, 20, 0xFFFF);
  tft.setCursor(5, 5);
  tft.setTextColor(0x0000);
  tft.setTextSize(2);
  tft.print("Score: ");
  tft.print(score);
}

void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(0);

  screenW = tft.width();
  screenH = tft.height();

  basketX = screenW / 2 - basketW / 2;
  basketY = screenH - basketH - 10;

  tft.fillScreen(0xFFFF);
  updateScore();
  drawBasket();

  for (int i = 0; i < NUM_APPLES; i++) {
    resetApple(i);
  }
}

void loop() {
  // Touch Input
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int x = map(p.y, TS_MINY, TS_MAXY, 0, screenW);
    eraseBasket();
    basketX = constrain(x - basketW / 2, 0, screenW - basketW);
    drawBasket();
  }

  // Move apples
  for (int i = 0; i < NUM_APPLES; i++) {
    eraseApple(i);
    appleY[i] += speed;

    // Check catch
    if (appleY[i] + appleSize >= basketY && appleY[i] <= basketY + basketH &&
        appleX[i] >= basketX && appleX[i] <= basketX + basketW) {
      score++;
      updateScore();
      resetApple(i);
      if (score % 5 == 0) speed++; // increase speed every 5 points
    }
    // Missed
    else if (appleY[i] > screenH) {
      resetApple(i);
    }

    drawApple(i);
  }

  delay(50);
}
