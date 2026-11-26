#include "RawSqliteDemo.h"

#include <sqlite3.h>

#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include "WeatherStation.h"

namespace
{
void checkSqlite(int rc, sqlite3* db, const std::string& operation)
{
    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(operation + ": " + sqlite3_errmsg(db));
    }
}

int insertStationRaw(sqlite3* db, const WeatherStation& station)
{
    const char* sql = R"(
        INSERT INTO WeatherStation
            (name, region, latitude, longitude, elevationM, status)
        VALUES (?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* rawStatement = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &rawStatement, nullptr);
    checkSqlite(rc, db, "Preparing INSERT");

    try
    {
        rc = sqlite3_bind_text(rawStatement, 1, station.name.c_str(),
                               -1, SQLITE_TRANSIENT);
        checkSqlite(rc, db, "Binding station name");
        rc = sqlite3_bind_text(rawStatement, 2, station.region.c_str(),
                               -1, SQLITE_TRANSIENT);
        checkSqlite(rc, db, "Binding station region");
        rc = sqlite3_bind_double(rawStatement, 3, station.latitude);
        checkSqlite(rc, db, "Binding latitude");
        rc = sqlite3_bind_double(rawStatement, 4, station.longitude);
        checkSqlite(rc, db, "Binding longitude");
        rc = sqlite3_bind_double(rawStatement, 5, station.elevationM);
        checkSqlite(rc, db, "Binding elevation");
        rc = sqlite3_bind_text(rawStatement, 6, station.status.c_str(),
                               -1, SQLITE_TRANSIENT);
        checkSqlite(rc, db, "Binding station status");

        rc = sqlite3_step(rawStatement);
        if (rc != SQLITE_DONE)
        {
            throw std::runtime_error(
                std::string("Executing INSERT: ") + sqlite3_errmsg(db)
            );
        }

        const int id = static_cast<int>(sqlite3_last_insert_rowid(db));
        sqlite3_finalize(rawStatement);
        return id;
    }
    catch (...)
    {
        sqlite3_finalize(rawStatement);
        throw;
    }
}

std::optional<WeatherStation> findStationRaw(sqlite3* db, int id)
{
    const char* sql = R"(
        SELECT id, name, region, latitude, longitude, elevationM, status
        FROM WeatherStation
        WHERE id = ?;
    )";

    sqlite3_stmt* rawStatement = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &rawStatement, nullptr);
    checkSqlite(rc, db, "Preparing SELECT");

    try
    {
        rc = sqlite3_bind_int(rawStatement, 1, id);
        checkSqlite(rc, db, "Binding station ID");

        rc = sqlite3_step(rawStatement);
        if (rc == SQLITE_ROW)
        {
            const auto textColumn = [rawStatement](int column)
            {
                const auto* value = sqlite3_column_text(rawStatement, column);
                return value
                    ? std::string(reinterpret_cast<const char*>(value))
                    : std::string{};
            };

            WeatherStation station;
            station.id = sqlite3_column_int(rawStatement, 0);
            station.name = textColumn(1);
            station.region = textColumn(2);
            station.latitude = sqlite3_column_double(rawStatement, 3);
            station.longitude = sqlite3_column_double(rawStatement, 4);
            station.elevationM = sqlite3_column_double(rawStatement, 5);
            station.status = textColumn(6);

            sqlite3_finalize(rawStatement);
            return station;
        }

        if (rc != SQLITE_DONE)
        {
            throw std::runtime_error(
                std::string("Executing SELECT: ") + sqlite3_errmsg(db)
            );
        }

        sqlite3_finalize(rawStatement);
        return std::nullopt;
    }
    catch (...)
    {
        sqlite3_finalize(rawStatement);
        throw;
    }
}
}

void runRawSqliteDemo()
{
    const std::string databaseFile = "weather_raw.db";
    std::filesystem::remove(databaseFile);

    sqlite3* db = nullptr;
    const int openResult = sqlite3_open(databaseFile.c_str(), &db);
    if (openResult != SQLITE_OK)
    {
        const std::string message = db
            ? sqlite3_errmsg(db)
            : "SQLite did not create a connection";
        if (db)
        {
            sqlite3_close(db);
        }
        throw std::runtime_error("Opening raw database: " + message);
    }

    try
    {
        const char* createTableSql = R"(
            CREATE TABLE IF NOT EXISTS WeatherStation (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                region TEXT NOT NULL,
                latitude REAL NOT NULL,
                longitude REAL NOT NULL,
                elevationM REAL NOT NULL,
                status TEXT NOT NULL
            );
        )";

        char* errorMessage = nullptr;
        const int createResult = sqlite3_exec(
            db, createTableSql, nullptr, nullptr, &errorMessage
        );
        if (createResult != SQLITE_OK)
        {
            const std::string message = errorMessage
                ? errorMessage
                : sqlite3_errmsg(db);
            sqlite3_free(errorMessage);
            throw std::runtime_error("Creating raw table: " + message);
        }

        const WeatherStation input{
            0, "Raw API Station", "Test Region", 50.45, 30.52, 179.0, "active"
        };
        const int id = insertStationRaw(db, input);
        const auto found = findStationRaw(db, id);

        std::cout << "\n=== SQLITE C API DEMONSTRATION ===\n";
        std::cout << "Inserted station ID: " << id << '\n';
        if (found)
        {
            std::cout << "Found: " << found->name
                      << " | " << found->region
                      << " | " << found->status << '\n';
        }
        else
        {
            std::cout << "Station not found\n";
        }
    }
    catch (...)
    {
        sqlite3_close(db);
        throw;
    }

    sqlite3_close(db);
}
