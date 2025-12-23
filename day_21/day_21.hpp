#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 21

NAMESPACE_DEF(DAY) {

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    static int grid_to_bitmask(const int row_size, const std::vector<bool>& grid) {
        const int n_bits = row_size * row_size;
        if (n_bits != grid.size()) throw std::logic_error("Expected square grid");
        // assumed grid layout: left to right, top to bottom
        // intended int mapping: most significant to least significant.
        int32_t out = 0;
        for (int i = 0; i < n_bits; ++i) {
            if (! grid.at(i)) continue;

            const int shift = (n_bits - i) - 1;
            out |= (1 << shift);
        }

        return out;
    }

    static void insert_every_grid_permutation(const int row_size, const int mapping_to, const std::vector<bool>& blueprint, std::map<int,int>& output) {
        using Grid_t = std::vector<bool>;
        std::function<Grid_t(unsigned int)> rotate_bp = [&blueprint, row_size](unsigned int n) -> Grid_t {

            Grid_t out(blueprint.size(), false);
            for (int i = 0; i < blueprint.size(); ++i) {
                const int x = (i % row_size);
                const int y = (i / row_size);

                // map to coordinate of the rotation. I don't want to look up matrix multiplication or deal with it so... hardcode :)
                // rx(x, 0) -> x; rx(x, 1) -> y_max - y; rx(x, 2) -> x_max - x; rx(x, 3) -> y; rx(x, n) -> rx(x, n%4)
                const std::array<int, 4> x_map = {{ x, (row_size-1)-y, (row_size-1)-x, y }};
                // ry(y, 0) -> y; ry(y, 1) -> x; ry(0, 2) -> y_max - y; ry(0, 3) -> x_max - x; ry(y, n) -> ry(y, n%4)
                const std::array<int, 4> y_map = {{ y, x, (row_size-1)-y, (row_size-1)-x }};

                const int nx = x_map.at(n%4);
                const int ny = y_map.at(n%4);

                out.at(ny * row_size + nx) = blueprint.at(y * row_size + x);
            }

            return out;
        };

        std::function<Grid_t(bool,bool, const Grid_t&)> flip_grid = [row_size](bool h_flip, bool v_flip, const Grid_t& in) -> Grid_t {
            Grid_t out(in.size(), false);

            // apply flipping by reading the axis in reverse or not.
            for (int i = 0; i < in.size(); ++i) {
                const int x = (i % row_size);
                const int y = (i / row_size);

                const int nx = (h_flip ? ((row_size-1)-x) : x);
                const int ny = (v_flip ? ((row_size-1)-y) : y);

                out.at(ny * row_size + nx) = in.at(y * row_size + x);
            }

            return out;
        };
        
        std::array<const Grid_t, 4> grids = {{ rotate_bp(0), rotate_bp(1), rotate_bp(2), rotate_bp(3) }};
        // lots of redundancy in here, up to 16 times the same value. But thanks to std::map, this takes care of itself :)
        for (const auto& rotation : grids) {
            int a = grid_to_bitmask(row_size, flip_grid(false, false, rotation));
            int b = grid_to_bitmask(row_size, flip_grid(false, true, rotation));
            int c = grid_to_bitmask(row_size, flip_grid(true, false, rotation));
            int d = grid_to_bitmask(row_size, flip_grid(true, true, rotation));

            //std::cout << a << ", " << b << ", " << c << ", " << d << " -> " << mapping_to << "\n";
            output.emplace(a, mapping_to);
            output.emplace(b, mapping_to);
            output.emplace(c, mapping_to);
            output.emplace(d, mapping_to);
        }

        // auto print = [row_size](const Grid_t& o) {
        //     for (int i = 0; i < o.size(); ++i) {
        //         std::cout << (o.at(i) ? '#' : '.');
        //         if (i%row_size == row_size - 1) std::cout << "\n";
        //     }
        // };
    }

    static void get_grid(const int row_size, std::istringstream& scanner, std::vector<bool>& grid) {
        while (scanner.peek() != ' ' && scanner.peek() != EOF) {
            for (int n = 0; n < row_size; ++n) {
                grid.emplace_back(scanner.get() != '.');
            }
            
            if (scanner.peek() == '/') {
                scanner.get();
            }
        }

        if (grid.size() != row_size * row_size) {
            throw std::logic_error("Expected to generate a square grid.");
        }
    }

    static int get_row_size(std::istringstream& scanner) {
        int row_size = 0;
        while (scanner.peek() != '/') {
            ++row_size;
            scanner.get();
        }

        return row_size;
    }

    // Given one line of input,
    // 1. deduce if it is a 2by2 or 3by3 rule
    // 2. Extract the grid blueprint input and output.
    // 3. insert all rotations and flips into the mapping.
    void extract_rule(const std::string& rule)
    {
        std::vector<bool> input_grid;
        std::vector<bool> output_grid;
        int input_row_size = 0;
        int output_row_size = 0;
        { // left side row size
            std::istringstream scanner(rule);
            input_row_size = get_row_size(scanner);
        }
        { // left side grid
            std::istringstream left_side_scanner(rule);
            get_grid(input_row_size, left_side_scanner, input_grid);
        }
        { // right side size & grid.
            const int n_skip_input = ((input_row_size + 1) * input_row_size - 1);
            const int n_skip_center = static_cast<int>(std::string(" => ").size());
            
            std::istringstream right_side_scanner(rule);
            right_side_scanner.seekg(n_skip_input + n_skip_center);

            output_row_size = get_row_size(right_side_scanner);
            right_side_scanner.seekg(n_skip_input + n_skip_center);

            get_grid(output_row_size, right_side_scanner, output_grid);
        }

        int out_value = grid_to_bitmask(output_row_size, output_grid);

        switch (input_row_size) {
            case 2:
                insert_every_grid_permutation(2, out_value, input_grid, two_by_two_rules);
                break;
            case 3:
                insert_every_grid_permutation(3, out_value, input_grid, three_by_three_rules);
                break;
            default: throw std::logic_error("Unexpected size for rule mapping");
        }
    } 

    void parse(std::ifstream &input) override {
        std::string rule;
        while (std::getline(input, rule))
        {
            extract_rule(rule); // inserts into a std::map every permutation of the input grid to the output grid, as bitmasks.
        }
    }

    void do_growth_step(const std::vector<bool>& in, std::vector<bool>& out) const {
        const auto in_row_size = static_cast<size_t>(std::sqrt(in.size()));
        const auto out_row_size = static_cast<size_t>(std::sqrt(out.size()));
        
        // subdivide.
        size_t in_subdivision_size;
        size_t out_subdivision_size;
        if (in.size() % 2 == 0) {
            in_subdivision_size = 2;
            out_subdivision_size = 3;
        } else {
            in_subdivision_size = 3;
            out_subdivision_size = 4;
        }
        const std::map<int,int>& rules = in_subdivision_size == 2 ? two_by_two_rules : three_by_three_rules;
        
        const size_t in_subdivision_rows = in_row_size / in_subdivision_size;
        const size_t in_subdivision_blocks = in_subdivision_rows * in_subdivision_rows;

        const size_t x_in_block_offset = in_subdivision_size;
        const size_t y_in_block_offset = in_subdivision_size * in_row_size;

        const size_t out_subdivision_rows = out_row_size / out_subdivision_size;
        // const size_t out_subdivision_blocks = out_subdivision_rows * out_subdivision_rows;

        const size_t x_out_block_offset = out_subdivision_size;
        const size_t y_out_block_offset = out_subdivision_size * out_row_size;
        
        for (size_t i = 0; i < in_subdivision_blocks; ++i) {
            const size_t x_in_block = i % in_subdivision_rows;
            const size_t y_in_block = i / in_subdivision_rows;

            const size_t x_in_offset = x_in_block * x_in_block_offset;
            const size_t y_in_offset = y_in_block * y_in_block_offset;
            const size_t in_block_start_offset = x_in_offset + y_in_offset;

            const size_t x_out_block = i % out_subdivision_rows;
            const size_t y_out_block = i / out_subdivision_rows;

            const size_t x_out_offset = x_out_block * x_out_block_offset;
            const size_t y_out_offset = y_out_block * y_out_block_offset;
            const size_t out_block_start_offset = x_out_offset + y_out_offset;
            
            int block_bitmask_value = 0;
            for (size_t j = 0; j < in_subdivision_size * in_subdivision_size; ++j) {
                const size_t x_in = j % in_subdivision_size;
                const size_t y_in = j / in_subdivision_size;
                const size_t shift_amount = ((in_subdivision_size * in_subdivision_size) - j) - 1;

                const size_t index = in_block_start_offset + (y_in * in_row_size) + x_in;
                if (in.at(index)) {
                    block_bitmask_value |= (1 << shift_amount);
                }
            }

            // Got the value, now project it to the output!
            const int mapped_value = rules.at(block_bitmask_value);
            for (size_t j = 0; j < out_subdivision_size * out_subdivision_size; ++j) {
                const size_t x_out = j % out_subdivision_size;
                const size_t y_out = j / out_subdivision_size;
                const size_t shift_amount = ((out_subdivision_size * out_subdivision_size) - j) - 1;

                const size_t index = out_block_start_offset + (y_out * out_row_size) + x_out;
                out.at(index) = 0 != (mapped_value & (1 << shift_amount));
            }
        }
    }

    static size_t size_of_next(const size_t size_of_input_vector) {
        const auto row = static_cast<size_t>(std::sqrt(size_of_input_vector));
        size_t row_size;
        if (row % 2 == 0) {
            row_size = (row / 2) * 3;
        } else {
            row_size = (row / 3) * 4;
        }
            
        return row_size * row_size;
    }
    
    void v1() const override {
        std::vector<bool> current = start_pattern;

        for (int i = 0; i < 5; ++i) {
            std::vector<bool> next(size_of_next(current.size()), false);
            do_growth_step(current, next);
            
            current = std::move(next);
        }

        size_t on_count = 0;
        for (auto v : current) {
            on_count += v;
        }
        
        reportSolution(on_count);
    }

    void v2() const override {
        std::vector<bool> current = start_pattern;

        for (int i = 0; i < 18; ++i) {
            std::vector<bool> next(size_of_next(current.size()), false);
            do_growth_step(current, next);
            
            current = std::move(next);
        }

        size_t on_count = 0;
        for (auto v : current) {
            on_count += v;
        }
        
        reportSolution(on_count);
    }

    void parseBenchReset() override {
        two_by_two_rules.clear();
        three_by_three_rules.clear();
    }

    private:
    std::map<int,int> two_by_two_rules;
    std::map<int,int> three_by_three_rules;
    const std::vector<bool> start_pattern = { false, true, false,   false, false, true,   true, true, true }; // From the puzzle description:  .#.   ..#   ###
};

} // namespace

#undef DAY