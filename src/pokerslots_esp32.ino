// ============================================================
//  POKER MACHINE – ESP32 + 2.8" ILI9341 (SKU: E32R28T)
//  5-Card Draw Poker mit Hold-Funktion
//
//  Spielablauf:
//  1. DEAL  → 5 Karten werden ausgeteilt
//  2. Spieler wählt Karten zum HALTEN (HOLD-Buttons)
//  3. DRAW  → nicht gehaltene Karten werden ersetzt
//  4. Ergebnis wird berechnet 
//
//  User_Setup.h Pins (E32R28T):
//    TFT_MISO 12 / TFT_MOSI 13 / TFT_SCLK 14
//    TFT_CS 15 / TFT_DC 2 / TFT_RST -1 / TFT_BL 21
//    TOUCH_CS 33
//  Touch HSPI: CLK=25, MOSI=32, MISO=39, IRQ=36
// ============================================================

#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <ArduinoJson.h>

// ── Touch HSPI Pins ───────────────────────────────────────────
#define TOUCH_CS_PIN   33
#define TOUCH_CLK_PIN  25
#define TOUCH_MOSI_PIN 32
#define TOUCH_MISO_PIN 39
#define TOUCH_IRQ_PIN  36

SPIClass touchSPI(HSPI);
XPT2046_Touchscreen ts(TOUCH_CS_PIN, TOUCH_IRQ_PIN);
TFT_eSPI tft = TFT_eSPI();

// ── Farben ────────────────────────────────────────────────────
#define COL_BG       0x0802
#define COL_YELLOW   0xFF60
#define COL_DARK     0x2104
#define COL_RED      0xF800
#define COL_GREEN    0x07E0
#define COL_WHITE    0xFFFF
#define COL_HELD     0x0450   // dunkles grün für gehaltene Karte
#define COL_ORANGE   0xFD00   // warnendes Orange für ALL IN

// ── Deck ──────────────────────────────────────────────────────
// Farben: H=Herz, K=Karo, P=Pik, T=Treff
// Werte:  2-9, 10=T, J, Q, K, A
const char* SUITS[4]  = {"H","K","P","T"};
const char* VALUES[13] = {"2","3","4","5","6","7","8","9","T","J","Q","K","A"};

// Karte als Index 0-51: suit*13 + value
struct Card { int suit; int val; };  // suit 0-3, val 0-12

Card deck[52];
int  deckIdx = 0;

Card hand[5];
bool held[5]  = {false,false,false,false,false};
bool cardVisible[5] = {false,false,false,false,false};

// ── Spielzustand ──────────────────────────────────────────────
int  balance = 1000;
int  bet     = 10;
int  wins    = 0;
int  games   = 0;

enum Phase { PHASE_IDLE, PHASE_HOLD, PHASE_RESULT };
Phase phase = PHASE_IDLE;

String lastMsg    = "Drueck DEAL";
String lastResult = "";

// ── Button Layout (320x240) ───────────────────────────────────
struct Btn { int x, y, w, h; const char* label; };

// 5 HOLD-Buttons unter den Karten
Btn btnHold[5] = {
  {  4, 148, 58, 22, "HOLD"},
  { 65, 148, 58, 22, "HOLD"},
  {126, 148, 58, 22, "HOLD"},
  {187, 148, 58, 22, "HOLD"},
  {248, 148, 58, 22, "HOLD"},
};

Btn btnDeal    = { 10, 195, 90, 36, "DEAL" };
Btn btnBetUp   = {225, 195, 45, 36, "BET+"};
Btn btnBetDown = {175, 195, 45, 36, "BET-"};
Btn btnAllIn   = {274, 195, 42, 36, "ALL"};

// ── setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  pinMode(21, OUTPUT);
  digitalWrite(21, HIGH);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(COL_BG);

  touchSPI.begin(TOUCH_CLK_PIN, TOUCH_MISO_PIN, TOUCH_MOSI_PIN, TOUCH_CS_PIN);
  ts.begin(touchSPI);
  ts.setRotation(1);

  drawUI();
}

// ── loop ──────────────────────────────────────────────────────
void loop() {
  handleTouch();
  handleSerial();
}

// ── Deck mischen ──────────────────────────────────────────────
void shuffleDeck() {
  int idx = 0;
  for (int s = 0; s < 4; s++)
    for (int v = 0; v < 13; v++)
      deck[idx++] = {s, v};
  // Fisher-Yates
  for (int i = 51; i > 0; i--) {
    int j = random(i + 1);
    Card tmp = deck[i]; deck[i] = deck[j]; deck[j] = tmp;
  }
  deckIdx = 0;
}

