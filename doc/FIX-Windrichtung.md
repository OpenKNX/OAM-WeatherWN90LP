# WN90LP: Fix Windrichtung 256–359° (uint8_t → uint16_t)

**Basis:** `v1dev` (`b0c86b8`, passt unverändert auch auf `v0.4.0`) · **Issue:** [OpenKNX/OAM-WeatherWN90LP#1](https://github.com/OpenKNX/OAM-WeatherWN90LP/issues/1) · **Umfang:** 6 Dateien, +40 / −28 · **knxprod:** keine Änderung nötig

## Problem

Die Station liefert in Register 0x016B 0–359°. Die Firmware hat den Wert in `uint8_t` gespeichert:

| Station | bisher auf dem Bus | mit Fix |
|---|---|---|
| 100° | 100° | 100° |
| 255° | verworfen (= Kennung „ungültig“ 0xFF) | 255° |
| 300° | **44°** | 300° |
| 359° | **103°** | 359° |

Die Skalierung auf den Bus (DPT 5.003, 0–360° → 0–255) übernimmt der KNX-Stack, und die ist korrekt. Der Fehler lag davor.

## Änderungen auf einen Blick

| Datei | Was | vorher | nachher |
|---|---|---|---|
| `HWSensorchannel.h` | neue Konstanten | – | `WINDDIR_MAX = 359`, `WINDDIR_INVALID = 0xFFFF` |
| `HWSensorchannel.h` | `m_winddir` | `uint8_t = 0xff` | `uint16_t = WINDDIR_INVALID` |
| `HWSensorchannel.h/.cpp` | `ReadWindDir()` | gibt `uint8_t` zurück | gibt `uint16_t` zurück |
| `HWSensorchannel.h/.cpp` | `GetWindDir()` / `SetWindDir()` | `uint8_t` | `uint16_t` |
| `HWSensors.h/.cpp` | `GetWindDir(channel)` | `uint8_t`, Fehler `0xff` | `uint16_t`, Fehler `WINDDIR_INVALID` |
| `Sensorchannel.h/.cpp` | `loop_winddir()`, `m_winddir_last_send_value` | `uint8_t` | `uint16_t` |
| `Sensorchannel.cpp` | Gültigkeitsprüfung | `winddir != 0xff` | `winddir <= WINDDIR_MAX` |
| `Sensorchannel.cpp` | Kalibrierung | `(uint8_t)(winddir + align)`, läuft modulo 256 | `(winddir + align) % 360` |
| `Sensorchannel.cpp` | Senden bei Änderung | einfache Differenz (359° → 2° = 357°) | kürzester Winkelabstand (359° → 2° = 3°) |

## Kern der Änderung

**`ReadWindDir()`** gibt nur noch geprüfte Gradwerte zurück, nie den rohen Registerwert:

```cpp
// vorher
if (data > 359)
    return 0xff;
uint8_t rawdata = data;          // 256..359 -> 0..103
return rawdata;

// nachher
if (data > WINDDIR_MAX)
    return WINDDIR_INVALID;
const uint16_t winddir = data;   // 0..359 bleibt erhalten
return winddir;
```

**`loop_winddir()`** rechnet die Kalibrierung modulo 360 und nutzt beim Senden bei Änderung den kürzesten Abstand:

```cpp
uint16_t winddir_aligned = static_cast<uint16_t>((winddir + ParamW90_SensorWindDirAlign_) % 360);

uint16_t diff = winddir > m_winddir_last_send_value ? winddir - m_winddir_last_send_value
                                                    : m_winddir_last_send_value - winddir;
if (diff > 180)
    diff = 360 - diff;

KoW90_SensorWindDir_.value(winddir_aligned, WindDirKODPT);
```

## Nicht geändert

- **ETS / knxprod:** Der Kalibrierparameter bleibt 0–255°, eine negative Korrektur ist weiterhin nicht möglich. Das ginge nur mit einer neuen knxprod.
- **Andere Messwerte:** unverändert.

## Getestet

- Build `develop_REG1-GW-RS485_USB` und `release_REG1-GW-RS485`: erfolgreich, keine neuen Warnungen in den geänderten Dateien
- Umrechnung separat nachgerechnet: 300° → Bus 213 → 300°, 359° → 254 → 359°, Kalibrierung 300° + 200° = 140°
- **Nicht getestet:** auf echter Hardware
