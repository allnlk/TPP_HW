#pragma once
#include <iostream>
#include <vector>
#include "Book.h"
#include "IBookNotifier.h"
#include "IBookRepository.h"

// Library відповідає лише за колекцію книг та сценарій «додати книгу».
// Залежності передаються ззовні через конструктор (Dependency Injection).
class Library {
public:
    Library(IBookRepository& repository, IBookNotifier& notifier);

    void addBook(const Book& book);
    void displayBooks(std::ostream& out = std::cout) const;
    const std::vector<Book>& getBooks() const { return books_; }

private:
    IBookRepository& repository_;
    IBookNotifier& notifier_;
    std::vector<Book> books_;
};
