#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 6

NAMESPACE_DEF(DAY) {

struct MemoryBanks;
inline std::ostream& operator<<(std::ostream& os, const MemoryBanks& banks);

struct MemoryBanks
{
    static constexpr int N_BANK = 16; static_assert(N_BANK > 0);

    std::array<int, N_BANK> banks;

    explicit MemoryBanks(std::vector<int> v_banks)
    {
        std::copy_n(v_banks.begin(), N_BANK, banks.begin());
    }

    static int next(int i) { return (i+1) % N_BANK; }

    int find_highest() const
    {
        int highest = banks.front();
        int i_highest = 0;
        for (int i = 1; i < banks.size(); ++i)
        {
            int here = banks.at(i);
            if (here > highest)
            {
                highest = here;
                i_highest = i;
            }
        }

        return i_highest;
    }

    void rebalance(int start_i)
    {
        int debt = banks.at(start_i);
        banks.at(start_i) = 0;

        int current = next(start_i);
        while (debt > 0)
        {
            ++banks.at(current);
            --debt;
            current = next(current);
        }
    }

    __int128_t serialize() const // should totally not do this because of platform dependencies, but this is for fun. Cant do 4 bits because a column can become >= 16.
    {
        constexpr int BITS_PER_COL = 6;
        constexpr int MAX_PER_COL = (1 << BITS_PER_COL) - 1;

        __int128_t result = 0;

        for (int i = 0; i < banks.size(); ++i)
        {
            __int128_t tmp = banks.at(i);
            if (tmp > MAX_PER_COL) throw std::runtime_error("overflow");

            result |= tmp << (BITS_PER_COL * i); // shifts 'endianness' but we do not care, as long as it is consistent. And because we do not have to deserialize.
        }

        return result;
    }
};

inline std::ostream& operator<<(std::ostream& os, const MemoryBanks& banks)
{
    std::ranges::for_each(banks.banks, [&os](auto& bank) { os << bank << " "; });
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        std::getline(input, line);
        std::istringstream iss(line);

        while (iss.good())
        {
            int x;
            iss >> x;
            inputs.emplace_back(x);
        }
    }

    static size_t rebalance_until_cycle(MemoryBanks& b)
    {
        std::set<__int128_t> seen;
        seen.emplace(b.serialize());

        while (true)
        {
            b.rebalance(b.find_highest());

            auto [_, novel] = seen.emplace(b.serialize());

            if (!novel) break;
        }

        return seen.size();
    }

    void v1() const override {
        MemoryBanks b(inputs);

        reportSolution(rebalance_until_cycle(b));
    }

    void v2() const override {
        MemoryBanks b(inputs);
        rebalance_until_cycle(b);
        // ok, now we are in a state of loop. Keep going. How much until it happens again?
        reportSolution(rebalance_until_cycle(b));
    }

    void parseBenchReset() override {
        inputs.clear();
    }

    private:
    std::vector<int> inputs;
};

} // namespace

#undef DAY