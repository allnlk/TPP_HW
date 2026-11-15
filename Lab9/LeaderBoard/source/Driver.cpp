#include "../include/Driver.h"

Driver::Driver(std::string const name, std::string const country) : m_name(name), m_country(country)
{}

std::string Driver::getName() const { return m_name; }

std::string Driver::getCountry() const { return m_country; }

std::string Driver::displayName() const { return m_name; }

bool Driver::operator==(Driver const& other) const
{
    return this == &other || (m_name == other.m_name && m_country == other.m_country);
}
