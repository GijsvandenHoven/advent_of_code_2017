#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 15

NAMESPACE_DEF(DAY) {

using gen_t = uint32_t;

struct Generator
{
    gen_t current;
    gen_t factor;
    std::function<bool(gen_t)> acceptance_criterion = [](gen_t) { return true; };

    gen_t next()
    {
        const uint64_t intermediate = static_cast<uint64_t>(current) * static_cast<uint64_t>(factor);
        constexpr uint64_t divisor = 0x7FFFFFFF;
        current = static_cast<gen_t>(intermediate % divisor);

        return current;
    }

    gen_t next_with_criterion()
    {
        do
        {
            next();
        } while (! acceptance_criterion(current));

        return current;
    }
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string gen_a_line;
        std::string gen_b_line;
        std::getline(input, gen_a_line);
        std::getline(input, gen_b_line);

        constexpr std::string_view skip_string = "Generator X starts with ";
        auto skipper = [&](std::istringstream& ss) {std::ranges::for_each(skip_string, [&](auto _) { ss.get(); }); };
        std::istringstream iss(gen_a_line);
        skipper(iss);
        iss >> A_start;
        iss = std::istringstream(gen_b_line);
        skipper(iss);
        iss >> B_start;

        // std::cout << A_start << " " << B_start << std::endl;
    }

    static bool bit_match(gen_t left, gen_t right, int bits = 16)
    {
        gen_t mask = static_cast<gen_t>(~0) >> (32 - bits);
        return (left & mask) == (right & mask);
    };

    void v1() const override {
        Generator A {A_start, A_factor };
        Generator B {B_start, B_factor };

        int match_count = 0;
        for (int i = 0; i < p1_sample_count; ++i)
        {
            gen_t a = A.next();
            gen_t b = B.next();

            match_count += bit_match(a, b);
        }

        reportSolution(match_count);
    }

    void v2() const override {
        Generator A {A_start, A_factor, p2_genA_acceptance_criteria };
        Generator B {B_start, B_factor, p2_genB_acceptance_criteria };

        int match_count = 0;
        for (int i = 0; i < p2_sample_count; ++i)
        {
            gen_t a = A.next_with_criterion();
            gen_t b = B.next_with_criterion();

            match_count += bit_match(a, b);
        }

        reportSolution(match_count);
    }

    void parseBenchReset() override {
        A_start = 0;
        B_start = 0;
    }

    private:
    gen_t A_start = 0;
    gen_t B_start = 0;
    static constexpr gen_t A_factor = 16807;
    static constexpr gen_t B_factor = 48271;
    static constexpr int p1_sample_count = 40'000'000;
    static constexpr int p2_sample_count = 5'000'000;
    static constexpr auto p2_genA_acceptance_criteria = [](gen_t v) -> bool { return v % 4 == 0; };
    static constexpr auto p2_genB_acceptance_criteria = [](gen_t v) -> bool { return v % 8 == 0; };
};

} // namespace

#undef DAY