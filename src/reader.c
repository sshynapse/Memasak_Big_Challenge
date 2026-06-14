#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "reader.h"

// Hapus karakter tersembunyi di akhir string.
void clean_input(char *str) {
    int len = strlen(str);
    // Mundur dari akhir string, hapus karakter whitespace satu per satu
    while (len > 0 && (str[len-1] == '\r' || str[len-1] == '\n' ||
                       str[len-1] == ' '  || str[len-1] == '\t')) {
        str[--len] = '\0'; // turunkan len dulu, lalu set null terminator
    }
}

// Ubah string ke huruf kecil (untuk pencarian case-insensitive) */
static void to_lower(char *str) {
    int i;
    for (i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]); // cast ke unsigned char agar aman di semua platform
}

// Deteksi nama vocab otomatis dari nama docword.
void find_vocab_name(const char *docword_path, char *vocab_path) {
    // Buat salinan lowercase agar pencarian "docword." tidak case-sensitive
    char lower[300];
    strncpy(lower, docword_path, sizeof(lower) - 1);
    lower[sizeof(lower) - 1] = '\0'; // pastikan null-terminated jika string terpotong
    to_lower(lower);

    const char *pos = strstr(lower, "docword."); // cari posisi kata "docword."
    if (pos != NULL) {
        int offset = pos - lower; // jarak dari awal string ke "docword." 

        // Salin prefix (misal "data/") ke vocab_path
        strncpy(vocab_path, docword_path, offset);
        vocab_path[offset] = '\0';

        // Sambung dengan "vocab." lalu sisa nama file setelah "docword." 
        strcat(vocab_path, "vocab.");
        strcat(vocab_path, docword_path + offset + strlen("docword."));
    } else {
        // Jika pola "docword." tidak ditemukan, minta input manual 
        printf("Nama vocab tidak terdeteksi otomatis.\n");
        printf("Masukkan nama file vocab : ");
        scanf("%s", vocab_path);
        clean_input(vocab_path);
    }
}

// Baca 3 baris pertama file docword -> D, W, N
MetaData read_metadata(const char *filename) {
    MetaData meta = {0, 0, 0}; // inisialisasi semua field ke 0

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("\n[ERROR] File '%s' tidak ditemukan!\n", filename);
        printf("Pastikan nama file dan path sudah benar.\n\n");
        exit(1);
    }

    // Format docword: baris 1=D (dokumen), 2=W (kata unik), 3=N (total NNZ)
    fscanf(fp, "%d", &meta.total_docs);
    fscanf(fp, "%d", &meta.total_words);
    fscanf(fp, "%ld", &meta.total_nnz);
    fclose(fp);

    return meta;
}

// Baca vocab + docword, hasilkan array WordFreq dinamis
WordFreq *read_dataset(const char *docword_path,
                       const char *vocab_path,
                       int total_words, long total_nnz) {

    // calloc: alokasi sekaligus inisialisasi semua byte ke 0 
    WordFreq *arr = (WordFreq *)calloc(total_words, sizeof(WordFreq));
    if (arr == NULL) {
        printf("[ERROR] Gagal alokasi memori!\n");
        exit(1);
    }

    // Baca vocab -> isi arr[i].word
    FILE *fp = fopen(vocab_path, "r");
    if (fp == NULL) {
        printf("\n[ERROR] File vocab '%s' tidak ditemukan!\n", vocab_path);
        printf("Pastikan file vocab ada di folder yang sama.\n\n");
        exit(1);
    }

    char buffer[200]; // buffer sementara untuk baca kata per baris
    int i;
    for (i = 0; i < total_words; i++) {
        if (fscanf(fp, "%s", buffer) == EOF) break; // hentikan jika file habis lebih awal

        // Alokasi dinamis per kata agar ukuran sesuai panjang kata
        arr[i].word = (char *)malloc(strlen(buffer) + 1); // +1 untuk '\0'
        if (arr[i].word == NULL) {
            printf("[ERROR] Gagal alokasi memori!\n");
            exit(1);
        }
        strcpy(arr[i].word, buffer);
        arr[i].freq = 0; // frekuensi awal 0, akan diakumulasi dari docword
    }
    fclose(fp);

    // Baca NNZ docword -> akumulasi frekuensi
    fp = fopen(docword_path, "r");
    if (fp == NULL) {
        printf("\n[ERROR] File '%s' tidak ditemukan!\n", docword_path);
        exit(1);
    }

    // Lewati 3 baris pertama (D, W, N) yang sudah dibaca di read_metadata sebelumnya
    int  d, w;
    long n;
    fscanf(fp, "%d", &d);
    fscanf(fp, "%d", &w);
    fscanf(fp, "%ld", &n);

    int  doc_id, word_id;
    long count, rows = 0;

    // Cetak titik progress setiap 1/20 dari total baris (= setiap 5%) 
    long report = (total_nnz > 0) ? total_nnz / 20 : 1;
    if (report == 0) report = 1; // hindari modulo 0 

    printf("Membaca data");
    fflush(stdout); // paksa buffer tampil sebelum loop panjang dimulai

    // Format tiap baris NNZ: docID wordID count
    while (fscanf(fp, "%d %d %ld", &doc_id, &word_id, &count) == 3) {
        int idx = word_id - 1; // wordID di file mulai dari 1, array dari 0
        if (idx >= 0 && idx < total_words)
            arr[idx].freq += count; // akumulasi frekuensi kata lintas dokumen

        rows++;
        if (rows % report == 0) {
            printf(".");
            fflush(stdout);
        }
    }

    printf("SELESAI!\n");
    fclose(fp);

    return arr;
}

// Ambil nama dataset dari path docword
void get_dataset_name(const char *docword_path, char *out) {
    char lower[300];
    strncpy(lower, docword_path, sizeof(lower) - 1);
    lower[sizeof(lower) - 1] = '\0';
    to_lower(lower);

    const char *pos = strstr(lower, "docword.");
    if (pos == NULL) {
        strcpy(out, "dataset"); // fallback jika pola tidak ditemukan
        return;
    }

    // Loncat melewati "docword." untuk menunjuk ke nama dataset
    const char *start = docword_path + (pos - lower) + strlen("docword.");

    // Salin karakter sampai ketemu titik berikutnya (ekstensi .txt)
    int j = 0;
    while (start[j] != '.' && start[j] != '\0') {
        out[j] = tolower((unsigned char)start[j]);
        j++;
    }
    out[j] = '\0'; // null-terminate hasil
}
