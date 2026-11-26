#pragma once

#include <string>

#include "Database.h"
#include "IWeatherStationRepository.h"

class SQLiteWeatherStationRepository final : public IWeatherStationRepository
{
public:
    explicit SQLiteWeatherStationRepository(const std::string& databaseFile);

    int add(const WeatherStation& station) override;
    std::optional<WeatherStation> findById(int id) override;
    std::vector<WeatherStation> findAll() override;
    bool update(const WeatherStation& station) override;
    bool remove(int id) override;

private:
    using Storage = decltype(createStorage(""));
    Storage storage_;
};