Card drawCard() { return deck[deckIdx++]; }

// ── Kartentext ────────────────────────────────────────────────
// z.B. "H5", "KA", "PJ"
String cardStr(Card c) {
  return String(SUITS[c.suit]) + String(VALUES[c.val]);
}

// ── DEAL ──────────────────────────────────────────────────────
void doDeal() {
  if (balance < bet) { lastMsg = "Kein Guthaben!"; drawMsg(); return; }

  balance -= bet;
  games++;
  shuffleDeck();

  for (int i = 0; i < 5; i++) {
    hand[i] = drawCard();
    held[i] = false;
    cardVisible[i] = true;
  }

  phase = PHASE_HOLD;
  lastMsg = "Wahle Karten zum Halten";
  lastResult = "";

  drawAllCards();
  drawHoldButtons();
  drawStats();
  drawMsg();
  // DEAL-Button wird zu DRAW
  drawActionButton();
}

// ── DRAW ──────────────────────────────────────────────────────
void doDraw() {
  for (int i = 0; i < 5; i++) {
    if (!held[i]) hand[i] = drawCard();
  }
  phase = PHASE_RESULT;

  // Ergebnis berechnen
  int result = evaluateHand();
  int winAmt = 0;
  if (result > 0) { wins++; winAmt = bet * result; }
  balance += winAmt;

  lastResult = getResultName(result);
  if (winAmt > 0)
    lastMsg = lastResult + "! +" + String(winAmt) + "EUR";
  else
    lastMsg = "Verloren. Kein Gewinn.";

  drawAllCards();
  clearHoldButtons();
  drawStats();
  drawMsg();
  drawActionButton();

  // JSON an PC
  sendResult(winAmt);
}

// ── Hand auswerten ────────────────────────────────────────────
// Gibt Multiplikator zurück (0 = verloren)
int evaluateHand() {
  int vals[5], suits[5];
  for (int i = 0; i < 5; i++) { vals[i] = hand[i].val; suits[i] = hand[i].suit; }

  // Sortieren (bubble)
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4-i; j++)
      if (vals[j] > vals[j+1]) { int t=vals[j]; vals[j]=vals[j+1]; vals[j+1]=t; }

  // Flush?
  bool flush = true;
  for (int i = 1; i < 5; i++) if (suits[i] != suits[0]) { flush = false; break; }

  // Straight?
  bool straight = (vals[4]-vals[0]==4 && countUnique(vals)==5);
  // Sonderfall: A-2-3-4-5
  bool wheel = (vals[0]==0&&vals[1]==1&&vals[2]==2&&vals[3]==3&&vals[4]==12);
  if (wheel) straight = true;

  // Zählen
  int counts[13] = {0};
  for (int i = 0; i < 5; i++) counts[vals[i]]++;
  int pairs=0, threes=0, fours=0;
  for (int i = 0; i < 13; i++) {
    if (counts[i]==2) pairs++;
    if (counts[i]==3) threes++;
    if (counts[i]==4) fours++;
  }

  if (straight && flush)        return (wheel||vals[0]==9) ? 800 : 50; // Royal/Straight Flush
  if (fours==1)                 return 25;   // Four of a Kind
  if (threes==1 && pairs==1)    return 9;    // Full House
  if (flush)                    return 6;    // Flush
  if (straight)                 return 4;    // Straight
  if (threes==1)                return 3;    // Three of a Kind
  if (pairs==2)                 return 2;    // Two Pair
  if (pairs==1) {
    // Nur Jacks or Better (val >= 9)
    for (int i=0;i<13;i++) if(counts[i]==2 && i>=9) return 1;
  }
  return 0;
}

int countUnique(int* arr) {
  bool seen[13]={false};
  int c=0;
  for(int i=0;i<5;i++) if(!seen[arr[i]]){seen[arr[i]]=true;c++;}
  return c;
}

String getResultName(int mult) {
  switch(mult) {
    case 800: return "ROYAL FLUSH";
    case 50:  return "STR. FLUSH";
    case 25:  return "FOUR OF A KIND";
    case 9:   return "FULL HOUSE";
    case 6:   return "FLUSH";
    case 4:   return "STRAIGHT";
    case 3:   return "THREE OF A KIND";
    case 2:   return "TWO PAIR";
    case 1:   return "JACKS OR BETTER";
    default:  return "KEIN GEWINN";
  }
}

