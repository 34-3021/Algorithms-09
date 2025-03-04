# dots from result.txt with format y,x

import matplotlib.pyplot as plt

x = []
y = []

with open('result.txt', 'r') as f:
    # 散点
    for line in f:
        line = line.strip()
        cury = float(line.split(',')[0])
        curx = float(line.split(',')[1])
        inverted = int(line.split(',')[2])
        lasty = y[-1] if y else 0
        lastx = x[-1] if x else 0
        if (lasty != cury+1 or lastx != curx+1) and (lasty != 0 and lastx != 0):
            yfrom = lasty if inverted else cury-(lastx-curx)
            yto = cury-(lastx-curx) if inverted else lasty
            plt.plot([lastx, curx+1], [yto, yfrom])
        y.append(cury)
        x.append(curx)

plt.scatter(x, y)
plt.show()
