//
// Created by lire on 8/19/20.
//

#include <iostream>
#include <time.h>
#include <iomanip>

class Timer {
    clock_t startT;
    clock_t endT;
    double time_taken;

public:
    void start() {
        startT = clock();
    }
    void stop() {
        endT = clock();
        time_taken = double(endT - startT) / double(CLOCKS_PER_SEC);
    }
    void printElapsed() {
        std::cout << "Time taken by program is : " << std::fixed << time_taken
                  << std::setprecision(5);
        std::cout << " sec " << std::endl;
    }
    double elapsed() {
        return double(endT - startT) / double(CLOCKS_PER_SEC);
    }
};
