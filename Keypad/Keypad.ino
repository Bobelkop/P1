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
byte rowPins[ROWS] = {2, 9, 6, 5};   // Y1..Y4 (top→bottom)
byte colPins[COLS] = {8, 7, 4, 3};   // X1..X4 (left→right)

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  keypad.setDebounceTime(20);
  keypad.setHoldTime(600);
}

void loop() {
  static String buf;
  char k = keypad.getKey();
  if (!k || k==' ') return;

  if (k == '#') { Serial.println("Entered: " + buf); buf = ""; }
  else if (k == '*') { if (buf.length()) buf.remove(buf.length()-1); }
  else { buf += k; Serial.println(buf); }   // live echo
}
