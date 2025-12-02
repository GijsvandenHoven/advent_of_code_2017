#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 2

NAMESPACE_DEF(DAY) {

struct Row
{
    std::vector<int> numbers;

    explicit Row(const std::string& line)
    {
        int v;
        std::istringstream iss(line);
        while (! iss.eof())
        {
            iss >> v;
            numbers.push_back(v);
        }
    }

    [[nodiscard]] int get_largest_diff() const
    {
        int lowest = numbers.back();
        int highest = numbers.back();
        for (auto& v : numbers)
        {
            lowest = std::min(lowest, v);
            highest = std::max(highest, v);
        }

        return highest - lowest;
    }

    [[nodiscard]] int get_only_even_division_result() const
    {
        // pairwise compare values
        for (int i = 0; i < numbers.size(); ++i)
        {
            for (int j = i+1; j < numbers.size(); ++j)
            {
                int A = numbers.at(i);
                int B = numbers.at(j);

                int k = A / B;
                int l = B / A;

                if (k * B == A)
                {
                    return k;
                }
                if (l * A == B)
                {
                    return l;
                }
            }
        }

        throw std::invalid_argument("Divisor pair does not exist");
    }
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line))
        {
            sheets.emplace_back(line);
        }
    }

    void v1() const override {
        auto result = std::accumulate(sheets.begin(), sheets.end(), 0, [](int v, auto& s)
        {
            return v + s.get_largest_diff();
        });
        reportSolution(result);
    }

    void v2() const override {
        auto result = std::accumulate(sheets.begin(), sheets.end(), 0, [](int v, auto& s)
        {
            return v + s.get_only_even_division_result();
        });
        reportSolution(result);
    }

    void parseBenchReset() override {
        // none
    }

    private:
    std::vector<Row> sheets;
};

} // namespace

#undef DAY