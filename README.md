# cacheSimulator
A cache simulator that I wrote in C for CS351

To use the cache simulator, first compile the project by running 
$ make

then to actually use the cache simulaor, run as follows

example:
./cachesim 8 2 4 < trace01.txt

the first three arguments after ./cachesim are total lines, lines per set, and block size.

the IO redirection is necessary because the trace files actually give the program input so that the program can simulate the cache.
