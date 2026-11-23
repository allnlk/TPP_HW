#pragma once

#include <string>

// Залежна (дочірня) сутність: зареєстрована подія камери.
struct RegisteredEvent
{
    int id = 0;
    int cameraId = 0;          // зовнішній ключ -> Camera(id)
    std::string eventTime;     // формат YYYY-MM-DD HH:MM:SS
    std::string eventType;     // motion, intrusion, person_detected, ...
    int severity = 0;          // рівень небезпеки (важливість) 1..5
    int durationSec = 0;       // тривалість події, секунди
    std::string status = "Registered";  // Lab10: поточний стан (патерн State)
};
