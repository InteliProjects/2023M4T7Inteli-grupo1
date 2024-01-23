#include <LiquidCrystal_I2C.h> // Library for using the LCD I2C display

LiquidCrystal_I2C lcd(0x27, 16, 2); // Display configurations

#define R_LED 4 // Red LED on pin 4
#define G_LED 5 // Green LED on pin 5
#define B_LED 18 // Blue LED on pin 18
#define BUTTON_PIN 12 // Button on pin 12 for attending emergencies
#define GEN_EMER_BUTTON_PIN 13 // New button for generating emergencies
#define BUZZER_PIN 19 // buzzer no pino 19

// Emergency class to simulate emergencies that occur in the hospital
class Emergency {
public:
  String type;
  String local;
  int level;
  bool handled;

  // Emergency class constructor
  Emergency(String _type, String _local, int _level) {
    type = _type;
    local = _local;
    level = _level;
    handled = false;
  }

  // Displays emergency information on the LCD simulating what doctors will see on a daily basis
  void printEmergency() {
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Emerg: " + type); // Emerg = Emergency
    lcd.setCursor(0, 1);
    lcd.print("Local: " + local);
  }

  // Controls the LEDs based on the level of the emergency and whether it has been attended to by a doctor
  void emergencyLevelLED() {
    if (handled) {
      digitalWrite(R_LED, LOW);
      digitalWrite(G_LED, LOW);
      digitalWrite(B_LED, LOW);
    } else {
        tone(BUZZER_PIN, 400, 500); // Buzzer emite som na frequência de 400Hz por 500ms
        delay(600); // Intervalo de 100ms entre cada emissão de som
      if (level == 3) { // Red for high severity
        digitalWrite(R_LED, HIGH);
        digitalWrite(G_LED, LOW);
        digitalWrite(B_LED, LOW);
      } else if (level == 2) { // Yellow for medium severity
        digitalWrite(R_LED, HIGH);
        digitalWrite(G_LED, HIGH);
        digitalWrite(B_LED, LOW);
      } else if (level == 1) { // Blue for mild severity
        digitalWrite(R_LED, LOW);
        digitalWrite(G_LED, LOW);
        digitalWrite(B_LED, HIGH);
      }
    }
  }

  // Checks if the attendance button was pressed
  void checkButtonHandled(int buttonPin) {
    if (digitalRead(buttonPin) == LOW) {
      handled = true;
      noTone(BUZZER_PIN); // Buzzer é silenciado
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Emergencia");
      lcd.setCursor(0, 1);
      lcd.print("atendida");
      delay(3000); // Replace with non-blocking delay later
      lcd.clear();
    }
  }
};

// Creating lists of hypothetical situations to be combined as emergencies (for testing only)
String types[] = {"Queda", "Dor", "Desmaio"};
String locals[] = {"Sala 1", "Sala 2", "Parking", "Recepcao"};
int levels[] = {1, 2, 3};

// Function to generate a random emergency
Emergency generateRandomEmergency() {
  String randomType = types[random(3)];
  String randomLocal = locals[random(4)];
  int randomLevel = levels[random(3)];
  return Emergency(randomType, randomLocal, randomLevel);
}

Emergency currentEmergency = generateRandomEmergency(); // Initialize with a random emergency

// Function to generate an emergency when the button is pressed
void generateEmergencyIfPressed() {
  if (digitalRead(GEN_EMER_BUTTON_PIN) == LOW) {
    delay(50); // Debounce delay
    if (digitalRead(GEN_EMER_BUTTON_PIN) == LOW) {
      while (digitalRead(GEN_EMER_BUTTON_PIN) == LOW); // Wait for button release
      currentEmergency = generateRandomEmergency();
      currentEmergency.printEmergency();
      currentEmergency.handled = false; // Ensure the new emergency is marked as unhandled
    }
  }
}

void setup() {
  pinMode(R_LED, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(B_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(GEN_EMER_BUTTON_PIN, INPUT_PULLUP); // Set up the new button pin
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init(); // Initialize the LCD
  currentEmergency.printEmergency(); // Print the first random emergency
}

void loop() {
  currentEmergency.emergencyLevelLED();
  currentEmergency.checkButtonHandled(BUTTON_PIN);
  generateEmergencyIfPressed(); // Check if the emergency generation button was pressed
}