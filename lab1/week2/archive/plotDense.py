# lines from result.txt with format y1,x1

import matplotlib.pyplot as plt

x = []
y = []

with open('f.txt', 'r') as f:
    # 散点
    for line in f:
        line = line.strip()
        cury = float(line.split(',')[0])
        curx = float(line.split(',')[1])
        y.append(cury)
        x.append(curx)

plt.scatter(x, y)
plt.show()
