// Tes logika sensor di laptop (tanpa ESP32): make -C firmware/test
#include <cstdio>
#include "../sensor-banjir/logika.h"

static int gagal = 0;
#define CEK(kondisi) do { if (!(kondisi)) { std::printf("GAGAL %s:%d  %s\n", __FILE__, __LINE__, #kondisi); gagal++; } } while (0)

static void tes_median_mengabaikan_bacaan_gagal() {
  float bacaan[5] = {50, -1, 52, 49, 300};
  CEK(jarakMedian(bacaan, 5) == 50);
}

static void tes_median_gagal_kalau_bacaan_valid_kurang_dari_tiga() {
  float bacaan[5] = {50, -1, -1, 49, -1};
  CEK(jarakMedian(bacaan, 5) < 0);
}

static void tes_tinggi_air_dari_jarak() {
  CEK(tinggiAir(30, 100) == 70);
}

static void tes_tinggi_air_tidak_negatif() {
  CEK(tinggiAir(120, 100) == 0);
}

static void tes_tinggi_air_tidak_valid_kalau_jarak_gagal() {
  CEK(tinggiAir(-1, 100) < 0);
}

static void tes_status_aman_di_bawah_ambang() {
  Detektor d(50, 10000);
  CEK(d.perbarui(20, 0) == AMAN);
}

static void tes_status_siaga_lalu_banjir_setelah_durasi() {
  Detektor d(50, 10000);
  CEK(d.perbarui(60, 1000) == SIAGA);
  CEK(d.perbarui(60, 10999) == SIAGA);
  CEK(d.perbarui(60, 11000) == BANJIR);
}

static void tes_turun_di_bawah_ambang_mengulang_hitungan() {
  Detektor d(50, 10000);
  d.perbarui(60, 0);
  CEK(d.perbarui(40, 5000) == AMAN);
  CEK(d.perbarui(60, 6000) == SIAGA);
  CEK(d.perbarui(60, 15000) == SIAGA);
}

static void tes_bacaan_gagal_jadi_error_tanpa_mengulang_hitungan() {
  Detektor d(50, 10000);
  d.perbarui(60, 0);
  CEK(d.perbarui(-1, 4000) == SENSOR_ERROR);
  CEK(d.perbarui(60, 10000) == BANJIR);
}

static void tes_cek_silang_pelampung() {
  CEK(sepakat(60, 50, true));
  CEK(sepakat(20, 50, false));
  CEK(!sepakat(60, 50, false));
  CEK(!sepakat(20, 50, true));
}

int main() {
  tes_median_mengabaikan_bacaan_gagal();
  tes_median_gagal_kalau_bacaan_valid_kurang_dari_tiga();
  tes_tinggi_air_dari_jarak();
  tes_tinggi_air_tidak_negatif();
  tes_tinggi_air_tidak_valid_kalau_jarak_gagal();
  tes_status_aman_di_bawah_ambang();
  tes_status_siaga_lalu_banjir_setelah_durasi();
  tes_turun_di_bawah_ambang_mengulang_hitungan();
  tes_bacaan_gagal_jadi_error_tanpa_mengulang_hitungan();
  tes_cek_silang_pelampung();
  if (gagal) { std::printf("%d cek gagal\n", gagal); return 1; }
  std::printf("semua tes lulus\n");
  return 0;
}
