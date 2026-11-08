#pragma once

#include <string>

// Основна (батьківська) сутність: камера відеоспостереження.
struct Camera
{
    int id = 0;
    std::string name;
    std::string location;
    std::string status;   // online / offline / maintenance
};
