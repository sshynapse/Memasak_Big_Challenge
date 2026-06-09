/*
 * ============================================================
 *  BIG CHALLENGE - STRUKTUR DATA DAN ALGORITMA
 *  SINF1004 - Universitas Syiah Kuala
 * ============================================================
 *  SECTION 1 - TYPES
 *  SECTION 2 - FILE I/O
 *  SECTION 3 - SORTING
 *  SECTION 4 - DISPLAY
 *  SECTION 5 - MAIN
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* ============================================================
 *  SECTION 1 - TYPES
 * ============================================================ */

#define MAX_KATA    150000
#define MAX_PANJANG 100
#define MAX_K       150

typedef struct {
    int  wordID;
    char kata[MAX_PANJANG];
    long frekuensi;
} Kata;

typedef struct {
    int  D;
    int  W;
    long N;
} MetaData;

Kata daftarKata[MAX_KATA];


/* ============================================================
 *  SECTION 2 - FILE I/O
 * ============================================================ */

/*
 * Ubah semua huruf string menjadi huruf kecil (lowercase)
 * Dipakai agar deteksi "docword." tidak case-sensitive
 */
void toLowerStr(char *str) {
    int i;
    for (i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

/*
 * Bersihkan karakter tersembunyi di akhir string
 * (spasi, \r, \n yang kadang muncul dari input Windows/Linux)
 */
void bersihkanInput(char *str) {
    int len = strlen(str);
    while (len > 0 && (str[len-1] == '\r' || str[len-1] == '\n' ||
                       str[len-1] == ' '  || str[len-1] == '\t')) {
        str[--len] = '\0';
    }
}

/*
 * Otomatis tentukan nama file vocab dari nama file docword.
 * Strategi:
 *   1. Cari "docword." (case-insensitive) → ganti dengan "vocab."
 *   2. Jika tidak ditemukan → minta user input manual
 *
 * Contoh:
 *   "docword.kos.txt"       → "vocab.kos.txt"
 *   "data/docword.kos.txt"  → "data/vocab.kos.txt"
 *   "Docword.KOS.txt"       → "vocab.KOS.txt"  (handle kapital)
 */
void tentukanNamaVocab(const char *namaDocword, char *namaVocab) {
    /* Buat salinan lowercase untuk pencarian */
    char salinLower[300];
    strncpy(salinLower, namaDocword, sizeof(salinLower) - 1);
    salinLower[sizeof(salinLower) - 1] = '\0';
    toLowerStr(salinLower);

    /* Cari posisi "docword." di versi lowercase */
    const char *posisiLower = strstr(salinLower, "docword.");
    if (posisiLower != NULL) {
        /* Hitung offset posisi dari awal string */
        int offset = posisiLower - salinLower;

        /* Salin bagian path sebelum "docword." dari nama ASLI */
        strncpy(namaVocab, namaDocword, offset);
        namaVocab[offset] = '\0';

        /* Tambahkan "vocab." + sisa nama file setelah "docword." */
        strcat(namaVocab, "vocab.");
        strcat(namaVocab, namaDocword + offset + strlen("docword."));
    } else {
        /* Fallback: tidak ada "docword." → minta input manual */
        printf("[INFO] Nama vocab tidak dapat dideteksi otomatis.\n");
        printf("Masukkan nama file vocab (contoh: vocab.kos.txt): ");
        scanf("%s", namaVocab);
        bersihkanInput(namaVocab);
    }
}

/*
 * Baca 3 baris pertama file docword → isi MetaData (D, W, N)
 */
MetaData bacaMetaData(const char *namaFile) {
    MetaData meta = {0, 0, 0};
    FILE *fp = fopen(namaFile, "r");
    if (fp == NULL) {
        printf("[ERROR] File '%s' tidak dapat dibuka!\n", namaFile);
        printf("        Pastikan nama file dan lokasinya sudah benar.\n");
        exit(1);
    }
    fscanf(fp, "%d", &meta.D);
    fscanf(fp, "%d", &meta.W);
    fscanf(fp, "%ld", &meta.N);
    fclose(fp);
    return meta;
}

/*
 * Baca vocab.*.txt → isi daftarKata[1..W].kata
 */
void bacaVocab(const char *namaFile, int W) {
    FILE *fp = fopen(namaFile, "r");
    if (fp == NULL) {
        printf("[ERROR] File vocab '%s' tidak dapat dibuka!\n", namaFile);
        printf("        Pastikan file vocab ada di folder yang sama dengan docword.\n");
        exit(1);
    }
    int i;
    for (i = 0; i <= W; i++) {
        daftarKata[i].wordID    = i;
        daftarKata[i].frekuensi = 0;
        strcpy(daftarKata[i].kata, "");
    }
    for (i = 1; i <= W; i++) {
        if (fscanf(fp, "%s", daftarKata[i].kata) == EOF) break;
        daftarKata[i].wordID = i;
    }
    fclose(fp);
    printf("[OK] Vocab berhasil dibaca: %d kata.\n", W);
}

/*
 * Baca NNZ dari docword (baris ke-4 dst) → akumulasi frekuensi
 */
void bacaDocword(const char *namaFile, long N) {
    FILE *fp = fopen(namaFile, "r");
    if (fp == NULL) {
        printf("[ERROR] File '%s' tidak dapat dibuka!\n", namaFile);
        exit(1);
    }
    int  D, W;
    long Ntemp;
    fscanf(fp, "%d", &D);
    fscanf(fp, "%d", &W);
    fscanf(fp, "%ld", &Ntemp);

    int  docID, wordID;
    long count, baris = 0;
    long laporan = (N > 0) ? N / 10 : 1;
    if (laporan == 0) laporan = 1;

    printf("[INFO] Membaca %ld baris NNZ...\n", N);
    while (fscanf(fp, "%d %d %ld", &docID, &wordID, &count) == 3) {
        if (wordID >= 0 && wordID < MAX_KATA)
            daftarKata[wordID].frekuensi += count;
        baris++;
        if (baris % laporan == 0) {
            printf("  Progress: %ld%% (%ld/%ld)\r", baris*100/N, baris, N);
            fflush(stdout);
        }
    }
    printf("\n");
    fclose(fp);
    printf("[OK] Selesai. Total baris terbaca: %ld\n", baris);
}

/*
 * Simpan hasil sorting ke file output
 */
void simpanKeFile(const char *namaFile, int W, int k, double waktuMs) {
    FILE *fp = fopen(namaFile, "w");
    if (fp == NULL) {
        printf("[ERROR] Tidak dapat membuat file '%s'!\n", namaFile);
        return;
    }
    fprintf(fp, "=== TOP %d KATA DENGAN FREKUENSI TERBESAR ===\n\n", k);
    int i, tampil = 0;
    for (i = 1; i <= W && tampil < k; i++) {
        if (daftarKata[i].frekuensi == 0 || strlen(daftarKata[i].kata) == 0)
            continue;
        fprintf(fp, "%-30s (%ld)\n", daftarKata[i].kata, daftarKata[i].frekuensi);
        tampil++;
    }
    fprintf(fp, "\nWaktu untuk mengurutkan: %.2f ms\n", waktuMs);
    fclose(fp);
    printf("[OK] Hasil disimpan ke: %s\n", namaFile);
}


/* ============================================================
 *  SECTION 3 - SORTING
 * ============================================================ */

void tukarKata(Kata *a, Kata *b) {
    Kata temp = *a; *a = *b; *b = temp;
}

/* --- INSERTION SORT - O(n²) --- */
double insertionSort(int W) {
    clock_t mulai = clock();
    int i, j;
    Kata kunci;
    for (i = 2; i <= W; i++) {
        kunci = daftarKata[i];
        j = i - 1;
        while (j >= 1 && daftarKata[j].frekuensi < kunci.frekuensi) {
            daftarKata[j+1] = daftarKata[j];
            j--;
        }
        daftarKata[j+1] = kunci;
    }
    clock_t selesai = clock();
    return (double)(selesai - mulai) / CLOCKS_PER_SEC * 1000.0;
}

/* --- QUICK SORT - O(n log n) rata-rata --- */
int partisi(int kiri, int kanan) {
    long pivot = daftarKata[kanan].frekuensi;
    int i = kiri - 1, j;
    for (j = kiri; j < kanan; j++) {
        if (daftarKata[j].frekuensi >= pivot) {
            i++;
            tukarKata(&daftarKata[i], &daftarKata[j]);
        }
    }
    tukarKata(&daftarKata[i+1], &daftarKata[kanan]);
    return i + 1;
}

void quickSortRekursif(int kiri, int kanan) {
    if (kiri < kanan) {
        int p = partisi(kiri, kanan);
        quickSortRekursif(kiri, p - 1);
        quickSortRekursif(p + 1, kanan);
    }
}

double quickSort(int W) {
    clock_t mulai = clock();
    quickSortRekursif(1, W);
    clock_t selesai = clock();
    return (double)(selesai - mulai) / CLOCKS_PER_SEC * 1000.0;
}

/* --- HEAP SORT - O(n log n) --- */
void heapify(int n, int i) {
    int terbesar = i;
    int kiri = 2*i, kanan = 2*i + 1;
    if (kiri  <= n && daftarKata[kiri].frekuensi  > daftarKata[terbesar].frekuensi) terbesar = kiri;
    if (kanan <= n && daftarKata[kanan].frekuensi > daftarKata[terbesar].frekuensi) terbesar = kanan;
    if (terbesar != i) {
        tukarKata(&daftarKata[i], &daftarKata[terbesar]);
        heapify(n, terbesar);
    }
}

double heapSort(int W) {
    clock_t mulai = clock();
    int i;
    for (i = W/2; i >= 1; i--)  heapify(W, i);       /* bangun max-heap */
    for (i = W;   i >= 2; i--) {                       /* cabut satu per satu */
        tukarKata(&daftarKata[1], &daftarKata[i]);
        heapify(i-1, 1);
    }
    /* hasil sekarang ascending → reverse jadi descending */
    int kiri = 1, kanan = W;
    while (kiri < kanan) { tukarKata(&daftarKata[kiri++], &daftarKata[kanan--]); }
    clock_t selesai = clock();
    return (double)(selesai - mulai) / CLOCKS_PER_SEC * 1000.0;
}


/* ============================================================
 *  SECTION 4 - DISPLAY
 * ============================================================ */

void tampilkanMetaData(MetaData meta, const char *namaFile) {
    printf("\n========================================\n");
    printf("  File     : %s\n", namaFile);
    printf("  Dokumen  : %d\n", meta.D);
    printf("  Kata unik: %d\n", meta.W);
    printf("  Total NNZ: %ld\n", meta.N);
    printf("========================================\n\n");
}

void tampilkanMenu() {
    printf("\n============= MENU =============\n");
    printf(" 1. Insertion Sort (metode sederhana)\n");
    printf(" 2. Quick Sort\n");
    printf(" 3. Heap Sort\n");
    printf(" 4. Tampilkan k kata terbesar ke layar\n");
    printf(" 5. Selesai\n");
    printf("================================\n");
    printf("Pilihan anda: ");
}

void tampilkanKKata(int W, int k, double waktuMs) {
    printf("\n=== TOP %d KATA DENGAN FREKUENSI TERBESAR ===\n\n", k);
    int i, tampil = 0;
    for (i = 1; i <= W && tampil < k; i++) {
        if (daftarKata[i].frekuensi == 0 || strlen(daftarKata[i].kata) == 0) continue;
        printf("%-30s (%ld)\n", daftarKata[i].kata, daftarKata[i].frekuensi);
        tampil++;
    }
    printf("\nWaktu untuk mengurutkan: %.2f ms\n", waktuMs);
}

void salinKata(Kata *sumber, Kata *tujuan, int W) {
    int i;
    for (i = 0; i <= W; i++) tujuan[i] = sumber[i];
}


/* ============================================================
 *  SECTION 5 - MAIN
 * ============================================================ */

int main() {
    MetaData meta;
    char namaDocword[300];
    char namaVocab[300];
    char namaOutput[300];
    int  pilihan, k;
    double waktuMs;

    Kata *cadangan = (Kata *)malloc(MAX_KATA * sizeof(Kata));
    if (cadangan == NULL) { printf("[ERROR] Gagal alokasi memori!\n"); return 1; }

    printf("============================================\n");
    printf("  BIG CHALLENGE - STRUKTUR DATA & ALGORITMA\n");
    printf("  SINF1004 - Universitas Syiah Kuala\n");
    printf("============================================\n\n");

    /* [1] Input nama file docword */
    printf("Tentukan file docword: ");
    scanf("%s", namaDocword);
    bersihkanInput(namaDocword);   /* <-- bersihkan \r atau spasi tersembunyi */

    /* [2] Baca metadata */
    meta = bacaMetaData(namaDocword);
    tampilkanMetaData(meta, namaDocword);

    /* [3] Tentukan nama vocab otomatis */
    tentukanNamaVocab(namaDocword, namaVocab);
    printf("[INFO] File vocab : %s\n", namaVocab);

    /* [4] Baca vocab & docword */
    bacaVocab(namaVocab, meta.W);
    bacaDocword(namaDocword, meta.N);

    /* [5] Simpan cadangan data asli */
    salinKata(daftarKata, cadangan, meta.W);

    /* [6] Loop menu */
    do {
        tampilkanMenu();
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                salinKata(cadangan, daftarKata, meta.W);
                printf("\n[INFO] Menjalankan Insertion Sort...\n");
                waktuMs = insertionSort(meta.W);
                printf("[OK] Selesai. Waktu: %.2f ms\n", waktuMs);
                printf("Nama file output: ");
                scanf("%s", namaOutput);
                simpanKeFile(namaOutput, meta.W, meta.W, waktuMs);
                break;

            case 2:
                salinKata(cadangan, daftarKata, meta.W);
                printf("\n[INFO] Menjalankan Quick Sort...\n");
                waktuMs = quickSort(meta.W);
                printf("[OK] Selesai. Waktu: %.2f ms\n", waktuMs);
                printf("Nama file output: ");
                scanf("%s", namaOutput);
                simpanKeFile(namaOutput, meta.W, meta.W, waktuMs);
                break;

            case 3:
                salinKata(cadangan, daftarKata, meta.W);
                printf("\n[INFO] Menjalankan Heap Sort...\n");
                waktuMs = heapSort(meta.W);
                printf("[OK] Selesai. Waktu: %.2f ms\n", waktuMs);
                printf("Nama file output: ");
                scanf("%s", namaOutput);
                simpanKeFile(namaOutput, meta.W, meta.W, waktuMs);
                break;

            case 4: {
                printf("Masukkan nilai k (10 < k < 150): ");
                scanf("%d", &k);
                if (k <= 10 || k >= 150) {
                    printf("[ERROR] Nilai k harus antara 11 dan 149!\n");
                    break;
                }
                printf("Pilih metode sorting:\n");
                printf("  1. Insertion Sort\n  2. Quick Sort\n  3. Heap Sort\n");
                printf("Pilihan: ");
                int ps;
                scanf("%d", &ps);
                salinKata(cadangan, daftarKata, meta.W);
                if      (ps == 1) { printf("[INFO] Insertion Sort...\n"); waktuMs = insertionSort(meta.W); }
                else if (ps == 2) { printf("[INFO] Quick Sort...\n");     waktuMs = quickSort(meta.W);     }
                else              { printf("[INFO] Heap Sort...\n");      waktuMs = heapSort(meta.W);      }
                tampilkanKKata(meta.W, k, waktuMs);
                break;
            }

            case 5:
                printf("\nProgram selesai. Terima kasih!\n");
                break;

            default:
                printf("[ERROR] Pilihan tidak valid! Masukkan 1-5.\n");
                break;
        }
    } while (pilihan != 5);

    free(cadangan);
    return 0;
}
