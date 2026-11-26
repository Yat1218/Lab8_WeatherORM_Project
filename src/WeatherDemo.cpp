#include "WeatherDemo.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Database.h"
#include "Observation.h"
#include "SQLiteWeatherStationRepository.h"
#include "WeatherStation.h"

void prepareWeatherTestData(const std::string& databaseFile)
{
    // Reset only the dedicated test database; other application files are untouched.
    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);
    enableForeignKeys(storage);
    storage.open_forever();
    storage.sync_schema();

    const std::vector<WeatherStation> stations = {
        {0, "Kyiv Observatory", "Kyiv", 50.4501, 30.5234, 179.0, "online"},
        {0, "Lviv Center", "Lviv", 49.8397, 24.0297, 296.0, "online"},
        {0, "Odesa Coast", "Odesa", 46.4825, 30.7233, 40.0, "online"},
        {0, "Kharkiv North", "Kharkiv", 49.9935, 36.2304, 152.0, "maintenance"},
        {0, "Dnipro Airport", "Dnipro", 48.4647, 35.0462, 147.0, "online"}
    };

    std::vector<int> stationIds;
    stationIds.reserve(stations.size());
    for (const auto& station : stations)
    {
        stationIds.push_back(static_cast<int>(storage.insert(station)));
    }

    const std::vector<Observation> observations = {
        {0, stationIds[0], "2026-10-01 08:00", 12.4, 62.0, 1015.0, 3.2, 0.0},
        {0, stationIds[0], "2026-10-10 08:00", 13.1, 58.0, 1012.0, 2.8, 0.4},
        {0, stationIds[0], "2026-10-20 08:00",  9.8, 71.0, 1018.0, 4.1, 1.2},

        {0, stationIds[1], "2026-10-01 08:00", 10.2, 76.0, 1010.0, 2.1, 2.4},
        {0, stationIds[1], "2026-10-10 08:00", 11.6, 73.0, 1008.0, 3.5, 0.8},
        {0, stationIds[1], "2026-10-20 08:00",  8.9, 81.0, 1014.0, 2.9, 3.1},

        {0, stationIds[2], "2026-10-01 08:00", 16.5, 65.0, 1016.0, 4.4, 0.0},
        {0, stationIds[2], "2026-10-10 08:00", 17.2, 68.0, 1013.0, 5.1, 0.0},
        {0, stationIds[2], "2026-10-20 08:00", 14.8, 72.0, 1019.0, 3.8, 0.6},

        {0, stationIds[3], "2026-10-01 08:00", 11.1, 55.0, 1013.0, 3.9, 0.0},
        {0, stationIds[3], "2026-10-10 08:00", 12.3, 60.0, 1011.0, 4.0, 0.2},
        {0, stationIds[3], "2026-10-20 08:00",  7.5, 68.0, 1017.0, 5.2, 1.8},

        {0, stationIds[4], "2026-10-01 08:00", 13.3, 61.0, 1014.0, 3.1, 0.1},
        {0, stationIds[4], "2026-10-10 08:00", 14.0, 64.0, 1012.0, 3.6, 0.0},
        {0, stationIds[4], "2026-10-20 08:00", 10.4, 70.0, 1016.0, 4.5, 0.9}
    };

    for (const auto& observation : observations)
    {
        storage.insert(observation);
    }

    std::cout << "\n=== TEST DATA PREPARED ===\n";
    std::cout << "Weather stations: " << storage.count<WeatherStation>() << '\n';
    std::cout << "Observations: " << storage.count<Observation>() << '\n';
}

void runRepositoryDemo(const std::string& databaseFile)
{
    SQLiteWeatherStationRepository concreteRepository(databaseFile);
    IWeatherStationRepository& repository = concreteRepository;

    std::cout << "\n=== REPOSITORY CRUD DEMONSTRATION ===\n";

    WeatherStation station{
        0, "Temporary Test Station", "Test Region",
        48.0, 31.0, 150.0, "offline"
    };

    const int id = repository.add(station);
    std::cout << "CREATE: inserted ID = " << id << '\n';

    auto found = repository.findById(id);
    std::cout << "READ: "
              << (found ? found->name : "not found") << '\n';

    if (found)
    {
        found->status = "online";
        const bool updated = repository.update(*found);
        std::cout << "UPDATE: " << std::boolalpha << updated << '\n';
    }

    const auto allStations = repository.findAll();
    std::cout << "READ ALL: " << allStations.size() << " records\n";

    const bool removed = repository.remove(id);
    std::cout << "DELETE: " << std::boolalpha << removed << '\n';
    std::cout << "Record exists after deletion: "
              << repository.findById(id).has_value() << '\n';
}

