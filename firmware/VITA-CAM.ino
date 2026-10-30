#include "esp_camera.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ======================
// AI Thinker ESP32-CAM
// ======================

// --- Definisikan Pin I2C untuk LCD ---
#define I2C_LCD_SDA 14
#define I2C_LCD_SCL 15

// Inisialisasi LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Pin Kamera ---
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Tombol dan Flash
#define BUTTON_PIN 13
#define FLASH_LED 4

// ========== DATA KALIBRASI TERBARU ==========
// Kelompok Normal (Tidak Anemia) : R=130, G=135, B=133
// Kelompok Anemia                : R=150, G=144, B=140

// Hitung nilai tengah (threshold)
#define THRESHOLD_R 140   // (130 + 150) / 2
#define THRESHOLD_G 139.5 // (135 + 144) / 2
#define THRESHOLD_B 136.5 // (133 + 140) / 2

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi I2C untuk LCD
  Wire.begin(I2C_LCD_SDA, I2C_LCD_SCL);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Deteksi");
  lcd.setCursor(0, 1);
  lcd.print("Anemia v3.0");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tekan Tombol");
  lcd.setCursor(0, 1);
  lcd.print("untuk Scan");

  // Inisialisasi Pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(FLASH_LED, OUTPUT);
  digitalWrite(FLASH_LED, LOW);

  // Inisialisasi Kamera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565;
  config.frame_size = FRAMESIZE_QQVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.println("Camera gagal");
    lcd.clear();
    lcd.print("Camera Error!");
    while (1);
  }
  Serial.println("Sistem siap");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Scanning...");
      lcd.setCursor(0, 1);
      lcd.print("Harap tunggu");
      
      prosesAnemia();
      
      while (digitalRead(BUTTON_PIN) == LOW);
      delay(500);
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tekan Tombol");
      lcd.setCursor(0, 1);
      lcd.print("untuk Scan");
    }
  }
}

// Fungsi ekstraksi RGB
void extractRGB(uint16_t pixel, uint8_t &r, uint8_t &g, uint8_t &b) {
  r = ((pixel >> 11) & 0x1F) << 3;
  g = ((pixel >> 5) & 0x3F) << 2;
  b = (pixel & 0x1F) << 3;
}

// Fungsi deteksi anemia berdasarkan kalibrasi TERBARU
String deteksiAnemia(uint8_t r, uint8_t g, uint8_t b) {
  // Data referensi dari kalibrasi terbaru
  const int normal_R = 130, normal_G = 135, normal_B = 133;
  const int anemia_R = 150, anemia_G = 144, anemia_B = 140;
  
  // Metode Euclidean Distance (paling akurat)
  float jarakKeNormal = sqrt(pow(r - normal_R, 2) + pow(g - normal_G, 2) + pow(b - normal_B, 2));
  float jarakKeAnemia = sqrt(pow(r - anemia_R, 2) + pow(g - anemia_G, 2) + pow(b - anemia_B, 2));
  
  // Debug jarak (untuk lihat di Serial Monitor)
  Serial.print("Jarak ke Normal: ");
  Serial.print(jarakKeNormal, 2);
  Serial.print(", Jarak ke Anemia: ");
  Serial.println(jarakKeAnemia, 2);
  
  // Keputusan berdasarkan jarak terdekat
  if (jarakKeAnemia < jarakKeNormal) {
    return "ANEMIA";
  } else {
    return "NORMAL";
  }
}

// Fungsi untuk menghitung persentase kecenderungan
float hitungPersentaseAnemia(uint8_t r, uint8_t g, uint8_t b) {
  const int normal_R = 130, normal_G = 135, normal_B = 133;
  const int anemia_R = 150, anemia_G = 144, anemia_B = 140;
  
  float jarakKeNormal = sqrt(pow(r - normal_R, 2) + pow(g - normal_G, 2) + pow(b - normal_B, 2));
  float jarakKeAnemia = sqrt(pow(r - anemia_R, 2) + pow(g - anemia_G, 2) + pow(b - anemia_B, 2));
  
  float totalJarak = jarakKeNormal + jarakKeAnemia;
  if (totalJarak == 0) return 50;
  
  // Persentase kecenderungan ke arah anemia (semakin tinggi semakin cenderung anemia)
  float persenAnemia = (1 - (jarakKeAnemia / totalJarak)) * 100;
  return persenAnemia;
}

