from matplotlib import pyplot as plt
from matplotlib.ticker import NullFormatter  # useful for `logit` scale
import numpy as np

xloc = [2**x for x in range (1,30)]
yloc = [10**x for x in range (1,12)]

xstd = [2**x for x in range(1,30)]
xbubble = [2**x for x in range(1,18)]

plt.figure()


#ystd = [0, 0, 0, 0, 0, 1, 3, 7, 16, 35, 74, 165, 358, 780, 1635, 3579, 7513, 15870, 33680, 70139, 147105, 305075, 644179, 1345888, 2799478, 5848743, 12102433, 25292757, 51897821]
#ycount = [1, 0, 0, 0, 0, 0, 1, 2, 3, 8, 16, 30, 65, 141, 306, 534, 1184, 2370, 4712, 9845, 25308, 69115, 159681, 338050, 695602, 1411910, 2844248, 5700768]
#ybubble = [ 0, 0, 0, 0, 1, 5, 48, 80, 265, 774, 2815, 13305, 65602, 305125, 1312169, 5368860, 21909132] 

ycount= [1922, 240, 221, 316, 490, 668, 1364, 2677, 4686, 11128, 20908, 43942, 90988, 182325, 356566, 695943, 1367424, 3319583, 5385354, 10951925, 33484040, 110629063, 229621188, 446624433, 904006881, 1798417743, 3564697907, 7087188097,]

ystd = [456, 58, 701, 322, 1249, 2288, 4996, 11037, 25636, 54855, 131811, 263058, 636378, 1255819, 2463528, 4634323, 8823081, 17907710, 37997277, 79414014, 168707401, 348715256, 729992902, 1536407397, 3198965524, 6662820838, 13924334248, 28652312405, 59225268930,]

ystdalmost = [208, 100, 71, 70, 465, 1370, 2770, 6377, 12925, 45718, 57680, 117965, 260258, 553279, 1132033, 1835084, 3582560, 7374332, 20847030, 33607710, 103486142, 156411844, 335988061, 654508435, 1385107325, 3219995612, 5630136051, 13352740297, 25455718513,]


plt.subplot(221)
plt.xscale('log', base=2)
plt.yscale('log', base=10)
plt.gca().set_xticks(xloc)
plt.gca().set_yticks(yloc)
plt.grid(True)
plt.xlabel('no. elements')
plt.ylabel('nanoseconds')
plt.title('random vectors')
plt.plot([2**x for x in range(1,29)], ycount, 'bs', xstd, ystd, 'g^', xstd, ystdalmost, 'r*',)
plt.legend(['count_sort','std::sort','std::sord almost sorted'])


plt.subplot(222)
plt.xscale('log', base=2)
plt.yscale('log', base=10)
plt.gca().set_xticks(xloc)
plt.gca().set_yticks(yloc)
plt.grid(True)
plt.xlabel('no. elements')
plt.ylabel('nanoseconds')
plt.title('random vectors')
plt.plot([2**x for x in range(1,29)], ycount, 'co', xstd, ystd, 'kH',)
plt.legend(['count_sort','std::sort','std::sord almost sorted'])


plt.show()

# https://matplotlib.org/3.1.1/tutorials/introductory/pyplot.html#sphx-glr-tutorials-introductory-pyplot-py
#plt.plot(xstd, ystd, 'bs', xcount, ycount, 'g^', xbubble, ybubble, 'r*')
