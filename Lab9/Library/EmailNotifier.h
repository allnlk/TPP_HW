#pragma once
#include <iostream>
#include "IBookNotifier.h"

class EmailNotifier : public IBookNotifier {
public:
    void notifyBookAdded(const Book&) override {
        std::cout << "Sending email notification about the new book..." << std::endl;
    }
};
