#include <memory>

struct test{};

int main() {
    std::unique_ptr<test> test_ptr;

    return 0;
}