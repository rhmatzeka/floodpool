// Baca tiap pin dua kali: dengan pull-up internal dan pull-down internal.
// up=1 down=0 -> tidak tersambung ke apa pun; up=0 down=0 -> ditarik ke GND; up=1 down=1 -> ditarik ke 3,3 V.
const int pins[] = { 32, 33, 25, 26, 27, 14, 12, 13 };
const char* nama[] = { "P32 LED", "P33 buzzer", "P25 Trig", "P26 Echo/pembagi", "P27 pelampung", "P14 SCL", "P12 (kosong)", "P13 SDA" };
void setup() { Serial.begin(115200); }
void loop() {
  for (int i = 0; i < 8; i++) {
    pinMode(pins[i], INPUT_PULLUP); delay(5); int up = digitalRead(pins[i]);
    pinMode(pins[i], INPUT_PULLDOWN); delay(5); int down = digitalRead(pins[i]);
    pinMode(pins[i], INPUT);
    Serial.printf("%-18s up=%d down=%d -> %s\n", nama[i], up, down,
      up && !down ? "lepas" : (!up && !down ? "ke GND" : (up && down ? "ke 3V3/pull-up" : "aneh")));
  }
  Serial.println("--");
  delay(1000);
}
