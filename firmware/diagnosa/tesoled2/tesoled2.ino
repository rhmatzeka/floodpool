#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Tes layar arus kecil: tanpa layar putih penuh. Tiap 2 detik lapor berapa persen layar menjawab.
Adafruit_SSD1306 layar(128, 64, &Wire, -1);
bool siap = false;
int n = 0, jawab = 0;
void setup() { Serial.begin(115200); Wire.begin(13, 14); }
void loop() {
  Wire.beginTransmission(0x3C);
  bool ok = Wire.endTransmission() == 0;
  n++; if (ok) jawab++;
  if (ok && !siap) siap = layar.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!ok) siap = false;
  if (siap && n % 10 == 0) {
    layar.clearDisplay();
    layar.setTextColor(SSD1306_WHITE);
    layar.setTextSize(2); layar.setCursor(4, 8); layar.print("LAYAR OK");
    layar.setTextSize(1); layar.setCursor(4, 40); layar.printf("waktu %lus", millis() / 1000);
    layar.display();
  }
  if (n == 20) { Serial.printf("TES2: menjawab %d/20\n", jawab); n = 0; jawab = 0; }
  delay(100);
}
