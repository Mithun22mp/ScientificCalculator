#pragma once

#include <iosfwd>

namespace calculator {

class CalculatorApp {
public:
    void run(std::istream& input, std::ostream& output);
};

} // namespace calculator