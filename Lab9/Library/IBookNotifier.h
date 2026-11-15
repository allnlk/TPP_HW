#pragma once
#include "Book.h"

// Абстракція механізму сповіщень (DIP, ISP: один вузький інтерфейс = одна дія).
class IBookNotifier {
public:
    virtual ~IBookNotifier() = default;
    virtual void notifyBookAdded(const Book& book) = 0;
};
