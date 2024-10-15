#include <iostream>
#include <concepts>

template <typename T>
concept Incrementable = requires(T a) { ++a; };

template <Incrementable T>
void increment(T& value) {
    ++value;
}

int main() {
    int x = 5;
    increment(x);
    std::cout << "Incremented value: " << x << std::endl;
    return 0;
}
