// Node sensor banjir: HC-SR04 + pelampung + OLED + buzzer + LED di ESP32 DevKit 38 pin.
// Logika (median, tinggi air, status, cek silang) ada di logika.h dan dites di firmware/test.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "logika.h"

// 1 = simulator Wokwi (buzzer pakai nada). upload.sh mengompilasi dengan SIMULASI=0 untuk ESP32 asli.
#ifndef SIMULASI
#define SIMULASI 1
#endif

// ---- Pengaturan demo (ember). Di lapangan: tinggi sensor sebenarnya, durasi 2 jam. ----
const char* ID_NODE = "node-01";
const float TINGGI_SENSOR_CM = 40;      // jarak sensor ke dasar ember
const float AMBANG_CM = 15;             // tinggi air yang dianggap banjir (pelampung dipasang setinggi ini)
const unsigned long DURASI_MS = 10000;  // air harus bertahan di atas ambang selama ini
const bool PELAMPUNG_NAIK_SAAT_LOW = true;  // balik kalau pelampungmu terbaca terbalik
const bool BUZZER_AKTIF_LOW = true;         // kebanyakan modul buzzer 3 pin bunyi saat pin LOW

// ---- Pin (label di papan: P25 / D25 / IO25 / G25, dst.) ----
const int PIN_TRIG = 25;
const int PIN_ECHO = 26;  // lewat pembagi tegangan 1k/2k di alat asli
const int PIN_PELAMPUNG = 27;
const int PIN_LED = 32;
const int PIN_BUZZER = 33;
// OLED di sisi kiri: pin kanan ESP32 tertutup papan saat dipasang di 2 breadboard.
// Jangan pakai P12 (pull-up layar bikin ESP32 gagal boot).
const int PIN_SDA = 13;
const int PIN_SCL = 14;

Adafruit_SSD1306 layar(128, 64, &Wire, -1);
bool adaLayar = false;
Detektor detektor(AMBANG_CM, DURASI_MS);

const char* namaStatus(Status s) {
  switch (s) {
    case AMAN: return "AMAN";
    case SIAGA: return "SIAGA";
    case BANJIR: return "BANJIR";
    default: return "SENSOR ERROR";
  }
}

float bacaJarakCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  unsigned long us = pulseIn(PIN_ECHO, HIGH, 30000);
  return us == 0 ? -1 : us / 58.0;
}

void buzzer(bool nyala) {
#if SIMULASI
  if (nyala) tone(PIN_BUZZER, 2000); else noTone(PIN_BUZZER);
#else
  digitalWrite(PIN_BUZZER, nyala != BUZZER_AKTIF_LOW ? HIGH : LOW);
#endif
}

void tampilkan(float tinggi, Status s, bool pelampungNaik, bool cocok) {
  // Cek layar tiap kali: kalau kabelnya sempat longgar lalu nyambung lagi, layar dinyalakan ulang.
  Wire.beginTransmission(0x3C);
  if (Wire.endTransmission() != 0) { adaLayar = false; return; }
  if (!adaLayar) adaLayar = layar.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!adaLayar) return;
  layar.clearDisplay();
  layar.setTextColor(SSD1306_WHITE);
  layar.setTextSize(1);
  layar.setCursor(0, 0);
  layar.print("Tinggi air");
  layar.setTextSize(2);
  layar.setCursor(0, 12);
  if (tinggi < 0) layar.print("--"); else { layar.print(tinggi, 1); layar.print(" cm"); }
  layar.setTextSize(1);
  layar.setCursor(0, 36);
  layar.print("Status: ");
  layar.print(namaStatus(s));
  layar.setCursor(0, 50);
  layar.print("Pelampung: ");
  layar.print(pelampungNaik ? "NAIK" : "TURUN");
  layar.print(cocok ? "  OK" : "  BEDA");
  layar.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_PELAMPUNG, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  buzzer(false);
  Wire.begin(PIN_SDA, PIN_SCL);
  adaLayar = layar.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!adaLayar) Serial.println("OLED tidak terdeteksi (cek SDA=13, SCL=14, alamat 0x3C)");
}

void loop() {
  float bacaan[5];
  for (int i = 0; i < 5; i++) { bacaan[i] = bacaJarakCm(); delay(60); }
  float jarak = jarakMedian(bacaan, 5);
  float tinggi = tinggiAir(jarak, TINGGI_SENSOR_CM);
  unsigned long sekarang = millis();
  Status s = detektor.perbarui(tinggi, sekarang);
  bool pelampungNaik = (digitalRead(PIN_PELAMPUNG) == LOW) == PELAMPUNG_NAIK_SAAT_LOW;
  bool cocok = tinggi >= 0 && sepakat(tinggi, AMBANG_CM, pelampungNaik);

  bool kedip = (sekarang / 500) % 2;
  digitalWrite(PIN_LED, s == BANJIR || ((s == SIAGA || s == SENSOR_ERROR) && kedip));
  buzzer(s == BANJIR && kedip);
  tampilkan(tinggi, s, pelampungNaik, cocok);

  // Satu baris JSON per bacaan; nanti baris inilah yang ditandatangani dan dikirim ke server.
  Serial.printf("{\"id\":\"%s\",\"ms\":%lu,\"jarak\":%.1f,\"tinggi\":%.1f,\"pelampung\":%d,\"status\":\"%s\",\"sepakat\":%s,\"layar\":%s}\n",
                ID_NODE, sekarang, jarak, tinggi, pelampungNaik, namaStatus(s), cocok ? "true" : "false", adaLayar ? "true" : "false");
  delay(200);
}
