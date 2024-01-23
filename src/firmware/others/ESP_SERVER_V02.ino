#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include <UbidotsEsp32Mqtt.h>

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

#define SSID "Inteli-welcome"        // "SHARE-RESIDENTE"   // "Inteli-welcome"
#define PASSWORD ""  // "Share@residente23" // ""

#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// UBIDOTS VARIABLES

  const char *UBIDOTS_TOKEN = "BBUS-mXOTtUc6rfcbbzq1AwPkLHlNCieapy";
  const char *DEVICE_LABEL = "ESP-SERVER-Sala01";        // Replace with the device label to subscribe to
  const char *WIFI_SSID = "Inteli-welcome";      // Put here your Wi-Fi SSID
  const char *WIFI_PASS = "";      // Put here your Wi-Fi password
  const char *VARIABLE_LABEL1 = "conexaoBle";       // Replace with your variable label to subscribe to


  const int PUBLISH_FREQUENCY = 1000; // Update rate in milliseconds
  unsigned long timer;
  Ubidots ubidots(UBIDOTS_TOKEN);

//


void connectToWifi() {
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");
}


// UBIDOTS FUNCTIONS:

  void callback(char *topic, byte *payload, unsigned int length){
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
  }

  void ubidotsMainConnection(){

    ubidots.setDebug(true);  // uncomment this to make debug messages available
    ubidots.connectToWifi(SSID, PASSWORD);
    ubidots.setCallback(callback);
    ubidots.setup();
    ubidots.reconnect();
    Serial.println("Connected to Ubidots via Wi-Fi");
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL1);
  }
//


// BLE FUNCTIONS:
  // Callback function that is called whenever a client is connected or disconnected
  class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) { deviceConnected = true; };
    void onDisconnect(BLEServer* pServer) { deviceConnected = false; };
  };

  void setupBLEService() {
    BLEService* pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();
  }

  void startBLEAdvertising() {
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);

    BLEDevice::startAdvertising();
  }

  void handleBLEConnections() {
    if (!deviceConnected && oldDeviceConnected) {
      delay(500);
      startBLEAdvertising();
      Serial.println("Start advertising");
      oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
    }
  }

  void updateBLECharacteristic() {
    pCharacteristic->setValue(value);
    pCharacteristic->notify();
  }
//


void setup() {
  Serial.begin(115200);

  connectToWifi();

  ubidotsMainConnection();

  BLEDevice::init("ESP32"); // Create the BLE Device

  pServer = BLEDevice::createServer(); // Create the BLE Server
  pServer->setCallbacks(new MyServerCallbacks());

  setupBLEService();
  startBLEAdvertising();

  Serial.println("Waiting for a client connection to notify...");
}

void loop() {
  handleBLEConnections();


  if(millis() - timer > PUBLISH_FREQUENCY){ // Ubidots publishing timer

    // Verifies if the ESP-SERVER connected to the Delphlocator's ESP through BLE
    if (deviceConnected == true) {
      Serial.print("CONEXÃO BLE FEITA COM SUCESSO");

      ubidots.add(VARIABLE_LABEL1, 1); // Pubblish "1" to sinalize that an BLE connection was made
      delay(250);
      ubidots.publish(DEVICE_LABEL);
      delay(250);

      updateBLECharacteristic();

      delay(1000);
    } else {
      ubidots.add(VARIABLE_LABEL1, 0); // // Pubblish "0" to sinalize that no BLE connection is made
      ubidots.publish(DEVICE_LABEL);
      };

    timer = millis();

  }
}