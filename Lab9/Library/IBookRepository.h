#pragma once
#include "Book.h"

// Абстракція сховища книг (DIP): Library залежить від неї, а не від конкретної БД.
class IBookRepository {
public:
    virtual ~IBookRepository() = default;
    virtual void save(const Book& book) = 0;
};
