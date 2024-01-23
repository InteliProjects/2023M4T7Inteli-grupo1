#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "BLEDevice.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
HTTPClient http;

#define R_LED 4                   // Red LED on pin 4
#define G_LED 5                   // Green LED on pin 5
#define B_LED 18                  // Blue LED on pin 18
#define A_EMER_BUTTON_PIN 12      // Button on pin 12 for ATTENDING emergencies
#define G_EMER_BUTTON_PIN 13      // Button for GENERATING emergencies
#define BUZZER_PIN 19             // Buzzer on pin 19
#define PAGER_NAME "pager1"       // Defining the PAGER_NAME

#define SSID "SHARE-RESIDENTE"        // "SHARE-RESIDENTE"   // "Inteli-welcome"
#define PASSWORD "Share@residente23"  // "Share@residente23" // ""
#define SERVER_ADDRESS "https://testesp32.maurodas.repl.co/update_local_patient"

static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

BLEClient* pClient;
BLERemoteCharacteristic* pRemoteChar;
BLEAdvertisedDevice* myDevice;

static boolean connected = false;
static boolean doScan = false;

void connectToWifi() {
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");
}

void httpRequest() {
  http.begin(SERVER_ADDRESS);

  String local = "TesteDaMadrugas";
  String jsonPayload = "{\"local\":\"" + local + "\",\"name\":\"" + "Fulanito" + "\"}";

  http.addHeader("Content-Type", "application/json");
  int res = http.POST(jsonPayload);

  if (res > 0) {
    String payload = http.getString();
    Serial.println("HTTP Response Code:");
    Serial.println(payload);
  } else {
    Serial.println("Error on HTTP request:");
    Serial.println(res);
  }

  http.end();
}

void generateEmergency() {
  String emerg = "Dor";
  String local = "Sala 01";

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Emerg: " + emerg); // Emerg = Emergency
  lcd.setCursor(0, 1);
  lcd.print("Local: " + local);

  digitalWrite(R_LED, HIGH);
  digitalWrite(G_LED, LOW);
  digitalWrite(B_LED, HIGH);

  tone(BUZZER_PIN, 400, 500);
}

void attendEmergency() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Emergencia");
  lcd.setCursor(0, 1);
  lcd.print("atendida");

  digitalWrite(R_LED, LOW);
  digitalWrite(G_LED, LOW);
  digitalWrite(B_LED, LOW);

  noTone(BUZZER_PIN);

  delay(3000);
  lcd.clear();
}

// BLE FUNCTIONS:

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
            // Serial.print("Characteristic (Notify) from server: ");
            // Serial.println(counter);
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
      httpRequest();
      // Serial.println("Connected!");
    }
  }

//

void setup() {
  Serial.begin(115200);
  lcd.init();

  connectToWifi();

  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
  pinMode(A_EMER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(G_EMER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  generateEmergency();

  // BLE
    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init("");
  //
}

void loop() {
  // BLE
    startBLEScan();
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEScanResults foundDevices = pBLEScan->start(5, false);
    processFoundDevices(foundDevices);
    printConnectionStatus();
  //

  if (digitalRead(G_EMER_BUTTON_PIN) == LOW) {
    Serial.println("Generate Emergency");
    generateEmergency();
  };

  if (digitalRead(A_EMER_BUTTON_PIN) == LOW) {
    Serial.println("Attend Emergency");
    attendEmergency();
  };
}