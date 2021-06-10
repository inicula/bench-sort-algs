#!/bin/sh
g++ -std=c++20 -O3 -march=native -flto -fno-exceptions -fno-rtti -Wall -Wextra -Wpedantic -Werror main.cpp -o ex -lfmt
