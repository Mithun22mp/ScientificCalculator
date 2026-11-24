#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string_view>

#include "calculator/parser/expression_parser.hpp"

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data,
                                      std::size_t size) {
    const calculator::ExpressionParser parser;

    const std::string_view expression =
        size == 0 ? std::string_view{}
                  : std::string_view(reinterpret_cast<const char*>(data), size);

    const calculator::CalculationResult result = parser.evaluate(expression);

    // Successful calculations must always produce finite results.
    if (result.success() && !std::isfinite(result.value)) {
        std::abort();
    }

    return 0;
}
