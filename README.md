# Memasak_Big_Challenge

big-challenge-ganjil-ea/
├── data/
│   ├── docword.kos.txt
│   ├── docword.nips.txt
│   ├── docword.enron.txt
│   ├── docword.nytimes.txt
│   ├── docword.pubmed.txt
│   ├── vocab.kos.txt
│   ├── vocab.nips.txt
│   ├── vocab.enron.txt
│   ├── vocab.nytimes.txt
│   └── vocab.pubmed.txt
├── output/
│   ├── result_bubblesort.txt
│   ├── result_quicksort.txt
│   └── result_heapsort.txt
├── src/
│   ├── main.c        <- entry point, menu, input user
│   ├── main.h        <- struct WordFreq, definisi umum
│   ├── reader.c      <- baca docword & vocab, akumulasi frekuensi
│   ├── reader.h      <- deklarasi fungsi reader
│   ├── sort.c        <- bubble / quicksort / heapsort
│   └── sort.h        <- deklarasi fungsi sorting
└── Makefile          <- kompilasi semua .c
