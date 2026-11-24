
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "calculator/core/engine.hpp"

namespace calculator {

struct HistoryEntry {
    std::string expression;
    double result = 0.0;
};

class CalculationHistory {
public:
    [[nodiscard]] bool add(
        std::string_view expression,
        const CalculationResult& result
    );

    [[nodiscard]] const std::vector<HistoryEntry>& entries() const noexcept;

    [[nodiscard]] std::optional<double> previous_answer() const noexcept;

    void clear() noexcept;

private:
    std::vector<HistoryEntry> entries_;
};

}
