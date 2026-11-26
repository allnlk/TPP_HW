#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>

std::vector<int> make() {
    return {3, 1, 4};
}

int main() {
    auto it = std::ranges::max_element(make());

    std::cout << *it << std::endl;
}
