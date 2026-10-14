# Sketsa diagnosa

Program kecil untuk mencari kabel yang salah atau longgar. Upload dengan
`arduino-cli upload --fqbn esp32:esp32:esp32 -p /dev/ttyUSB0 firmware/diagnosa/<nama>`
(compile dulu dengan `arduino-cli compile --fqbn esp32:esp32:esp32 firmware/diagnosa/<nama>`),
lalu baca serial 115200. Setelah selesai, upload lagi program utama: `./firmware/upload.sh /dev/ttyUSB0`.

- `indikator` – LED papan ESP32 (GPIO2) menyala selama layar OLED menjawab; serial tiap 0,5 detik:
  status SDA/SCL (`ditarik-3V3(ok)` = layar dapat VCC, `ke-GND` = layar tidak dapat VCC,
  `lepas` = tidak tersambung / VCC lemah) dan `layar=MENJAWAB/tidak`.
- `cekpin` – baca P32, P33, P25, P26, P27, P14, P12, P13 dengan pull-up dan pull-down:
  `lepas`, `ke GND`, atau `ke 3V3/pull-up`. P26 harus `ke GND` (resistor 2 kΩ ke baris GND).
- `tesoled2` – tes layar arus kecil, lapor `menjawab n/20` tiap 2 detik.
- `i2cscan` – cari perangkat I2C di pasangan pin 13/14, 14/13, 21/22.
