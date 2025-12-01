#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 1

NAMESPACE_DEF(DAY) {

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        std::getline(input, line);

        std::ranges::for_each(line, [this](char c)
        {
            numbers.emplace_back(c - '0');
        });
    }

    // not 1994 - high
    // not 205 - low
    void v1() const override {
        int prev = numbers.back();
        int count = 0;
        for (auto v : numbers)
        {
            if (prev == v)
            {
                count += v;
            }
            prev = v;
        }

        reportSolution(count);
    }

    void v2() const override {
        // allowed to assume even number of elements.
        const int jump_ahead = static_cast<int>(numbers.size() / 2);
        auto counterpart = [&](int i) -> int
        {
            return static_cast<int>((i + jump_ahead) % numbers.size());
        };

        int count = 0;
        for (int i = 0; i < numbers.size(); ++i)
        {
            int v = numbers.at(i);
            int other = numbers.at(counterpart(i));
            std:: cout << v << " , " << other << "\n";
            if (other == v)
            {
                count += v;
            }
        }

        reportSolution(count);
    }

    void parseBenchReset() override {
        numbers.clear();
    }

    private:
    std::vector<int> numbers;
};

} // namespace

#undef DAY