
#include "calculator/core/history.hpp"

#include <cmath>

namespace calculator {

bool CalculationHistory::add(std::string_view expression,
                             const CalculationResult& result) {
    if (!result.success() || expression.empty() ||
        !std::isfinite(result.value)) {
        return false;
    }

    entries_.push_back({std::string(expression), result.value});

    return true;
}

const std::vector<HistoryEntry>& CalculationHistory::entries() const noexcept {
    return entries_;
}

std::optional<double> CalculationHistory::previous_answer() const noexcept {
    if (entries_.empty()) {
        return std::nullopt;
    }

    return entries_.back().result;
}

void CalculationHistory::clear() noexcept {
    entries_.clear();
}

} // namespace calculator
