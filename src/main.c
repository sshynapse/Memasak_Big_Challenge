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

    printf("Tentukan file docword: ");
    scanf("%299s", docword_path);
    clean_input(docword_path);

    find_vocab_name(docword_path, vocab_path);
    get_dataset_name(docword_path, dataset_name);

    printf("\nFile docword : %s\n", docword_path);
    printf("File vocab   : %s\n", vocab_path);
    printf("Dataset      : %s\n", dataset_name);

    MetaData meta = read_metadata(docword_path);
    printf("\nD (dokumen) = %d\n", meta.total_docs);
    printf("W (kata unik) = %d\n", meta.total_words);
    printf("N (baris NNZ) = %ld\n\n", meta.total_nnz);

    WordFreq *arr = read_dataset(docword_path, vocab_path, meta.total_words, meta.total_nnz);
    unsigned long n = (unsigned long)meta.total_words;

    printf("/*************************** Petunjuk *****************************\n");
    printf("File docword dan vocabulary telah dibuka.\n");
    printf("Data telah dibaca baris per baris dan disimpan dalam struktur\n");
    printf("data WordFreq (array kata + total frekuensi).\n");
    printf("Selanjutnya, MENU pilihan ditampilkan dan pilihan ditentukan\n");
    printf("oleh user. Proses dilanjutkan berdasarkan pilihan user.\n");
    printf("************************** Akhir Petunjuk *************************/\n");

    int choice;
    SortResult res;
    unsigned long k;

    do {
        printf("\nPilihan:\n");
        printf("1) Urutkan data menggunakan salah satu metode pengurutan\n");
        printf("   sederhana (Bubble Sort).\n");
        printf("   Simpan keluaran dalam sebuah file teks secara descending\n");
        printf("   order, termasuk waktu yang dibutuhkan untuk mengurutkan\n");
        printf("   data tersebut.\n");
        printf("2) Urutkan data menggunakan metode quicksort.\n");
        printf("   Simpan keluaran dalam sebuah file teks secara descending\n");
        printf("   order dan tampilkan waktu yang dibutuhkan untuk\n");
        printf("   mengurutkan data tsb.\n");
        printf("3) Urutkan data menggunakan metode heapsort.\n");
        printf("   Simpan keluaran dalam sebuah file teks secara descending\n");
        printf("   order dan tampilkan waktu yang dibutuhkan untuk\n");
        printf("   mengurutkan data tsb.\n");
        printf("4) Tampilkan ke layar monitor 100 kata dengan frekuensi\n");
        printf("   terbesar termasuk waktu yang dibutuhkan untuk mengurutkan\n");
        printf("   data tsb.\n");
        printf("5) Selesai\n");
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
            printf("Waktu untuk mengurutkan: %.2f ms\n", res.elapsed_ms);
            break;

        case 2:
            res = sort_quick(arr, n);
            snprintf(filename, sizeof(filename), "output_quick_%s.txt", dataset_name);
            sort_write_output(filename, arr, n, n, res.elapsed_ms);
            printf("\nData berhasil diurutkan dengan Quick Sort.\n");
            printf("Hasil disimpan ke file: %s\n", filename);
            printf("Waktu untuk mengurutkan: %.2f ms\n", res.elapsed_ms);
            break;

        case 3:
            res = sort_heap(arr, n);
            snprintf(filename, sizeof(filename), "output_heap_%s.txt", dataset_name);
            sort_write_output(filename, arr, n, n, res.elapsed_ms);
            printf("\nData berhasil diurutkan dengan Heap Sort.\n");
            printf("Hasil disimpan ke file: %s\n", filename);
            printf("Waktu untuk mengurutkan: %.2f ms\n", res.elapsed_ms);
            break;

        case 4:
            printf("Nilai k (default 100, isi 0 untuk pakai default): ");
            if (scanf("%lu", &k) != 1) {
                int c;
                while ((c = getchar()) != '\n' && c != EOF) { }
                printf("Input k tidak valid.\n");
                break;
            }
            if (k == 0) k = 100;
            if (k > n) k = n;

            res = sort_quick(arr, n);
            printf("\n%lu kata dengan frekuensi terbesar:\n\n", k);
            sort_print_top(arr, n, k, res.elapsed_ms);
            break;

        case 5:
            printf("\nProgram selesai.\n");
            break;

        default:
            printf("Pilihan tidak valid! Masukkan angka 1-5.\n");
        }

    } while (choice != 5);

    sort_free(arr, n);

    return 0;
}