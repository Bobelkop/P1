#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

// ---------- RFID del ----------
#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// ---------- Keypad del ----------
const byte ROWS = 4, COLS = 4;

char keys[ROWS][COLS] = {
  {' ', '1','2','3'},
  {' ', '4','5','6'},
  {' ', '7','8','9'},
  {' ', '*','0','#'}
};

// Brug de samme pins som i dit NewKeyPad-program (tilpas hvis nødvendigt)
byte rowPins[ROWS] = {7, 0, 3, 4};   // Y1..Y4 (top→bottom)
byte colPins[COLS] = {1, 2, 5, 6};   // X1..X4 (left→right)

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------- LED + buzzer ----------
const uint8_t LED_GREEN  = A2;
const uint8_t LED_RED    = A0;
const uint8_t LED_YELLOW = A1;

const uint8_t  BUZZER_PIN = A3;
const uint16_t BEEP_MS    = 60;
const uint16_t BEEP_FREQ  = 2500;

const uint32_t OK_MS     = 3000;
const uint32_t WRONG_MS  = 3000;
const uint16_t BLINK_MS  = 80;

// ---------- Tilstande ----------
String buf;                    // det brugeren taster
String currentUID = "";        // sidst læste kort
String currentPIN = "";        // PIN som hører til det kort
bool waitingForPIN = false;    // om vi er i "indtast kode" tilstand

uint32_t greenOffAt  = 0;
uint32_t redOffAt    = 0;
uint32_t yellowOffAt = 0;

// ---------- HJÆLPEFUNKTIONER ----------

// Returnerer UID som "D1:30:EC:19" – eller "" hvis intet kort
String readRFID()
{
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return "";

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return "";
  }

  String strID = "";
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] < 0x10) strID += "0";
    strID += String(rfid.uid.uidByte[i], HEX);
    if (i != 3) strID += ":";
  }
  strID.toUpperCase();

  Serial.print("UID: ");
  Serial.println(strID);

  return strID;
}

// Finder PIN-kode for et givent UID og gemmer den i currentPIN
bool selectPINForUID(const String &uid)
{
  if (uid.indexOf("D1:30:EC:19") >= 0) {   // brick
    currentPIN = "1111";                   // PIN til dette kort
    return true;
  }
  else if (uid.indexOf("E3:07:B8:2E") >= 0) { // kort
    currentPIN = "2222";                      // PIN til dette kort
    return true;
  }
  else if (uid.indexOf("07:94:F3:13") >= 0) { // Phillipkort
    currentPIN = "0001";                      // PIN til dette kort
    return true;
  }
  else if (uid.indexOf("CD:00:68:EE") >= 0) { // Rasmuskort
    currentPIN = "6969";                      // PIN til dette kort
    return true;
  }
  else if (uid.indexOf("6D:31:7C:EE") >= 0) {          // Patrikkort
    currentPIN = "0420";                      // PIN til dette kort
    return true;
  }

  currentPIN = "";
  return false;
}

void resetState()
{
  buf = "";
  currentUID = "";
  currentPIN = "";
  waitingForPIN = false;
}

// ---------- SETUP / LOOP ----------

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED,    LOW);
  digitalWrite(LED_GREEN,  LOW);

  keypad.setDebounceTime(20);
  keypad.setHoldTime(600);

  Serial.println("Klar. Først scan RFID, derefter indtast tilhørende kode på keypad og afslut med #");
}

void loop() {
  uint32_t now = millis();

  // ikke-blokerende LED timeouts
  if (greenOffAt  && now >= greenOffAt)  { digitalWrite(LED_GREEN,  LOW); greenOffAt  = 0; }
  if (redOffAt    && now >= redOffAt)    { digitalWrite(LED_RED,    LOW); redOffAt    = 0; }
  if (yellowOffAt && now >= yellowOffAt) { digitalWrite(LED_YELLOW, LOW); yellowOffAt = 0; }

  // 1) Hvis vi IKKE venter på PIN, så kigger vi efter et kort
  if (!waitingForPIN) {
    String uid = readRFID();
    if (uid != "") {
      currentUID = uid;

      if (selectPINForUID(uid)) {
        // kendt kort -> bed om kode
        Serial.println("Gyldigt kort. Indtast din kode og afslut med #");
        waitingForPIN = true;

        // lille "kort OK" feedback
        digitalWrite(LED_YELLOW, HIGH);
        yellowOffAt = now + 2 * BLINK_MS;
        tone(BUZZER_PIN, 2000, 80);
      } else {
        // ukendt kort
        Serial.println("Adgang NÆGTET (ukendt kort)");
        digitalWrite(LED_RED, HIGH);
        redOffAt = now + WRONG_MS;
        tone(BUZZER_PIN, 400, 200);
        resetState();-
      }
    }

    if (!waitingForPIN) return; // stadig ikke noget kort → slut denne loop-runde
  }

  // 2) HERFRA: vi har et gyldigt kort, og currentPIN er sat.
  //    Nu læser vi keypad-input
  char k = keypad.getKey();
  if (!k || k == ' ') return;

  // blink gul ved hvert tryk
  digitalWrite(LED_YELLOW, HIGH);
  yellowOffAt = now + BLINK_MS;
  tone(BUZZER_PIN, BEEP_FREQ, BEEP_MS);

  if (k == '#') { // ENTER
    Serial.print("Indtastet kode: ");
    Serial.println(buf);

    if (buf.equals(currentPIN)) {
      // SUCCESS
      Serial.println("Adgang TILLADT (korrekt kode)");
      digitalWrite(LED_GREEN, HIGH);
      greenOffAt = now + OK_MS;
      digitalWrite(LED_RED, LOW); redOffAt = 0;

      // success-lyd
      tone(BUZZER_PIN, 2000, 60);
      delay(70);
      tone(BUZZER_PIN, 2600, 80);

      // Her kan du aktivere relæ / lås
    } else {
      // FAILURE
      Serial.println("Forkert kode - adgang NÆGTET");
      digitalWrite(LED_RED, HIGH);
      redOffAt = now + WRONG_MS;
      digitalWrite(LED_GREEN, LOW); greenOffAt = 0;
      tone(BUZZER_PIN, 400, 150);
    }

    // efter et forsøg (uanset hvad) går vi tilbage til "scan kort"
    resetState();
  }
  else if (k == '*') { // BACKSPACE
    if (buf.length()) buf.remove(buf.length() - 1);
  }
  else {
    if (buf.length() < 16) buf += k;
  }
}
