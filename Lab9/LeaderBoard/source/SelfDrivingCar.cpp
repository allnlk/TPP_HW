#include "../include/SelfDrivingCar.h"

SelfDrivingCar::SelfDrivingCar(std::string algorithmVersion, std::string company)
    : m_algorithmVersion(std::move(algorithmVersion)), m_company(std::move(company))
{}

std::string SelfDrivingCar::getAlgorithmVersion() const { return m_algorithmVersion; }

void SelfDrivingCar::setAlgorithmVersion(std::string algorithmVersion)
{
    m_algorithmVersion = std::move(algorithmVersion);
}

std::string SelfDrivingCar::getCompany() const { return m_company; }

std::string SelfDrivingCar::displayName() const
{
    return "Self Driving Car - " + m_company + " (" + m_algorithmVersion + ")";
}
