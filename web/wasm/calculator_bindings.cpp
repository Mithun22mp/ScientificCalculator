#include "calculator/core/engine.hpp"

#include <string>

#include <emscripten/bind.h>

namespace {

const char* error_message(calculator::ErrorCode error) noexcept {
    using calculator::ErrorCode;

    switch (error) {
    case ErrorCode::none:
        return "";
    case ErrorCode::division_by_zero:
        return "Division by zero";
    case ErrorCode::invalid_expression:
        return "Invalid expression";
    case ErrorCode::invalid_number:
        return "Invalid number";
    case ErrorCode::unknown_identifier:
        return "Unknown identifier";
    case ErrorCode::mismatched_parentheses:
        return "Mismatched parentheses";
    case ErrorCode::domain_error:
        return "Math domain error";
    case ErrorCode::numeric_overflow:
        return "Numeric overflow";
    }

    return "Unknown calculation error";
}

emscripten::val evaluate_expression(const std::string& expression) {
    const calculator::CalculatorEngine engine;
    const calculator::CalculationResult result =
        engine.evaluate(expression);

    emscripten::val response = emscripten::val::object();

    response.set("success", result.success());
    response.set("value", result.value);
    response.set("error", error_message(result.error));

    return response;
}

} // namespace

EMSCRIPTEN_BINDINGS(scientific_calculator) {

    emscripten::function(
        "evaluateExpression",
        &evaluate_expression
    );
}
