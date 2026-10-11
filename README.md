# Dana Gotong Royong Banjir

Kas banjir warga tanpa bendahara: sensor ketinggian air (ESP32) memicu pencairan dana dari smart contract saat banjir. Tahap sekarang: **node sensor untuk demo meja (ember air)**.

## Isi
- `firmware/sensor-banjir/` – program ESP32 (`sensor-banjir.ino`) + logika yang bisa dites (`logika.h`) + rangkaian Wokwi (`diagram.json`, `libraries.txt`)
- `firmware/test/` – tes logika di laptop: `make -C firmware/test`
- `firmware/upload.sh` – kompilasi + upload ke ESP32 asli + serial monitor

## Simulasi di Wokwi (tanpa alat)
1. Buka https://wokwi.com/projects/new/esp32
2. Tab `sketch.ino`: tempel isi `sensor-banjir.ino`
3. Tab `diagram.json`: tempel isi `diagram.json`
4. Tombol ▾ di samping tab → **New file…** → `logika.h`, tempel isinya. Ulangi untuk `libraries.txt`.
5. Klik ▶ (Start). Klik sensor HC-SR04 lalu geser jaraknya:
   - jarak 35 cm → tinggi air 5 cm → **AMAN**
   - jarak 20 cm → tinggi air 20 cm → **SIAGA** (LED kedip), setelah 10 detik **BANJIR** (LED nyala, buzzer bunyi)
   - geser saklar (= pelampung) untuk melihat cek silang `OK` / `BEDA`

## Rangkaian alat asli (ESP32 DevKit 38 pin)
| Komponen | Pin komponen → pin ESP32 |
|---|---|
| HC-SR04 | VCC → `5V`, GND → `GND`, TRIG → `D25` |
| | ECHO → resistor 1 kΩ → **titik tengah** → resistor 2 kΩ → `GND`; titik tengah → `D26` |
| Float switch | satu kabel → `D27`, satu lagi → `GND` |
| OLED SSD1306 (4 pin) | VCC → `3V3`, GND → `GND`, SDA → `D21`, SCL → `D22` |
| Modul buzzer aktif (3 pin) | VCC → `3V3`, GND → `GND`, I/O → `D33` |
| LED merah | kaki panjang → resistor 220 Ω → `D32`; kaki pendek → `GND` |

Label pin bisa tertulis `D25`, `IO25`, atau `G25`. Jangan pakai pin `SD0`–`SD3`, `CLK`, `CMD`. Pembagi tegangan ECHO wajib: HC-SR04 mengeluarkan 5V, pin ESP32 hanya tahan 3,3V.

Pengaturan demo ada di bagian atas `sensor-banjir.ino`: `TINGGI_SENSOR_CM` (jarak sensor ke dasar ember), `AMBANG_CM`, `DURASI_MS`. Kalau pelampung atau buzzer terbalik, ubah `PELAMPUNG_NAIK_SAAT_LOW` / `BUZZER_AKTIF_LOW`.

## Upload ke ESP32 asli
```
arduino-cli board list      # cari port, biasanya /dev/ttyUSB0
./firmware/upload.sh /dev/ttyUSB0
```
Serial monitor menampilkan satu baris JSON per bacaan (`id`, `tinggi`, `status`, `sepakat`, ...). Baris ini nanti ditandatangani dan dikirim ke server.
