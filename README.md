# Лабораторна робота №8 — Робота з БД через ORM

**Варіант 18:** система моніторингу погодних станцій.

Проєкт демонструє роботу SQLite C API та `sqlite_orm`, модель зв'язку 1:N, патерн Repository, CRUD, фільтрацію, сортування, `LIMIT`, агрегатні запити, `GROUP BY`, `INNER JOIN`, а також успішну й невдалу транзакції.

## Структура

```text
WeatherStationsORM/
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── README.md
├── include/
│   ├── Database.h
│   ├── IWeatherStationRepository.h
│   ├── Observation.h
│   ├── RawSqliteDemo.h
│   ├── SQLiteWeatherStationRepository.h
│   ├── WeatherDemo.h
│   └── WeatherStation.h
└── src/
    ├── main.cpp
    ├── RawSqliteDemo.cpp
    ├── SQLiteWeatherStationRepository.cpp
    └── WeatherDemo.cpp
```

## Вимоги

- Visual Studio з компонентами **Desktop development with C++** та підтримкою CMake;
- компілятор із підтримкою C++20;
- CMake 3.25 або новіший (для версії пресетів 6);
- vcpkg;
- змінна середовища `VCPKG_ROOT`, що вказує на каталог vcpkg;
- генератор Ninja (або відкриття CMake-проєкту безпосередньо у Visual Studio).

## Збирання у Windows

1. Переконайтеся, що `VCPKG_ROOT` указує на каталог vcpkg.
2. Відкрийте термінал у каталозі проєкту.
3. Виконайте конфігурацію:

```powershell
cmake --preset windows-vcpkg
```

4. Зберіть застосунок:

```powershell
cmake --build --preset windows-debug
```

5. Запустіть виконуваний файл із каталогу `build` (у відповідній конфігурації).

Під час першої конфігурації vcpkg завантажить залежності, визначені у `vcpkg.json`: `sqlite3` та `sqlite-orm`. Для відтворюваного складання методичка також рекомендує зафіксувати `builtin-baseline`; конкретний commit потрібно взяти з тієї версії vcpkg, яку встановлено на комп’ютері, і додати до `vcpkg.json` перед здачею.

## Що перевіряє програма

1. Додавання станції та пошук за ID через SQLite C API (prepared statement, bind-параметри, читання стовпців).
2. Підготовка контрольованої БД: 5 погодних станцій і 15 спостережень.
3. CRUD основної сутності через `IWeatherStationRepository` та `SQLiteWeatherStationRepository`.
4. Фільтрація станцій, сортування, вибірка трьох спостережень із найбільшою кількістю опадів та комбінований запит.
5. `COUNT`, `SUM`, `AVG` і `GROUP BY`.
6. `INNER JOIN` двох пов'язаних таблиць.
7. Успішна транзакція: додавання станції та першого спостереження.
8. Відкат транзакції після порушення зовнішнього ключа.
9. Окремий сценарій без явної транзакції для демонстрації часткового збереження першої операції.

## Файли баз даних

Програма створює окремі демонстраційні БД у робочому каталозі:

- `weather_raw.db` — порівняння SQLite C API;
- `weather_stations.db` — тестові дані, Repository, запити та транзакції;
- `weather_no_transaction.db` — приклад виконання без явної транзакції.

Функція підготовки даних щоразу очищує тільки файл `weather_stations.db`, щоб результати запитів були відтворюваними. Окремі демонстраційні файли не містять користувацьких даних.

## Примітка щодо скриншотів

Після збирання та запуску додайте до звіту власні скриншоти вікна консолі з блоками `TEST DATA PREPARED`, `REPOSITORY CRUD DEMONSTRATION`, `AGGREGATE DEMONSTRATION`, `INNER JOIN DEMONSTRATION`, `COMMIT`, `ROLLBACK` і `WITHOUT EXPLICIT TRANSACTION`.
