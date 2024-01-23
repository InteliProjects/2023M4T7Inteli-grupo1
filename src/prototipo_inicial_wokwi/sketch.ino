#include <LiquidCrystal_I2C.h> // biblioteca para utilizar o display LCD I2C;

LiquidCrystal_I2C lcd(0x27, 16, 2); // configurações do display;

#define R_LED 4 // LED vermelho no pino 4
#define G_LED 5 // LED verde no pino 5
#define B_LED 18 // LED azul no pino 18
#define BUTTON_PIN 12 // botão no pino 12
#define BUZZER_PIN 19 // buzzer no pino 19

// classe para simular as emergências que ocorrem no hospital;
class Emergency {
  public:
    String type;
    String local;
    int level;
    bool handled;

    // construtor da classe Emergency;
    Emergency(String _type, String _local, int _level) {
      type = _type;
      local = _local;
      level = _level;
      handled = false;
    }

    // exibe informações da emergência no LCD simulando o que os médicos verão no dia a dia;
    void printEmergency() {
      lcd.backlight();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Emerg: " + type); // Emerg = Emergência
      lcd.setCursor(0, 1);
      lcd.print("Local: " + local);
    }

    // controla os LEDs com base no nível da emergência e se foi tratada por algum médico;
    void emergencyLevelLED() {
      if (handled) {
        digitalWrite(R_LED, LOW);
        digitalWrite(G_LED, LOW);
        digitalWrite(B_LED, LOW);
      } else {

        tone(BUZZER_PIN, 400, 500); // Buzzer emite som na frequência de 400Hz por 500ms
        delay(600); // Intervalo de 100ms entre cada emissão de som

        if (level == 3) { // vermelho para alta severidade;
          digitalWrite(R_LED, HIGH);
          digitalWrite(G_LED, LOW);
          digitalWrite(B_LED, LOW);
        } else if (level == 2) { // amarelo para média severidade;
          digitalWrite(R_LED, HIGH);
          digitalWrite(G_LED, HIGH);
          digitalWrite(B_LED, LOW);
        } else if (level == 1) { // azul para branda severidade;
          digitalWrite(R_LED, LOW);
          digitalWrite(G_LED, LOW);
          digitalWrite(B_LED, HIGH);
        }
      }
    }

    // verifica se o botão de atendimento foi pressionado
    void checkButtonHandled(int buttonPin) {
      if (digitalRead(buttonPin) == LOW) {
        handled = true;
        noTone(BUZZER_PIN); // Buzzer é silenciado
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Emergencia");
        lcd.setCursor(0, 1);
        lcd.print("atendida");
        delay(4000); // tirar depois, troca pelo millis
        lcd.clear();
      }
    }
};

// criando listas de situações hipotéticas para serem combinadas como emergências (apenas para teste);
String types[] = {"Queda", "Dor", "Desmaio"};
String locals[] = {"Sala 1", "Sala 2", "Parking", "Recepcao"};
int levels[] = {1, 2, 3};

// função para gerar uma emergência aleatória;
Emergency generateRandomEmergency() {
  String randomType = types[random(3)];
  String randomLocal = locals[random(4)];
  int randomLevel = levels[random(3)];
  return Emergency(randomType, randomLocal, randomLevel);
}

Emergency currentEmergency("", "", 0); // iniciando a variável com esses valores apenas para estar de acordo com o construtor que demanda 3 parâmetros;

unsigned long lastEmergencyTime = 0;
unsigned long emergencyInterval = 7000; // intervalo de 7 segundos entre emergências;

void setup() {
  // inicialização do hardware e configurações;

  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();

  // inicializa a emergência atual com uma emergência aleatória;
  currentEmergency = generateRandomEmergency();
  currentEmergency.printEmergency();

  lastEmergencyTime = millis(); // inicializa o tempo da última emergência
}

void loop() {
  // execução principal do programa
  currentEmergency.emergencyLevelLED();
  currentEmergency.checkButtonHandled(BUTTON_PIN);

  unsigned long currentTime = millis();

  // verifica se é hora de gerar uma nova emergência após um intervalo
  if (currentEmergency.handled && (currentTime - lastEmergencyTime >= emergencyInterval)) {
    currentEmergency = generateRandomEmergency();
    currentEmergency.printEmergency();
    lastEmergencyTime = currentTime;
  }
}
