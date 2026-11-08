#pragma once

#include <string>

inline const std::string kRawDb          = "surveillance_raw.db";
inline const std::string kOrmDb          = "surveillance_orm.db";
inline const std::string kSchemaDb       = "surveillance_schema.db";
inline const std::string kRepositoryDb   = "surveillance_repository.db";
inline const std::string kQueriesDb      = "surveillance_queries.db";
inline const std::string kTransactionsDb = "surveillance_transactions.db";

void runComparisonDemo();   // Завдання 2
void runSchemaDemo();       // Завдання 3
void runRepositoryDemo();   // Завдання 4
void runQueryDemo();        // Завдання 5: фільтрація, сортування, limit
void runAggregateDemo();    // Завдання 5: агрегація
void runJoinDemo();         // Завдання 5: JOIN
void runTransactionDemo();  // Завдання 6
