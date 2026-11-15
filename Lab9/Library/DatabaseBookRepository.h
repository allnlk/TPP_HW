#pragma once
#include <iostream>
#include "IBookRepository.h"

class DatabaseBookRepository : public IBookRepository {
public:
    void save(const Book&) override {
        std::cout << "Saving book to database..." << std::endl;
    }
};
