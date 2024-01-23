#include "BLEDevice.h"

static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

BLEClient* pClient;
BLERemoteCharacteristic* pRemoteChar;
BLEAdvertisedDevice* myDevice;

static boolean connected = false;
static boolean doScan = false;

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) { connected = true; }
  void onDisconnect(BLEClient* pclient) { connected = false; }
};

void connectToServer() {
  Serial.print("Connecting to server: ");
  Serial.println(myDevice->getAddress().toString().c_str());

  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
  pClient->connect(myDevice);

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService) {
    pRemoteChar = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteChar) {
      if (pRemoteChar->canNotify()) {
        pRemoteChar->registerForNotify([](BLERemoteCharacteristic* pBLERemoteCharacteristic,
                                          uint8_t* pData,
                                          size_t length,
                                          bool isNotify) {
          uint32_t counter = pData[0];
          for (int i = 1; i < length; i++) {
            counter |= (pData[i] << (i * 8));
          }
          Serial.print("Characteristic (Notify) from server: ");
          Serial.println(counter);
        });
      }
      connected = true;
    } else {
      Serial.println("Characteristic not found on the server");
    }
  } else {
    Serial.println("Service not found on the server");
  }
}

void startBLEScan() {
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->start(5, false);
}

void processFoundDevices(BLEScanResults& foundDevices) {
  for (int i = 0; i < foundDevices.getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    if (device.haveServiceUUID() && device.isAdvertisingService(serviceUUID)) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(device.toString().c_str());

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(device);
      connectToServer();
      break;
    }
  }
}

void printConnectionStatus() {
  if (connected) {
    Serial.println("Connected!");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
}

void loop() {
  startBLEScan();

  BLEScan* pBLEScan = BLEDevice::getScan();
  BLEScanResults foundDevices = pBLEScan->start(5, false);

  processFoundDevices(foundDevices);
  printConnectionStatus();

  delay(1000);
}