# OAM-WeatherWN90LP – Fork mit Fix für die Windrichtung

Fork von [OpenKNX/OAM-WeatherWN90LP](https://github.com/OpenKNX/OAM-WeatherWN90LP), Branch `v1dev`.
OpenKNX-Applikation für die Wetterstation **Ecowitt WN90LP** (RS485/Modbus RTU) am **OpenKNX REG1 RS485 Gateway**.

Dieser Fork enthält genau eine inhaltliche Änderung gegenüber dem Original: den Fix für die Windrichtung aus
[Issue #1](https://github.com/OpenKNX/OAM-WeatherWN90LP/issues/1). Ansonsten entspricht `v1dev` hier dem Original-Branch `v1dev`
(Stand `b0c86b8`, „v1.1.0-Dev“, Quellcode identisch mit Release `v0.4.0`).

## Das Problem

Die WN90LP liefert die Windrichtung in Register `0x016B` als **0–359°**. Die Firmware hat den Wert intern in einem
`uint8_t` (0–255) gespeichert. Dadurch:

| Station | bisher auf dem Bus | mit Fix |
|---|---|---|
| 100° | 100° | 100° |
| 255° | verworfen (0xFF war die Kennung für „ungültig“) | 255° |
| 300° | **44°** | 300° |
| 359° | **103°** | 359° |

Wind aus etwa WSW über W bis N (256–359°) erschien also als N bis O. Das lässt sich nachträglich nicht herausrechnen,
weil 44° sowohl 44° als auch 300° gewesen sein kann.

Die Skalierung auf den Bus (DPT 5.003, 0–360° → 0–255) übernimmt der KNX-Stack, und die ist korrekt. Der Fehler lag davor.

## Die Änderung

- Windrichtung durchgehend `uint16_t` statt `uint8_t`: `ReadWindDir()`, `GetWindDir()`, `SetWindDir()`, `m_winddir`,
  `HWSensors::GetWindDir()`, `loop_winddir()`, `m_winddir_last_send_value`
- Benannte Konstanten `WINDDIR_MAX = 359` und `WINDDIR_INVALID = 0xFFFF` (in `HWSensorchannel.h`) statt verstreuter `0xff`
- `ReadWindDir()` gibt nur geprüfte Gradwerte (0–359) oder `WINDDIR_INVALID` zurück, nie den rohen Registerwert
- `loop_winddir()` akzeptiert nur `winddir <= WINDDIR_MAX`
- Kalibrierung modulo 360: `(winddir + Kalibrierwert) % 360` statt eines Überlaufs bei 256
- „Senden bei Änderung“ nutzt den kürzesten Winkelabstand: 359° → 2° zählt als 3°, nicht als 357°

Umfang: 6 Dateien in `src/`, +40 / −28 Zeilen. Details mit Vorher/Nachher-Code: [doc/FIX-Windrichtung.md](doc/FIX-Windrichtung.md)

**Keine Änderung an ETS/knxprod.** Kommunikationsobjekte, Parameter und Datenpunkttypen bleiben gleich.
Eine vorhandene ETS-Applikation 0.4.0 bleibt passend.

## Prüfung des gesamten `v1dev`-Stands auf Nebenwirkungen

Ich habe den ganzen Branch darauf geprüft, ob die Typänderung an anderer Stelle etwas beeinflusst:

| Bereich | Ergebnis |
|---|---|
| Alle Verwendungen der Windrichtung | Vollständig umgestellt; keine weitere Stelle liest oder schreibt den Wert |
| Flash-Speicherung (`save()`/`restore()`, `flashSize()`, `W90_FLASH_VERSION`) | Windrichtung wird **nicht** im Flash gespeichert → Flash-Layout unverändert, keine Versionserhöhung nötig, gespeicherte Min/Max- und Regenwerte bleiben erhalten |
| Thread-Sicherheit | Zugriff weiterhin über denselben Mutex (`mx8`); 16-Bit-Zugriffe sind auf dem RP2040 atomar |
| KNX-Objekt / DPT 5.003 | Der Stack nimmt 0–360° entgegen (`valueToBusValueUnsigned8`, case 3); `uint16_t` wird über den passenden `KNXValue`-Konstruktor übergeben |
| ETS-Parameter „von mehr als“ / „Messwert kalibrieren“ | Weiterhin 0–255°; beide funktionieren mit dem neuen Code (Kalibrierung jetzt korrekt modulo 360) |
| Build | `release_REG1-GW-RS485` und `develop_REG1-GW-RS485_USB` erfolgreich, keine neuen Warnungen |
| Strenger Build (`-Wall -Wextra -Wconversion`) | Keine neuen Befunde in den geänderten Stellen (siehe unten) |

**Bestehende Auffälligkeiten im Original**, die mit dem Fix nichts zu tun haben und die Lauffähigkeit nach meiner Prüfung
nicht beeinträchtigen. Sie sind hier nur der Vollständigkeit halber genannt und bewusst **nicht** geändert:

- `ModbusMaster.cpp`, `requestFrom()`: gibt eine nicht initialisierte Variable zurück (Compiler-Warnung). Die Funktion wird
  im Projekt nirgends aufgerufen (Überbleibsel der ModbusMaster-Bibliothek).
- `Sensorchannel.cpp`, LED-Status (`switch`): Der Fall `ACTIVITY` fällt bewusst in `OK` durch. Das ist so gewollt, nur ohne
  `[[fallthrough]]`-Kennzeichnung.
- `HWSensorchannel::GetRain()`: Der `int32_t`-Wert läuft intern über ein `float`. Relevant wäre das erst ab rund
  167 km Regen, also praktisch nie.
- Diverse `double`→`float`-Umwandlungen bei Messwerten: unkritisch (Nachkommastellen jenseits der Messgenauigkeit).

## Bauen

Wie beim Original (PlatformIO, RP2040). Die Bibliotheken werden als Geschwisterverzeichnisse erwartet (siehe
`dependencies.txt` und die Symlinks in `lib/`), z. B. per OpenKNX-Skripten in `scripts/` oder manuell:

```bash
pio run -e release_REG1-GW-RS485        # Firmware für das REG1 RS485 Gateway
pio run -e develop_REG1-GW-RS485_USB    # Debug-Build mit USB-Konsole
```

Ergebnis: `.pio/build/<env>/firmware.uf2`

## Getestet / nicht getestet

- ✅ Build beider REG1-Umgebungen
- ✅ Umrechnung nachgerechnet: 300° → Bus 213 → 300°, 359° → 254 → 359°, 255° gültig, Kalibrierung 300° + 200° = 140°, Abstand 359° → 2° = 3°
- ❌ **Nicht auf echter Hardware getestet.** Bitte vor dem produktiven Einsatz mit der Station prüfen.

## Hinweis zur Versionsnummer

Die Firmware meldet weiterhin dieselbe Applikationsversion wie das Original. Damit bleibt die ETS-Applikation passend,
der Fork ist aber am Gerät nicht von der Originalversion zu unterscheiden. Sobald der Fix im Original übernommen ist,
sollte man wieder auf die offizielle Version wechseln.

---

Original-README:

> # SEN-UP1-8xTH
> OpenKNX 8channel Temperature/Humidity Sensor
>
> see https://github.com/OpenKNX/OpenKNX/wiki/SEN-UP1-8xTH
