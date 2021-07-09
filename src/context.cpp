#include "context.h"

#include <iostream>

void Context::error(std::string_view msg) {
    std::cout << "Error! " << msg << std::endl;
}