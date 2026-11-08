#pragma once

#include <iostream>

#include "Camera.h"
#include "RegisteredEvent.h"

inline void printCamera(const Camera& camera)
{
    std::cout << camera.id << " | "
              << camera.name << " | "
              << camera.location << " | "
              << camera.status << '\n';
}

inline void printEvent(const RegisteredEvent& event)
{
    std::cout << event.id << " | camera " << event.cameraId << " | "
              << event.eventTime << " | "
              << event.eventType << " | severity "
              << event.severity << " | "
              << event.durationSec << " s\n";
}
