#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 5

NAMESPACE_DEF(DAY) {

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line))
        {
            offsets.emplace_back(std::stoi(line));
        }
    }

    static int traverse_and_count_until_exit(std::vector<int>& jumps, bool insane_rules = false)
    {
        int at = 0;
        int step_count = 0;
        while (at >= 0 && at < jumps.size())
        {
            int offset = jumps.at(at);

            if (insane_rules)
            {
                jumps.at(at) += (offset >= 3 ? -1 : 1);
            } else
            {
                jumps.at(at) += 1;
            }

            at += offset;
            ++step_count;
        }
        return step_count;
    }

    void v1() const override {
        auto copy = offsets;
        reportSolution(traverse_and_count_until_exit(copy));
    }

    void v2() const override {
        auto copy = offsets;
        reportSolution(traverse_and_count_until_exit(copy, true));
    }

    void parseBenchReset() override {
        offsets.clear();
    }

    private:
    std::vector<int> offsets;
};

} // namespace

#undef DAY