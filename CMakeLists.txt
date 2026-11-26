cmake_minimum_required(VERSION 3.21)

project(WeatherStationsORM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(WeatherStationsORM
    src/main.cpp
    src/RawSqliteDemo.cpp
    src/SQLiteWeatherStationRepository.cpp
    src/WeatherDemo.cpp
)

target_include_directories(WeatherStationsORM
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(WeatherStationsORM
    PRIVATE
        sqlite_orm::sqlite_orm
        unofficial::sqlite3::sqlite3
)

set_property(TARGET WeatherStationsORM PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
