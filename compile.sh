#!/bin/sh
g++ -std=c++20 -O3 -march=native -flto -fno-exceptions -Wall -Wextra -Wpedantic main.cpp -o ex
