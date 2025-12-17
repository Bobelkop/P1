#include <Arduino.h>
#include <Keypad.h>

const byte ROWS = 4, COLS = 4;

// Your keypad has a "missing" first column on the top 3 rows.
// Map blanks with ' ' so we can ignore them.
char keys[ROWS][COLS] = {
  {' ', '1','2','3'},
  {' ', '4','5','6'},
  {' ', '7','8','9'},
  {' ', '*','0','#'}
};

// Ribbon pin1→D9, ... pin8→D2 (as you wired earlier)
//byte rowPins[ROWS] = {2, 9, 6, 5};   // Y1..Y4 (top→bottom)
//byte colPins[COLS] = {8, 7, 4, 3};   // X1..X4 (left→right)

byte rowPins[ROWS] = {7, 0, 3, 4};   // Y1..Y4 (top→bottom)
byte colPins[COLS] = {1, 2, 5, 6};   // X1..X4 (left→right)

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Password Settings
const char* PASSWORD = "6769";
const uint32_t OK_MS     = 3000;
const uint32_t WRONG_MS  = 3000;   
const uint16_t BLINK_MS  = 80;


const uint8_t LED_GREEN = A2;
const uint8_t LED_RED = A0;
const uint8_t LED_YELLOW = A1;

const uint8_t  BUZZER_PIN = A3;    
const uint16_t BEEP_MS    = 60;    
const uint16_t BEEP_FREQ  = 2500; 

String buf;
uint32_t greenOffAt = 0;
uint32_t redOffAt   = 0;
uint32_t yellowOffAt= 0;

String code = "";

void setup() {
  Serial.begin(9600);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED,    LOW);
  digitalWrite(LED_GREEN,  LOW);

  keypad.setDebounceTime(20);
  keypad.setHoldTime(600);
}

void loop() {
  // non-blocking LED timeouts
  char key = keypad.getKey();
  if (key) {
    // Feedback: yellow LED + buzzer
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    if (key == '#') {
      Serial.println(code);
      code = "";
    } else if (key == '*') {
      code = "";
    } else {
      code += key;
    }
  }

  if (Serial.available()) {
    delay(50);
    String response = Serial.readStringUntil('\n');
    Serial.print("Recieved from pi: ");
    Serial.println(response);
    if (response == "OK") {
      digitalWrite(LED_GREEN, HIGH);
      delay(3000);
      digitalWrite(LED_GREEN, LOW);
    } else if (response == "FAIL") {

      digitalWrite(LED_RED, HIGH);
      delay(2000);
      digitalWrite(LED_RED, LOW);
    }
  }
}
