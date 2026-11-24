#include <iostream>

#include "calculator/app/calculator_app.hpp"

int main() {
    calculator::CalculatorApp application;
    application.run(std::cin, std::cout);

    return 0;
}
