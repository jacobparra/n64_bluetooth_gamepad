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
static const uint8_t THRESHOLD = 8;

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

static signed char xAxisValue = 0;
static signed char yAxisValue = 0;

BleGamepad bleGamepad("Nintento64 BLE", "Jacob's Garage.", 100);
Adafruit_ADS1115 adc;

signed char readAxis(int axis)
{
  int reading = adc.readADC_SingleEnded(axis);
  reading = map(reading, 0, 26100, -127, 127);

  if (reading < -127) {
    reading = -127;
  }

  if (reading > 127) {
    reading = 127;
  }

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

void handleDPadChange()
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

  bleGamepad.setHat(dPadValue);
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

  pinMode(START_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(Z_PIN, INPUT_PULLUP);

  pinMode(A_PIN, INPUT_PULLUP);
  pinMode(B_PIN, INPUT_PULLUP);
}

void loop()
{
  handlePinChange(&cPadUpPinValue, C_PAD_UP_PIN, BUTTON_7);
  handlePinChange(&cPadDownPinValue, C_PAD_DOWN_PIN, BUTTON_8);
  handlePinChange(&cPadLeftPinValue, C_PAD_LEFT_PIN, BUTTON_9);
  handlePinChange(&cPadRightPinValue, C_PAD_RIGHT_PIN, BUTTON_10);
  handlePinChange(&startPinValue, START_PIN, BUTTON_3);
  handlePinChange(&leftPinValue, LEFT_PIN, BUTTON_4);
  handlePinChange(&rightPinValue, RIGHT_PIN, BUTTON_5);
  handlePinChange(&zPinValue, Z_PIN, BUTTON_6);
  handlePinChange(&aPinValue, A_PIN, BUTTON_1);
  handlePinChange(&bPinValue, B_PIN, BUTTON_2);

  handleDPadChange();

  signed char newXAxisValue = readAxis(X_AXIS);
  signed char newYAxisValue = readAxis(Y_AXIS);

  if (xAxisValue != newXAxisValue || yAxisValue != -newYAxisValue) {
    xAxisValue = newXAxisValue;
    yAxisValue = -newYAxisValue;
    bleGamepad.setAxes(xAxisValue, yAxisValue);
  }

  bleGamepad.notify();

  delay(10);
}
