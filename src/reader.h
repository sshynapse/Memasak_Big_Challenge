#ifndef READER_H
#define READER_H

#include "sort.h"   // WordFreq didefinisikan di sort.h 

// Metadata dari 3 baris pertama file docword 
typedef struct {
    int  total_docs;    // D: jumlah dokumen 
    int  total_words;   // W: jumlah kata unik
    long total_nnz;     // N: jumlah baris NNZ
} MetaData;

// Bersihkan karakter tersembunyi di akhir string
void      clean_input      (char *str);

// Deteksi nama vocab otomatis dari nama docword
void      find_vocab_name  (const char *docword_path, char *vocab_path);

// Baca 3 baris pertama docword -> D, W, N
MetaData  read_metadata    (const char *filename);

// Baca vocab + docword, return array WordFreq dinamis
// Caller wajib panggil sort_free(arr, W) setelah selesai
WordFreq *read_dataset     (const char *docword_path,
                            const char *vocab_path,
                            int total_words, long total_nnz);

// Ambil nama dataset dari path docword
void      get_dataset_name (const char *docword_path, char *out);

#endif
