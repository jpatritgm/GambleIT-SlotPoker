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
