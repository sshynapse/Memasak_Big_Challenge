#ifndef SORT_H
#define SORT_H

typedef struct {
    char  *word;
    long   freq;
} WordFreq;

typedef struct {
    double elapsed_ms;
} SortResult;

// Pengurutan bubble sort
SortResult sort_bubble(WordFreq *arr, unsigned long n);

// Pengurutan quick sort – O(n log n) rata-rata, O(n^2) terburuk
SortResult sort_quick(WordFreq *arr, unsigned long n);

// Pengurutan heap sort – O(n log n) rata-rata dan terburuk
SortResult sort_heap(WordFreq *arr, unsigned long n);

int  sort_write_output(const char *filename, const WordFreq *arr, unsigned long n, unsigned long k, double elapsed_ms);

// Cetak elemen kata dan frekuensi tertinggi (top k) dari array arr dengan ukuran n, serta waktu yang dibutuhkan untuk pengurutan.
void sort_print_top(const WordFreq *arr, unsigned long n, unsigned long k, double elapsed_ms);

// Bebaskan memori yang digunakan oleh array WordFreq.
void sort_free(WordFreq *arr, unsigned long n);

#endif