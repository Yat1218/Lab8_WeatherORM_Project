#include "SQLiteWeatherStationRepository.h"

SQLiteWeatherStationRepository::SQLiteWeatherStationRepository(
    const std::string& databaseFile
)
    : storage_(createStorage(databaseFile))
{
    enableForeignKeys(storage_);
    storage_.open_forever();
    storage_.sync_schema();
}

int SQLiteWeatherStationRepository::add(const WeatherStation& station)
{
    return static_cast<int>(storage_.insert(station));
}

std::optional<WeatherStation>
SQLiteWeatherStationRepository::findById(int id)
{
    return storage_.get_optional<WeatherStation>(id);
}

std::vector<WeatherStation> SQLiteWeatherStationRepository::findAll()
{
    return storage_.get_all<WeatherStation>();
}

bool SQLiteWeatherStationRepository::update(const WeatherStation& station)
{
    const auto existing = storage_.get_optional<WeatherStation>(station.id);
    if (!existing)
    {
        return false;
    }

    storage_.update(station);
    return true;
}

bool SQLiteWeatherStationRepository::remove(int id)
{
    const auto existing = storage_.get_optional<WeatherStation>(id);
    if (!existing)
    {
        return false;
    }

    storage_.remove<WeatherStation>(id);
    return true;
}
