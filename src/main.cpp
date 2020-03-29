#include <Arduino.h>
#include <BleGamepad.h>
#include "Adafruit_ADS1015.h"

static const uint8_t D_PAD_UP_PIN = 27;
static const uint8_t D_PAD_DOWN_PIN = 25;
static const uint8_t D_PAD_LEFT_PIN = 12;
static const uint8_t D_PAD_RIGHT_PIN = 32;

static const uint8_t C_PAD_UP_PIN = 19;
static const uint8_t C_PAD_DOWN_PIN = 23;
static const uint8_t C_PAD_LEFT_PIN = 5;
static const uint8_t C_PAD_RIGHT_PIN = 26;

static const uint8_t START_PIN = 16;
static const uint8_t LEFT_PIN = 0;
static const uint8_t RIGHT_PIN = 33;
static const uint8_t Z_PIN = 4;

static const uint8_t A_PIN = 17;
static const uint8_t B_PIN = 18;

static const uint8_t X_AXIS = 0;
static const uint8_t Y_AXIS = 1;
static const uint8_t THRESHOLD = 5;

static signed char dPadValue = DPAD_CENTERED;

static int cPadUpPinValue = 1;
static int cPadDownPinValue = 1;
static int cPadLeftPinValue = 1;
static int cPadRightPinValue = 1;

static int startPinValue = 1;
static int leftPinValue = 1;
static int rightPinValue = 1;
static int zPinValue = 1;

static int aPinValue = 1;
static int bPinValue = 1;

static int xAxisValue = 0;
static int yAxisValue = 0;

BleGamepad bleGamepad;
Adafruit_ADS1115 adc(0x48);

int readAxis(int axis)
{
  int reading = adc.readADC_SingleEnded(axis);
  Serial.print("reading");
  reading = map(reading, 0, 26075, -127, 127);

  if (abs(reading) < THRESHOLD)
  {
    return 0;
  }
  return reading;
}

void handlePinChange(int *currentPinValue, uint8_t pin, uint16_t bleButton)
{
  int newPinValue = digitalRead(pin);
  if (*currentPinValue == newPinValue)
  {
    return;
  }
  *currentPinValue = newPinValue;

  if (*currentPinValue == LOW)
  {
    bleGamepad.press(bleButton);
  }
  else
  {
    bleGamepad.release(bleButton);
  }
}

void IRAM_ATTR dPadChange()
{
  signed char newDPadValue = 0;
  if (digitalRead(D_PAD_UP_PIN) == LOW)
  {
    if (digitalRead(D_PAD_LEFT_PIN) == LOW)
    {
      newDPadValue = DPAD_UP_LEFT;
    }
    else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
    {
      newDPadValue = DPAD_UP_RIGHT;
    }
    else
    {
      newDPadValue = DPAD_UP;
    }
  }
  else if (digitalRead(D_PAD_DOWN_PIN) == LOW)
  {
    if (digitalRead(D_PAD_LEFT_PIN) == LOW)
    {
      newDPadValue = DPAD_DOWN_LEFT;
    }
    else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
    {
      newDPadValue = DPAD_DOWN_RIGHT;
    }
    else
    {
      newDPadValue = DPAD_DOWN;
    }
  }
  else if (digitalRead(D_PAD_LEFT_PIN) == LOW)
  {
    newDPadValue = DPAD_LEFT;
  }
  else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
  {
    newDPadValue = DPAD_RIGHT;
  }
  else
  {
    newDPadValue = DPAD_CENTERED;
  }

  if (dPadValue == newDPadValue)
  {
    return;
  }
  dPadValue = newDPadValue;

  bleGamepad.setAxes(0, 0, 0, 0, 0, 0, dPadValue);
}

