#!/bin/bash
rm -rf output.txt
clear
g++ main.cpp -o main
time ./main n100.hardblocks n100.pl n100.nets #>> output.txt
# time ./main n200.hardblocks n200.pl n200.nets
# time ./main n300.hardblocks n300.pl n300.nets