#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 24

NAMESPACE_DEF(DAY) {

struct Part {
    int A;
    int B;

    [[nodiscard]] int strength() const { return A + B; }
};
    
CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            std::istringstream iss(line);
            parts.emplace_back();
            iss >> parts.back().A;
            iss.clear();
            iss.get();
            iss >> parts.back().B;
        }

        // std::ranges::for_each(parts, [](auto& p) {
        //     std::cout << p.A << ", " << p.B << "\n";
        // });
        if (parts.size() > 64) {
            throw std::logic_error("The bitmask solution uses a 64 bit int, but more than 64 parts are given.");
        }
    }

    int get_strength_from_mask(uint64_t taken_bitmask) const {
        int score = 0;
        auto get_strength = [&](int i) {
            uint64_t mask = 1;
            mask <<= i;
            if (0 != (taken_bitmask & mask)) {
                return parts.at(i).strength();
            } else return 0;
        };

        for (int i = 0; i < parts.size(); ++i) {
            score += get_strength(i);
        }
        return score;
    }

    int score_all_bridges(uint64_t& taken_bitmask, int need_connector_type, const std::function<int(uint64_t)>& produced_bridge_scoring_callback) const {
        auto toggle = [&taken_bitmask](int index) {
            uint64_t mask = 1;
            mask <<= index;
            taken_bitmask ^= mask;
        };
        auto taken = [&taken_bitmask](int index) {
            uint64_t mask = 1;
            mask <<= index;
            return 0 != (taken_bitmask & mask);
        };

        int best_score_here = 0;
        
        for (int i = 0; i < parts.size(); ++i) {

            if (taken(i)) continue;
            
            auto& item = parts.at(i);

            std::pair<bool, int> potential_new_connector {false, -1};
            if (item.A == need_connector_type) {
                potential_new_connector.first = true;
                potential_new_connector.second = item.B;
            } else if (item.B == need_connector_type) { // else if because there is no point in trying a symmetric connector twice.
                potential_new_connector.first = true;
                potential_new_connector.second = item.A;
            }

            if (potential_new_connector.first) {
                toggle(i);
                int candidate_score = score_all_bridges(taken_bitmask, potential_new_connector.second, produced_bridge_scoring_callback);
                toggle(i);

                best_score_here = std::max(best_score_here, candidate_score);
            }
        }

        if (best_score_here == 0) { // 'base case' : could not extend the bridge beyond what we picked at this recursion depth. Convert the bitmask into a score.
            best_score_here = produced_bridge_scoring_callback(taken_bitmask);
        }

        return best_score_here;
    }

    void v1() const override {
        uint64_t taken_bitmask = 0;
        int connector = 0;
        auto callback = [this](uint64_t b) { return get_strength_from_mask(b); };
        
        int best = score_all_bridges(taken_bitmask, connector, callback);
        reportSolution(best);
    }

    void v2() const override {
        uint64_t taken_bitmask = 0;
        int connector = 0;
        
        int best_len = score_all_bridges(taken_bitmask, connector, [](uint64_t b) { return std::popcount(b); });

        taken_bitmask = 0; // should be redundant but let's be safe here.
        int strongest_max_len = score_all_bridges(taken_bitmask, connector, [best_len, this](uint64_t b) -> int {
            if (std::popcount(b) < best_len) return 0;

            return get_strength_from_mask(b);
        });
        
        reportSolution(strongest_max_len);
    }

    void parseBenchReset() override {
        parts.clear();
    }

    private:
    std::vector<Part> parts;
};

} // namespace

#undef DAY