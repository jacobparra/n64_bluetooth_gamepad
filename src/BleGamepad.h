#ifndef ESP32_BLE_GAMEPAD_H
#define ESP32_BLE_GAMEPAD_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "BleConnectionStatus.h"
#include "BLEHIDDevice.h"
#include "BLECharacteristic.h"

#define BUTTON_A      1     // B0000000000000001
#define BUTTON_B      2     // B0000000000000010
#define BUTTON_X      4     // B0000000000000100
#define BUTTON_Y      8     // B0000000000001000
#define BUTTON_L1     16    // B0000000000010000
#define BUTTON_R1     32    // B0000000000100000
#define BUTTON_THUMBL 64    // B0000000001000000
#define BUTTON_THUMBR 128   // B0000000010000000
#define BUTTON_START  256   // B0000000100000000
#define BUTTON_BACK   512   // B0000001000000000
#define BUTTON_HOME   1024  // B0000010000000000

#define HAT_UP         0   // B00000000
#define HAT_UP_RIGHT   16  // B00010000
#define HAT_RIGHT 		 32  // B00100000
#define HAT_DOWN_RIGHT 48  // B00110000
#define HAT_DOWN 		   64  // B01000000
#define HAT_DOWN_LEFT  80  // B01010000
#define HAT_LEFT       96  // B01100000
#define HAT_UP_LEFT    112 // B01110000
#define HAT_CENTERED   128 // B10000000

class BleGamepad {
private:
  uint16_t buttons;
  uint8_t hat;
  uint16_t xAxis;
  uint16_t yAxis;
  uint16_t zAxis;
  uint16_t rZAxis;
  uint8_t rXAxis;
  uint8_t rYAxis;
  uint8_t message[10];
  BleConnectionStatus* connectionStatus;
  BLEHIDDevice* hid;
  BLECharacteristic* inputGamepad;
  static void taskServer(void* pvParameter);
public:
  BleGamepad(std::string deviceName = "ESP32 BLE Gamepad", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
  void begin(void);
  void end(void);
  void notify();
  void setLeftAxes(uint16_t x, uint16_t y, bool autoNotify = true);
  void setRightAxes(uint16_t z, uint16_t rZ, bool autoNotify = true);
  void setLeftTrigger(uint8_t rX, bool autoNotify = true);
  void setRightTrigger(uint8_t rY, bool autoNotify = true);
  void setHat(uint8_t h = HAT_CENTERED, bool autoNotify = true);
  void press(uint16_t b = BUTTON_A, bool autoNotify = true);
  void release(uint16_t b = BUTTON_A, bool autoNotify = true);
  bool isPressed(uint16_t b = BUTTON_A);
  bool isConnected(void);
  void setBatteryLevel(uint8_t level);
  uint8_t batteryLevel;
  std::string deviceManufacturer;
  std::string deviceName;
protected:
  virtual void onStarted(BLEServer *pServer) { };
};

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_GAMEPAD_H
