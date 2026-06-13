#include "sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Kembalikan waktu saat ini dalam milidetik (double) untuk pengukuran durasi.
static double now_ms(void)
{
    return (double)clock() / (CLOCKS_PER_SEC / 1000.0);
}

static inline void swap(WordFreq *a, WordFreq *b)
{
    WordFreq tmp = *a;
    *a = *b;
    *b = tmp;
}

// Bubble sort – O(n²) selalu, stabil, in-place
SortResult sort_bubble(WordFreq *arr, unsigned long n)
{
    double t0 = now_ms();

    for (unsigned long i = 0; i < n - 1; i++) {
        int swapped = 0;

        for (unsigned long j = 0; j < n - 1 - i; j++) {
            if (arr[j].freq < arr[j + 1].freq) {
                swap(&arr[j], &arr[j + 1]);
                swapped = 1;
            }
        }

        if (!swapped) break;
    }

    SortResult res;
    res.elapsed_ms = now_ms() - t0;
    return res;
}

// Quick Sort – rata-rata O(n log n), worst-case O(n²) jika pivot buruk
// Pilih pivot dengan median-of-three untuk mengurangi risiko O(n²) pada data hampir terurut.
static unsigned long median_of_three(WordFreq *arr, unsigned long lo, unsigned long hi)
{
    unsigned long mid = lo + (hi - lo) / 2;

    if (arr[lo].freq < arr[mid].freq) swap(&arr[lo],  &arr[mid]);
    if (arr[lo].freq < arr[hi].freq)  swap(&arr[lo],  &arr[hi]);
    if (arr[mid].freq < arr[hi].freq) swap(&arr[mid], &arr[hi]);

    swap(&arr[mid], &arr[hi]);
    return hi;
}

// Mengembalikan indeks pivot akhir.
static unsigned long partition(WordFreq *arr, unsigned long lo, unsigned long hi)
{
    if (hi - lo >= 2)
        median_of_three(arr, lo, hi);

    long pivot_freq = arr[hi].freq;
    unsigned long i = lo;

    for (unsigned long j = lo; j < hi; j++) {
        if (arr[j].freq >= pivot_freq) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }
    swap(&arr[i], &arr[hi]);
    return i;
}

// Menghindari stack overflow pada quicksort rekursif dengan menggunakan pendekatan iteratif dan stack manual.
static void quicksort_iterative(WordFreq *arr, unsigned long lo, unsigned long hi)
{
    unsigned long stack_size = 64;
    unsigned long *stack = (unsigned long *)malloc(stack_size * 2 * sizeof(unsigned long));
    if (!stack) {
        if (lo < hi) {
            unsigned long p = partition(arr, lo, hi);
            if (p > 0) quicksort_iterative(arr, lo, p - 1);
            quicksort_iterative(arr, p + 1, hi);
        }
        return;
    }

    unsigned long top = 0;
    stack[top++] = lo;
    stack[top++] = hi;

    while (top > 0) {
        unsigned long h = stack[--top];
        unsigned long l = stack[--top];

        if (l >= h) continue;

        unsigned long p = partition(arr, l, h);

        unsigned long left_size  = (p > l) ? (p - l - 1) : 0;
        unsigned long right_size = (p < h) ? (h - p)     : 0;

        if (top + 4 > stack_size) {
            stack_size *= 2;
            unsigned long *tmp = (unsigned long *)realloc(stack,
                              stack_size * 2 * sizeof(unsigned long));
            if (!tmp) { free(stack); return; }
            stack = tmp;
        }

        if (left_size >= right_size) {
            if (p > l + 1) { stack[top++] = l; stack[top++] = p - 1; }
            if (p < h)     { stack[top++] = p + 1; stack[top++] = h; }
        } else {
            if (p < h)     { stack[top++] = p + 1; stack[top++] = h; }
            if (p > l + 1) { stack[top++] = l; stack[top++] = p - 1; }
        }
    }

    free(stack);
}

SortResult sort_quick(WordFreq *arr, unsigned long n)
{
    SortResult res;
    if (n <= 1) { res.elapsed_ms = 0.0; return res; }

    double t0 = now_ms();
    quicksort_iterative(arr, 0, n - 1);
    res.elapsed_ms = now_ms() - t0;
    return res;
}

// Heap Sort – O(n log n) dijamin, tidak stabil, in-place
// Sift-down untuk mempertahankan properti max-heap.
static void sift_down(WordFreq *arr, unsigned long root, unsigned long end)
{
    while (1) {
        unsigned long child = 2 * root + 1;
        if (child > end) break;

        if (child + 1 <= end && arr[child + 1].freq > arr[child].freq)
            child++;

        if (arr[root].freq >= arr[child].freq)
            break;
        swap(&arr[root], &arr[child]);
        root = child;
    }
}

// Build max-heap dari array arr dengan n elemen.
static void build_max_heap(WordFreq *arr, unsigned long n)
{
    if (n == 0) return;
    unsigned long start = (n / 2);
    while (start > 0) {
        start--;
        sift_down(arr, start, n - 1);
    }
}

SortResult sort_heap(WordFreq *arr, unsigned long n)
{
    SortResult res;
    if (n <= 1) { res.elapsed_ms = 0.0; return res; }

    double t0 = now_ms();
    build_max_heap(arr, n);
    unsigned long end = n - 1;
    while (end > 0) {
        swap(&arr[0], &arr[end]);
        end--;
        sift_down(arr, 0, end);
    }

    unsigned long lo = 0, hi = n - 1;
    while (lo < hi) {
        swap(&arr[lo], &arr[hi]);
        lo++; hi--;
    }

    res.elapsed_ms = now_ms() - t0;
    return res;
}

int sort_write_output(const char *filename, const WordFreq *arr, unsigned long n, unsigned long k, double elapsed_ms)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "[sort] Gagal membuka file: %s\n", filename);
        return -1;
    }

    unsigned long limit = (k < n) ? k : n;
    for (unsigned long i = 0; i < limit; i++) {
        fprintf(fp, "%s (%ld)\n", arr[i].word, arr[i].freq);
    }
    fprintf(fp, "\nWaktu untuk mengurutkan: %.2f ms\n", elapsed_ms);

    fclose(fp);
    return 0;
}

void sort_print_top(const WordFreq *arr, unsigned long n, unsigned long k, double elapsed_ms)
{
    unsigned long limit = (k < n) ? k : n;
    for (unsigned long i = 0; i < limit; i++) {
        printf("%s (%ld)\n", arr[i].word, arr[i].freq);
    }
    printf("\nWaktu untuk mengurutkan: %.2f ms\n", elapsed_ms);
}

void sort_free(WordFreq *arr, unsigned long n)
{
    if (!arr) return;
    for (unsigned long i = 0; i < n; i++) {
        free(arr[i].word);
    }
    free(arr);
}
