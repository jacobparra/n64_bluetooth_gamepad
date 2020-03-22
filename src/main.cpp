#include <Arduino.h>
#include <BleGamepad.h>
#include "Adafruit_ADS1015.h"

const int D_PAD_UP_PIN = 27;
const int D_PAD_DOWN_PIN = 25;
const int D_PAD_LEFT_PIN = 12;
const int D_PAD_RIGHT_PIN = 32;

const int C_PAD_UP_PIN = 19;
const int C_PAD_DOWN_PIN = 23;
const int C_PAD_LEFT_PIN = 5;
const int C_PAD_RIGHT_PIN = 26;

const int START_PIN = 16;
const int LEFT_PIN = 0;
const int RIGHT_PIN = 33;
const int Z_PIN = 4;

const int A_PIN = 17;
const int B_PIN = 18;

const int X_AXIS = 0;
const int Y_AXIS = 1;
const int THRESHOLD = 5;

BleGamepad bleGamepad;
Adafruit_ADS1115 adc(0x48);

int xReading;
int yReading;

int readAxis(int axis)
{
  int reading = adc.readADC_SingleEnded(axis);
  Serial.print("reading");
  Serial.println(reading);
  reading = map(reading, 0, 26075, -127, 127);

  if (abs(reading) < THRESHOLD)
  {
    return 0;
  }
  return reading;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

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
  if (digitalRead(D_PAD_UP_PIN) == LOW)
  {
    if (digitalRead(D_PAD_LEFT_PIN) == LOW)
    {
      Serial.println("D_PAD_UP_LEFT_PIN");
    }
    else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
    {
      Serial.println("D_PAD_UP_RIGHT_PIN");
    }
    else
    {
      Serial.println("D_PAD_UP_PIN");
    }
  }
  else if (digitalRead(D_PAD_DOWN_PIN) == LOW)
  {
    if (digitalRead(D_PAD_LEFT_PIN) == LOW)
    {
      Serial.println("D_PAD_DOWN_LEFT_PIN");
    }
    else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
    {
      Serial.println("D_PAD_DOWN_RIGHT_PIN");
    }
    else
    {
      Serial.println("D_PAD_DOWN_PIN");
    }
  }
  else if (digitalRead(D_PAD_LEFT_PIN) == LOW)
  {
    Serial.println("D_PAD_LEFT_PIN");
  }
  else if (digitalRead(D_PAD_RIGHT_PIN) == LOW)
  {
    Serial.println("D_PAD_RIGHT_PIN");
  }
  else
  {
    // Serial.println("D_PAD_CENTERED");
  }

  if (digitalRead(C_PAD_UP_PIN) == LOW)
  {
    Serial.println("C_PAD_UP_PIN");
  }
  if (digitalRead(C_PAD_DOWN_PIN) == LOW)
  {
    Serial.println("C_PAD_DOWN_PIN");
  }
  if (digitalRead(C_PAD_LEFT_PIN) == LOW)
  {
    Serial.println("C_PAD_LEFT_PIN");
  }
  if (digitalRead(C_PAD_RIGHT_PIN) == LOW)
  {
    Serial.println("C_PAD_RIGHT_PIN");
  }

  if (digitalRead(START_PIN) == LOW)
  {
    Serial.println("START_PIN");
  }
  if (digitalRead(LEFT_PIN) == LOW)
  {
    Serial.println("LEFT_PIN");
  }
  if (digitalRead(RIGHT_PIN) == LOW)
  {
    Serial.println("RIGHT_PIN");
  }
  if (digitalRead(Z_PIN) == LOW)
  {
    Serial.println("Z_PIN");
  }

  if (digitalRead(A_PIN) == LOW)
  {
    Serial.println("A_PIN");
  }
  if (digitalRead(B_PIN) == LOW)
  {
    Serial.println("B_PIN");
  }

  // xReading = readAxis(X_AXIS);
  // yReading = readAxis(Y_AXIS);

  // Serial.print("X: ");
  // Serial.print(xReading);
  // Serial.print(" | Y: ");
  // Serial.println(yReading);

  // if (bleGamepad.isConnected())
  // {
  //   bleGamepad.setAxes(xReading, yReading, 0, 0, 0, 0, DPAD_CENTERED);
  // }

  delay(20);
}
