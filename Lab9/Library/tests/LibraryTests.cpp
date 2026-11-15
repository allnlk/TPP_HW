#include <gtest/gtest.h>
#include <sstream>
#include "Library.h"

namespace {

// Тестові підміни (fake) — доводять, що реалізації можна замінити без змін Library.
class FakeRepository : public IBookRepository {
public:
    void save(const Book& book) override {
        saved.push_back(book.getTitle());
        log.push_back("save");
    }
    std::vector<std::string> saved;
    std::vector<std::string> log;
};

class FakeNotifier : public IBookNotifier {
public:
    explicit FakeNotifier(std::vector<std::string>& sharedLog) : log(sharedLog) {}
    void notifyBookAdded(const Book& book) override {
        notified.push_back(book.getAuthor());
        log.push_back("notify");
    }
    std::vector<std::string> notified;
    std::vector<std::string>& log;
};

struct Fixture {
    FakeRepository repo;
    FakeNotifier notifier{repo.log};
    Library library{repo, notifier};
};

}  // namespace

TEST(LibraryTest, AddBookStoresBookInCollection) {
    Fixture f;
    f.library.addBook(Book{"Dune", "Frank Herbert"});
    ASSERT_EQ(f.library.getBooks().size(), 1u);
    EXPECT_EQ(f.library.getBooks()[0].getTitle(), "Dune");
    EXPECT_EQ(f.library.getBooks()[0].getAuthor(), "Frank Herbert");
}

TEST(LibraryTest, AddBookCallsRepositoryThenNotifier) {
    Fixture f;
    f.library.addBook(Book{"Dune", "Frank Herbert"});
    EXPECT_EQ(f.repo.saved, std::vector<std::string>{"Dune"});
    EXPECT_EQ(f.notifier.notified, std::vector<std::string>{"Frank Herbert"});
    EXPECT_EQ(f.repo.log, (std::vector<std::string>{"save", "notify"}));
}

TEST(LibraryTest, DisplayBooksKeepsOriginalOutputFormat) {
    Fixture f;
    f.library.addBook(Book{"The Catcher in the Rye", "J.D. Salinger"});
    f.library.addBook(Book{"To Kill a Mockingbird", "Harper Lee"});
    std::ostringstream out;
    f.library.displayBooks(out);
    EXPECT_EQ(out.str(),
              "Title: The Catcher in the Rye, Author: J.D. Salinger\n"
              "Title: To Kill a Mockingbird, Author: Harper Lee\n");
}

TEST(LibraryTest, EmptyLibraryDisplaysNothing) {
    Fixture f;
    std::ostringstream out;
    f.library.displayBooks(out);
    EXPECT_EQ(out.str(), "");
}
