#include <Wire.h>
// Cari perangkat I2C di beberapa pasangan pin (SDA, SCL), termasuk kalau kabel tertukar.
const int pasangan[][2] = { {13, 14}, {14, 13}, {21, 22} };
void setup() { Serial.begin(115200); }
void loop() {
  for (auto &p : pasangan) {
    Wire.end();
    Wire.begin(p[0], p[1]);
    Serial.printf("SDA=%d SCL=%d:", p[0], p[1]);
    int n = 0;
    for (uint8_t a = 1; a < 127; a++) {
      Wire.beginTransmission(a);
      if (Wire.endTransmission() == 0) { Serial.printf(" 0x%02X", a); n++; }
    }
    Serial.println(n ? "" : " (tidak ada)");
  }
  Serial.printf("level pin13=%d pin14=%d\n", digitalRead(13), digitalRead(14));
  delay(1000);
}
