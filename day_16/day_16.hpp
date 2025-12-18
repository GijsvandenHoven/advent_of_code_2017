#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 16

NAMESPACE_DEF(DAY) {

using State_t = std::array<char, 16>;
struct State
{
    State_t objects{};
    int start_ptr = 0;

    [[nodiscard]] char& find(char c)
    {
        for (char& object : objects)
        {
            if (object == c) return object;
        }

        throw std::logic_error(std::string{"Char not found: "} + c);
    }

    [[nodiscard]] std::string print(const int tabs = 1) const
    {
        std::ostringstream oss;
        for (int i = 0; i < tabs; ++i) { oss << "\t"; }

        for (int i = 0; i < objects.size(); ++i)
        {
            int index = at(i);
            oss << objects.at(index);
        }
        oss << "\n";

        return oss.str();
    }

    [[nodiscard]] int at(int i) const
    {
        return (start_ptr + i) % static_cast<int>(objects.size());
    }
};

class Move
{
public:
    virtual ~Move() = default;
    virtual void execute(State& state) const = 0;
};

class Spin : public Move
{
    int amount;

public:
    explicit Spin(int a) : Move(), amount(a) {}

    void execute(State& state) const override
    {
        // std::cout << "Execute Spin with " << amount << std::endl;
        if (state.start_ptr - amount < 0)
        {
            int remaining = amount - state.start_ptr;
            state.start_ptr = static_cast<int>(state.objects.size()) - remaining;
        } else
        {
            state.start_ptr -= amount;
        }
    }
};

class Exchange : public Move
{
    int posA;
    int posB;

public:
    Exchange(int a, int b) : Move(), posA(a), posB(b) {}

    void execute(State& state) const override
    {
        // std::cout << "Execute Exchange with: " << posA << " " << posB << std::endl;
        int mapped_A = state.at(posA);
        int mapped_B = state.at(posB);

        std::swap(state.objects.at(mapped_A), state.objects.at(mapped_B));
    }
};

class Partner : public Move
{
    char a;
    char b;

public:
    Partner(char _a, char _b) : Move(), a(_a), b(_b) {}

    void execute(State& state) const override
    {
        // std::cout << "Execute Partner with: " << a << " " << b << std::endl;
        std::swap(state.find(a), state.find(b));
    }
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        std::getline(input, line);

        std::istringstream iss(line);
        std::ostringstream oss;

        auto commit = [this](const std::string& token)
        {
            std::unique_ptr<Move> m;
            char mode = token.at(0);
            switch (mode)
            {
                case 's':
                    m = std::make_unique<Spin>(std::stoi(token.substr(1)));
                    break;
                case 'x':
                {
                    size_t split = token.find_first_of('/');
                    int left = std::stoi(token.substr(1, split - 1));
                    int right = std::stoi(token.substr(split + 1));
                    m = std::make_unique<Exchange>(left, right);
                    break;
                }
                case 'p':
                {
                    size_t split = token.find_first_of('/');
                    m = std::make_unique<Partner>(token.at(split-1), token.at(split+1));
                    break;
                }
                default: throw std::logic_error("Unknown input: " + token);
            }

            moves.emplace_back(std::move(m));
        };

        while (iss.peek() != EOF)
        {
            char c = static_cast<char>(iss.get());
            if (c == ',')
            {
                commit(oss.str());
                oss = std::ostringstream();
            } else
            {
                oss << c;
            }
        }
        commit(oss.str());
    }

    void v1() const override {
        State s = { initial_state };
        std::cout << "Initial state:\n";
        // std::cout << s.print();
        for (auto& m : moves)
        {
            m->execute(s);
            // std::cout << s.print();
        }
        reportSolution(s.print(0));
    }

    void v2() const override {
        // Dr. Evil type problem. Let's just look for repeated states. Surely there's not a 16 factorial permutation problem.
        State s = { initial_state };

        std::map<int, std::string> step_to_state;
        std::set<std::string> seen;

        seen.emplace(s.print());
        step_to_state.emplace(0, s.print());

        constexpr int goal = 1'000'000'000;
        int i = 0;
        while (i < goal)
        {
            for (auto& m : moves)
            {
                ++i;
                m->execute(s);
            }

            auto result = s.print(0);
            if (! seen.contains(result))
            {
                step_to_state.emplace(i, result);
                seen.emplace(result);
            } else
            {
                // the repeat is 'abcdefghijklmnop'. So free, there is not even a run-up before going into steady state :)
                break;
            }
        }

        // alt: search the map values for abcd(...) to get the first time it was seen and calculate the cycle amount from there instead.
        int first_time_seen = 0;

        int cycle_size = i - first_time_seen;
        int times_more_we_need = (goal % cycle_size) - first_time_seen; // subtract the lead up (if it existed), assumes lead up is < cycle_size.

        reportSolution(step_to_state[times_more_we_need]);
    }

    void parseBenchReset() override {
        moves.clear();
    }

    private:
    State_t initial_state = {{ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p' }};
    std::vector<std::unique_ptr<Move>> moves;
};

} // namespace

#undef DAY