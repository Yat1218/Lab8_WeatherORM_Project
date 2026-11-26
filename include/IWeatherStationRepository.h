#pragma once

#include <optional>
#include <vector>

#include "WeatherStation.h"

class IWeatherStationRepository
{
public:
    virtual ~IWeatherStationRepository() = default;

    virtual int add(const WeatherStation& station) = 0;
    virtual std::optional<WeatherStation> findById(int id) = 0;
    virtual std::vector<WeatherStation> findAll() = 0;
    virtual bool update(const WeatherStation& station) = 0;
    virtual bool remove(int id) = 0;
};
