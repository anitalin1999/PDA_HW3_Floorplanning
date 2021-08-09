#!/bin/bash
rm -rf output.txt
clear
g++ main.cpp -o main
./main n100.hardblocks n100.pl n100.nets >> output.txt