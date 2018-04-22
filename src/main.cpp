#include <iostream>
#include <init.hpp>
int main() {
    init();
    std::cout << "hi" << std::endl;
    while(processEvents()) {
        swapFB();
    }
    deinit();
    return 0;
}
