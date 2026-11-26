#pragma once

#include <sqlite3.h>
#include <sqlite_orm/sqlite_orm.h>

#include <stdexcept>
#include <string>

#include "Observation.h"
#include "WeatherStation.h"

inline auto createStorage(const std::string& filename)
{
    using namespace sqlite_orm;

    return make_storage(
        filename,

        make_table(
            "WeatherStation",
            make_column("id", &WeatherStation::id,
                        primary_key().autoincrement()),
            make_column("name", &WeatherStation::name, not_null()),
            make_column("region", &WeatherStation::region, not_null()),
            make_column("latitude", &WeatherStation::latitude, not_null()),
            make_column("longitude", &WeatherStation::longitude, not_null()),
            make_column("elevationM", &WeatherStation::elevationM, not_null()),
            make_column("status", &WeatherStation::status, not_null())
        ),

        make_table(
            "Observation",
            make_column("id", &Observation::id,
                        primary_key().autoincrement()),
            make_column("stationId", &Observation::stationId, not_null()),
            make_column("observedAt", &Observation::observedAt, not_null()),
            make_column("temperatureC", &Observation::temperatureC, not_null()),
            make_column("humidityPercent", &Observation::humidityPercent, not_null()),
            make_column("pressureHpa", &Observation::pressureHpa, not_null()),
            make_column("windSpeedMs", &Observation::windSpeedMs, not_null()),
            make_column("precipitationMm", &Observation::precipitationMm, not_null()),
            foreign_key(&Observation::stationId)
                .references(&WeatherStation::id)
        )
    );
}

template <typename Storage>
void enableForeignKeys(Storage& storage)
{
    storage.on_open = [](sqlite3* db)
    {
        char* errorMessage = nullptr;
        const int rc = sqlite3_exec(
            db,
            "PRAGMA foreign_keys = ON;",
            nullptr,
            nullptr,
            &errorMessage
        );

        if (rc != SQLITE_OK)
        {
            const std::string message = errorMessage
                ? errorMessage
                : sqlite3_errmsg(db);
            sqlite3_free(errorMessage);
            throw std::runtime_error(
                "Cannot enable foreign key constraints: " + message
            );
        }
    };
}
