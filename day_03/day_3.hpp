#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 3

NAMESPACE_DEF(DAY) {

    CLASS_DEF(DAY) {
        public:
        DEFAULT_CTOR_DEF(DAY)

        void parse(std::ifstream &input) override {
            input >> N;

            if (N < 0) throw std::invalid_argument("N must be positive");
        }

        void v1() const override {
            // find out which ring of the spiral we are on by square root
            int ring = static_cast<int>(std::ceil(std::sqrt(N))) / 2;
            // This value represents how many steps from the center... if diagonal steps were allowed.
            // For every step not directly above/below/to the side of the spiral exit, we need to add one step
            int bottom_right = (ring * 2) * (ring * 2);
            if (bottom_right % 2 == 0)
            {
                bottom_right = static_cast<int>(std::pow(std::sqrt(bottom_right) + 1, 2)); // bottom right is always an odd number with an odd root.
            }
            // picture a square. To grow it one size, push out all walls.
            // The corners got double counted, so +1 per corner. New corners are made, so +1 per corner. there are 4 corners. Add 8 per ring.
            int ring_size = ring * 8;
            int seg_size = ring_size / 4;
            // Can also be computed by the difference of this bottom right and the previous bottom right { (2(ring-1))^2 }

            // int prev_ring_bot_right = (2*(ring-1)) * (2*(ring-1));
            // if (prev_ring_bot_right % 2 == 0)
            // {
            //     prev_ring_bot_right = static_cast<int>(std::pow(std::sqrt(prev_ring_bot_right) + 1, 2));
            // }
            //
            // std::cout << ring << " ; " << bottom_right << " ; " << prev_ring_bot_right << std::endl;
            // std::cout << "---\n";
            // int total_in_ring = bottom_right - prev_ring_bot_right;
            // std::cout << total_in_ring << "\n";


            int bottom_segment_end = bottom_right;
            int bottom_segment_start = bottom_right - seg_size;
            int bottom_segment_center = (bottom_segment_start + bottom_segment_end) / 2;
            if (N >= bottom_segment_start && N <= bottom_segment_end) // N is in here. How far from center?
            {
                reportSolution(ring + (std::abs(bottom_segment_center - N)));
                return;
            }

            int left_segment_end = bottom_segment_start;
            int left_segment_start = left_segment_end - seg_size;
            int left_segment_center = (left_segment_start + left_segment_end) / 2;
            if (N >= left_segment_start && N <= left_segment_end)
            {
                reportSolution(ring + (std::abs(left_segment_center - N)));
                return;
            }

            int top_segment_end = left_segment_start;
            int top_segment_start = top_segment_end - seg_size;
            int top_segment_center = (top_segment_start + top_segment_end) / 2;
            if (N >= top_segment_start && N <= top_segment_end)
            {
                reportSolution(ring + (std::abs(top_segment_center - N)));
                return;
            }

            int right_segment_end = top_segment_start;
            int right_segment_start = right_segment_end - (seg_size - 1); // annoying asymmetry. Could technically not do this, N could never be that value, but it feels wrong.
            int right_segment_center = (right_segment_start + right_segment_end) / 2; // will truncate the off-by-one.
            if (N >= right_segment_start && N <= right_segment_end)
            {
                reportSolution(ring + (std::abs(right_segment_center - N)));
                return;
            }

            throw std::invalid_argument("Invalid input");
        }

// for benchmarking vs visualising.
#define D3_ENABLE_PRINTER 0

        void v2() const override {
            // It really sounds better to just brute force this. In an actual 2d grid. With neighbours starting at 0.
            // Deal with the "out of bounds" cases by keeping an apron of 0. If we are about to write to this apron, it's time to resize.
            std::vector<std::vector<int>> grid;
            grid.resize(5); // Hardcoding a bigger number is just not as much fun, even though it is faster.
            std::ranges::for_each(grid, [&](auto& v) { v.resize(5); });

            auto printer = [&grid](const std::string& pre = "", const std::string& post = "")
            {
#if D3_ENABLE_PRINTER
                std::cout << pre;
                for (auto& v : grid)
                {
                    for (auto& i : v)
                    {
                        // if (i < 1'000'000'000) std::cout << ".";
                        // if (i < 100'000'000) std::cout << ".";
                        // if (i < 10'000'000) std::cout << ".";
                        if (i < 1'000'000) std::cout << ".";
                        if (i < 100'000) std::cout << ".";
                        if (i < 10'000) std::cout << ".";
                        if (i < 1'000) std::cout << ".";
                        if (i < 100) std::cout << ".";
                        if (i < 10) std::cout << ".";
                        std::cout << i;
                    }
                    std::cout << "\n";
                }
                std::cout << post;
#endif
            };


            reportSolution(fill_until_cap(N, grid, printer));
        }

        // this is the worst code I have written all year.
        static int fill_until_cap(const int N, auto& grid_to_use, const auto& printer)
        {
            const int center = static_cast<int>(grid_to_use.size() / 2);
            int step_start_x = center;
            int step_start_y = center;
            grid_to_use.at(center).at(center) = 1;

            static constexpr std::array<std::array<int,2>, 4> rotation_matrix = {{ {1, 0}, {0, -1}, {-1, 0}, {0, 1} }};
            auto get_rotation_index = [i = 0]() mutable
            {
                const int old_i = i;
                i = (i + 1) % 4;
                return rotation_matrix.at(old_i);
            };

            auto neighbour_sum = [&grid_to_use](int cx, int cy)
            {
                return
                    grid_to_use.at(cy-1).at(cx-1) +
                        grid_to_use.at(cy).at(cx-1) +
                            grid_to_use.at(cy+1).at(cx-1) +
                                grid_to_use.at(cy-1).at(cx) +
                                    grid_to_use.at(cy).at(cx) +
                                        grid_to_use.at(cy+1).at(cx) +
                                            grid_to_use.at(cy-1).at(cx+1) +
                                                grid_to_use.at(cy).at(cx+1) +
                                                    grid_to_use.at(cy+1).at(cx+1);
            };

            auto filler = [&](int sx, int sy, int step_size)
            {
                // check safety of end location.
                auto [dx, dy] = get_rotation_index();
                int ex = sx + (dx * step_size);
                int ey = sy + (dy * step_size);

                if (ex == 0 || ey == 0 || ex == grid_to_use.size()-1 || ey == grid_to_use.size()-1)
                {
                    // need to resize. Do a recursion with a bigger grid.
                    auto new_size = grid_to_use.size() * 2;
                    // yes, you COULD write the existing spiral to the right spot and save performance.
                    // The risk of doing this wrong is not worth it until the PO makes a ticket asking for performance :)
                    grid_to_use.clear();
                    grid_to_use.resize(new_size);
                    std::ranges::for_each(grid_to_use, [&](auto& v) { v.resize(new_size); });
                    return fill_until_cap(N, grid_to_use, printer);
                }

                int x = sx + dx;
                int y = sy + dy;
                int record = 0;
                for (int i = 0; i < step_size; i++)
                {
                    // get neighbour sum
                    int new_value = neighbour_sum(x, y);

                    record = std::max(record, new_value);
                    grid_to_use.at(y).at(x) = new_value;
                    x += dx;
                    y += dy;

                    if (new_value > N)
                    {
                        printer("FOUND AT " + std::to_string(x) + ", " + std::to_string(y) + "\n");
                        return new_value; // stop immediately if we found it
                    }
                }

                step_start_x = ex; // yup, it's stateful like that too. Cry about it.
                step_start_y = ey;

                return record;
            };

            int step_size = 1;
            while (true)
            {
                int produced = filler(step_start_x, step_start_y, step_size);
                if (produced > N) { return produced; }

                printer("S = " + std::to_string(step_size) + "\n", "\n");

                produced = filler(step_start_x, step_start_y, step_size);
                if (produced > N) { return produced; }

                printer("S = " + std::to_string(step_size) + "\n", "\n");

                ++step_size;
            }
        }

        void parseBenchReset() override {
            // none
        }

        private:
        int N = -1;
    };

} // namespace

#undef DAY