#pragma once
#include "IParticipant.h"

// Більше НЕ успадковує Driver: самокерований автомобіль не має імені водія.
class SelfDrivingCar : public IParticipant {
public:
    SelfDrivingCar(std::string algorithmVersion, std::string company)
        : algorithmVersion_(std::move(algorithmVersion)), company_(std::move(company)) {}

    const std::string& getAlgorithmVersion() const { return algorithmVersion_; }
    const std::string& getCompany() const { return company_; }
    std::string displayName() const override {
        return "Self Driving Car - " + company_ + " (" + algorithmVersion_ + ")";
    }

private:
    std::string algorithmVersion_;
    std::string company_;
};
