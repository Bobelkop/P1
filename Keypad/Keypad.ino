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


void setup() {
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
  uint32_t now = millis();
  if (greenOffAt && now >= greenOffAt) { digitalWrite(LED_GREEN, LOW); greenOffAt = 0; }
  if (redOffAt   && now >= redOffAt)   { digitalWrite(LED_RED,   LOW); redOffAt   = 0; }
  if (yellowOffAt&& now >= yellowOffAt){ digitalWrite(LED_YELLOW,LOW); yellowOffAt= 0; }

  char k = keypad.getKey();
  if (!k || k==' ') return;

  // blink yellow on any keypress
  digitalWrite(LED_YELLOW, HIGH);
  yellowOffAt = now + BLINK_MS;

  tone(BUZZER_PIN, BEEP_FREQ, BEEP_MS);

  if (k == '#') { // ENTER
    if (buf.equals(PASSWORD)) {
      // success: green for OK_MS
      digitalWrite(LED_GREEN, HIGH);
      greenOffAt = now + OK_MS;
      // ensure red is off
      digitalWrite(LED_RED, LOW); redOffAt = 0;

       tone(BUZZER_PIN, 2000, 60);
      delay(70);
      tone(BUZZER_PIN, 2600, 80);
    } else {
      // failure: red for WRONG_MS
      digitalWrite(LED_RED, HIGH);
      redOffAt = now + WRONG_MS;

      tone(BUZZER_PIN, 400, 150);
      // ensure green is off
      digitalWrite(LED_GREEN, LOW); greenOffAt = 0;
    }
    buf = "";
  } else if (k == '*') { // BACKSPACE
    if (buf.length()) buf.remove(buf.length()-1);
  } else {
    if (buf.length() < 16) buf += k;
  }
}