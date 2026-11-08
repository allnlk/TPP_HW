#include <exception>
#include <iostream>

#include "Demos.h"
#include "TestData.h"

namespace
{
struct Step
{
    const char* name;
    void (*run)();
};

void prepareQueriesDatabase()
{
    std::cout << "\n=== PREPARING TEST DATA FOR QUERIES ===\n";
    prepareTestData(kQueriesDb);
}
}

int main()
{
    const Step steps[] = {
        {"Comparison (Task 2)",  runComparisonDemo},
        {"Schema (Task 3)",      runSchemaDemo},
        {"Repository (Task 4)",  runRepositoryDemo},
        {"Test data (Task 5)",   prepareQueriesDatabase},
        {"Filtering/sorting",    runQueryDemo},
        {"Aggregation",          runAggregateDemo},
        {"Join",                 runJoinDemo},
        {"Transactions (Task 6)", runTransactionDemo}
    };

    int failures = 0;
    for (const auto& step : steps)
    {
        try
        {
            step.run();
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Step '" << step.name << "' failed: "
                      << ex.what() << '\n';
            ++failures;
        }
    }

    return failures == 0 ? 0 : 1;
}
