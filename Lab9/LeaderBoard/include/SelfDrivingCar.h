#pragma once

#include <string>

#include "IParticipant.h"

// Більше НЕ успадковує Driver: в оригіналі "ім'ям водія" ставилась версія алгоритму (порушення LSP).
class SelfDrivingCar : public IParticipant
{
    std::string m_algorithmVersion;
    std::string m_company;

public:
    SelfDrivingCar(std::string algorithmVersion, std::string company);

    std::string getAlgorithmVersion() const;
    void setAlgorithmVersion(std::string algorithmVersion);
    std::string getCompany() const;
    std::string displayName() const override;
};
