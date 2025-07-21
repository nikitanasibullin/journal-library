# Библиотека для записи сообщений в журнал.

Этот проект представляет собой библиотеку `libjournal` для логирования сообщений в файл или через TCP сокет, с поддержкой статической и динамической сборки. Проект включает утилиты для отправки и получения логов, а также тесты для проверки функциональности.
Сообщения, логируемые данной библиотекой, содержат также информацию о уровне важности сообщения и времени его получения.

## Структура проекта

Проект состоит из пяти папок:

- **consoleapp**: Содержит `consoleapp.cpp` и `CMakeLists.txt`, компилируемые в приложение для отправки логов в файл.
- **include**: Содержит заголовочный файл `myJournal.hpp` с интерфейсом библиотеки.
- **src**: Содержит исходный код библиотеки `myJournal.cpp` и `CMakeLists.txt` для сборки `libjournal.a` или `libjournal.so`.
- **tests**: Содержит `tests_journal.cpp` и `CMakeLists.txt` для запуска тестов с GoogleTest.
- **socket_consoleapp**: Содержит `socket_consoleapp.cpp`и `CMakeLists.txt`, компилируемые в приложение для получения логов через TCP сокет.

## Сборка проекта

### Требования
- CMake 3.14 или выше
- Компилятор с поддержкой C++17
- Ubuntu/Debian
- (GoogleTest загружается автоматически для тестов)

### Сборка библиотеки
1. Перейдите в папку `src`:
   ```bash
   cd src
   mkdir build && cd build
   ```
2. Статическая сборка (`libjournal.a`):
   ```bash
   cmake ..
   make
   make install
   ```
   - Создаёт `src/build/lib/libjournal.a`.
3. Динамическая сборка (`libjournal.so`):
   ```bash
   cmake -DBUILD_SHARED_LIB=ON ..
   make
   make install
   ```
   - Создаёт `src/build/lib/libjournal.so`.

### Сборка consoleapp
1. Перейдите в папку `consoleapp`:
   ```bash
   cd ../../consoleapp
   mkdir build && cd build
   ```
2. Скомпилируйте:
   ```bash
   cmake ..
   make
   ```
3. Запустите:
- Статическая:
     ```bash
     ./consoleapp log.txt INFO --socket 127.0.0.1 8080
     ```
- Динамическая:
     ```bash
     LD_LIBRARY_PATH=../src/build/lib ./consoleapp log.txt INFO --socket 127.0.0.1 8080
     ```

### Сборка и запуск тестов
1. Перейдите в папку `tests`:
   ```bash
   cd ../../tests
   mkdir build && cd build
   ```
2. Скомпилируйте:
   ```bash
   cmake ..
   make
   ```
3. Запустите тесты:
 - Статическая:
     ```bash
     ./tests
     ```
- Динамическая:
     ```bash
     LD_LIBRARY_PATH=../src/build/lib ./tests
     ```


### Сборка socket_consoleapp
1. Перейдите в папку `socket_consoleapp`:
   ```bash
   cd ../../socket_consoleapp
   mkdir build && cd build
   ```
2. Скомпилируйте (аналогично `consoleapp`):
   ```bash
   cmake ..
   make
   ```
3. Запустите сервер:
	например порт=8080, частота=3, период=5;
 - Статическая:
     ```bash
     ./socket_consoleapp 8080 3 5 
     ```
- Динамическая:
     ```bash
     LD_LIBRARY_PATH=../src/build/lib ./socket_consoleapp 8080 3 5 
     ```

## Использование
- **consoleapp**: Консольное многопоточное приложение которое позволяет записывать сообщений в журнал(файл).
- **tests**: Проверяет функциональность библиотеки `journal` и консольного приложения consoleapp с помощью GoogleTest.
(+Дополнительно)
- **socket_consoleapp**: Принимает сообщения через TCP сокет, выводит их и статистику (всего сообщений, за последний час, по уровням логирования, длина сообщений).
(Тестов для данной части проекта нет. Эта часть тестировалась вручную с помощью утилиты Netcat)


## Примечания
- Библиотека поддерживает статическую сборку по умолчанию (`libjournal.a`) и динамическую (`libjournal.so`) сборку.

- Для динамической сборки используйте `LD_LIBRARY_PATH` (как в инструкции к сброке) или скопируйте `libjournal.so` в `/usr/lib`:
  ```bash
  sudo cp src/build/lib/libjournal.so /usr/lib/
  ```
