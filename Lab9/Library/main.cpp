#include "DatabaseBookRepository.h"
#include "EmailNotifier.h"
#include "Library.h"

int main() {
    DatabaseBookRepository repository;
    EmailNotifier notifier;
    Library library(repository, notifier);

    library.addBook(Book{"The Catcher in the Rye", "J.D. Salinger"});
    library.addBook(Book{"To Kill a Mockingbird", "Harper Lee"});
    library.displayBooks();
    return 0;
}