// ── JSON senden ───────────────────────────────────────────────
void sendResult(int winAmt) {
  StaticJsonDocument<256> doc;
  doc["action"]  = "result";
  doc["win"]     = winAmt;
  doc["balance"] = balance;
  doc["wins"]    = wins;
  doc["games"]   = games;
  doc["result"]  = lastResult;
  doc["bet"]     = bet;
  JsonArray cards = doc.createNestedArray("cards");
  for (int i = 0; i < 5; i++) cards.add(cardStr(hand[i]));
  serializeJson(doc, Serial);
  Serial.println();
}

// ── Serial empfangen ──────────────────────────────────────────
void handleSerial() {
  if (!Serial.available()) return;
  String line = Serial.readStringUntil('\n');
  line.trim();
  if (!line.length()) return;
  // Hier könnten später Befehle vom PC kommen
}

// ── Touch ─────────────────────────────────────────────────────
void handleTouch() {
  if (!ts.tirqTouched() || !ts.touched()) return;
  TS_Point p = ts.getPoint();
  int tx = map(p.x, 300, 3800, 0, 320);
  int ty = map(p.y, 300, 3800, 0, 240);

  if (phase == PHASE_IDLE || phase == PHASE_RESULT) {
    if (hitBtn(btnDeal, tx, ty))    { phase=PHASE_IDLE; doDeal(); }
    if (hitBtn(btnBetUp, tx, ty))   { bet = min(balance, bet+10); drawStats(); drawActionButton(); }
    if (hitBtn(btnBetDown, tx, ty)) { bet = max(10, bet-10);      drawStats(); drawActionButton(); }
    if (hitBtn(btnAllIn, tx, ty))   { bet = balance; drawStats(); drawActionButton(); }
  }
  else if (phase == PHASE_HOLD) {
    for (int i = 0; i < 5; i++) {
      if (hitBtn(btnHold[i], tx, ty)) {
        held[i] = !held[i];
        drawCard_i(i);
        drawHoldBtn(i);
      }
    }
    if (hitBtn(btnDeal, tx, ty)) doDraw();
    if (hitBtn(btnBetUp, tx, ty))   { bet = min(balance, bet+10); drawStats(); drawActionButton(); }
    if (hitBtn(btnBetDown, tx, ty)) { bet = max(10, bet-10);      drawStats(); drawActionButton(); }
    if (hitBtn(btnAllIn, tx, ty))   { bet = balance; drawStats(); drawActionButton(); }
  }
  delay(180);
}

bool hitBtn(Btn b, int tx, int ty) {
  return tx>=b.x && tx<=b.x+b.w && ty>=b.y && ty<=b.y+b.h;
}

// ── Zeichnen ──────────────────────────────────────────────────
void drawUI() {
  tft.fillScreen(COL_BG);
  tft.setTextColor(COL_YELLOW, COL_BG);
  tft.setTextSize(2);
  tft.setCursor(75, 4);
  tft.print("Pokerslots");
  tft.drawFastHLine(0, 20, 320, COL_YELLOW);

  drawAllCards();
  drawStats();
  drawMsg();
  drawActionButton();
}

// Alle 5 Karten zeichnen
void drawAllCards() {
  for (int i = 0; i < 5; i++) drawCard_i(i);
}

// Eine Karte zeichnen (Index 0-4)
// Kartenbreite 58px, Höhe 110px, y=24, x = i*62+4
void drawCard_i(int i) {
  int x = i * 62 + 4;
  int y = 24;
  int w = 58;
  int h = 110;

  uint16_t border = held[i] ? COL_GREEN : COL_YELLOW;
  uint16_t bg     = held[i] ? COL_HELD  : COL_DARK;

  tft.fillRoundRect(x, y, w, h, 4, bg);
  tft.drawRoundRect(x, y, w, h, 4, border);

  if (!cardVisible[i]) return;

  // Farbe der Karte
  bool red = (hand[i].suit == 0 || hand[i].suit == 1); // H oder K
  uint16_t col = red ? COL_RED : COL_WHITE;

  tft.setTextColor(col, bg);
  tft.setTextSize(2);

  // Wert oben links
  String v = String(VALUES[hand[i].val]);
  tft.setCursor(x + 4, y + 4);
  tft.print(v);

  // Farbe unten rechts
  String s = String(SUITS[hand[i].suit]);
  tft.setCursor(x + 4, y + h - 18);
  tft.print(s);

  // Großes Symbol Mitte
  tft.setTextSize(3);
  String full = String(SUITS[hand[i].suit]) + String(VALUES[hand[i].val]);
  int tw = full.length() * 18;
  tft.setCursor(x + (w - tw)/2, y + 40);
  tft.print(full);
}

