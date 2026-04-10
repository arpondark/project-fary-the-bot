#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int leftX = 10;
int rightX = 118;
int eyeH = 22;
int eyeFrame = 0;

int eyeYOffset = 0;
int pupilYShift = -2;

unsigned long lastTrigger = 0;
unsigned long lastExpressionChange = 0;

int expression = 0;

// mouth animation
int mouthOffset = 0;
bool mouthDir = true;

// ---------------- EYE BLINK ----------------
void tickEyes() {
  eyeFrame++;
  int phase = eyeFrame % 120;

  if (phase < 100) eyeH = 22;
  else if (phase < 110) eyeH = 22 - ((phase - 100) * 2.2);
  else eyeH = (phase - 110) * 2.2;
}

// ---------------- HANDS ----------------
void drawLeftHand(int x, int y) {
  display.fillRoundRect(x, y + 5, 12, 8, 2, SSD1306_WHITE);
  display.fillRoundRect(x + 8, y + 5, 12, 3, 1, SSD1306_WHITE);
  display.fillRoundRect(x + 2, y + 2, 5, 4, 2, SSD1306_WHITE);
}

void drawRightHand(int x, int y) {
  display.fillRoundRect(x - 12, y + 5, 12, 8, 2, SSD1306_WHITE);
  display.fillRoundRect(x - 20, y + 5, 12, 3, 1, SSD1306_WHITE);
  display.fillRoundRect(x - 7, y + 2, 5, 4, 2, SSD1306_WHITE);
}

// ---------------- EXPRESSIONS ----------------
void drawExpression(int y, int m)
{
  switch(expression)
  {
    case 1:
      display.drawLine(25, y - 10, 42, y - 18, SSD1306_WHITE);
      display.drawLine(86, y - 18, 103, y - 10, SSD1306_WHITE);
      break;

    case 2:
      display.drawLine(25, y - 18, 42, y - 10, SSD1306_WHITE);
      display.drawLine(86, y - 10, 103, y - 18, SSD1306_WHITE);
      break;

    case 3:
      display.fillRect(30, y - 2, 24, 4, SSD1306_BLACK);
      display.fillRect(74, y - 2, 24, 4, SSD1306_BLACK);
      display.setCursor(95, y - 18);
      display.print("Z");
      break;

    case 4:
      display.drawCircle(64, m + 2, 3, SSD1306_WHITE);
      break;

    case 5:
      display.fillRect(30, y - 6, 24, 12, SSD1306_BLACK);
      display.fillRect(74, y - 6, 24, 12, SSD1306_BLACK);
      break;
  }
}

// ---------------- FACE ----------------
void drawFace() {
  display.clearDisplay();

  int baseEyeY = 28 + eyeYOffset;
  int baseMouthY = 46;

  display.drawLine(25, baseEyeY - 14, 42, baseEyeY - 18, SSD1306_WHITE);
  display.drawLine(86, baseEyeY - 18, 103, baseEyeY - 14, SSD1306_WHITE);

  int lEx = 42, rEx = 86;

  display.fillCircle(lEx, baseEyeY, 11, SSD1306_WHITE);
  display.fillCircle(rEx, baseEyeY, 11, SSD1306_WHITE);

  if (eyeH < 22) {
    int maskH = (22 - eyeH) / 2;
    display.fillRect(lEx-12, baseEyeY-12, 24, maskH, SSD1306_BLACK);
    display.fillRect(lEx-12, baseEyeY+12-maskH, 24, maskH, SSD1306_BLACK);
    display.fillRect(rEx-12, baseEyeY-12, 24, maskH, SSD1306_BLACK);
    display.fillRect(rEx-12, baseEyeY+12-maskH, 24, maskH, SSD1306_BLACK);
  }

  if (eyeH > 8) {
    display.fillCircle(lEx + 3, baseEyeY + pupilYShift, 4, SSD1306_BLACK);
    display.fillCircle(rEx - 3, baseEyeY + pupilYShift, 4, SSD1306_BLACK);
    display.fillCircle(lEx + 1, baseEyeY + pupilYShift - 1, 2, SSD1306_WHITE);
    display.fillCircle(rEx - 5, baseEyeY + pupilYShift - 1, 2, SSD1306_WHITE);
  }

  // animated mouth
  int mY = baseMouthY + mouthOffset;

  display.drawPixel(64, mY + 1, SSD1306_WHITE);
  display.drawLine(61, mY + 2, 63, mY + 2, SSD1306_WHITE);
  display.drawLine(65, mY + 2, 67, mY + 2, SSD1306_WHITE);
  display.drawPixel(60, mY, SSD1306_WHITE);
  display.drawPixel(68, mY, SSD1306_WHITE);

  display.drawPixel(22, baseEyeY + 10, SSD1306_WHITE);
  display.drawPixel(106, baseEyeY + 10, SSD1306_WHITE);

  drawExpression(baseEyeY, baseMouthY);

  if (expression == 0) {
    drawLeftHand(leftX, 46);
    drawRightHand(rightX, 46);
  }

  display.display();
}