void prosesAnemia() {
  // Nyalakan flash
  digitalWrite(FLASH_LED, HIGH);
  delay(500);
  
  // Ambil gambar
  camera_fb_t *fb = esp_camera_fb_get();
  
  // Matikan flash
  digitalWrite(FLASH_LED, LOW);

  if (!fb) {
    Serial.println("ERROR: Gagal mengambil gambar");
    lcd.clear();
    lcd.print("Gambar Gagal!");
    lcd.setCursor(0, 1);
    lcd.print("Coba lagi");
    delay(2000);
    return;
  }

  // Proses pixel
  uint16_t *pixels = (uint16_t *)fb->buf;
  long sumR = 0, sumG = 0, sumB = 0;
  long jumlahPixel = 0;
  int width = 160;
  int height = 120;
  
  // ROI (Region of Interest) - area tengah
  int x1 = 50, x2 = 110;
  int y1 = 40, y2 = 80;

  for (int y = y1; y < y2; y++) {
    for (int x = x1; x < x2; x++) {
      uint16_t pixel = pixels[y * width + x];
      uint8_t r, g, b;
      extractRGB(pixel, r, g, b);
      sumR += r;
      sumG += g;
      sumB += b;
      jumlahPixel++;
    }
  }

  // Hitung rata-rata RGB
  float Ravg = (float)sumR / jumlahPixel;
  float Gavg = (float)sumG / jumlahPixel;
  float Bavg = (float)sumB / jumlahPixel;

  // Deteksi anemia berdasarkan kalibrasi terbaru
  String status = deteksiAnemia((uint8_t)Ravg, (uint8_t)Gavg, (uint8_t)Bavg);
  float persenAnemia = hitungPersentaseAnemia((uint8_t)Ravg, (uint8_t)Gavg, (uint8_t)Bavg);

  // ========== TAMPILKAN DI LCD ==========
  lcd.clear();
  
  // Baris 1: Status
  lcd.setCursor(0, 0);
  if (status == "ANEMIA") {
    lcd.print(">> ANEMIA! <<");
  } else {
    lcd.print(">> NORMAL <<");
  }
  
  // Baris 2: Tampilkan nilai RGB
  lcd.setCursor(0, 1);
  char buffer[17];
  snprintf(buffer, sizeof(buffer), "R%03d G%03d B%03d", (int)Ravg, (int)Gavg, (int)Bavg);
  lcd.print(buffer);

  // ========== KIRIM KE SERIAL MONITOR ==========
  Serial.println("\n========================================");
  Serial.println("     HASIL DETEKSI ANEMIA v3.0");
  Serial.println("========================================");
  Serial.println("Data Kalibrasi:");
  Serial.println("  Normal (Tidak Anemia) : R=130, G=135, B=133");
  Serial.println("  Anemia                : R=150, G=144, B=140");
  Serial.println("----------------------------------------");
  Serial.print("Nilai Terdeteksi : R=");
  Serial.print((int)Ravg);
  Serial.print(", G=");
  Serial.print((int)Gavg);
  Serial.print(", B=");
  Serial.println((int)Bavg);
  Serial.println("----------------------------------------");
  
  // Tampilkan analisis per komponen
  Serial.println("Analisis per komponen:");
  if (Ravg > THRESHOLD_R) {
    Serial.print("  R: ");
    Serial.print((int)Ravg);
    Serial.println(" (↑ lebih tinggi dari normal -> mengarah ke ANEMIA)");
  } else {
    Serial.print("  R: ");
    Serial.print((int)Ravg);
    Serial.println(" (↓ normal)");
  }
  
  if (Gavg > THRESHOLD_G) {
    Serial.print("  G: ");
    Serial.print((int)Gavg);
    Serial.println(" (↑ lebih tinggi dari normal -> mengarah ke ANEMIA)");
  } else {
    Serial.print("  G: ");
    Serial.print((int)Gavg);
    Serial.println(" (↓ normal)");
  }
  
  if (Bavg > THRESHOLD_B) {
    Serial.print("  B: ");
    Serial.print((int)Bavg);
    Serial.println(" (↑ lebih tinggi dari normal -> mengarah ke ANEMIA)");
  } else {
    Serial.print("  B: ");
    Serial.print((int)Bavg);
    Serial.println(" (↓ normal)");
  }
  
  Serial.println("----------------------------------------");
  Serial.print("STATUS AKHIR : ");
  
  if (status == "ANEMIA") {
    Serial.println("⚠️  TERINDIKASI ANEMIA ⚠️");
    Serial.print("Tingkat kecenderungan anemia: ");
    Serial.print(persenAnemia, 1);
    Serial.println("%");
    Serial.println("-> Konsultasikan dengan dokter");
  } else {
    Serial.println("✓ NORMAL (Tidak Anemia) ✓");
    Serial.print("Tingkat kecenderungan normal: ");
    Serial.print(100 - persenAnemia, 1);
    Serial.println("%");
  }
  Serial.println("========================================\n");

  // Kirim data format CSV untuk logging
  Serial.print("DATA_CSV:");
  Serial.print(status);
  Serial.print(",");
  Serial.print((int)Ravg);
  Serial.print(",");
  Serial.print((int)Gavg);
  Serial.print(",");
  Serial.print((int)Bavg);
  Serial.print(",");
  Serial.println(persenAnemia, 1);

  esp_camera_fb_return(fb);
  
  // Tahan hasil di LCD selama 3 detik
  delay(3000);
}
