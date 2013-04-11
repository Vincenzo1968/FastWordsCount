FastWordsCount
==============

Program for counting words(and lines, chars, etc) in text files.

### How to compile:

*Linux*:

    gcc -Wall -W -std=c99 -O3 -funroll-loops -static words.c -o words
    
*Windows*:

    gcc -Wall -W -std=c99 -O3 -funroll-loops -static words.c -o words.exe
    
### How to run:

*Linux*:

    ./words myfile.txt
    
*Windows*:

    words myfile.txt
    
