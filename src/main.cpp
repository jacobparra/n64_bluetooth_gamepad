#include <Arduino.h>
#include <Adafruit_ADS1015.h>

#include "BleGamepad.h"

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
static const uint8_t THRESHOLD = 100;

int aPinValue = 1;
int bPinValue = 1;
int zPinValue = 1;
int leftPinValue = 1;
int rightPinValue = 1;
int startPinValue = 1;

uint8_t dPadValue = HAT_CENTERED;
uint16_t xAxisValue = 0;
uint16_t yAxisValue = 0;
uint16_t zAxisValue = 0;
uint16_t rZAxisValue = 0;

BleGamepad bleGamepad("Nintento64 BLE", "Jacob Parra.", 100);
Adafruit_ADS1115 adc;

uint16_t readAxis(int axis)
{
  int reading = adc.readADC_SingleEnded(axis);
  reading = map(reading, 0, 26100, -2047, 2047);

  if (reading < -2047) {
    reading = -2047;
  }

  if (reading > 2047) {
    reading = 2047;
  }

  if (abs(reading) < THRESHOLD)
  {
    return 0;
  }
  return reading;
}

void handleButtonChange(int *currentPinValue, uint8_t pin, uint16_t bleButton)
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

void handleDPadChange()
{
  uint8_t newDPadValue = 0;
  if (digitalRead(D_PAD_UP_PIN) == LOW)
  {
    if (digitalRead(D_PAD_LEFT_PIN) == LOW)
    {
      newDPadValue = HAT_UP_LEFT;
    }
    else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
    {
      newDPadValue = HAT_UP_RIGHT;
    }
    else
    {
      newDPadValue = HAT_UP;
    }
  }
  else if (digitalRead(D_PAD_DOWN_PIN) == LOW)
  {
    if (digitalRead(D_PAD_LEFT_PIN) == LOW)
    {
      newDPadValue = HAT_DOWN_LEFT;
    }
    else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
    {
      newDPadValue = HAT_DOWN_RIGHT;
    }
    else
    {
      newDPadValue = HAT_DOWN;
    }
  }
  else if (digitalRead(D_PAD_LEFT_PIN) == LOW)
  {
    newDPadValue = HAT_LEFT;
  }
  else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
  {
    newDPadValue = HAT_RIGHT;
  }
  else
  {
    newDPadValue = HAT_CENTERED;
  }

  if (dPadValue == newDPadValue)
  {
    return;
  }
  dPadValue = newDPadValue;

  bleGamepad.setHat(dPadValue);
}

void handleCPadChange()
{
  uint16_t newZAxisValue = 0;
  uint16_t newRZAxisValue = 0;
  if (digitalRead(C_PAD_UP_PIN) == LOW)
  {
    newRZAxisValue = -2047;
  }
  else if (digitalRead(C_PAD_DOWN_PIN) == LOW)
  {
    newRZAxisValue = 2047;
  }
  else if (digitalRead(C_PAD_LEFT_PIN) == LOW)
  {
    newZAxisValue = -2047;
  }
  else if (digitalRead(C_PAD_RIGHT_PIN) == LOW)
  {
    newZAxisValue = 2047;
  }


  if (zAxisValue != newZAxisValue || rZAxisValue != newRZAxisValue) {
    zAxisValue = newZAxisValue;
    rZAxisValue = newRZAxisValue;
    bleGamepad.setRightAxes(zAxisValue, rZAxisValue);
  }
}

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

  pinMode(A_PIN, INPUT_PULLUP);
  pinMode(B_PIN, INPUT_PULLUP);
  pinMode(Z_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(START_PIN, INPUT_PULLUP);
}

void loop()
{
  handleButtonChange(&aPinValue, A_PIN, BUTTON_A);
  handleButtonChange(&bPinValue, B_PIN, BUTTON_X);
  handleButtonChange(&zPinValue, Z_PIN, BUTTON_THUMBL);
  handleButtonChange(&leftPinValue, LEFT_PIN, BUTTON_L1);
  handleButtonChange(&rightPinValue, RIGHT_PIN, BUTTON_R1);
  handleButtonChange(&startPinValue, START_PIN, BUTTON_START);

  handleDPadChange();
  handleCPadChange();

  uint16_t newXAxisValue = readAxis(X_AXIS);
  uint16_t newYAxisValue = readAxis(Y_AXIS);

  if (xAxisValue != newXAxisValue || yAxisValue != -newYAxisValue) {
    xAxisValue = newXAxisValue;
    yAxisValue = -newYAxisValue;
    bleGamepad.setLeftAxes(xAxisValue, yAxisValue);
  }

  bleGamepad.notify();

  delay(20);
}
