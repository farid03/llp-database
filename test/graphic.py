import csv
import random

from matplotlib import pyplot as plt
import pylab

num = [[]]
time = [[]]
xlab = [
    'Количество элементов в дереве, шт',
    'Количество элементов в дереве, шт',
    'Количество элементов в дереве, шт',
    'Количество элементов в дереве, шт',
    'Количество элементов в дереве, шт',
    'Количество элементов в дереве, шт',
    'Количество зависимостей элемента, шт',
    'Количество элементов в дереве, шт',
]
tit = [
    'Добавление элемента в дерево',
    'Обновление элемента по id',
    'Выборка элемента по id',
    'Выборка элементов по условию',
    'Поиск элементов по родителю (по отношению)',
    'Удаление элемента без зависимостей',
    'Удаление элемента с зависимостями',
    'Размер файла от количества элементов',
]
ymax = [0.0005, 0.00100, 0.0003, 1.3000, 1.1000, 0.00007, 0.2350, 3703964 / 1024]
with open('data20000_2') as csvfile:
    spamreader = csv.DictReader(csvfile)
    i = 0
    for row in spamreader:
        if row["elem_count"] == "$" and row["time"] == "$":
            print(max(time[i])) # для определения корректных границ по оси oy
            i += 1
            num.append([])
            time.append([])
            continue
        if row["elem_count"] == "elem_count":
            continue
        num[i].append(int(row["elem_count"]))
        time[i].append(float(row["time"]))

GITHUB_COLOR = "#0d1117"
GRAY = "gray"
for i in range(8):
    fig = plt.figure()
    fig.patch.set_facecolor(GITHUB_COLOR)
    fig.set_figwidth(8)
    ax = plt.axes()
    ax.set(facecolor=GITHUB_COLOR)
    if i != 7:
        plt.ylabel('Время, c', color=GRAY)
    else:
        plt.ylabel('Размер файла, КиБ', color=GRAY)
        time[i] = [j / 1024 for j in time[i]]

    plt.xlabel(xlab[i], color=GRAY)
    plt.title(tit[i], color=GRAY)
    ax.plot(num[i], time[i], linewidth=1)
    ax.tick_params(axis='both', colors=GRAY)
    ax.spines["top"].set_color(GRAY)
    ax.spines["bottom"].set_color(GRAY)
    ax.spines["right"].set_color(GRAY)
    ax.spines["left"].set_color(GRAY)

    pylab.ylim(ymax=ymax[i])
    plt.show()