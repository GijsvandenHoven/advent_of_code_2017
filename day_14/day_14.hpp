#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 14

NAMESPACE_DEF(DAY) {

using Hasher = Day10::Day10;

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::getline(input, key);
    }

    static std::array<uint8_t, 256> get_initial_knot_hash()
    {
        std::array<uint8_t, 256> hash{};
        std::iota(hash.begin(), hash.end(), 0);
        return hash;
    }

    static void make_hash(std::array<uint8_t, 16>& result, const std::string& input)
    {
        std::vector<uint8_t> input_bits;
        for (auto& c : input)
        {
            input_bits.emplace_back(static_cast<uint8_t>(c));
        }
        // add the random nonsense just like day 10.
        input_bits.emplace_back(17);
        input_bits.emplace_back(31);
        input_bits.emplace_back(73);
        input_bits.emplace_back(47);
        input_bits.emplace_back(23);

        uint8_t skip_size = 0;
        uint8_t position = 0;
        auto state = get_initial_knot_hash();
        for (int i = 0; i < 64; ++i)
        {
            Hasher::knot_hash_step(position, skip_size, state, input_bits);
        }

        Hasher::sparse_to_dense(state, result);
    }

    void make_grid(std::array<std::array<bool, 128>, 128>& out) const
    {
        std::array<std::array<uint8_t, 16>, 128> hashes {};
        for (int i = 0; i < hashes.size(); ++i)
        {
            auto& row = hashes.at(i);
            std::string this_row_key = key + "-" + std::to_string(i);
            make_hash(row, this_row_key);
        }

        for (int i = 0; i < out.size(); ++i)
        {
            auto& row = hashes.at(i);
            for (int j = 0; j < out.at(i).size(); ++j)
            {
                const int indexer = j / 8;
                uint8_t bitmask = 1 << (7-(j % 8)); // highest to lowest bit!
                auto value = row.at(indexer);
                out[i][j] = (value & bitmask) != 0;
            }
        }
    }

    void v1() const override {
        std::array<std::array<bool, 128>, 128> grid{};
        make_grid(grid);

        int occupied = 0;
        for (auto& row : grid)
        {
            for (bool v : row)
            {
                occupied += v;
            }
        }

        reportSolution(occupied);
    }

    static void mark_region(const int y, const int x, std::set<int>& seen,
                     const std::function<bool(int, int)>& coordinate_occupancy_checker,
                     const std::function<int(int, int)>& coordinate_int_converter) // maybe lambdas was not the best idea.
    {
        // auto start_size = seen.size();
        seen.emplace(coordinate_int_converter(x, y));
        std::queue<std::pair<int,int>> work;
        work.emplace(y,x);

        while (! work.empty())
        {
            auto [yy, xx] = work.front();
            work.pop();

            const std::array<std::pair<int,int>, 4> neighbours = {{
                { yy, xx+1 },
                { yy, xx-1 },
                { yy+1, xx },
                { yy-1, xx }
            }};

            for (auto [ny, nx] : neighbours)
            {
                const int id = coordinate_int_converter(nx, ny);
                if (coordinate_occupancy_checker(nx, ny) && ! seen.contains(id))
                {
                    seen.insert(id);
                    work.emplace(ny, nx);
                }
            }
        }

        // auto new_size = seen.size();
        //
        // std::cout << "Marked region of size " << (new_size - start_size) << "\n";
    }

    void v2() const override {
        std::array<std::array<bool, 128>, 128> grid{};
        make_grid(grid);

        // for (auto& row : grid)
        // {
        //     for (auto b : row)
        //     {
        //         std::cout << (b ? "#" : ".");
        //     }
        //     std::cout << "\n";
        // }

        // region checking is not that bad, just note the coordinates to explore in a queue like a BFS. Use an interface to abstract away array boundaries.
        auto coordinate_is_occupied = [&grid](int x, int y) -> bool
        {
            if (y < 0 || y >= grid.size()) return false;
            if (x < 0 || x >= grid.at(y).size()) return false;

            return grid.at(y).at(x);
        };
        auto coordinate_to_int = [grid_size = static_cast<int>(grid.size())](int x, int y) -> int
        {
            return y * grid_size + x;
        };

        int regions = 0;
        std::set<int> seen;
        for (int i = 0; i < grid.size(); ++i)
        {
            for (int j = 0; j < grid.at(i).size(); ++j)
            {
                int id = coordinate_to_int(j, i);
                if (grid.at(i).at(j) && ! seen.contains(id))
                {
                    ++regions;
                    // insert into the 'seen' set all region buddies based on what this function finds.
                    mark_region(i, j, seen, coordinate_is_occupied, coordinate_to_int);
                }
            }
        }

        reportSolution(regions);
    }

    void parseBenchReset() override {
        key.clear();
    }

    private:
    std::string key;
};

} // namespace

#undef DAY