void runQueryDemo(const std::string& databaseFile)
{
    using namespace sqlite_orm;

    auto storage = createStorage(databaseFile);
    enableForeignKeys(storage);
    storage.open_forever();
    storage.sync_schema();

    std::cout << "\n=== FILTERING / SORTING / LIMIT DEMONSTRATION ===\n";

    std::cout << "\n1. Online stations:\n";
    const auto onlineStations = storage.get_all<WeatherStation>(
        where(c(&WeatherStation::status) == "online")
    );
    for (const auto& station : onlineStations)
    {
        std::cout << station.name << " | " << station.region << '\n';
    }

    std::cout << "\n2. Stations sorted by name:\n";
    const auto sortedStations = storage.get_all<WeatherStation>(
        order_by(&WeatherStation::name).asc()
    );
    for (const auto& station : sortedStations)
    {
        std::cout << station.name << '\n';
    }

    std::cout << "\n3. Three observations with the highest precipitation:\n";
    const auto topRainfall = storage.get_all<Observation>(
        order_by(&Observation::precipitationMm).desc(),
        limit(3)
    );
    for (const auto& observation : topRainfall)
    {
        std::cout << observation.observedAt
                  << " | " << observation.precipitationMm << " mm\n";
    }

    std::cout << "\n4. Observations with precipitation above 1 mm:\n";
    const auto rainyObservations = storage.get_all<Observation>(
        where(c(&Observation::precipitationMm) > 1.0),
        order_by(&Observation::precipitationMm).desc()
    );
    for (const auto& observation : rainyObservations)
    {
        std::cout << observation.observedAt
                  << " | " << observation.precipitationMm << " mm\n";
    }
}

void runAggregateDemo(const std::string& databaseFile)
{
    using namespace sqlite_orm;

    auto storage = createStorage(databaseFile);
    enableForeignKeys(storage);
    storage.open_forever();
    storage.sync_schema();

    std::cout << "\n=== AGGREGATE DEMONSTRATION ===\n";

    const auto observationCount = storage.count<Observation>();
    const auto totalPrecipitation = storage.sum(&Observation::precipitationMm);
    const auto averageTemperature = storage.avg(&Observation::temperatureC);

    std::cout << "Observation count: " << observationCount << '\n';
    std::cout << "Total precipitation: "
        << (totalPrecipitation ? *totalPrecipitation : 0.0) << " mm\n";
    std::cout << "Average temperature: "
        << averageTemperature << " C\n";

    std::cout << "Precipitation by station ID:\n";
    const auto precipitationByStation = storage.select(
        columns(
            &Observation::stationId,
            sum(&Observation::precipitationMm)
        ),
        group_by(&Observation::stationId)
    );

    for (const auto& [stationId, precipitation] : precipitationByStation)
    {
        std::cout << "Station ID: " << stationId
            << " | Total: " << (precipitation ? *precipitation : 0.0)   
                  << " mm\n";
    }
}

void runJoinDemo(const std::string& databaseFile)
{
    using namespace sqlite_orm;

    auto storage = createStorage(databaseFile);
    enableForeignKeys(storage);
    storage.open_forever();
    storage.sync_schema();

    std::cout << "\n=== INNER JOIN DEMONSTRATION ===\n";

    const auto joinedRows = storage.select(
        columns(
            &WeatherStation::name,
            &Observation::observedAt,
            &Observation::temperatureC,
            &Observation::humidityPercent,
            &Observation::precipitationMm
        ),
        inner_join<Observation>(
            on(c(&WeatherStation::id) == &Observation::stationId)
        ),
        order_by(&Observation::id)
    );

    for (const auto& [stationName, observedAt, temperature,
                      humidity, precipitation] : joinedRows)
    {
        std::cout << stationName << " | " << observedAt
                  << " | T=" << temperature << " C"
                  << " | H=" << humidity << "%"
                  << " | P=" << precipitation << " mm\n";
    }
}

