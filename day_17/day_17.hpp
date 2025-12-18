#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 17

NAMESPACE_DEF(DAY) {

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        std::getline(input, line);
        jump_size = std::stoi(line);
    }

    static void insert_after(int i, int v, std::deque<int>& deque)
    {
        auto iter = std::next(deque.begin(), i + 1);
        if (iter > deque.end()) throw std::logic_error("Inserting after end.");

        deque.insert(iter, v);
    }

    void v1() const override {
        std::deque<int> values;
        values.emplace_front(0);
        values.emplace_back(1); // equivalent to insert_after(0). Counts as first of N_JUMPS.

        int d_index = 1;
        for (int i = 2; i <= N_JUMPS; ++i)
        {
            int new_index = (d_index + jump_size) % static_cast<int>(values.size());
            insert_after(new_index, i, values);
            d_index = new_index + 1;

            //for (auto& v : values) std::cout << v << " "; std::cout << "\n";
        }

        auto iter = std::find(values.begin(), values.end(), 2017);
        auto answer = *std::next(iter);

        reportSolution(answer);
    }

    void v2() const override {
#if I_AM_PATIENT // honestly seems doable in like an hour. The deque choice might have helped a bit? And it would only be on the order of 10s to 100s of megabytes.
        std::deque<int> values;
        values.emplace_front(0);
        values.emplace_back(1); // equivalent to insert_after(0). Counts as first of N_JUMPS.

        int d_index = 1;
        for (int i = 2; i <= 50'000'000; ++i)
        {
            if (i%1'000'000 == 0) std::cout << i << "\n";

            int new_index = (d_index + jump_size) % static_cast<int>(values.size());
            insert_after(new_index, i, values);
            d_index = new_index + 1;

            // for (auto& v : values) std::cout << v << " "; std::cout << "\n";
        }

        auto iter = std::find(values.begin(), values.end(), 2017);
        auto answer = *std::next(iter);
#else
        int size = 2;
        int d_index = 1;
        int answer = 1; // invariant :) - the 'answer' shall hold the value after [0] at step [i].
        for (int i = 2; i <= 50'000'000; ++i)
        {
            int new_index = (d_index + jump_size) % size;
            d_index = new_index + 1;
            ++size;

            if (new_index == 0) // we inserted something after '0', so it is our answer until we overwrite again.
            {
                answer = i;
            }
        }
#endif

        reportSolution(answer);
    }

    void parseBenchReset() override {
        jump_size = 0;
    }

    private:
    int jump_size = 0;
    static constexpr int N_JUMPS = 2017;
};

} // namespace

#undef DAY