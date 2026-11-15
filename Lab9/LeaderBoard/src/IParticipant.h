#pragma once
#include <string>

// Спільна абстракція учасника перегонів. Race та LeaderBoard працюють лише з нею:
// більше немає dynamic_cast і "порожнього" name у SelfDrivingCar (LSP, OCP).
class IParticipant {
public:
    virtual ~IParticipant() = default;
    virtual std::string displayName() const = 0;
};
