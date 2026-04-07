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

// ---------------- EYE BLINK ----------------
void tickEyes()
{
  eyeFrame++;
  int phase = eyeFrame % 120;

  if (phase < 100)
  {
    eyeH = 22;
  }
  else if (phase < 110)
  {
    eyeH = 22 - ((phase - 100) * 2.2);
  }
  else
  {
    eyeH = (phase - 110) * 2.2;
  }
}

// ---------------- HANDS ----------------
void drawLeftHand(int x, int y)
{
  display.fillRoundRect(x, y + 5, 12, 8, 2, SSD1306_WHITE);
  display.fillRoundRect(x + 8, y + 5, 12, 3, 1, SSD1306_WHITE);
  display.fillRoundRect(x + 2, y + 2, 5, 4, 2, SSD1306_WHITE);
}

void drawRightHand(int x, int y)
{
  display.fillRoundRect(x - 12, y + 5, 12, 8, 2, SSD1306_WHITE);
  display.fillRoundRect(x - 20, y + 5, 12, 3, 1, SSD1306_WHITE);
  display.fillRoundRect(x - 7, y + 2, 5, 4, 2, SSD1306_WHITE);
}

// ---------------- FACE ----------------
void drawFace()
{
  display.clearDisplay();

  int baseEyeY = 28 + eyeYOffset;
  int baseMouthY = 46;

  // eyebrows
  display.drawLine(25, baseEyeY - 14, 42, baseEyeY - 18, SSD1306_WHITE);
  display.drawLine(86, baseEyeY - 18, 103, baseEyeY - 14, SSD1306_WHITE);

  int lEx = 42, rEx = 86, eyeR = 11;

  // eyes
  display.fillCircle(lEx, baseEyeY, eyeR, SSD1306_WHITE);
  display.fillCircle(rEx, baseEyeY, eyeR, SSD1306_WHITE);

  // blink mask
  if (eyeH < 22)
  {
    int maskH = (22 - eyeH) / 2;
    display.fillRect(lEx - 12, baseEyeY - 12, 24, maskH, SSD1306_BLACK);
    display.fillRect(lEx - 12, baseEyeY + 12 - maskH, 24, maskH, SSD1306_BLACK);
    display.fillRect(rEx - 12, baseEyeY - 12, 24, maskH, SSD1306_BLACK);
    display.fillRect(rEx - 12, baseEyeY + 12 - maskH, 24, maskH, SSD1306_BLACK);
  }

  // pupils
  if (eyeH > 8)
  {
    display.fillCircle(lEx + 3, baseEyeY + pupilYShift, 4, SSD1306_BLACK);
    display.fillCircle(rEx - 3, baseEyeY + pupilYShift, 4, SSD1306_BLACK);
    display.fillCircle(lEx + 1, baseEyeY + pupilYShift - 1, 2, SSD1306_WHITE);
    display.fillCircle(rEx - 5, baseEyeY + pupilYShift - 1, 2, SSD1306_WHITE);
  }

  // mouth
  display.drawPixel(64, baseMouthY + 1, SSD1306_WHITE);
  display.drawLine(61, baseMouthY + 2, 63, baseMouthY + 2, SSD1306_WHITE);
  display.drawLine(65, baseMouthY + 2, 67, baseMouthY + 2, SSD1306_WHITE);
  display.drawPixel(60, baseMouthY, SSD1306_WHITE);
  display.drawPixel(68, baseMouthY, SSD1306_WHITE);

  // cheek dots
  display.drawPixel(22, baseEyeY + 10, SSD1306_WHITE);
  display.drawPixel(106, baseEyeY + 10, SSD1306_WHITE);

  // hands
  drawLeftHand(leftX, 46);
  drawRightHand(rightX, 46);

  display.display();
}

// ---------- STARTUP SEQUENCE ----------
void drawCenteredBoldLine(const char *text, int y, uint8_t size)
{
  int16_t textWidth = strlen(text) * 6 * size;
  int16_t x = (SCREEN_WIDTH - textWidth) / 2;
  if (x < 0)
    x = 0;

  display.setTextSize(size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x, y);
  display.print(text);
  display.setCursor(x + 1, y);
  display.print(text);
}

uint8_t getBestTextSize(const char *text, uint8_t maxSize)
{
  for (int8_t size = maxSize; size >= 1; size--)
  {
    int16_t textWidth = strlen(text) * 6 * size;
    if (textWidth <= SCREEN_WIDTH)
    {
      return size;
    }
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
  showBootScreen("Hi! I'm", "Fari");
  delay(5000);

  showBootScreen("A clone Of", "Fariba");
  delay(5000);

  showBootScreen("Powered BY", "DRAC Studios");
  delay(5000);

  showBootScreen("By", "ARPON");
  delay(5000);

  display.clearDisplay();
  display.display();
}

// ---------------- ANIMATION ----------------
void animateHands()
{

  // move in
  for (int i = 0; i < 38; i++)
  {
    if (leftX < 52)
      leftX++;
    if (rightX > 76)
      rightX--;

    if (i % 8 == 0)
    {
      eyeYOffset--;
      pupilYShift--;
    }

    if (eyeYOffset < -6)
      eyeYOffset = -6;
    if (pupilYShift < -5)
      pupilYShift = -5;

    tickEyes();
    drawFace();
    delay(20);
  }

  delay(1500);

  // move out
  for (int i = 0; i < 38; i++)
  {
    if (leftX > 10)
      leftX--;
    if (rightX < 118)
      rightX++;

    if (i % 10 == 0)
    {
      eyeYOffset++;
      pupilYShift++;
    }

    if (eyeYOffset > 0)
      eyeYOffset = 0;
    if (pupilYShift > -2)
      pupilYShift = -2;

    tickEyes();
    drawFace();
    delay(15);
  }
}

// ---------------- SETUP ----------------
void setup()
{
  Wire.begin(20, 21); // SDA = 20, SCL = 21 (your config)

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    for (;;)
      ;
  }

  display.clearDisplay();
  display.display();

  bootSequence();
}

// ---------------- LOOP ----------------
void loop()
{
  tickEyes();
  drawFace();
  delay(15);

  if (millis() - lastTrigger >= 2000)
  {
    animateHands();
    lastTrigger = millis();
  }
}