// ---------------- HAND ANIMATION ----------------
void animateHands() {
  for (int i = 0; i < 38; i++) {
    if (leftX < 52) leftX++;
    if (rightX > 76) rightX--;

    tickEyes();
    drawFace();
    delay(20);
  }

  for (int i = 0; i < 38; i++) {
    if (leftX > 10) leftX--;
    if (rightX < 118) rightX++;

    tickEyes();
    drawFace();
    delay(15);
  }
}

// ---------------- BOOT FIXED ----------------
void drawCenteredBoldLine(const char *text, int y, uint8_t size)
{
  display.setTextSize(size);
  display.setTextColor(SSD1306_WHITE);

  int16_t x1, y1;
  uint16_t w, h;

  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  int x = (SCREEN_WIDTH - w) / 2;
  if (x < 0) x = 0;

  display.setCursor(x, y);
  display.print(text);
  display.setCursor(x + 1, y);
  display.print(text);
}

uint8_t getBestTextSize(const char *text, uint8_t maxSize)
{
  for (int8_t size = maxSize; size >= 1; size--)
  {
    display.setTextSize(size);

    int16_t x1, y1;
    uint16_t w, h;

    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

    if (w <= SCREEN_WIDTH) return size;
  }
  return 1;
}

void showBootScreen(const char *top, const char *bottom)
{
  display.clearDisplay();

  uint8_t topSize = getBestTextSize(top, 2);
  uint8_t bottomSize = getBestTextSize(bottom, 2);

  drawCenteredBoldLine(top, 6, topSize);
  drawCenteredBoldLine(bottom, 34, bottomSize);

  display.display();
}

void bootSequence()
{
  showBootScreen("Hi! I'm", "Fari"); delay(5000);
  showBootScreen("A clone Of", "Fariba"); delay(5000);
  showBootScreen("Powered BY", "DRAC Studios"); delay(5000);
  showBootScreen("Made By", "ARPON"); delay(5000);

  display.clearDisplay();
  display.display();
}

// ---------------- SETUP ----------------
void setup() {
  Wire.begin(20, 21);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  bootSequence();

  randomSeed(analogRead(0));
}

// ---------------- LOOP ----------------
void loop() {

  // mouth animation (Fariba only)
  if (expression == 0)
  {
    if (mouthDir) mouthOffset++;
    else mouthOffset--;

    if (mouthOffset > 2) mouthDir = false;
    if (mouthOffset < -1) mouthDir = true;
  }
  else
  {
    mouthOffset = 0;
  }

  tickEyes();
  drawFace();
  delay(15);

  unsigned long interval = (expression == 0) ? 10000 : 2000;

  if (millis() - lastExpressionChange > interval) {
    expression = random(0, 6);
    lastExpressionChange = millis();
  }

  if (expression == 0 && millis() - lastTrigger >= 2000) {
    animateHands();
    lastTrigger = millis();
  }
}