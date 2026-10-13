# FloodPool

A community flood fund with no treasurer. Neighbourhoods in Indonesia already pool money for disasters (*gotong royong*), but someone has to hold the cash and decide who gets paid. FloodPool replaces that person with water-level sensors (ESP32) and a smart contract: when trusted sensors report flooding above a threshold for long enough, the pool pays out automatically.

Current stage: **sensor node for a desk demo (a bucket of water)**.

## Contents
- `firmware/sensor-banjir/` – ESP32 program (`sensor-banjir.ino`), testable logic (`logika.h`), and the Wokwi circuit (`diagram.json`, `libraries.txt`)
- `firmware/test/` – logic tests that run on a laptop: `make -C firmware/test`
- `firmware/upload.sh` – compile, upload to a real ESP32, and open the serial monitor
- `docs/panduan-rakit.html` – illustrated step-by-step wiring guide (Indonesian)

Identifiers in the code are in Indonesian: *tinggi* = height, *jarak* = distance, *pelampung* = float switch, *ambang* = threshold, *sepakat* = agree, *AMAN / SIAGA / BANJIR* = safe / alert / flood.

## How it works
1. An ultrasonic sensor looks down at the water and measures the distance. Water height = mounting height − distance. Each reading is the median of 5 pings, so one bad echo is ignored.
2. A float switch mounted at the threshold height cross-checks the ultrasonic sensor. If they disagree, the node reports it (`"sepakat": false`), which later lowers the sensor's reliability score.
3. Water above the threshold switches the status to **SIAGA** (alert). If it stays there for the set duration, the status becomes **BANJIR** (flood): the LED stays on and the buzzer sounds.
4. Every reading is printed as one JSON line. In the next stage these lines are signed on the device and sent to the server.

## Simulate in Wokwi (no hardware needed)
1. Open https://wokwi.com/projects/new/esp32
2. In the `sketch.ino` tab, paste the contents of `sensor-banjir.ino`
3. In the `diagram.json` tab, paste the contents of `diagram.json`
4. Use the ▾ button next to the tabs → **New file…** → `logika.h`, and paste its contents. Do the same for `libraries.txt`.
5. Press ▶ (Start). Click the HC-SR04 sensor and move the distance slider:
   - 35 cm → water height 5 cm → **AMAN** (safe)
   - 20 cm → water height 20 cm → **SIAGA** (LED blinks), then after 10 seconds **BANJIR** (LED on, buzzer sounds)
   - flip the slide switch (the float) to see the `OK` / `BEDA` (mismatch) cross-check

## Wiring the real hardware (ESP32 DevKit, 38 pins)

| Part | Part pin → ESP32 pin |
|---|---|
| HC-SR04 | VCC → `5V`, GND → `GND`, TRIG → `D25` |
| | ECHO → 1 kΩ resistor → **midpoint** → 2 kΩ resistor → `GND`; midpoint → `D26` |
| Float switch | one wire → `D27`, the other → `GND` |
| OLED SSD1306 (4 pins) | VCC → `3V3`, GND → `GND`, SDA → `D13`, SCL → `D14` (left-side pins: the right-side pins are covered when the board spans two breadboards) |
| Active buzzer module (3 pins) | VCC → `3V3`, GND → `GND`, I/O → `D33` |
| Red LED | long leg → 220 Ω resistor → `D32`; short leg → `GND` |

Pin labels may read `D25`, `IO25`, `G25`, or `P25`. Do not use the `SD0`–`SD3`, `CLK`, or `CMD` pins. The ECHO voltage divider is required: the HC-SR04 outputs 5 V, and ESP32 pins only tolerate 3.3 V.

Demo settings are at the top of `sensor-banjir.ino`: `TINGGI_SENSOR_CM` (sensor height above the bucket floor), `AMBANG_CM` (threshold), `DURASI_MS` (how long the water must stay above it). If the float or the buzzer works the wrong way round, flip `PELAMPUNG_NAIK_SAAT_LOW` / `BUZZER_AKTIF_LOW`.

## Upload to a real ESP32
```
arduino-cli board list      # find the port, usually /dev/ttyUSB0
./firmware/upload.sh /dev/ttyUSB0
```
The serial monitor prints one JSON line per reading (`id`, `tinggi`, `status`, `sepakat`, ...).

## Roadmap
1. Build the sensor node on a breadboard (bucket demo).
2. Send readings over WiFi, signed on the device (secp256k1).
3. Flood pool smart contract: pays out automatically when sensors with a good reliability score stay above the threshold long enough.
4. **Flood data for AI agents:** AI agents (ride-hailing, logistics, navigation) ask "is street X flooded?" and pay per query in stablecoins via x402. The income is split between the flood pool and the residents who host the sensors.
