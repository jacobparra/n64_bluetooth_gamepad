#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "sdkconfig.h"

#include "BleConnectionStatus.h"
#include "BleGamepad.h"

#include "esp32-hal-log.h"

static const uint8_t _hidReportDescriptor[] = {
  USAGE_PAGE(1),       0x01,        // Usage Page (Generic Desktop)
  USAGE(1),            0x05,        // Usage (Gamepad)
  COLLECTION(1),       0x01,        // Collection (Application)
  REPORT_ID(1),        0x01,        //   Report ID (1)
  // Buttons - 11 bits
  USAGE_PAGE(1),       0x09,        //   Usage Page (Button)
  USAGE(2),            0x01, 0x00,  //   Usage (Vendor Usage 0x01) - Button A
  USAGE(2),            0x02, 0x00,  //   Usage (Vendor Usage 0x02) - Button B
  USAGE(2),            0x04, 0x00,  //   Usage (Vendor Usage 0x04) - Button X
  USAGE(2),            0x05, 0x00,  //   Usage (Vendor Usage 0x05) - Button Y
  USAGE(2),            0x07, 0x00,  //   Usage (Vendor Usage 0x07) - Button L shoulder
  USAGE(2),            0x08, 0x00,  //   Usage (Vendor Usage 0x08) - Button R shoulder
  USAGE(2),            0x0e, 0x00,  //   Usage (Vendor Usage 0x0e) - Button L thumb
  USAGE(2),            0x0f, 0x00,  //   Usage (Vendor Usage 0x0f) - Button R thumb
  USAGE(2),            0x0c, 0x00,  //   Usage (Vendor Usage 0x0c) - Button Start
  USAGE_PAGE(1),       0x0c,        //   Usage Page (Consumer Devices)
  USAGE(2),            0x24, 0x02,  //   Usage (AC Back)
  USAGE(2),            0x23, 0x02,  //   Usage (AC Home)
  LOGICAL_MINIMUM(1),  0x00,        //   Logical Minimum (0)
  LOGICAL_MAXIMUM(1),  0x01,        //   Logical Maximum (1)
  REPORT_SIZE(1),      0x01,        //   Report Size (1)
  REPORT_COUNT(1),     0x0b,        //   Report Count (11)
  HIDINPUT(1),         0x02,        //   Input (Data,Var,Abs)
  // Padding - 1 bit
  REPORT_SIZE(1),      0x01,        //   Report Size (1)
  REPORT_COUNT(1),     0x01,        //   Report Count (1)
  HIDINPUT(1),         0x03,        //   Input (Cnst,Var,Abs)
  // Hat switch - 4 bits
  USAGE_PAGE(1),       0x01,        //   Usage Page (Generic Desktop)
  USAGE(1),            0x39,        //   Usage (Hat switch)
  LOGICAL_MAXIMUM(1),  0x07,        //   Logical Maximum (7)
  PHYSICAL_MAXIMUM(2), 0x3b, 0x01,  //   Physical Maximum (315)
  UNIT(2),             0x14, 0x00,  //   Unit (Degrees,EngRotation)
  REPORT_SIZE(1),      0x04,        //   Report Size (4)
  REPORT_COUNT(1),     0x01,        //   Report Count (1)
  HIDINPUT(1),         0x42,        //   Input (Data,Var,Abs,Null)
  UNIT(2),             0x00, 0x00,  //   Unit (None)
  // 4 joystick axis (analogs) - 48 bits (6 bytes)
  USAGE(1),            0x01,        //   Usage (Pointer)
  COLLECTION(1),       0x00,        //   Collection (Physical)
  USAGE(1),            0x30,        //     Usage (X)
  USAGE(1),            0x31,        //     Usage (Y)
  USAGE(1),            0x32,        //     Usage (Z)
  USAGE(1),            0x35,        //     Usage (Rz)
  LOGICAL_MINIMUM(2),  0x01, 0xf8,  //     Logical Minimum (-2047)
  LOGICAL_MAXIMUM(2),  0xff, 0x07,  //     Logical Maximum (2047)
  PHYSICAL_MINIMUM(2), 0x01, 0xf8,  //     Physical Minimum (-2047)
  PHYSICAL_MAXIMUM(2), 0xff, 0x07,  //     Physical Maximum (2047)
  REPORT_SIZE(1),      0x0c,        //     Report Size (12)
  REPORT_COUNT(1),     0x04,        //     Report Count (4)
  HIDINPUT(1),         0x02,        //     Input (Data,Var,Abs)
  // 2 trigger axis (analogs) - 16 bits (2 bytes)
  USAGE_PAGE(1),       0x02,        //     Usage Page (Simulation Controls)
  USAGE(1),            0xc5,        //     Usage (Brake)
  USAGE(1),            0xc4,        //     Usage (Accelerator)
  LOGICAL_MINIMUM(1),  0x00,        //     Logical Minimum (0)
  LOGICAL_MAXIMUM(1),  0xff,        //     Logical Maximum (255)
  PHYSICAL_MINIMUM(1), 0x00,        //     Physical Minimum (0)
  PHYSICAL_MAXIMUM(1), 0xff,        //     Physical Maximum (255)
  REPORT_SIZE(1),      0x08,        //     Report Size (8)
  REPORT_COUNT(1),     0x02,        //     Report Count (2)
  HIDINPUT(1),         0x02,        //     Input (Data,Var,Abs)
  END_COLLECTION(0),                //   End Collection
  END_COLLECTION(0)                 // End Collection
};

