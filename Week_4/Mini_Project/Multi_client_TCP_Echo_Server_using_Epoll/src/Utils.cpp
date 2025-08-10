#include "../include/Utils.h"
#include <iostream>
#include <cstdio>   // for perror
#include <cstdlib>  // for exit, EXIT_FAILURE

void Utils::errorExit(const std::string &msg) {
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}