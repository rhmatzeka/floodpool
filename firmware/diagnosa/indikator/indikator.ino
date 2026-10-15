#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Indikator rakit: LED papan (GPIO2) menyala saat layar OLED menjawab; layar langsung diisi tulisan.
// Serial tiap detik: status SDA/SCL (ditarik pull-up layar atau tidak) dan apakah layar menjawab.
Adafruit_SSD1306 layar(128, 64, &Wire, -1);
bool siap = false;
const char* cek(int pin) {
  pinMode(pin, INPUT_PULLUP); delay(2); int up = digitalRead(pin);
  pinMode(pin, INPUT_PULLDOWN); delay(2); int down = digitalRead(pin);
  pinMode(pin, INPUT);
  if (up && down) return "ditarik-3V3(ok)";
  if (!up && !down) return "ke-GND";
  return "lepas";
}
void setup() { Serial.begin(115200); pinMode(2, OUTPUT); }
void loop() {
  Wire.end();
  const char* sda = cek(13);
  const char* scl = cek(14);
  Wire.begin(13, 14);
  Wire.beginTransmission(0x3C);
  bool ok = Wire.endTransmission() == 0;
  digitalWrite(2, ok);
  if (ok && !siap) siap = layar.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!ok) siap = false;
  if (siap) {
    layar.clearDisplay(); layar.setTextColor(SSD1306_WHITE);
    layar.setTextSize(2); layar.setCursor(4, 8); layar.print("LAYAR OK");
    layar.setTextSize(1); layar.setCursor(4, 40); layar.printf("waktu %lus", millis() / 1000);
    layar.display();
  }
  Serial.printf("IND: SDA=%s SCL=%s layar=%s\n", sda, scl, ok ? "MENJAWAB" : "tidak");
  delay(500);
}
