#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "BLEDevice.h"
#include "UbidotsEsp32Mqtt.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define R_LED 4                   // Red LED on pin 4
#define G_LED 5                   // Green LED on pin 5
#define B_LED 18                  // Blue LED on pin 18
#define A_EMER_BUTTON_PIN 12      // Button on pin 12 for ATTENDING emergencies
#define G_EMER_BUTTON_PIN 13      // Button for GENERATING emergencies
#define BUZZER_PIN 19             // Buzzer on pin 19
#define PAGER_NAME "pager1"       // Defining the PAGER_NAME

#define SSID "Inteli-welcome"        // "SHARE-RESIDENTE"   // "Inteli-welcome"
#define PASSWORD ""  // "Share@residente23" // ""

// UBIDOTS VARIABLES

  const char *UBIDOTS_TOKEN = "BBUS-mXOTtUc6rfcbbzq1AwPkLHlNCieapy";
  const char *DEVICE_LABEL = PAGER_NAME;     
  const char *WIFI_SSID = "Inteli-welcome";   
  const char *WIFI_PASS = "";      
  const char *VARIABLE_LABEL1 = "chamadoCriadoPeloPager"; // Equals 1 when an emergency calling is made through the pager button
  const char *VARIABLE_LABEL2 = "atendeuUltimoChamado";   // Equals 1 when the button for confirming an emergency calling attendance is pressed
  const char *VARIABLE_LABEL3 = "emergencialocal";        // Saves the local of an emergency calling sent by Ubidots platform
  const char *VARIABLE_LABEL4 = "emergenciatipo";         // Saves the type of an emergency calling sent by Ubidots platform
  const char *VARIABLE_LABEL5 = "emergenciaseveridade";   // Saves the severity of an emergency calling sent by Ubidots platform

  const int PUBLISH_FREQUENCY = 1000; // Update rate in milliseconds
  unsigned long timer;
  Ubidots ubidots(UBIDOTS_TOKEN);

  bool atendeu = false; // Control variable for attendance button loop

//



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

// Creating lists of hypothetical situations to be combined as emergencies (for testing only)
  String types[] = {"Queda", "Dor", "Desmaio"};
  String locals[] = {"Sala 1", "Sala 2", "Parking", "Recepcao"};
  int levels[] = {1, 2, 3};
//

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

  //tone(BUZZER_PIN, 400, 500);
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

  delay(2000);
  lcd.clear();
  lcd.noBacklight();
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
      Serial.println("Connected!");
    }
  }

//

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
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL3);
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL4);
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL5); 
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
  
  ubidotsMainConnection();

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


   
  if(millis() - timer > PUBLISH_FREQUENCY){

  // Verifies if the button for creating an emergency calling was pressed
    if (digitalRead(G_EMER_BUTTON_PIN) == LOW) {
      
        Serial.println("CHAMADO CRIADO");

        ubidots.add(VARIABLE_LABEL1, 1); // Pubblish "1" to sinalize that an emergency calling was created by the device
        delay(250);
        ubidots.publish(DEVICE_LABEL);
        delay(250);
      
        generateEmergency();
    } else {
      ubidots.add(VARIABLE_LABEL1, 0); // Pubblish "1" to sinalize that an emergency calling was created by the device
      ubidots.publish(DEVICE_LABEL);
    };
  //

  // Verifies if the button for attending an emergency calling was pressed

    if (digitalRead(A_EMER_BUTTON_PIN) == LOW) {

      atendeu = true;

    } else {
      ubidots.add(VARIABLE_LABEL2, 0); // Pubblish "1" to sinalize that the last emergency calling was attended by the device
      ubidots.publish(DEVICE_LABEL);
    };

    if (atendeu == true){
        Serial.println("EMERGENCIA ATENDIDA");

        ubidots.add(VARIABLE_LABEL2, 1); // Pubblish "1" to sinalize that the last emergency calling was attended by the device
        ubidots.publish(DEVICE_LABEL);
        
        attendEmergency();
        atendeu = false;}

  //

  timer = millis();
  }

  // Establish connection to Ubidots
    if (!ubidots.connected())
    {
      ubidots.reconnect();
      ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL3);
      ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL4);
      ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL5); 
    }
    ubidots.loop();
  //

}