#pragma once

#include <string_view>

#include "calculator/core/engine.hpp"

namespace calculator {

class ExpressionParser {
public:
    [[nodiscard]] CalculationResult evaluate(
        std::string_view expression
    ) const;
};

}