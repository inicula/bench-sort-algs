from matplotlib import pyplot as plt
import numpy as np

xstd = [2**x for x in range(1,30)]
xcount = [2**x for x in range(1,29)]
xbubble = [2**x for x in range(1,18)]


ystd = [0, 0, 0, 0, 0, 1, 3, 7, 16, 35, 74, 165, 358, 780, 1635, 3579, 7513, 15870, 33680, 70139, 147105, 305075, 644179, 1345888, 2799478, 5848743, 12102433, 25292757, 51897821]
ycount = [1, 0, 0, 0, 0, 0, 1, 2, 3, 8, 16, 30, 65, 141, 306, 534, 1184, 2370, 4712, 9845, 25308, 69115, 159681, 338050, 695602, 1411910, 2844248, 5700768]
ybubble = [ 0, 0, 0, 0, 1, 5, 48, 80, 265, 774, 2815, 13305, 65602, 305125, 1312169, 5368860, 21909132] 

plt.plot(xstd, ystd, 'bs', xcount, ycount, 'g^', xbubble, ybubble, 'r*')
plt.xscale('log', base=2)
plt.yscale('log', base=2)
plt.grid(True)
plt.legend(['std::sort','count_sort','bubble_sort'])
plt.show()


# https://matplotlib.org/3.1.1/tutorials/introductory/pyplot.html#sphx-glr-tutorials-introductory-pyplot-py