void runTransactionDemo(const std::string& databaseFile)
{
    auto storage = createStorage(databaseFile);
    enableForeignKeys(storage);
    storage.open_forever();
    storage.sync_schema();

    std::cout << "\n=== SUCCESSFUL TRANSACTION (COMMIT) ===\n";
    std::cout << "Before: stations=" << storage.count<WeatherStation>()
              << ", observations=" << storage.count<Observation>() << '\n';

    const bool committed = storage.transaction([&]()
    {
        WeatherStation station{
            0, "New Kyiv Weather Station", "Kyiv Region",
            50.10, 30.20, 165.0, "online"
        };
        const int stationId = static_cast<int>(storage.insert(station));

        Observation firstObservation{
            0, stationId, "2026-10-25 08:00",
            8.5, 75.0, 1017.0, 2.4, 0.7
        };
        storage.insert(firstObservation);
        std::cout << "Inserted station and first observation, station ID: "
                  << stationId << '\n';
        return true;
    });

    std::cout << "COMMIT result: " << std::boolalpha << committed << '\n';
    std::cout << "After: stations=" << storage.count<WeatherStation>()
              << ", observations=" << storage.count<Observation>() << '\n';
}

void runRollbackDemo(const std::string& databaseFile)
{
    using namespace sqlite_orm;

    auto storage = createStorage(databaseFile);
    enableForeignKeys(storage);
    storage.open_forever();
    storage.sync_schema();

    std::cout << "\n=== FAILED TRANSACTION (ROLLBACK) ===\n";
    const auto stationsBefore = storage.count<WeatherStation>();
    const auto observationsBefore = storage.count<Observation>();
    std::cout << "Before: stations=" << stationsBefore
              << ", observations=" << observationsBefore << '\n';

    try
    {
        storage.transaction([&]()
        {
            WeatherStation station{
                0, "Rollback Test Station", "Test Region",
                49.0, 32.0, 120.0, "online"
            };
            storage.insert(station);

            // Deliberately invalid stationId violates the FOREIGN KEY constraint.
            Observation invalidObservation{
                0, -1, "2026-10-26 08:00",
                7.0, 80.0, 1015.0, 3.0, 2.0
            };
            storage.insert(invalidObservation);
            return true;
        });
    }
    catch (const std::exception& ex)
    {
        std::cout << "Transaction failed as expected: " << ex.what() << '\n';
        std::cout << "ROLLBACK performed\n";
    }

    const auto stationsAfter = storage.count<WeatherStation>();
    const auto observationsAfter = storage.count<Observation>();
    const auto rollbackStation = storage.get_all<WeatherStation>(
        where(c(&WeatherStation::name) == "Rollback Test Station")
    );

    std::cout << "After: stations=" << stationsAfter
              << ", observations=" << observationsAfter << '\n';
    std::cout << "Rollback Test Station exists: "
              << std::boolalpha << !rollbackStation.empty() << '\n';
}

void runWithoutTransactionDemo(const std::string& databaseFile)
{
    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);
    enableForeignKeys(storage);
    storage.open_forever();
    storage.sync_schema();

    std::cout << "\n=== SAME OPERATIONS WITHOUT EXPLICIT TRANSACTION ===\n";

    try
    {
        WeatherStation station{
            0, "Partially Saved Station", "Test Region",
            48.5, 30.5, 130.0, "online"
        };
        const int stationId = static_cast<int>(storage.insert(station));
        std::cout << "First operation saved, station ID: " << stationId << '\n';

        Observation invalidObservation{
            0, -1, "2026-10-26 09:00",
            6.0, 82.0, 1012.0, 2.0, 1.5
        };
        storage.insert(invalidObservation);
    }
    catch (const std::exception& ex)
    {
        std::cout << "Second operation failed: " << ex.what() << '\n';
    }

    std::cout << "After operations: stations="
              << storage.count<WeatherStation>()
              << ", observations=" << storage.count<Observation>() << '\n';
}
