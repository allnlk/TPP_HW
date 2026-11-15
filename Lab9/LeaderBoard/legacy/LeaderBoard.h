#pragma once
// Початкова (legacy) реалізація — відтворення структури кату Racing-Car-Katas/LeaderBoard.
// Використовується лише для характеризаційних тестів "до рефакторингу".
#include <map>
#include <string>
#include <vector>

class Driver {
public:
    Driver(std::string name, std::string country) : name(name), country(country) {}
    virtual ~Driver() = default;
    std::string name;
    std::string country;
};

class SelfDrivingCar : public Driver {
public:
    SelfDrivingCar(std::string algorithmVersion, std::string company)
        : Driver("", company), algorithmVersion(algorithmVersion) {}
    std::string algorithmVersion;
};

class Race {
public:
    Race(std::string name, std::vector<Driver*> results);
    int position(Driver* driver) const;
    int getPoints(Driver* driver) const;
    std::string getDriverName(Driver* driver) const;

    std::string name;
    std::vector<Driver*> results;

private:
    static const std::vector<int> points;
    std::map<Driver*, std::string> driverNames;
};

class LeaderBoard {
public:
    explicit LeaderBoard(std::vector<Race*> races) : races(races) {}
    std::map<std::string, int> driverPoints() const;
    std::vector<std::string> driverRankings() const;

private:
    std::vector<Race*> races;
};
