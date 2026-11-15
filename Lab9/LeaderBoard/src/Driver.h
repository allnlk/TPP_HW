#pragma once
#include "IParticipant.h"

class Driver : public IParticipant {
public:
    Driver(std::string name, std::string country)
        : name_(std::move(name)), country_(std::move(country)) {}

    const std::string& getName() const { return name_; }
    const std::string& getCountry() const { return country_; }
    std::string displayName() const override { return name_; }

private:
    std::string name_;
    std::string country_;
};
