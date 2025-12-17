#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 13

struct Layer
{
    int offset;
    int length;

    Layer(int o, int l) : offset(o), length(l) {}

    [[nodiscard]] bool is_scanner_at_top(const int departure_time = 0) const
    {
        int time_when_you_are_here = departure_time + offset;
        // Scanner is here at k * (2 * length-1)
        return time_when_you_are_here % (2 * (length - 1)) == 0;
    }
};

NAMESPACE_DEF(DAY) {

    CLASS_DEF(DAY) {
        public:
        DEFAULT_CTOR_DEF(DAY)

        void parse(std::ifstream &input) override {
            std::string line;
            while (std::getline(input, line))
            {
                int a;
                int b;
                std::istringstream iss(line);
                iss >> a;
                iss.get();
                iss >> b;

                layers.emplace_back(a, b);
            }
        }

        void v1() const override {
            const int severity = std::accumulate(layers.begin(), layers.end(), 0, [](auto acc, auto& l)
            {
               return acc + (l.offset * l.length * l.is_scanner_at_top());
            });

            reportSolution(severity);
        }

        void v2() const override {
            int t = 0;

            while (true)
            {
                const bool caught = std::ranges::any_of(layers, [t](auto& l) { return l.is_scanner_at_top(t); });
                if (caught)
                {
                    ++t;
                } else break;
            }

            reportSolution(t);
        }

        void parseBenchReset() override {
            layers.clear();
        }

        private:
        std::vector<Layer> layers;
    };

} // namespace

#undef DAY