#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "WiFiClientSecure.h"
#include "BLEDevice.h"

// Certificado MQTT
  const char* CA_cert = \
  "-----BEGIN CERTIFICATE-----\n" \
  "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\n" \
  "-----END CERTIFICATE-----";
//

// #define dos dados de conexão WiFi e MQTT
  #define SSID "Inteli-COLLEGE" // | "AndroidAP3112" | "SHARE-RESIDENTE"   | "Inteli-COLLEGE"
  #define PASSWORD "QazWsx@123" // | "npgm3125"      | "Share@residente23" | "QazWsx@123"
  #define PORT 8883
  #define MQTT_SERVER "be247e754cbb4e339e7338c891441b48.s1.eu.hivemq.cloud"
  #define MQTT_USER "mauro_teste_mqtt"
  #define MQTT_PASS "#hLntL5qH4RKWg5"
//

// #define dos componentes da protoboard
  #define R_LED 7
  #define G_LED 6
  #define B_LED 5
  #define LED_CON 17  
  #define A_EMER_BUTTON_PIN 41
  #define G_EMER_BUTTON_PIN 42
  #define BUZZER_PIN 19
  #define I2C_SDA 40
  #define I2C_SCL 39
//

// Tópicos e valores MQTT
  bool bleConnection = false;

  const char* topic1 = "BLE_connection";
  const char* value1 = "pager1,Sala 11";

  const char* topic2 = "create_emergency";
  const char* value2 = "pager1";

  const char* topic3 = "attend_emergency";
  const char* value3 = "pager1";

  const char* topic4 = "create_emergency_webApp";
//

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClientSecure client;
PubSubClient mqtt_client(client);

class ButtonDebouncer {
private:
  unsigned long lastDebounceTime;
  unsigned long debounceDelay;

public:
  ButtonDebouncer(unsigned long delay) : lastDebounceTime(0), debounceDelay(delay) {}

  bool isButtonPressed(int buttonPin) {
    if (digitalRead(buttonPin) == LOW && (millis() - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = millis();
      return true;
    }
    return false;
  }
};

class WiFiManager {
public:
  void connect() {
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to Wi-Fi...");
    }
    Serial.println("Connected to Wi-Fi");
  }
};

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

  char charArray[message.length() + 1];
  message.toCharArray(charArray, message.length() + 1);

  // Inicializa o ponteiro strtok
  char *token = strtok(charArray, ",");

  String emerg, local, level;

  if (token != NULL) {
    emerg = String(token);
    token = strtok(NULL, ",");
  }
  if (token != NULL) {
    local = String(token);
    token = strtok(NULL, ",");
  }
  if (token != NULL) {
    level = String(token);
  }

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Emerg: " + emerg);
  lcd.setCursor(0, 1);
  lcd.print("Local: " + local);
}

class MQTTManager {
public:
  void sendTopicToHiveMQ(const char* topic_, const char* value_) {
    if (mqtt_client.connect("ESP32", MQTT_USER , MQTT_PASS)) {   
      Serial.print("Connected, mqtt_client state: ");
      Serial.println(mqtt_client.state());

      mqtt_client.publish(topic_, value_);
    }
    else {
      Serial.println("Connected failed!  mqtt_client state:");
      Serial.print(mqtt_client.state());
      Serial.println("WiFiClientSecure client state:");
      char lastError[100];
      client.lastError(lastError,100);
      Serial.print(lastError);
    }
  }

  void reconnect() {
    // Loop until we're reconnected
    while (!mqtt_client.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (mqtt_client.connect("ESP32", MQTT_USER , MQTT_PASS)) {
        Serial.println("connected");
        mqtt_client.subscribe(topic4);
      } else {
        Serial.print("failed, rc=");
        Serial.print(mqtt_client.state());
        Serial.println(" try again in 3 seconds");
        delay(3000);
      }
    }
  }

  void mqttSetup() {
    client.setCACert(CA_cert);
    mqtt_client.setServer(MQTT_SERVER, PORT);
    mqtt_client.setCallback(callback);
  }
};

// Funções BLE
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
          pRemoteChar->registerForNotify([](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
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
      Serial.println("BLE connection = true");
    }
  }
//

class BLEManager {
public:
  void bleSetup() {
    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init("");
    startBLEScan();
    BLEScan* pBLEScan = BLEDevice::getScan();
    BLEScanResults foundDevices = pBLEScan->start(5, false);
    processFoundDevices(foundDevices);
    printConnectionStatus();
  }
};

class EmergencyManager {
public:
  void attendEmergency() {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Emergencia");
    lcd.setCursor(0, 1);
    lcd.print("atendida");

    digitalWrite(R_LED, LOW);
    digitalWrite(G_LED, LOW);
    digitalWrite(B_LED, LOW);

    noTone(BUZZER_PIN);
  }

  void generateEmergency() {
    String emerg = "Dor";
    String local = "Sala 01";

    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Emerg: " + emerg);
    lcd.setCursor(0, 1);
    lcd.print("Local: " + local);

    digitalWrite(R_LED, HIGH);
    digitalWrite(G_LED, LOW);
    digitalWrite(B_LED, HIGH);

    tone(BUZZER_PIN, 400, 500);
  }
};

// Declarando instâncias
WiFiManager wifiManager;
MQTTManager mqttManager;
BLEManager bleManager;
EmergencyManager emergencyManager;
ButtonDebouncer aButtonDebouncer(200);
ButtonDebouncer gButtonDebouncer(200);

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();

  // pinModes
    pinMode(R_LED, OUTPUT);
    pinMode(G_LED, OUTPUT);
    pinMode(B_LED, OUTPUT);
    pinMode(LED_CON, OUTPUT);
    pinMode(A_EMER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(G_EMER_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
  //

  wifiManager.connect();
  mqttManager.mqttSetup();
}

void loop() {
  if (!client.connected()) {
    mqttManager.reconnect();
  } else {
    if (!connected) {
      bleManager.bleSetup();
    } else {
      if (!bleConnection) {
        for (int i = 0; i < 1; i++) {
          mqttManager.sendTopicToHiveMQ(topic1, value1);
        }
        bleConnection = true;
      } else {
        if (gButtonDebouncer.isButtonPressed(G_EMER_BUTTON_PIN)) {
          emergencyManager.generateEmergency();
          mqttManager.sendTopicToHiveMQ(topic2, value2);
          Serial.println("Emergência criada!");
        }
        if (aButtonDebouncer.isButtonPressed(A_EMER_BUTTON_PIN)) {
          emergencyManager.attendEmergency();
          mqttManager.sendTopicToHiveMQ(topic3, value3);
          Serial.println("Emergência atendida!");
        }
      }
    }
  }
  mqtt_client.loop();
}
