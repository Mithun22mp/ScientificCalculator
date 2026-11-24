#include "calculator/app/calculator_app.hpp"

#include <cctype>
#include <iomanip>
#include <istream>
#include <limits>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

#include "calculator/core/engine.hpp"
#include "calculator/core/history.hpp"

namespace calculator {
namespace {

constexpr int display_precision = 15;

void print_error(ErrorCode error, std::ostream& output) {
    switch (error) {
    case ErrorCode::division_by_zero:
        output << "Error: Division by zero.\n";
        break;

    case ErrorCode::invalid_expression:
    case ErrorCode::invalid_number:
        output << "Error: Invalid expression.\n";
        break;

    case ErrorCode::unknown_identifier:
        output << "Error: Unknown function or constant.\n";
        break;

    case ErrorCode::mismatched_parentheses:
        output << "Error: Mismatched parentheses.\n";
        break;

    case ErrorCode::domain_error:
        output << "Error: Mathematical domain error.\n";
        break;

    case ErrorCode::numeric_overflow:
        output << "Error: Numeric overflow.\n";
        break;

    case ErrorCode::none:
        break;
    }
}

std::optional<std::string>
resolve_previous_answer(const std::string& expression,
                        const std::optional<double>& previous_answer) {
    std::string resolved;
    resolved.reserve(expression.size());

    for (std::size_t position = 0; position < expression.size();) {
        const auto character = static_cast<unsigned char>(expression[position]);

        if (std::isalpha(character) == 0) {
            resolved += expression[position];
            ++position;
            continue;
        }

        const std::size_t start = position;

        while (position < expression.size()) {
            const auto identifier_character =
                static_cast<unsigned char>(expression[position]);

            if (std::isalnum(identifier_character) == 0 &&
                identifier_character != '_') {
                break;
            }

            ++position;
        }

        const std::string identifier =
            expression.substr(start, position - start);

        if (identifier == "ans") {
            if (!previous_answer.has_value()) {
                return std::nullopt;
            }

            std::ostringstream value;
            value << std::setprecision(
                         std::numeric_limits<double>::max_digits10)
                  << *previous_answer;

            resolved += value.str();
        } else {
            resolved += identifier;
        }
    }

    return resolved;
}

void print_history(const CalculationHistory& history, std::ostream& output) {
    const auto& entries = history.entries();

    if (entries.empty()) {
        output << "No calculations in history.\n";
        return;
    }

    output << "\nCalculation History\n";
    output << "-------------------\n";

    for (std::size_t index = 0; index < entries.size(); ++index) {
        output << index + 1 << ". " << entries[index].expression << " = "
               << entries[index].result << '\n';
    }

    output << "-------------------\n";
}

} // namespace

void CalculatorApp::run(std::istream& input, std::ostream& output) {
    CalculatorEngine engine;
    CalculationHistory history;

    std::string expression;

    output << "Scientific Calculator\n";
    output << "Enter an expression to calculate.\n";
    output << "Commands: history, clear, exit\n";
    output << "Use ans to reuse the previous answer.\n\n";

    output << std::setprecision(display_precision);

    while (true) {
        output << ">> ";

        if (!std::getline(input, expression)) {
            break;
        }

        if (expression == "exit" || expression == "quit") {
            break;
        }

        if (expression.empty()) {
            continue;
        }

        if (expression == "history") {
            print_history(history, output);
            continue;
        }

        if (expression == "clear") {
            history.clear();
            output << "Calculation history cleared.\n";
            continue;
        }

        const auto resolved_expression =
            resolve_previous_answer(expression, history.previous_answer());

        if (!resolved_expression.has_value()) {
            output << "Error: No previous answer available. "
                   << "Calculate something first.\n";
            continue;
        }

        const auto result = engine.evaluate(*resolved_expression);

        if (!result.success()) {
            print_error(result.error, output);
            continue;
        }

        if (!history.add(expression, result)) {
            output << "Error: Invalid calculation result.\n";
            continue;
        }

        output << "= " << result.value << '\n';
    }

    output << "Calculator closed.\n";
}

} // namespace calculator
