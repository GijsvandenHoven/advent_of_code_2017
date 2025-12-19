#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 19

NAMESPACE_DEF(DAY) {

enum class Dir
{
    UP, DOWN, LEFT, RIGHT
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line))
        {
            characters.emplace_back();
            for (auto& c : line)
            {
                characters.back().emplace_back(c);
            }
        }
    }

    // Collects letters while walking. Makes the turn and a step before returning. Returns true if there is more walking to be done. This is the case when it finds a way to turn to.
    bool walk_and_turn(int& coord, Dir& d, std::vector<char>& collected_markers, int& step_counter) const
    {
        const int width = static_cast<int>(characters.back().size());

        auto coord_to_rc = [this](int coord)
        {
            int row = coord / static_cast<int>(characters.front().size());
            int col = coord % static_cast<int>(characters.front().size());
            return std::make_pair(row, col);
        };

        auto next = [&](Dir dd) -> int { switch (dd) { case Dir::UP: return coord - width; case Dir::DOWN: return coord + width; case Dir::LEFT: return coord - 1; case Dir::RIGHT: return coord + 1; default: throw std::logic_error("Invalid Direction"); }; };

        auto oppose = [&](Dir dd) -> Dir { switch (dd) { case Dir::UP: return Dir::DOWN; case Dir::DOWN: return Dir::UP; case Dir::LEFT: return Dir::RIGHT; case Dir::RIGHT: return Dir::LEFT; default: throw std::logic_error("Invalid Direction"); }; };

        auto update_direction = [&]() -> Dir
        {
            std::array<std::pair<int, Dir>, 4> candidates = {{ {next(Dir::UP), Dir::UP}, {next(Dir::DOWN), Dir::DOWN}, {next(Dir::LEFT), Dir::LEFT}, {next(Dir::RIGHT), Dir::RIGHT} }};
            for (auto& [candidate_coord, candidate_dir] : candidates)
            {
                if (candidate_dir == oppose(d)) continue;

                auto [cr, cc] = coord_to_rc(candidate_coord);
                if (characters.at(cr).at(cc) != ' ') return candidate_dir;
            }

            // if there is no way to turn, just keep going. The switch will find out the next step is a space. You can see in the input as well that no plus is adjacent to a path it does not connect to.
            return d;
        };

        Dir old_dir = d;
        while (old_dir == d)
        {
            auto [row, col] = coord_to_rc(coord);
            char at = characters.at(row).at(col);
            // std::cout << "inspect: [" << coord << "] " << row << " - " << col << ": " << at << "\n";

            switch (at)
            {
            case '+': d = update_direction(); break;
            case ' ': return false;
            default:
                if (at >= 'A' && at <= 'Z')
                {
                    collected_markers.emplace_back(at);
                }
            }

            coord = next(d);
            ++step_counter; // This is invariant. Whenever the coordinate is updated, so is the step counter.
        }
        return true;
    }

    void v1() const override {
        auto start = std::ranges::find(characters.front(), '|');

        // Represents the full coordinate mapped to a single int (because row = 0)
        int coordinate = static_cast<int>(std::distance(characters.front().begin(), start));
        Dir facing = Dir::DOWN;
        std::vector<char> collected;
        int _ = 0;

        while (walk_and_turn(coordinate, facing, collected, _)) {}

        std::ostringstream ostream;
        for (auto& c : collected)
        {
            ostream << c;
        }

        reportSolution(ostream.str());
    }

    void v2() const override {
        auto start = std::ranges::find(characters.front(), '|');

        // Represents the full coordinate mapped to a single int (because row = 0)
        int coordinate = static_cast<int>(std::distance(characters.front().begin(), start));
        Dir facing = Dir::DOWN;
        std::vector<char> _;
        int step_counter = 0;

        while (walk_and_turn(coordinate, facing, _, step_counter)) {}

        reportSolution(step_counter);
    }

    void parseBenchReset() override {
        characters.clear();
    }

    private:
    // Rejoice! Eric has wrapped the whole thing in whitespace. There will be no need to check for out of bounds. But watchout, CLion loves eating whitespaces. Only open in basic text editors.
    std::vector<std::vector<char>> characters; // row-major order.
};

} // namespace

#undef DAY