#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 1

NAMESPACE_DEF(DAY) {

constexpr int FULL_ROTATION = 100;
constexpr int DIAL_START_STATE = 50;

enum class Direction
{
    R = 0,
    L,
    ERROR
};

struct Instruction
{
    Direction d;
    int magnitude;

    explicit Instruction(const std::string& in)
        : d(Direction::ERROR), magnitude(0)
    {
        switch (in.at(0))
        {
            case 'R': d = Direction::R; break;
            case 'L': d = Direction::L; break;
            default: throw std::invalid_argument("Invalid direction: " + in);
        }

        auto digit_iter = std::next(in.begin());
        while (digit_iter != in.end())
        {
            magnitude *= 10;
            magnitude += (*digit_iter) - '0';
            ++digit_iter;
        }
    }

    [[nodiscard]] int get_rotation() const
    {
        return d == Direction::R ? magnitude : -magnitude;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Direction& direction)
{
    switch (direction)
    {
        case Direction::R: os << "R"; break;
        case Direction::L: os << "L"; break;
        default: os << "__ERR__"; break;
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Instruction& instruction)
{
    os << instruction.d << instruction.magnitude;
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line))
        {
            instructions.emplace_back(line);
        }
    }

    void v1() const override {
        int state = DIAL_START_STATE;
        int on_zero_count = 0;
        std::ranges::for_each(instructions, [&](auto& instruction)
        {
          state += instruction.get_rotation();
          if (state % FULL_ROTATION == 0)
          {
            on_zero_count++;
          }
        });
        reportSolution(on_zero_count);
    }

    void v2() const override {
        int state = DIAL_START_STATE;
        int lowest_ever_state = state;
        int at_zero_count = 0;

        // This solution works by counting magnitudes of '100' changing.
        // It would be very messy to do this with a start state of '50':
        // If the state of the dial ever crosses from positive to negative, or the other way around,
        // Lots of complicated edge case handling would be needed depending on the rotation direction, the previous state, and the new state.
        // Solution: Normalise so that the state is always positive. Run the instructions once and find out the "most left" it ever gets.
        // Then offset for that.
        std::ranges::for_each(instructions, [&](auto& instruction)
        {
            state += instruction.get_rotation();
            lowest_ever_state = std::min(lowest_ever_state, state);
        });

        const int required_offset = FULL_ROTATION * (1 + std::abs(lowest_ever_state / FULL_ROTATION));
        state = DIAL_START_STATE + required_offset;

        std::ranges::for_each(instructions, [&](auto& instruction)
        {
            const int old_state = state;
            const int new_state = old_state + instruction.get_rotation();

            int times_touching_zero_this_step = 0;

            // Remember: It is assumed the state shall never go negative. This is why `-50/100` == `50/100` is not a problem that is handled here.
            const int current_multiple_of_full_rotation = (old_state / FULL_ROTATION);
            const int new_multiple_of_full_rotation = (new_state / FULL_ROTATION);
            times_touching_zero_this_step += std::abs(current_multiple_of_full_rotation - new_multiple_of_full_rotation);

            const bool started_at_zero = (old_state % FULL_ROTATION) == 0;
            const bool ended_at_zero = (new_state % FULL_ROTATION) == 0;
            if ((instruction.d == Direction::L) && (started_at_zero != ended_at_zero))
            {
                times_touching_zero_this_step += ended_at_zero ? 1 : -1;
            }

            { // LOOP VARIABLE UPDATE BLOCK
                state = new_state;
                at_zero_count += times_touching_zero_this_step;
            }
        });
        reportSolution(at_zero_count);
    }

    void parseBenchReset() override {
        instructions.clear();
    }

    private:
    std::vector<Instruction> instructions;
};

} // namespace

#undef DAY