BleGamepad::BleGamepad(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) :
  buttons(0),
  hat(HAT_CENTERED),
  xAxis(0),
  yAxis(0),
  zAxis(0),
  rZAxis(0),
  rXAxis(0),
  rYAxis(0),
  message{0,0,0,0,0,0,0,0,0,0},
  hid(0)
{
  this->deviceName = deviceName;
  this->deviceManufacturer = deviceManufacturer;
  this->batteryLevel = batteryLevel;
  this->connectionStatus = new BleConnectionStatus();
}

void BleGamepad::begin(void)
{
  xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleGamepad::end(void)
{
}

void BleGamepad::notify()
{
  if (!this->isConnected())
    return;

  uint8_t m[10];
  m[0] = buttons;
  m[1] = hat | (buttons >> 8);
  m[2] = xAxis;
  m[3] = (xAxis >> 8) | (yAxis << 4);
  m[4] = yAxis >> 4;
  m[5] = zAxis;
  m[6] = (zAxis >> 8) | (rZAxis << 4);
  m[7] = rZAxis >> 4;
  m[8] = rXAxis;
  m[9] = rYAxis;

  if (message[0] != m[0] || message[1] != m[1] || message[2] != m[2] || message[3] != m[3] ||
      message[4] != m[4] || message[5] != m[5] || message[6] != m[6] || message[7] != m[7] ||
      message[8] != m[8] || message[9] != m[9])
  {
    message[0] = m[0];
    message[1] = m[1];
    message[2] = m[2];
    message[3] = m[3];
    message[4] = m[4];
    message[5] = m[5];
    message[6] = m[6];
    message[7] = m[7];
    message[8] = m[8];
    message[9] = m[9];
    this->inputGamepad->setValue(message, sizeof(message));
    this->inputGamepad->notify();
  }
}

void BleGamepad::setLeftAxes(uint16_t x, uint16_t y, bool autoNotify)
{
  xAxis = x;
  yAxis = y;
}

void BleGamepad::setRightAxes(uint16_t z, uint16_t rZ, bool autoNotify)
{
  zAxis = z;
  rZAxis = rZ;
}

void BleGamepad::setLeftTrigger(uint8_t rX, bool autoNotify)
{
  rXAxis = rX;
}

void BleGamepad::setRightTrigger(uint8_t rY, bool autoNotify)
{
  rYAxis = rY;
}

void BleGamepad::setHat(uint8_t h, bool autoNotify)
{
  hat = h;
}

void BleGamepad::press(uint16_t b, bool autoNotify)
{
  buttons = buttons | b;
}

void BleGamepad::release(uint16_t b, bool autoNotify)
{
  buttons = buttons & ~b;
}

bool BleGamepad::isPressed(uint16_t b)
{
  if ((b & buttons) > 0)
    return true;
  return false;
}

bool BleGamepad::isConnected(void) {
  return this->connectionStatus->connected;
}

void BleGamepad::setBatteryLevel(uint8_t level) {
  this->batteryLevel = level;
  if (hid != 0)
    this->hid->setBatteryLevel(this->batteryLevel);
}

void BleGamepad::taskServer(void* pvParameter) {
  BleGamepad* BleGamepadInstance = (BleGamepad *) pvParameter; //static_cast<BleGamepad *>(pvParameter);
  BLEDevice::init(BleGamepadInstance->deviceName);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(BleGamepadInstance->connectionStatus);

  BleGamepadInstance->hid = new BLEHIDDevice(pServer);
  BleGamepadInstance->inputGamepad = BleGamepadInstance->hid->inputReport(1); // <-- input REPORTID from report map
  BleGamepadInstance->connectionStatus->inputGamepad = BleGamepadInstance->inputGamepad;

  BleGamepadInstance->hid->manufacturer()->setValue(BleGamepadInstance->deviceManufacturer);

  BleGamepadInstance->hid->pnp(0x01,0x02e5,0xabcd,0x0110);
  BleGamepadInstance->hid->hidInfo(0x00,0x01);

  BLESecurity *pSecurity = new BLESecurity();

  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  BleGamepadInstance->hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  BleGamepadInstance->hid->startServices();

  BleGamepadInstance->onStarted(pServer);

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_GAMEPAD);
  pAdvertising->addServiceUUID(BleGamepadInstance->hid->hidService()->getUUID());
  pAdvertising->start();
  BleGamepadInstance->hid->setBatteryLevel(BleGamepadInstance->batteryLevel);

  log_d("Advertising started!");
  vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}
