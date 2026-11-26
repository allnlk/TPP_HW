#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v{1, -2, 3, -4, 5};

    std::remove_if(
        v.begin(),
        v.end(),
        [](int x) { return x < 0; }
    );
    std::cout << v.size() << std::endl;

    for (int x : v) {
        std::cout << x << ' ';
    }
}
