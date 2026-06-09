# Documentation for the SlotPoker Game

Inhalt dieser Dokumentation:
1. [Idee und Projektstart](#idee-und-projektstart)
2. [Planung und Recherche](#planung-und-recherche)
3. [Arbeitsunterteilung](#arbeitsunterteilung)
4. [Anforderungen](#anforderungen)
5. [Projektzeitplan](#projektzeitplan)
6. [Probleme und Lösungen](#probleme-und-lösungen)
7. [Ressourcen](#ressourcen)
8. [Recherche](#recherche)

# Idee und Projektstart
### Erste Ideen
Digitale Slot Maschine mit einem Pull Lever, um eine "Runde" zu starten und mit Buttons um den Einsatz zu wählen. Außerdem gibt es virtuelle Credits, welche angezeigt werden können. Bei Gewinn/Jackpot soll Gewinn-Animation abgespielt werden (+ Online Statistik -> regelmäßig aktualisieren)
### Zusammenfassung des Projekts
***Ziel dieses Projekts ist es, ein neues Spiel - eine Kombination aus Poker Texas Hold'em und einem Slot-Game - auf einer online Website zu hosten und dieses neben der Website auch minimalistisch auf einem LCD Display darzustellen. Dazu werden die essentiellen Daten mittels einem json File von der VM, auf welcher die Website mit allen Nutzerdaten gehostet wird, auf das Display geschickt bzw. von dem ESP/Display mit einem Skript von der Website gepullt.***

# Planung und Recherche
***Vor dem Arbeitsstart haben wir uns erstmals angesehen, welche Komponenten wir für eine gute Umsetzung dieses Projektes brauchen und welche Technologien am besten geeignet sind. Wir haben uns schließlich entschieden, das Spiel auf einer normalen HTML+CSS Website umzusetzen und für das Backend Python zu verwenden. Außerdem kam es zu der Entscheidung, ein LCD Display (2.8 inch) für den HW-Teil einzubauen, auf welchem später eine minimalistische Version des Spiels ausführbar sein soll.***

# Arbeitsunterteilung
### Web
***Es wurde eine Website erstellt, auf welcher das Spiel und ein Dashboard mit Gewinn/Verlust, Credits/Kontostand vorhanden ist. Diese wird mithilfe von Arduino Code online, sowie auch auf dem Display gehostet.***
> HTML Code für die Website
```
<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Video Poker – Dashboard</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Bebas+Neue&family=DM+Mono:wght@400;500&display=swap');

  * { margin:0; padding:0; box-sizing:border-box; }

  body {
    background: #080118;
    min-height: 100vh;
    display: flex; flex-direction: column; align-items: center;
    font-family: 'DM Mono', monospace;
  }

  .container { width:100%; max-width:800px; padding:40px 24px 60px; }

  /* Header */
  header { text-align:center; margin-bottom:32px; }
  h1 {
    font-family:'Bebas Neue',sans-serif;
    font-size: clamp(36px,8vw,56px);
    letter-spacing:.12em; color:#f8ff2f;
    text-shadow: 0 0 40px rgba(248,255,47,.5);
  }
  .subtitle { font-size:11px; letter-spacing:.3em; text-transform:uppercase; color:rgba(248,255,47,.35); margin-top:4px; }
  .status-pill {
    display:inline-flex; align-items:center; gap:7px;
    margin-top:12px; padding:5px 14px;
    border:1px solid rgba(248,255,47,.15); border-radius:99px;
    font-size:11px; letter-spacing:.15em; color:rgba(248,255,47,.45);
  }
  .dot { width:7px; height:7px; border-radius:50%; background:#555; transition:background .4s; }
  .dot.on  { background:#4eff91; box-shadow:0 0 8px #4eff91; }
  .dot.off { background:#ff4444; }

  .divider { width:100%; height:1px; background:linear-gradient(90deg,transparent,#f8ff2f,transparent); margin:28px 0; }

  /* Karten */
  .hand-section { margin-bottom:24px; }
  .hand-label { font-size:10px; letter-spacing:.25em; text-transform:uppercase; color:rgba(248,255,47,.35); margin-bottom:12px; }

  .cards {
    display:flex; gap:10px; justify-content:center;
  }

  .card {
    width:90px; height:130px;
    background:rgba(248,255,47,.04);
    border:2px solid rgba(248,255,47,.2);
    border-radius:10px;
    display:flex; flex-direction:column;
    align-items:center; justify-content:center;
    position:relative;
    transition: border-color .3s, box-shadow .3s, transform .3s;
    animation: cardIn .35s ease;
  }
  @keyframes cardIn { from { opacity:0; transform:translateY(-12px) rotateX(30deg); } to { opacity:1; transform:none; } }

  .card.red  { color: #ff5555; }
  .card.black{ color: #f0f0f0; }
  .card.empty{ color: rgba(248,255,47,.15); border-color:rgba(248,255,47,.08); }

  .card-suit { font-size:13px; position:absolute; top:7px; left:9px; opacity:.8; }
  .card-val  { font-size:30px; font-weight:500; letter-spacing:-.01em; }
  .card-bot  { font-size:13px; position:absolute; bottom:7px; right:9px; opacity:.8; }

  /* Result Badge */
  .result-badge {
    text-align:center; margin-bottom:24px; min-height:38px;
  }
  .badge {
    display:inline-block;
    padding:8px 24px;
    border:1px solid rgba(248,255,47,.3);
    border-radius:99px;
    font-family:'Bebas Neue',sans-serif;
    font-size:22px; letter-spacing:.15em;
    color:#f8ff2f;
    text-shadow:0 0 16px rgba(248,255,47,.4);
    transition: all .4s;
  }
  .badge.win  { border-color:#f8ff2f; box-shadow:0 0 24px rgba(248,255,47,.2); }
  .badge.lose { color:rgba(248,255,47,.3); border-color:rgba(248,255,47,.1); }
  .badge.idle { color:rgba(248,255,47,.3); border-color:rgba(248,255,47,.08); }

  /* Stats */
  .stats-grid { display:grid; grid-template-columns:1fr 1fr 1fr 1fr; gap:12px; margin-bottom:24px; }

  .stat {
    background:rgba(248,255,47,.03);
    border:1px solid rgba(255,221,0,.2);
    border-radius:12px; padding:16px;
    position:relative; overflow:hidden;
  }
  .stat::before { content:''; position:absolute; top:0; left:0; right:0; height:2px; background:linear-gradient(90deg,transparent,#f8ff2f44,transparent); }
  .stat-lbl { font-size:9px; letter-spacing:.25em; text-transform:uppercase; color:rgba(248,255,47,.35); margin-bottom:8px; }
  .stat-val { font-size:22px; font-weight:500; color:#f8ff2f; }
  .stat-val.eur::before { content:'€'; font-size:12px; opacity:.55; margin-right:2px; vertical-align:2px; }
  .stat-sub { font-size:10px; color:rgba(248,255,47,.3); margin-top:4px; }

  /* Auszahlungstabelle */
  .payout-section { margin-top:28px; }
  .section-title { font-size:10px; letter-spacing:.3em; text-transform:uppercase; color:rgba(248,255,47,.3); margin-bottom:12px; }

  .payout-table { width:100%; border-collapse:collapse; font-size:12px; }
  .payout-table td { padding:6px 10px; border-bottom:1px solid rgba(248,255,47,.06); color:rgba(248,255,47,.6); }
  .payout-table td:last-child { text-align:right; color:#f8ff2f; }
  .payout-table tr.active td { color:#f8ff2f; background:rgba(248,255,47,.06); }
  .payout-table tr.active td:first-child::before { content:'▶ '; }

  /* Verlauf */
  .history-section { margin-top:28px; }
  .history-list { display:flex; flex-direction:column; gap:6px; max-height:240px; overflow-y:auto; }
  .history-list::-webkit-scrollbar { width:3px; }
  .history-list::-webkit-scrollbar-thumb { background:rgba(248,255,47,.2); border-radius:99px; }

  .history-item {
    display:flex; align-items:center; gap:10px;
    padding:8px 12px;
    background:rgba(248,255,47,.02); border:1px solid rgba(248,255,47,.06);
    border-radius:8px; font-size:11px;
    animation:fadeIn .3s ease;
  }
  @keyframes fadeIn { from{opacity:0;transform:translateY(-4px)} to{opacity:1;transform:none} }
  .h-num   { color:rgba(248,255,47,.2); min-width:24px; }
  .h-cards { letter-spacing:2px; color:rgba(248,255,47,.7); font-size:12px; }
  .h-hand  { color:rgba(248,255,47,.5); flex:1; }
  .h-amt   { font-weight:500; }
  .h-amt.win  { color:#f8ff2f; }
  .h-amt.lose { color:rgba(248,255,47,.25); }
</style>
</head>
<body>
<div class="container">

  <header>
    <h1>Video Poker</h1>
    <div class="subtitle">Live Dashboard</div>
    <div class="status-pill">
      <div class="dot" id="dot"></div>
      <span id="statusTxt">Verbinde...</span>
    </div>
  </header>

  <div class="divider"></div>

  <!-- Karten -->
  <div class="hand-section">
    <div class="hand-label">Letzte Hand</div>
    <div class="cards" id="cards">
      <div class="card empty"><span class="card-val">?</span></div>
      <div class="card empty"><span class="card-val">?</span></div>
      <div class="card empty"><span class="card-val">?</span></div>
      <div class="card empty"><span class="card-val">?</span></div>
      <div class="card empty"><span class="card-val">?</span></div>
    </div>
  </div>

  <!-- Result Badge -->
  <div class="result-badge">
    <span class="badge idle" id="badge">Warte auf Spiel...</span>
  </div>

  <!-- Stats -->
  <div class="stats-grid">
    <div class="stat">
      <div class="stat-lbl">Guthaben</div>
      <div class="stat-val eur" id="balance">1000</div>
      <div class="stat-sub">verfügbar</div>
    </div>
    <div class="stat">
      <div class="stat-lbl">Letzter Gewinn</div>
      <div class="stat-val eur" id="lastWin">—</div>
      <div class="stat-sub" id="lastWinSub">noch kein Spiel</div>
    </div>
    <div class="stat">
      <div class="stat-lbl">Gewinn-Quote</div>
      <div class="stat-val" id="ratio">0/0</div>
      <div class="stat-sub" id="pct">— noch kein Spiel</div>
    </div>
    <div class="stat">
      <div class="stat-lbl">Gesamtgewinn</div>
      <div class="stat-val eur" id="totalWon">0</div>
      <div class="stat-sub">aufsummiert</div>
    </div>
  </div>

  <!-- Auszahlungstabelle -->
  <div class="payout-section">
    <div class="section-title">Auszahlungstabelle (× Einsatz)</div>
    <table class="payout-table" id="payoutTable">
      <tr data-mult="800"><td>Royal Flush</td><td>× 800</td></tr>
      <tr data-mult="50"> <td>Straight Flush</td><td>× 50</td></tr>
      <tr data-mult="25"> <td>Four of a Kind</td><td>× 25</td></tr>
      <tr data-mult="9">  <td>Full House</td><td>× 9</td></tr>
      <tr data-mult="6">  <td>Flush</td><td>× 6</td></tr>
      <tr data-mult="4">  <td>Straight</td><td>× 4</td></tr>
      <tr data-mult="3">  <td>Three of a Kind</td><td>× 3</td></tr>
      <tr data-mult="2">  <td>Two Pair</td><td>× 2</td></tr>
      <tr data-mult="1">  <td>Jacks or Better</td><td>× 1</td></tr>
      <tr data-mult="0">  <td>Kein Gewinn</td><td>—</td></tr>
    </table>
  </div>

  <!-- Verlauf -->
  <div class="history-section">
    <div class="section-title">Spielverlauf</div>
    <div class="history-list" id="historyList">
      <div style="color:rgba(248,255,47,.2);font-size:12px;padding:8px 0;">Noch keine Runden gespielt.</div>
    </div>
  </div>

</div>

<script>
  const WS_URL = 'ws://localhost:8765';
  let ws;
  let wins=0, games=0, totalWon=0, lastMult=null;

  // Suit → HTML-Symbol
  const SUIT_SYM = { H:'♥', K:'♦', P:'♠', T:'♣' };
  const RED_SUITS = new Set(['H','K']);

  // Karte z.B. "HJ" → {suit:"H", val:"J"}
  function parseCard(str) {
    return { suit: str[0], val: str.slice(1) };
  }

  function renderCards(cardStrs) {
    const container = document.getElementById('cards');
    container.innerHTML = '';
    cardStrs.forEach(str => {
      const c   = parseCard(str);
      const red = RED_SUITS.has(c.suit);
      const div = document.createElement('div');
      div.className = 'card ' + (red ? 'red' : 'black');
      div.innerHTML = `
        <span class="card-suit">${SUIT_SYM[c.suit]}</span>
        <span class="card-val">${c.val}</span>
        <span class="card-bot">${SUIT_SYM[c.suit]}</span>
      `;
      container.appendChild(div);
    });
  }

  function highlightPayout(mult) {
    document.querySelectorAll('#payoutTable tr').forEach(tr => {
      tr.classList.toggle('active', parseInt(tr.dataset.mult) === mult);
    });
  }

  function applyResult(d) {
    games = d.games; wins = d.wins;
    const win = d.win || 0;
    totalWon += win;
    lastMult  = d.mult ?? null;

    // Karten anzeigen
    if (d.cards) renderCards(d.cards);

    // Badge
    const badge = document.getElementById('badge');
    if (win > 0) {
      badge.textContent = (d.result || 'Gewinn') + ' · +€' + win;
      badge.className = 'badge win';
    } else {
      badge.textContent = 'Kein Gewinn';
      badge.className = 'badge lose';
    }

    // Payout-Tabelle
    if (d.mult !== undefined) highlightPayout(d.mult);

    // Stats
    document.getElementById('balance').textContent  = d.balance;
    document.getElementById('lastWin').textContent  = win > 0 ? win : '0';
    document.getElementById('lastWinSub').textContent = win > 0
      ? (d.result || 'Gewinn')
      : 'nichts gewonnen';
    document.getElementById('totalWon').textContent = totalWon;

    const pct = games ? Math.round(wins/games*100) : 0;
    document.getElementById('ratio').textContent = wins+'/'+games;
    document.getElementById('pct').textContent   = games ? pct+'% Trefferquote' : '— noch kein Spiel';

    addHistory(d);
  }

  function addHistory(d) {
    const list = document.getElementById('historyList');
    if (list.querySelector('div[style]')) list.innerHTML = '';

    const cardStr = d.cards ? d.cards.join(' ') : '— — — — —';
    const isWin   = (d.win || 0) > 0;
    const item    = document.createElement('div');
    item.className = 'history-item';
    item.innerHTML = `
      <span class="h-num">#${games}</span>
      <span class="h-cards">${cardStr}</span>
      <span class="h-hand">${d.result || ''}</span>
      <span class="h-amt ${isWin?'win':'lose'}">${isWin ? '+€'+d.win : '-€'+(d.bet||'?')}</span>
    `;
    list.insertBefore(item, list.firstChild);
    if (list.children.length > 50) list.removeChild(list.lastChild);
  }

  function setStatus(on, txt) {
    document.getElementById('dot').className   = 'dot ' + (on?'on':'off');
    document.getElementById('statusTxt').textContent = txt;
  }

  function connect() {
    ws = new WebSocket(WS_URL);
    ws.onopen  = () => setStatus(true,  'ESP32 verbunden');
    ws.onclose = () => { setStatus(false, 'Keine Verbindung – reconnect...'); setTimeout(connect, 3000); };
    ws.onerror = () => setStatus(false, 'Bridge nicht erreichbar');
    ws.onmessage = (e) => {
      const d = JSON.parse(e.data);
      if (d.type === 'state') {
        document.getElementById('balance').textContent = d.balance;
        wins=d.wins; games=d.games;
        const pct = games ? Math.round(wins/games*100):0;
        document.getElementById('ratio').textContent = wins+'/'+games;
        document.getElementById('pct').textContent   = games ? pct+'% Trefferquote':'— noch kein Spiel';
      }
      if (d.type === 'result') applyResult(d);
    };
  }

  connect();
</script>
</body>
</html>
```
> Python Code für die Verbindung zwischen ESP32 und Browser
```
"""
bridge.py – Verbindet ESP32 (UART) mit dem Browser (WebSocket + HTTP)

Installation:  pip install pyserial websockets
Starten:       python bridge.py
Browser:       http://localhost:8080
"""

import asyncio, json, serial, threading, http.server, socketserver, os, websockets

SERIAL_PORT = "COM5"      # ← deinen Port anpassen
SERIAL_BAUD = 115200
WS_PORT     = 8765
HTTP_PORT   = 8080

connected_ws = set()

# Spielzustand (nur für initialen State-Push)
state = {"balance": 1000, "wins": 0, "games": 0}

# ── Broadcast ─────────────────────────────────────────────────
async def broadcast(msg: str):
    for ws in list(connected_ws):
        try:    await ws.send(msg)
        except: connected_ws.discard(ws)

# ── Serial ────────────────────────────────────────────────────
def serial_thread(loop):
    try:
        ser = serial.Serial(SERIAL_PORT, SERIAL_BAUD, timeout=1)
        print(f"[Serial] Verbunden mit {SERIAL_PORT}")
    except serial.SerialException as e:
        print(f"[Serial] FEHLER: {e}")
        print(f"  Passe SERIAL_PORT in bridge.py an (aktuell: {SERIAL_PORT})")
        return

    while True:
        try:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if not line: continue

            data = json.loads(line)

            if data.get("action") == "result":
                # Zustand aktualisieren
                state["balance"] = data.get("balance", state["balance"])
                state["wins"]    = data.get("wins",    state["wins"])
                state["games"]   = data.get("games",   state["games"])

                # Multiplikator berechnen (für Payout-Tabellen-Highlight)
                win  = data.get("win", 0)
                bet  = data.get("bet", 10)
                mult = round(win / bet) if bet > 0 and win > 0 else 0

                # An Browser weiterleiten
                ws_msg = json.dumps({
                    "type":    "result",
                    "win":     win,
                    "bet":     bet,
                    "balance": data.get("balance"),
                    "wins":    data.get("wins"),
                    "games":   data.get("games"),
                    "result":  data.get("result", ""),
                    "cards":   data.get("cards", []),
                    "mult":    mult,
                })
                asyncio.run_coroutine_threadsafe(broadcast(ws_msg), loop)

        except json.JSONDecodeError: pass
        except Exception as ex: print(f"[Serial] {ex}")

# ── WebSocket ─────────────────────────────────────────────────
async def ws_handler(websocket):
    connected_ws.add(websocket)
    try:
        # Sofort aktuellen Zustand senden
        await websocket.send(json.dumps({
            "type":    "state",
            "balance": state["balance"],
            "wins":    state["wins"],
            "games":   state["games"],
        }))
        async for _ in websocket:
            pass  # Browser schickt keine Befehle mehr
    finally:
        connected_ws.discard(websocket)

# ── HTTP ──────────────────────────────────────────────────────
class Handler(http.server.SimpleHTTPRequestHandler):
    def log_message(self, *args): pass

def run_http():
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    with socketserver.TCPServer(("", HTTP_PORT), Handler) as httpd:
        print(f"[HTTP]   http://localhost:{HTTP_PORT}")
        httpd.serve_forever()

# ── Main ──────────────────────────────────────────────────────
async def main():
    loop = asyncio.get_running_loop()
    threading.Thread(target=serial_thread, args=(loop,), daemon=True).start()
    threading.Thread(target=run_http, daemon=True).start()
    async with websockets.serve(ws_handler, "localhost", WS_PORT):
        print(f"[WS]     ws://localhost:{WS_PORT}")
        print("─────────────────────────────────")
        print("  Browser: http://localhost:8080")
        print("  Strg+C zum Beenden")
        await asyncio.Future()

if __name__ == "__main__":
    asyncio.run(main())
```

### HW/LCD-Display
***Es wurde ein LCD Touch-Display (ESP32E) als HW verwendet, worauf man eine minimale Version unseres Spiels spielen kann, welches der selben Logik, wie dem Spiel auf der Website entspricht.***
> Arduino Code
```
// ============================================================
//  POKER MACHINE – ESP32 + 2.8" ILI9341 (SKU: E32R28T)
//  5-Card Draw Poker mit Hold-Funktion
//
//  Spielablauf:
//  1. DEAL  → 5 Karten werden ausgeteilt
//  2. Spieler wählt Karten zum HALTEN (HOLD-Buttons)
//  3. DRAW  → nicht gehaltene Karten werden ersetzt
//  4. Ergebnis wird berechnet & JSON an PC geschickt
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

// ── Deck ──────────────────────────────────────────────────────
// Farben: H=Herz, K=Karo, P=Pik, T=Treff
// Werte:  2-9, 10=T, J, Q, K, A
const char* SUITS[4]  = {"H","D","S","C"};
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

  if (straight && flush)        return (wheel||vals[0]==9) ? 500 : 30; // Royal/Straight Flush
  if (fours==1)                 return 15;   // Four of a Kind
  if (threes==1 && pairs==1)    return 8;    // Full House
  if (flush)                    return 5;    // Flush
  if (straight)                 return 3;    // Straight
  if (threes==1)                return 2;    // Three of a Kind
  if (pairs==2)                 return 0.5;    // Two Pair
  if (pairs==1) {
    // Nur Jacks or Better (val >= 9)
    for (int i=0;i<13;i++) if(counts[i]==2 && i>=9) return 0;
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
    case 500: return "ROYAL FLUSH";
    case 30:  return "STR. FLUSH";
    case 15:  return "FOUR OF A KIND";
    case 8:   return "FULL HOUSE";
    case 5:   return "FLUSH";
    case 3:   return "STRAIGHT";
    case 2:   return "THREE OF A KIND";
    case 1:   return "TWO PAIR";
    case 0:   return "JACKS OR BETTER";
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
    if (hitBtn(btnBetUp, tx, ty))   { bet = min(balance, bet+10); drawStats(); }
    if (hitBtn(btnBetDown, tx, ty)) { bet = max(10, bet-10);      drawStats(); }
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
    if (hitBtn(btnBetUp, tx, ty))   { bet = min(balance, bet+10); drawStats(); }
    if (hitBtn(btnBetDown, tx, ty)) { bet = max(10, bet-10);      drawStats(); }
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
```
> [!IMPORTANT]
> Für die Entwicklung des Codes wurde teilweise KI zur Hilfe genommen
### Virtualisierung
***Es war geplant, für das Hosting der Webseite(n) eine Azure VM zu verwende, welche aber über längere Zeit mehrere Probleme verursacht hat, weswegen wir uns auf eine andere Lösung umentschieden haben.***
### 3d-Modellierung
> 3d-Gehäuse für Elektronik + Display
<img width="736" height="512" alt="image" src="https://github.com/user-attachments/assets/f70d8958-2a35-401c-aa4b-5c1d166b3961" />


# Anforderungen
### Funktionale Anforderungen
- Funktionierendes Spiel welches als eine Kombination aus Poker und Slots funktioniert
- es werden Daten wie Kontostand gespeichert
- Web-Dashboard
- Gewinnstatistiken
- HW funktional eingebaut (-> LCD Display)

### Nicht-funktionale Anforderungen
- Benutzerfreundlichkeit: eine benutzerfreundliche GUI auf dem Display
- Animationen / Soundeffekte bei Gewinn
- LED Effekte
- Online Multiplayer Leaderboard
- Benutzerverwaltung
- gute UI damit man ohne Hebel spielen kann
- Gewinnraten Analyse


# Projektzeitplan
### Woche 1-4: Ideensammlung, Grundrecherche, Anfertigung des Pflichtenheftes
*In dieser Zeit wurden innerhalb der Gruppe erste Ideen für unser Projekt gesammelt und erste Grundrecherche zu vorstellbaren Teilen aus dem Projekt gemacht. Außerdem haben wir in diesem Zeitraum das Pflichtenheft mit den [Projektanforderungen](#anforderungen) geschrieben.*
### Woche 5-6: Hardwarebereitstellung, Website-Grundstruktur
*In diesen Wochen haben wir uns entschieden, für den HW-Teil ein LCD-Display einzubauen, da ich dieses bereits in Besitz hatte und wir somit kein neues besorgen mussten. Dazu haben wir auch mithilfe von KI eine grundlegende Website entworfen, um ein erstmaliges Bild zu bekommen, wie unsere tatsächliche Website aussehen könnte.*
### Woche 7-9: Eigener Website-Entwurf und Beginn einer einfachen Dokumentation in Obsidian
*Hier wurde eine eigene Main-Website für das unser Spiel entworfen, welche sich einigermaßen an die Struktur und das Design der vorherigen KI-Website hielt. Außerdem wurde eine einfache Dokumentation aller bisher gemachten Schritte in Obsidian begonnen und regelmäßig auf den neusten Stand gebracht.*
### Woche 10-12: Display Einbindung und erste Tests, 3d-Gehäuse Design
*In dieser Zeit haben wir erste Tests mit dem Display gemacht und nach Ideen gesucht, wie wir die Website auf dem Display anzeigen könnten. Wir sind letztendlich zu der Lösung gekommen, dass wir mit einer Kombination aus Arduino Code, html und Python eine minimalistische Version der Website auf das Display hochladen können, während dieser selbe Code auch die eigentliche html Seite hostet. Außerdem wurde in diesem Zeitraum auch das 3d-Gehäuse designed.*
> [!NOTE]
> Für das Finden dieser Lösung wurde KI zur Hilfe genommen
### Woche 12-13: Projektende und übersichtlichere Dokumentation auf Github
*Hier haben wir die letzten Tests von Website und Display und eine übersichtlichere Dokumentation auf Github gemacht.*
### Woche 14: Projektpräsentation
*In der letzten Woche wird unser fertiges Projekt präsentiert.*
# Probleme und Lösungen
### *Problem #1* - Die Buttons
Wir hatten ursprünglich vor, 3-4 Buttons einzubinden, welche den Einsatz beim Spiel kontrollieren, eine Spielstatistik anzeigen, und ein neues Spiel starten können sollten. Jedoch haben wir herausgefunden, dass der ESP beim Display nur 2 Inputs hat und wir somit nur 2 Buttons implementieren hätten können, was nicht ausreichend gewesen wäre. Deshalb haben wir beschlossen, stattdessen "digitale Buttons" zu verwenden, welche direkt auf dem Display sichtbar und klickbar sind. 

### *Problem #2* - Das Gehäuse
Die erste Version des Gehäuses war nicht optimal designed, da es 1. zu klein für das Display war und man 2. auch nur schwer Elektronik/Kabel in den freien Bereich, welcher für die Elektronik gedacht war, geben konnte. Aus diesem Grund musste die Box neu designed und gedruckt werden.

### *Problem 3* - Die VM
Wir hatten uns erst entschieden, für das Hosting der Website eine Azure Ubuntu VM zu verwenden, jedoch hat diese massive Probleme gebracht, wie dass ich mich nicht von meinem Windows Laptop zu der VM verbunden konnte, weil sie angeblich nicht erreichbar und in einem anderen Netzwerk war. Ich habe dies mehrmals überprüft, aber es schien zu kompliziert dieses Problem zu lösen, da wir auch schon etwas Zeitdrang hatten, weshalb wir uns auf die verwendete Lösung, die Arduino-html-Python Kombination zu verwenden, umentschieden haben.
# Ressourcen
### Hardware
- LCD Touch-Display ESP32E
- 1 Laptop zum Hosten der Website und Hochladen des Codes auf ESP32
- (3d-Drucker)
### Software
- Arduino IDE
- VSC für html Code
### Wissen
- Grundlagen für das Programmieren eines Mikrocontrollers
- Grundlagen in html, CSS und Python Programmierung
- Beherrschen eines 3d-Modellierung-Programms (verwendet wurde Autodesk Maya)
- Grundlagen in der Linux Kommandozeile, Wissen über grundlegende Befehle

# Recherche
### Quellen
- Microsoft Azure: https://portal.azure.com
- LCD-Display: https://www.lcdwiki.com/2.8inch_ESP32-32E_Display
- CSS Guide: https://developer.mozilla.org/en-US/docs/Web/CSS/Guides, https://developer.mozilla.org/de/docs/Web/CSS/Reference/Values/clamp
- Python: https://realpython.com, https://websockets.readthedocs.io/en/stable/, https://pyserial.readthedocs.io/en/latest/
- Funktionen für Karten auf Display: https://github.com/Bodmer/TFT_espi
