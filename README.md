# VITA-CAM
Development of an ESP32-CAM-based device for non-invasive anemia screening through conjunctival image color analysis, including firmware development, color calibration, and device performance testing

## Deskripsi Proyek
VITA-CAM dikembangkan sebagai perangkat portabel yang memanfaatkan kamera ESP32-CAM untuk mengambil citra konjungtiva mata, kemudian mengolah karakteristik warna citra sebagai dasar klasifikasi awal anemia.

Perangkat dirancang agar dapat melakukan pemrosesan secara langsung dan menampilkan hasil klasifikasi melalui LCD tanpa memerlukan koneksi internet.

## Tujuan Proyek
1. Mengembangkan perangkat pengambilan citra konjungtiva berbasis ESP32-CAM.
2. Menerapkan pengolahan fitur warna untuk mendukung klasifikasi awal anemia.
3. Melakukan kalibrasi dan pengujian untuk meningkatkan akurasi serta konsistensi hasil pengukuran.
4. Mengevaluasi hasil klasifikasi menggunakan data pembanding hemoglobin (Hb).

## Teknologi dan Komponen
- ESP32-CAM AI Thinker
- Kamera OV2640
- Sensor pencahayaan LED
- LCD 16×2
- Arduino IDE
- Algoritma pengolahan warna RGB

## Struktur Repositori
```text
VITA-CAM/
├── firmware/
│   └── VITA-CAM.ino
├── dataset/
├── hardware-design/
├── documentation/
├── test-results/
└── README.md
```

## Tahapan Pengembangan
1. Perancangan dan pembuatan prototipe perangkat.
2. Pengembangan program dan sistem pengambilan citra.
3. Pengolahan fitur warna citra konjungtiva.
4. Kalibrasi dan penyesuaian parameter klasifikasi.
5. Pengujian serta evaluasi kinerja perangkat.

## Status Proyek
Prototipe perangkat telah dikembangkan dan dapat melakukan pengambilan citra serta menampilkan hasil klasifikasi. Tahap pengembangan selanjutnya difokuskan pada kalibrasi, pengujian, dan evaluasi akurasi hasil pengukuran.

## Dokumentasi
Repositori ini digunakan untuk menyimpan kode program, dataset, desain perangkat keras, dokumentasi proses pengembangan, serta hasil pengujian VITA-CAM.

## Catatan

VITA-CAM merupakan prototipe penelitian untuk mendukung deteksi dini atau skrining awal anemia, bukan alat diagnosis medis dan tidak menggantikan pemeriksaan hemoglobin secara klinis.

