
#pragma once

#include <string_view>

namespace calculator {

enum class ErrorCode {
    none,
    division_by_zero,
    invalid_expression,
    invalid_number,
    unknown_identifier,
    mismatched_parentheses,
    domain_error,
    numeric_overflow
};

struct CalculationResult {
    double value = 0.0;
    ErrorCode error = ErrorCode::none;

    [[nodiscard]] bool success() const noexcept;
};

class CalculatorEngine {
public:
    [[nodiscard]] CalculationResult add(double a, double b) const noexcept;

    [[nodiscard]] CalculationResult subtract(double a, double b) const noexcept;

    [[nodiscard]] CalculationResult multiply(double a, double b) const noexcept;

    [[nodiscard]] CalculationResult divide(double a, double b) const noexcept;

    [[nodiscard]] CalculationResult evaluate(std::string_view expression) const;
};

} // namespace calculator
