#include "LeaderBoard.h"
#include <algorithm>

const std::vector<int> Race::points = {25, 18, 15};

Race::Race(std::string name, std::vector<Driver*> results) : name(name), results(results) {
    for (auto* driver : results) {
        std::string driverName = driver->name;
        // Code smell: перевірка конкретного типу всередині Race
        if (auto* car = dynamic_cast<SelfDrivingCar*>(driver)) {
            driverName = "Self Driving Car - " + car->country + " (" + car->algorithmVersion + ")";
        }
        driverNames[driver] = driverName;
    }
}

int Race::position(Driver* driver) const {
    for (std::size_t i = 0; i < results.size(); ++i)
        if (results[i] == driver) return static_cast<int>(i);
    return static_cast<int>(results.size());
}

int Race::getPoints(Driver* driver) const {
    std::size_t p = static_cast<std::size_t>(position(driver));
    return p < points.size() ? points[p] : 0;
}

std::string Race::getDriverName(Driver* driver) const { return driverNames.at(driver); }

std::map<std::string, int> LeaderBoard::driverPoints() const {
    std::map<std::string, int> result;
    for (auto* race : races)
        for (auto* driver : race->results)
            result[race->getDriverName(driver)] += race->getPoints(driver);
    return result;
}

std::vector<std::string> LeaderBoard::driverRankings() const {
    auto pts = driverPoints();
    std::vector<std::pair<std::string, int>> v(pts.begin(), pts.end());
    std::sort(v.begin(), v.end(), [](auto& a, auto& b) { return a.second > b.second; });
    std::vector<std::string> names;
    for (auto& p : v) names.push_back(p.first);
    return names;
}