void drawHoldButtons() {
  for (int i = 0; i < 5; i++) drawHoldBtn(i);
}

void drawHoldBtn(int i) {
  uint16_t bg  = held[i] ? COL_GREEN : COL_DARK;
  uint16_t fg  = held[i] ? COL_BG    : COL_YELLOW;
  tft.fillRoundRect(btnHold[i].x, btnHold[i].y, btnHold[i].w, btnHold[i].h, 3, bg);
  tft.drawRoundRect(btnHold[i].x, btnHold[i].y, btnHold[i].w, btnHold[i].h, 3, fg);
  tft.setTextColor(fg, bg);
  tft.setTextSize(1);
  int tw = 4 * 6;
  tft.setCursor(btnHold[i].x + (btnHold[i].w - tw)/2, btnHold[i].y + 7);
  tft.print("HOLD");
}

void clearHoldButtons() {
  for (int i = 0; i < 5; i++) {
    tft.fillRect(btnHold[i].x, btnHold[i].y, btnHold[i].w, btnHold[i].h, COL_BG);
  }
}

void drawActionButton() {
  const char* label = (phase == PHASE_HOLD) ? "DRAW" : "DEAL";
  tft.fillRoundRect(btnDeal.x, btnDeal.y, btnDeal.w, btnDeal.h, 5, COL_YELLOW);
  tft.drawRoundRect(btnDeal.x, btnDeal.y, btnDeal.w, btnDeal.h, 5, COL_YELLOW);
  tft.setTextColor(COL_BG, COL_YELLOW);
  tft.setTextSize(2);
  int tw = strlen(label) * 12;
  tft.setCursor(btnDeal.x + (btnDeal.w - tw)/2, btnDeal.y + 10);
  tft.print(label);

  // BET Buttons
  tft.fillRoundRect(btnBetDown.x, btnBetDown.y, btnBetDown.w, btnBetDown.h, 5, COL_DARK);
  tft.drawRoundRect(btnBetDown.x, btnBetDown.y, btnBetDown.w, btnBetDown.h, 5, COL_YELLOW);
  tft.setTextColor(COL_YELLOW, COL_DARK);
  tft.setTextSize(1);
  tft.setCursor(btnBetDown.x+14, btnBetDown.y+13);
  tft.print("BET-");

  tft.fillRoundRect(btnBetUp.x, btnBetUp.y, btnBetUp.w, btnBetUp.h, 5, COL_DARK);
  tft.drawRoundRect(btnBetUp.x, btnBetUp.y, btnBetUp.w, btnBetUp.h, 5, COL_YELLOW);
  tft.setTextColor(COL_YELLOW, COL_DARK);
  tft.setCursor(btnBetUp.x+10, btnBetUp.y+13);
  tft.print("BET+");

  // ALL IN Button (orange)
  tft.fillRoundRect(btnAllIn.x, btnAllIn.y, btnAllIn.w, btnAllIn.h, 5, COL_ORANGE);
  tft.drawRoundRect(btnAllIn.x, btnAllIn.y, btnAllIn.w, btnAllIn.h, 5, COL_RED);
  tft.setTextColor(COL_BG, COL_ORANGE);
  tft.setTextSize(1);
  tft.setCursor(btnAllIn.x+5, btnAllIn.y+8);
  tft.print("ALL");
  tft.setCursor(btnAllIn.x+8, btnAllIn.y+20);
  tft.print("IN");
}

void drawStats() {
  tft.fillRect(110, 196, 60, 34, COL_BG);
  tft.setTextSize(1);
  tft.setTextColor(COL_YELLOW, COL_BG);
  tft.setCursor(112, 198);
  tft.print("GUT:");
  tft.setTextSize(1);
  tft.setCursor(112, 210);
  tft.print(String(balance)+"E");
  tft.setCursor(112, 222);
  tft.print("BET:"+String(bet)+"E");
}

void drawMsg() {
  tft.fillRect(0, 232, 320, 8, COL_BG);
  tft.setTextSize(1);
  tft.setTextColor(COL_YELLOW, COL_BG);
  int tw = lastMsg.length() * 6;
  tft.setCursor(max(0,(320-tw)/2), 232);
  tft.print(lastMsg);
}
