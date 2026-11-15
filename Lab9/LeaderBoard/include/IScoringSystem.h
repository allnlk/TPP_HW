#pragma once
#include <cstddef>
#include <utility>
#include <vector>

// Стратегія нарахування балів (OCP/DIP): нова система = новий клас.
class IScoringSystem
{
public:
    virtual ~IScoringSystem() = default;
    // index — позиція у заїзді з нуля (0 = перше місце)
    virtual int pointsForPosition(std::size_t index) const = 0;
};

class TableScoringSystem : public IScoringSystem
{
    std::vector<int> m_table;

public:
    explicit TableScoringSystem(std::vector<int> table) : m_table(std::move(table)) {}
    int pointsForPosition(std::size_t index) const override
    {
        return index < m_table.size() ? m_table[index] : 0;
    }
};

// Початкові правила кату: 25 / 18 / 15
class ClassicScoringSystem : public TableScoringSystem
{
public:
    ClassicScoringSystem() : TableScoringSystem({25, 18, 15}) {}
};

// Розширення: нарахування балів топ-10
class ExtendedScoringSystem : public TableScoringSystem
{
public:
    ExtendedScoringSystem() : TableScoringSystem({25, 18, 15, 12, 10, 8, 6, 4, 2, 1}) {}
};
