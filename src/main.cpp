#include <Arduino.h>
#include <BleGamepad.h>
#include "Adafruit_ADS1015.h"

const int BUTTON_PIN = 32;
const int X_AXIS = 0;
const int Y_AXIS = 1;
const int THRESHOLD = 5;

BleGamepad bleGamepad;
Adafruit_ADS1115 adc;

int buttonState = 0;
int xReading;
int yReading;

int readAxis(int axis)
{
  int reading = adc.readADC_SingleEnded(axis);
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

  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
}

void loop()
{
  buttonState = digitalRead(BUTTON_PIN);
  xReading = readAxis(X_AXIS);
  yReading = readAxis(Y_AXIS);

  Serial.print("X: ");
  Serial.print(xReading);
  Serial.print(" | Y: ");
  Serial.println(yReading);

  if (bleGamepad.isConnected())
  {
    if (buttonState == HIGH)
    {
      Serial.println("HIGH");
      bleGamepad.press(BUTTON_1);
    }
    else
    {
      Serial.println("LOW");
      bleGamepad.release(BUTTON_1);
    }
    bleGamepad.setAxes(xReading, yReading, 0, 0, 0, 0, DPAD_CENTERED);
  }

  delay(20);
}
