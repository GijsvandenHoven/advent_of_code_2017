#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 10

NAMESPACE_DEF(DAY) {

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        std::getline(input, line);
        std::istringstream iss(line);

        while (iss.peek() != EOF)
        {
            int total = 0;
            int c;
            while ((c = iss.get()) != ',' && c != EOF)
            {
                total = total * 10 + (c - '0');
            }
            ranges.emplace_back(total);
        }

        // std::ranges::for_each(ranges, [](int v) { std::cout << v << ", "; });
        // std::cout << "\n";

        iss = std::istringstream(line);
        while (iss.peek() != EOF)
        {
            ascii_ranges.emplace_back(static_cast<char>(iss.get()));
        }
        ascii_ranges.emplace_back(17);
        ascii_ranges.emplace_back(31);
        ascii_ranges.emplace_back(73);
        ascii_ranges.emplace_back(47);
        ascii_ranges.emplace_back(23);

        // std::ranges::for_each(ascii_ranges, [](int v) { std::cout << v << ", "; });
        // std::cout << "\n";
    }

    static void knot_hash_step(uint8_t& position, uint8_t& skip_size, std::array<uint8_t, 256>& numbers, const std::vector<uint8_t>& inputs)
    {
        for (uint8_t range : inputs)
        {
            uint8_t start = position;
            uint8_t end = position + range - 1;

            int reversals = range / 2;
            for (int i = 0; i < reversals; ++i)
            {
                std::swap(numbers.at(start), numbers.at(end));
                ++start;
                --end;
            }

            position += (range + skip_size);
            ++skip_size;
        }
    }

    static void sparse_to_dense(const std::array<uint8_t, 256>& numbers, std::array<uint8_t, 16>& dense_hash)
    {
        for (int i = 0; i < numbers.size() / 16; ++i)
        {
            uint8_t block_hash = 0;
            for (int j = 0; j < 16; ++j)
            {
                block_hash ^= numbers[i * 16 + j];
            }
            dense_hash.at(i) = block_hash;
        }
    }

    static void dense_to_hex(const std::array<uint8_t, 16>& dense_hash, std::array<char, 32>& out)
    {
        constexpr auto to_hex = [](uint8_t n){
            char digit_base = '0';
            char letter_base = 'a';
            switch (n)
            {
            case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
                return digit_base + n;
            case 10: case 11: case 12: case 13: case 14: case 15:
                return letter_base + (n - 10);
            default:
                throw std::logic_error("Invalid number for hex str: " + std::to_string(n));
            }
        };
        for (int i = 0; i < dense_hash.size(); ++i)
        {
            const uint8_t left = (dense_hash.at(i) & 240) >> 4;
            const uint8_t right = (dense_hash.at(i) & 15);
            out.at(2 * i) = static_cast<char>(to_hex(left));
            out.at(2* i + 1) = static_cast<char>(to_hex(right));
        }
    }

    void v1() const override {
        uint8_t skip_size = 0;
        uint8_t position = 0;
        std::array<uint8_t, 256> numbers{};
        std::iota(numbers.begin(), numbers.end(), 0);

        knot_hash_step(position, skip_size, numbers, ranges);

        reportSolution(numbers.at(0) * numbers.at(1));
    }

    void v2() const override {
        uint8_t skip_size = 0;
        uint8_t position = 0;
        std::array<uint8_t, 256> numbers{};
        std::iota(numbers.begin(), numbers.end(), 0);

        auto print = [&]() { std::ranges::for_each(numbers, [](auto v) { std::cout << static_cast<int>(v) << ", "; }); };

        for (int i = 0; i < 64; ++i)
        {
            print(); std::cout << "\n";
            knot_hash_step(position, skip_size, numbers, ascii_ranges);
        }

        std::array<uint8_t, 16> dense_hash{};
        sparse_to_dense(numbers, dense_hash);

        std::array<char, 32> out{};
        dense_to_hex(dense_hash, out);

        std::ostringstream ss;
        std::ranges::for_each(out, [&ss](char c) { ss << c; });

        reportSolution(ss.str());

        std::cout << "\n\n\n\nTEST GROUND\n\n\n";
        // std::array<uint8_t, 16> despair {{ 65, 27, 9, 1, 4, 3, 40, 50, 91, 7, 6, 0, 2, 5, 68, 22 }};
        // std::array<uint8_t, 256> fake{};
        // std::copy_n(despair.begin(), 16, fake.begin());
        // std::array<uint8_t, 16> test_out{};
        // sparse_to_dense(fake, test_out);
        //
        // for (int v : test_out)
        // {
        //     std::cout << v << ", ";
        // }

        // std::array<uint8_t, 16> despair {{ 64,7,255,0,0,0,0,0,0,0,0,0,0,0,0,0 }};
        // std::array<char, 32> test_out{};
        // dense_to_hex(despair, test_out);
        // std::ostringstream test_ss;
        // std::ranges::for_each(test_out, [&test_ss](char c) { test_ss << c; });
        // std::cout << "? " << test_ss.str() << "\n";
    }

    void parseBenchReset() override {
        ranges.clear();
        ascii_ranges.clear();
    }

    private:
    std::vector<uint8_t> ranges;
    std::vector<uint8_t> ascii_ranges;
};

} // namespace

#undef DAY