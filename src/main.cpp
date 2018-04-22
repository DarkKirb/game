#include <iostream>
#include <init.hpp>
int main(int argc, const char ** argv) {
    init(argc, argv);
    std::cout << "hi" << std::endl;
    while(processEvents()) {
        swapFB();
    }
    deinit();
    return 0;
}
