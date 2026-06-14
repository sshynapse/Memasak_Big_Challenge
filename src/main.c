#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "sort.h"

int main(void)
{
    char docword_path[300];
    char vocab_path[300]  = {0};
    char dataset_name[100] = {0};
    char filename[400];

    printf("\n=======================================================\n");
    printf(" BIG CHALLENGE GANJIL - Struktur Data dan Algoritma B   \n");
    printf("=======================================================\n\n");

    printf("Tentukan file docword: ");
    scanf("%299s", docword_path);
    clean_input(docword_path);

    find_vocab_name(docword_path, vocab_path);
    get_dataset_name(docword_path, dataset_name);

    MetaData meta = read_metadata(docword_path);

    printf("\nDATASET YANG DIMUAT\n");
    printf("--------------------------------------------------------\n");
    printf("File docword      : %s\n", docword_path);
    printf("File vocab        : %s\n", vocab_path);
    printf("Jumlah dokumen(D) : %d\n", meta.total_docs);
    printf("Kata unik(W)      : %d\n", meta.total_words);
    printf("Total NNZ(N)      : %ld baris\n", meta.total_nnz);
    printf("--------------------------------------------------------\n\n");

    WordFreq *arr = read_dataset(docword_path, vocab_path, meta.total_words, meta.total_nnz);
    unsigned long n = (unsigned long)meta.total_words;

    printf("\n/*************************** Petunjuk ***************************** \n");
    printf("  1. File docword dan vocabulary telah dibuka.                      \n");
    printf("  2. Data telah dibaca baris per baris dan disimpan dalam struktur  \n");
    printf("     data WordFreq (array kata + total frekuensi).                   \n");
    printf("  3. Selanjutnya, MENU pilihan ditampilkan dan pilihan ditentukan   \n");
    printf("     oleh user. Proses dilanjutkan berdasarkan pilihan user.         \n");
    printf("************************** Akhir Petunjuk *************************/\n");

    int choice;
    SortResult res;

    do {
         printf("\n=====================================================================\n");
        printf("                            MENU UTAMA                                  \n");
        printf("=====================================================================\n");
        printf(" [1] Urutkan data menggunakan salah satu metode pengurutan\n");
        printf("     sederhana (Bubble Sort).\n");
        printf("     -Simpan keluaran dalam sebuah file teks secara descending order,\n");
        printf("     termasuk waktu yang dibutuhkan untuk mengurutkan data tersebut-\n");
        printf(" [2] Urutkan data menggunakan metode quicksort.\n");
        printf("     -Simpan keluaran dalam sebuah file teks secara descending order\n");
        printf("     dan tampilkan waktu yang dibutuhkan untuk mengurutkan data-\n");
        printf(" [3] Urutkan data menggunakan metode heapsort.\n");
        printf("     -Simpan keluaran dalam sebuah file teks secara descending order\n");
        printf("     dan tampilkan waktu yang dibutuhkan untuk mengurutkan data-\n");
        printf(" [4] Tampilkan ke layar monitor k kata dengan frekuensi terbesar\n");
        printf("     termasuk waktu yang dibutuhkan untuk mengurutkan data tersebut.\n");
        printf(" [5] Selesai\n");
        printf("-----------------------------------------------------------------------\n");
        printf("Pilihan anda: ");

        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("Input tidak valid, masukkan angka 1-5.\n");
            continue;
        }

        switch (choice) {
        case 1:
            res = sort_bubble(arr, n);
            snprintf(filename, sizeof(filename), "output_bubble_%s.txt", dataset_name);
            sort_write_output(filename, arr, n, n, res.elapsed_ms);
            printf("\nData berhasil diurutkan dengan Bubble Sort.\n");
            printf("Hasil disimpan ke file: %s\n", filename);
            printf("Waktu untuk mengurutkan: %.0f ms (%.2f detik)\n", res.elapsed_ms, res.elapsed_ms / 1000.0);
            break;

        case 2:
            res = sort_quick(arr, n);
            snprintf(filename, sizeof(filename), "output_quick_%s.txt", dataset_name);
            sort_write_output(filename, arr, n, n, res.elapsed_ms);
            printf("\nData berhasil diurutkan dengan Quick Sort.\n");
            printf("Hasil disimpan ke file: %s\n", filename);
            printf("Waktu untuk mengurutkan: %.0f ms (%.2f detik)\n", res.elapsed_ms, res.elapsed_ms / 1000.0);
            break;

        case 3:
            res = sort_heap(arr, n);
            snprintf(filename, sizeof(filename), "output_heap_%s.txt", dataset_name);
            sort_write_output(filename, arr, n, n, res.elapsed_ms);
            printf("\nData berhasil diurutkan dengan Heap Sort.\n");
            printf("Hasil disimpan ke file: %s\n", filename);
            printf("Waktu untuk mengurutkan: %.0f ms (%.2f detik)\n", res.elapsed_ms, res.elapsed_ms / 1000.0);
            break;

        case 4: {
            unsigned long k;
            printf("\nTAMPILKAN K KATA TERBESAR\n");
            printf("Masukkan nilai k (11 - 149) : ");
            scanf("%lu", &k);

            if (k <= 10 || k >= 150) {
                printf("\n[ERROR] Nilai k harus antara 11 dan 149!\n");
                break;
            }
            if (k > n) k = n;
            res = sort_quick(arr, n);

            printf("------------------------------------------------\n");
            printf("Top-%lu kata dengan frekuensi terbesar\n", k);
            sort_print_top(arr, n, k, res.elapsed_ms);  
            printf("------------------------------------------------\n");

            break;
        }

        case 5:
            printf("\nProgram selesai...\n");
            break;

        default:
            printf("Pilihan tidak valid! Masukkan angka 1-5.\n");
        }

    } while (choice != 5);

    sort_free(arr, n);
    return 0;
}
