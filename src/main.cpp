#include <iostream>
#include <unistd.h>
using std::cout, std::cin, std::endl;

int main() {
    cout << "[DEVICE] Boot sequence complete." << endl;
    cout << "[DEVICE] Ready. Waiting for audio stream..." << endl;

    while (true) {
        // represents always on hardware 
        sleep(1);  
    }

    return 0;
}
