# LLP Lab 1

## Сборка и запуск
### Linux
**Требования:**
* CMake 3.23 или новее
* Boost 1.74.0 или новее
* Компилятор C++ (clang 14 или новее, gcc 11 или новее)
```shell
git clone https://github.com/farid03/llp-database .
cmake -B build && cd build && make
./llp_database
```

### Windows
**Требования:**

```shell
git clone https://github.com/farid03/llp-database .

```

## Описание

## Тесты
### Добавление элемента в дерево
![img_1.png](docs/img_1.png)
### Обновление элемента по id
![img_2.png](docs/img_2.png)
### Выборка элемента по id
![img_3.png](docs/img_3.png)
### Выборка элементов по условию
![img_4.png](docs/img_4.png)
### Поиск элементов по родителю (по отношению)
![img_5.png](docs/img_5.png)
### Удаление элемента без зависмостей
![img_6.png](docs/img_6.png)
### Удаление элемента с зависимостями
![img_7.png](docs/img_7.png)
### Размер файла от количества элементов
![img_8.png](docs/img_8.png)
