#!/bin/bash
cat ./header.py > plot.py
./ex >> plot.py
echo "plt.show()" >> plot.py
