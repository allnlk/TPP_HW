#pragma once
#include <string>

// Спільна абстракція учасника перегонів. Race та Leaderboard працюють лише з нею:
// немає typeid/static_cast, а SelfDrivingCar не вдає з себе Driver (OCP, LSP).
class IParticipant
{
public:
    virtual ~IParticipant() = default;
    virtual std::string displayName() const = 0;
};
