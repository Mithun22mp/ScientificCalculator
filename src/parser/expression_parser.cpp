
#include "calculator/parser/expression_parser.hpp"

#include <charconv>
#include <cctype>
#include <cmath>
#include <numbers>
#include <string>
#include <system_error>

namespace calculator {
namespace {

class Parser {
public:
    explicit Parser(std::string_view expression)
        : expression_(expression) {}

    CalculationResult parse() {
        skip_whitespace();

        if (position_ == expression_.size()) {
            return failure(ErrorCode::invalid_expression);
        }

        const double value = parse_expression();

        if (error_ != ErrorCode::none) {
            return failure(error_);
        }

        skip_whitespace();

        if (position_ != expression_.size()) {
            return failure(ErrorCode::invalid_expression);
        }

        if (!std::isfinite(value)) {
            return failure(ErrorCode::numeric_overflow);
        }

        return {value, ErrorCode::none};
    }

private:
    std::string_view expression_;
    std::size_t position_ = 0;
    ErrorCode error_ = ErrorCode::none;

    static CalculationResult failure(ErrorCode error) {
        return {0.0, error};
    }

    void skip_whitespace() {
        while (position_ < expression_.size() &&
               std::isspace(
                   static_cast<unsigned char>(expression_[position_]))) {
            ++position_;
        }
    }

    bool consume(char expected) {
        skip_whitespace();

        if (position_ < expression_.size() &&
            expression_[position_] == expected) {
            ++position_;
            return true;
        }

        return false;
    }

    double parse_expression() {
        double value = parse_term();

        while (error_ == ErrorCode::none) {
            if (consume('+')) {
                value += parse_term();
            } else if (consume('-')) {
                value -= parse_term();
            } else {
                break;
            }
        }

        return value;
    }

    double parse_term() {
        double value = parse_unary();

        while (error_ == ErrorCode::none) {
            if (consume('*')) {
                value *= parse_unary();
            } else if (consume('/')) {
                const double divisor = parse_unary();

                if (error_ != ErrorCode::none) {
                    break;
                }

                if (divisor == 0.0) {
                    error_ = ErrorCode::division_by_zero;
                    break;
                }

                value /= divisor;
            } else {
                break;
            }
        }

        return value;
    }

    double parse_unary() {
        if (consume('+')) {
            return parse_unary();
        }

        if (consume('-')) {
            return -parse_unary();
        }

        return parse_power();
    }

    double parse_power() {
        const double base = parse_primary();

        if (error_ != ErrorCode::none || !consume('^')) {
            return base;
        }

        const double exponent = parse_unary();

        if (error_ != ErrorCode::none) {
            return 0.0;
        }

        const double result = std::pow(base, exponent);

        if (std::isnan(result)) {
            error_ = ErrorCode::domain_error;
            return 0.0;
        }

        if (!std::isfinite(result)) {
            error_ = ErrorCode::numeric_overflow;
            return 0.0;
        }

        return result;
    }

    double parse_primary() {
        skip_whitespace();

        if (consume('(')) {
            const double value = parse_expression();

            if (error_ == ErrorCode::none && !consume(')')) {
                error_ = ErrorCode::mismatched_parentheses;
            }

            return value;
        }

        if (position_ >= expression_.size()) {
            error_ = ErrorCode::invalid_expression;
            return 0.0;
        }

        const char current = expression_[position_];

        if (std::isdigit(static_cast<unsigned char>(current)) ||
            current == '.') {
            return parse_number();
        }

        if (std::isalpha(static_cast<unsigned char>(current))) {
            return parse_identifier();
        }

        error_ = ErrorCode::invalid_expression;
        return 0.0;
    }

    double parse_number() {
        const char* begin = expression_.data() + position_;
        const char* end = expression_.data() + expression_.size();

        double value = 0.0;

        const auto result = std::from_chars(
            begin,
            end,
            value,
            std::chars_format::general
        );

        if (result.ec == std::errc::result_out_of_range) {
            error_ = ErrorCode::numeric_overflow;
            return 0.0;
        }

        if (result.ec != std::errc{} || result.ptr == begin) {
            error_ = ErrorCode::invalid_number;
            return 0.0;
        }

        position_ += static_cast<std::size_t>(result.ptr - begin);
        return value;
    }

    std::string read_identifier() {
        const std::size_t start = position_;

        while (position_ < expression_.size()) {
            const unsigned char character =
                static_cast<unsigned char>(expression_[position_]);

            if (!std::isalnum(character) && character != '_') {
                break;
            }

            ++position_;
        }

        return std::string(
            expression_.substr(start, position_ - start)
        );
    }

    double parse_identifier() {
        const std::string name = read_identifier();

        if (name == "pi") {
            return std::numbers::pi_v<double>;
        }

        if (name == "e") {
            return std::numbers::e_v<double>;
        }

        if (!consume('(')) {
            error_ = ErrorCode::unknown_identifier;
            return 0.0;
        }

        const double argument = parse_expression();

        if (error_ == ErrorCode::none && !consume(')')) {
            error_ = ErrorCode::mismatched_parentheses;
            return 0.0;
        }

        if (error_ != ErrorCode::none) {
            return 0.0;
        }

        return evaluate_function(name, argument);
    }

    double evaluate_function(
        const std::string& name,
        double argument
    ) {
        constexpr double degrees_to_radians =
            std::numbers::pi_v<double> / 180.0;

        if (name == "sqrt") {
            if (argument < 0.0) {
                error_ = ErrorCode::domain_error;
                return 0.0;
            }
            return std::sqrt(argument);
        }

        if (name == "sin") {
            return std::sin(argument * degrees_to_radians);
        }

        if (name == "cos") {
            return std::cos(argument * degrees_to_radians);
        }

        if (name == "tan") {
            return std::tan(argument * degrees_to_radians);
        }

        if (name == "asin" || name == "acos") {
            if (argument < -1.0 || argument > 1.0) {
                error_ = ErrorCode::domain_error;
                return 0.0;
            }

            const double result = name == "asin"
                ? std::asin(argument)
                : std::acos(argument);

            return result / degrees_to_radians;
        }

        if (name == "atan") {
            return std::atan(argument) / degrees_to_radians;
        }

        if (name == "ln" || name == "log") {
            if (argument <= 0.0) {
                error_ = ErrorCode::domain_error;
                return 0.0;
            }

            return name == "ln"
                ? std::log(argument)
                : std::log10(argument);
        }

        if (name == "abs") {
            return std::abs(argument);
        }

        if (name == "exp") {
            return std::exp(argument);
        }

        error_ = ErrorCode::unknown_identifier;
        return 0.0;
    }
};

} // namespace

CalculationResult ExpressionParser::evaluate(
    std::string_view expression
) const {
    return Parser(expression).parse();
}

} // namespace calculator
