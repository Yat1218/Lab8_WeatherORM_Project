#include <exception>
#include <iostream>
#include <string>

#include "RawSqliteDemo.h"
#include "WeatherDemo.h"

int main()
{
    const std::string mainDatabase = "weather_stations.db";
    const std::string noTransactionDatabase = "weather_no_transaction.db";

    try
    {
        runRawSqliteDemo();
        prepareWeatherTestData(mainDatabase);
        runRepositoryDemo(mainDatabase);
        runQueryDemo(mainDatabase);
        runAggregateDemo(mainDatabase);
        runJoinDemo(mainDatabase);
        runTransactionDemo(mainDatabase);
        runRollbackDemo(mainDatabase);
        runWithoutTransactionDemo(noTransactionDatabase);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Application error: " << ex.what() << '\n';
        return 1;
    }

    std::cout << "\nAll laboratory demonstrations completed.\n";
    return 0;
}
