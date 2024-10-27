#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "RFID Based Door Lock"
#define BLYNK_AUTH_TOKEN ""

#include <ESP32Servo.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Broches LCD
LiquidCrystal lcd(19, 18, 17, 16, 15, 14);

// Définir la broche du servo
const int servoPin = 12;
Servo myServo;

// Broches de LED et de sonnerie
const int RedLED = 5;
const int GreenLED = 4;
const int Buzzer = 21;

// Jeton d'authentification Blynk
char auth[] = "";

// Vos identifiants WiFi
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// Variables pour la lecture RFID
int count = 0;
char c;
String id;

// Temps de retard
int wait = 5000;

void setup() {
  // Initialiser la communication série
  Serial.begin(9600);

  // Initialiser les broches
  pinMode(RedLED, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  
  // Attacher le servo
  myServo.attach(servoPin);
  myServo.write(90); // Supposons que 90 est la position verrouillée

  // Initialiser l'écran LCD
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  lcd.print("  THE BRIGHT LIGHT ");
  lcd.setCursor(0, 1);
  lcd.print(" RFID BASED LOCK SYS");
  lcd.setCursor(0, 2);
  lcd.print(" CARD:   ");
  lcd.setCursor(0, 3);
  lcd.print(" NAME:   ");

  // Connectez-vous au WiFi et à Blynk
  Blynk.begin(auth, ssid, pass);

  Serial.println("Welcome to Your Sweet Home");
  Serial.println("Please scan your RFID Card");
}

void loop() {
  Blynk.run(); // Exécutez Blynk

  while (Serial.available() > 0) {
    c = Serial.read();
    count++;
    id += c;
    if (count == 12) {
      Serial.print(id);
      if (id == "E280689401A9") {
        handleValidCard("ZAID");
      } else if (id == "E2000019060C") {
        handleValidCard("Jassim");
      } else if (id == "G46RD9V40F3A") {
        handleValidCard("Mattalah");
      } else if (id == "B71001B76894") {
        handleValidCard("Mahmoud");
      } else if (id == "1C81159073FD") {
        handleValidCard("Salah");
      } else if (id == "B710D0186022") {
        handleValidCard("El kandsi");
      } else {
        handleInvalidCard();
      }
    }
  }
  count = 0;
  id = "";
  delay(1000);
}

void handleValidCard(String name) {
  Serial.println("Valid Card");
  lcd.setCursor(0, 2);
  lcd.print(" CARD: VALID   ");
  lcd.setCursor(0, 3);
  lcd.print(" NAME: " + name + " ");
  digitalWrite(GreenLED, HIGH);
  myServo.write(0); // Supposons que 0 est la position déverrouillée

  // Consigner l'événement dans Blynk
  Blynk.logEvent("valid_card", "Valid Card: " + name);
  Blynk.virtualWrite(V0, "Valid Card: " + name); // Affichage sur Blynk

  delay(wait);
  myServo.write(90); // Verrouillez à nouveau la porte
  digitalWrite(GreenLED, LOW);
  lcd.setCursor(0, 2);
  lcd.print(" CARD:           ");
  lcd.setCursor(0, 3);
  lcd.print(" NAME:           ");
}

void handleInvalidCard() {
  Serial.println("Invalid Card");
  lcd.setCursor(0, 2);
  lcd.print(" CARD: INVALID   ");
  lcd.setCursor(0, 3);
  lcd.print(" NAME: UNKNOWN     ");
  digitalWrite(RedLED, HIGH);
  digitalWrite(Buzzer, HIGH);

  // Consigner l'événement dans Blynk
  Blynk.logEvent("invalid_card", "Invalid Card");
  Blynk.virtualWrite(V0, "Invalid Card"); // Affichage sur Blynk

  delay(wait);
  digitalWrite(RedLED, LOW);
  digitalWrite(Buzzer, LOW);
  lcd.setCursor(0, 2);
  lcd.print(" CARD:           ");
  lcd.setCursor(0, 3);
  lcd.print(" NAME:           ");
}