void IRAM_ATTR cPadUpPinChange() { handlePinChange(&cPadUpPinValue, C_PAD_UP_PIN, BUTTON_7); }
void IRAM_ATTR cPadDownPinChange() { handlePinChange(&cPadDownPinValue, C_PAD_DOWN_PIN, BUTTON_8); }
void IRAM_ATTR cPadLeftPinChange() { handlePinChange(&cPadLeftPinValue, C_PAD_LEFT_PIN, BUTTON_9); }
void IRAM_ATTR cPadRightPinChange() { handlePinChange(&cPadRightPinValue, C_PAD_RIGHT_PIN, BUTTON_10); }

void IRAM_ATTR startPinChange() { handlePinChange(&startPinValue, START_PIN, BUTTON_3); }
void IRAM_ATTR leftPinChange() { handlePinChange(&leftPinValue, LEFT_PIN, BUTTON_4); }
void IRAM_ATTR rightPinChange() { handlePinChange(&rightPinValue, RIGHT_PIN, BUTTON_5); }
void IRAM_ATTR zPinChange() { handlePinChange(&zPinValue, Z_PIN, BUTTON_6); }

void IRAM_ATTR aPinChange() { handlePinChange(&aPinValue, A_PIN, BUTTON_1); }
void IRAM_ATTR bPinChange() { handlePinChange(&bPinValue, B_PIN, BUTTON_2); }

void setup()
{
  Serial.begin(115200);

  bleGamepad.begin();

  adc.setGain(GAIN_ONE);
  adc.begin();

  pinMode(D_PAD_UP_PIN, INPUT_PULLUP);
  pinMode(D_PAD_DOWN_PIN, INPUT_PULLUP);
  pinMode(D_PAD_LEFT_PIN, INPUT_PULLUP);
  pinMode(D_PAD_RIGHT_PIN, INPUT_PULLUP);

  pinMode(C_PAD_UP_PIN, INPUT_PULLUP);
  pinMode(C_PAD_DOWN_PIN, INPUT_PULLUP);
  pinMode(C_PAD_LEFT_PIN, INPUT_PULLUP);
  pinMode(C_PAD_RIGHT_PIN, INPUT_PULLUP);

  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(Z_PIN, INPUT_PULLUP);

  pinMode(A_PIN, INPUT_PULLUP);
  pinMode(B_PIN, INPUT_PULLUP);

  attachInterrupt(D_PAD_UP_PIN, dPadChange, CHANGE);
  attachInterrupt(D_PAD_DOWN_PIN, dPadChange, CHANGE);
  attachInterrupt(D_PAD_LEFT_PIN, dPadChange, CHANGE);
  attachInterrupt(D_PAD_RIGHT_PIN, dPadChange, CHANGE);

  attachInterrupt(C_PAD_UP_PIN, cPadUpPinChange, CHANGE);
  attachInterrupt(C_PAD_DOWN_PIN, cPadDownPinChange, CHANGE);
  attachInterrupt(C_PAD_LEFT_PIN, cPadLeftPinChange, CHANGE);
  attachInterrupt(C_PAD_RIGHT_PIN, cPadRightPinChange, CHANGE);

  attachInterrupt(START_PIN, startPinChange, CHANGE);
  attachInterrupt(LEFT_PIN, leftPinChange, CHANGE);
  attachInterrupt(RIGHT_PIN, rightPinChange, CHANGE);
  attachInterrupt(Z_PIN, zPinChange, CHANGE);

  attachInterrupt(A_PIN, aPinChange, CHANGE);
  attachInterrupt(B_PIN, bPinChange, CHANGE);
}

void loop()
{
  // xAxisValue = 0 = readAxis(X_AXIS);
  // yAxisValue = 0 = readAxis(Y_AXIS);

  // Serial.print("X: ");
  // Serial.print(xAxisValue = 0);
  // Serial.print(" | Y: ");

  // if (bleGamepad.isConnected())
  // {
  //   bleGamepad.setAxes(xAxisValue = 0, yAxisValue = 0, 0, 0, 0, 0, DPAD_CENTERED);
  // }

  delay(20);
}
