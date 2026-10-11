// Logika sensor banjir tanpa ketergantungan hardware, supaya bisa dites di laptop.
#pragma once

enum Status { AMAN, SIAGA, BANJIR, SENSOR_ERROR };

const int MAKS_BACAAN = 9;

// Median dari bacaan yang valid (> 0). Butuh minimal 3 bacaan valid, kalau kurang hasilnya -1.
inline float jarakMedian(const float* bacaan, int n) {
  float valid[MAKS_BACAAN];
  int k = 0;
  for (int i = 0; i < n && k < MAKS_BACAAN; i++) {
    if (bacaan[i] > 0) valid[k++] = bacaan[i];
  }
  if (k < 3) return -1;
  for (int i = 1; i < k; i++) {
    float x = valid[i];
    int j = i - 1;
    while (j >= 0 && valid[j] > x) { valid[j + 1] = valid[j]; j--; }
    valid[j + 1] = x;
  }
  return valid[(k - 1) / 2];  // median bawah: selalu bacaan asli, bukan rata-rata
}

// Sensor menghadap ke bawah dari ketinggian tinggiSensorCm di atas dasar.
inline float tinggiAir(float jarakCm, float tinggiSensorCm) {
  if (jarakCm < 0) return -1;
  float t = tinggiSensorCm - jarakCm;
  return t < 0 ? 0 : t;
}

// BANJIR kalau air bertahan di atas ambang selama durasiMs; sebelum itu SIAGA.
struct Detektor {
  float ambangCm;
  unsigned long durasiMs;
  bool diAtas = false;
  unsigned long sejakMs = 0;

  Detektor(float ambang, unsigned long durasi) : ambangCm(ambang), durasiMs(durasi) {}

  Status perbarui(float tinggiCm, unsigned long sekarangMs) {
    if (tinggiCm < 0) return SENSOR_ERROR;
    if (tinggiCm < ambangCm) { diAtas = false; return AMAN; }
    if (!diAtas) { diAtas = true; sejakMs = sekarangMs; }
    return sekarangMs - sejakMs >= durasiMs ? BANJIR : SIAGA;
  }
};

// Cek silang: pelampung (dipasang setinggi ambang) harus setuju dengan sensor jarak.
inline bool sepakat(float tinggiCm, float ambangCm, bool pelampungNaik) {
  return (tinggiCm >= ambangCm) == pelampungNaik;
}
