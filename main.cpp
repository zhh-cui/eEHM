#include <iostream>
#include "include/eEHM.h"

int main(int argc, char **argv) {
    eEHM *error_handling;
    try {
        error_handling = new eEHM();
        error_handling->SetUserHandler(SIGFPE);
        error_handling->SetUserHandler(SIGSEGV);
        error_handling->SetUserHandler(SIGINT);
    } catch (signal_error ex) {
        std::cout << ex.what() << std::endl;
    }

    try {
        if (error_handling->isValid()) {
            int a = 0;
            int b = 10 / a;
        }
    } catch (signal_error ex) {
        std::cout << ex.what() << std::endl;
    }

    try {
        if (error_handling->isValid()) {
            int *a = NULL;
            int b = 10 * (*a);
        }
    } catch (signal_error ex) {
        std::cout << ex.what() << std::endl;
    }

    try {
        if (error_handling->isValid()) {
            while (true) {
            }
        }
    } catch (signal_error ex) {
        std::cout << ex.what() << std::endl;
    }

    return 0;
}

