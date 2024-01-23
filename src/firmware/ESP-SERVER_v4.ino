#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define RESET_BUTTON 25
#define RGB_BLUE 26
#define RGB_RED 27

class BLEController {
public:
  BLEController() {
    BLEDevice::init("ESP32"); // Inicializando o dispositivo BLE como "ESP32"
    
    // Criando um servidor BLE e configurando os callbacks
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks(this));
    
    // Configurando o serviço e iniciando o 'Advertising' BLE
    setupBLEService();
    startBLEAdvertising();
  }

  void handleConnections() {
    // Lógica de conexão / desconexão
    if (!deviceConnected && oldDeviceConnected) {
      delay(500);
      startBLEAdvertising();  // Se desconectado, reinicia
      Serial.println("Start advertising");
      oldDeviceConnected = deviceConnected;
    }

    if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
    }
  }

  void updateCharacteristic() {
    pCharacteristic->setValue(value); // Atualizando o valor da característica
    pCharacteristic->notify(); // Notificando os clientes
  }

  bool isDeviceConnected() const {
    return deviceConnected; // Retornando o estado da conexão do dispositivo
  }

private:
  BLEServer* pServer = nullptr;
  BLECharacteristic* pCharacteristic = nullptr;

  bool deviceConnected = false;
  bool oldDeviceConnected = false;
  uint32_t value = 0;

  // UUIDs do serviço e característica BLE
  static const char* SERVICE_UUID;
  static const char* CHARACTERISTIC_UUID;

  class MyServerCallbacks : public BLEServerCallbacks {
  public:
    MyServerCallbacks(BLEController* controller) : controller(controller) {}
    void onConnect(BLEServer* pServer) { controller->deviceConnected = true; }; // Callback de conexão
    void onDisconnect(BLEServer* pServer) { controller->deviceConnected = false; }; // Callback de desconexão
  private:
    BLEController* controller;
  };

  void setupBLEService() {
    BLEService* pService = pServer->createService(SERVICE_UUID); // Cria o serviço BLE

    // Característica BLE com propriedades: Leitura, Escrita e Notificação
    pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_WRITE |
      BLECharacteristic::PROPERTY_NOTIFY
    );
    
    pCharacteristic->addDescriptor(new BLE2902()); // Adiciona um descritor BLE2902 à característica

    pService->start(); // Inicia o serviço
  }

  void startBLEAdvertising() {
    // Configura o 'advertising' BLE
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);


    // Inicia o 'advertising'
    BLEDevice::startAdvertising();
  }
};

// Definição dos UUIDs do serviço e característica (pego de um gerador aleatório)
const char* BLEController::SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
const char* BLEController::CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";

BLEController* controller; // Instância global da classe BLEController

void setup() {
  Serial.begin(115200);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(RGB_RED, OUTPUT);

  controller = new BLEController(); // Inicializa a instância do BLEController
  Serial.println("Waiting for a client connection to notify...");

  digitalWrite(RGB_BLUE, HIGH);
}

void loop() {
  if (digitalRead(RESET_BUTTON) == LOW) {
    for (int i = 0; i < 10; i++) { 
      digitalWrite(RGB_RED, HIGH);
      digitalWrite(RGB_BLUE, HIGH);
      delay(250);
      digitalWrite(RGB_RED, LOW);
      digitalWrite(RGB_BLUE, LOW);
      delay(250);
    }

    ESP.restart();

    digitalWrite(RGB_RED, LOW);
    digitalWrite(RGB_BLUE, HIGH);
  }

  // controller->handleConnections(); // Lógica de conexão e atualização da característica

  // if (controller->isDeviceConnected()) {
  //   controller->updateCharacteristic();
  //   delay(1000);
  // }
}
