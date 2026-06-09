# Documentation for the SlotPoker Game

Inhalt dieser Dokumentation:
1. [Idee und Projektstart](idee-und-projektstart)
2. [Planung und Recherche](#planung-und-recherche)
3. ff
4. ff
5. [Anforderungen](#anforderungen)
6. [Abgedeckte Kriterien](#abgedeckte-kriterien)
7. [Projektzeitplan](#projektzeitplan)
8. [Probleme und Lösungen](#probleme-und-lösungen)
9. [Ressourcen](#ressourcen)
10. [Recherche](#recherche)

# Idee und Projektstart
### Erste Ideen
Digitale Slot Maschine mit einem Pull Lever, um eine "Runde" zu starten und mit Buttons um den Einsatz zu wählen. Außerdem gibt es virtuelle Credits, welche angezeigt werden können. Bei Gewinn/Jackpot soll Gewinn-Animation abgespielt werden (+ Online Statistik -> regelmäßig aktualisieren)
### Zusammenfassung des Projekts
***Ziel dieses Projekts ist es, ein neues Spiel - eine Kombination aus Poker Texas Hold'em und einem Slot-Game - auf einer online Website zu hosten und dieses neben der Website auch minimalistisch auf einem LCD Display darzustellen. Dazu werden die essentiellen Daten mittels einem json File von der VM, auf welcher die Website mit allen Nutzerdaten gehostet wird, auf das Display geschickt bzw. von dem ESP/Display mit einem Skript von der Website gepullt.***

# Planung und Recherche
***Vor dem Arbeitsstart haben wir uns erstmals angesehen, welche Komponenten wir für eine gute Umsetzung dieses Projektes brauchen und welche Technologien am besten geeignet sind. Wir haben uns schließlich entschieden, das Spiel auf einer normalen HTML+CSS Website umzusetzen und für das Backend Python zu verwenden. Außerdem kam es zu der Entscheidung, ein LCD Display (2.8 inch) für den HW-Teil einzubauen, auf welchem später eine minimalistische Version des Spiels ausführbar sein soll.***



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
- Benutzerverwaltung + UI damit man ohne hebel spielen kann
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
- ...
