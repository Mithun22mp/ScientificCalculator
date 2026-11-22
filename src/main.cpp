
#include <iomanip>
#include <iostream>
#include <string>

#include "calculator/core/engine.hpp"

namespace {

void print_error(calculator::ErrorCode error) {
    using calculator::ErrorCode;

    switch (error) {
        case ErrorCode::division_by_zero:
            std::cout << "Error: Division by zero.\n";
            break;

        case ErrorCode::invalid_expression:
        case ErrorCode::invalid_number:
            std::cout << "Error: Invalid expression.\n";
            break;

        case ErrorCode::unknown_identifier:
            std::cout << "Error: Unknown function or constant.\n";
            break;

        case ErrorCode::mismatched_parentheses:
            std::cout << "Error: Mismatched parentheses.\n";
            break;

        case ErrorCode::domain_error:
            std::cout << "Error: Mathematical domain error.\n";
            break;

        case ErrorCode::numeric_overflow:
            std::cout << "Error: Numeric overflow.\n";
            break;

        case ErrorCode::none:
            break;
    }
}

} // namespace

int main() {
    calculator::CalculatorEngine engine;
    std::string expression;

    std::cout << "Scientific Calculator\n";
    std::cout << "Enter an expression to calculate.\n";
    std::cout << "Type exit to close the calculator.\n\n";

    std::cout << std::setprecision(15);

    while (true) {
        std::cout << ">> ";

        if (!std::getline(std::cin, expression)) {
            break;
        }

        if (expression == "exit" || expression == "quit") {
            break;
        }

        if (expression.empty()) {
            continue;
        }

        const auto result = engine.evaluate(expression);

        if (!result.success()) {
            print_error(result.error);
            continue;
        }

        std::cout << "= " << result.value << '\n';
    }

    std::cout << "Calculator closed.\n";

    return 0;
}
