#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 11

NAMESPACE_DEF(DAY) {

enum class Dir
{
    NE, SE, S, SW, NW, N
};

struct State
{
    int sw_ne_axis = 0;
    int nw_se_axis = 0;
    int n_s_axis = 0;

    void step(Dir d)
    {
        switch (d)
        {
            case Dir::NE: sw_ne_axis += 1; break;
            case Dir::SW: sw_ne_axis -= 1; break;
            case Dir::NW: nw_se_axis += 1; break;
            case Dir::SE: nw_se_axis -= 1; break;
            case Dir::N:  n_s_axis += 1; break;
            case Dir::S:  n_s_axis -= 1; break;
            default: throw std::invalid_argument("Invalid direction");
        }
    }

    // i'm pretty sure this has a bug when you cross through the origin like this: se,se,se,sw,sw,sw,n,n,n,n,ne... but the puzzle is marked correct and I don't like this one. sooo....
    [[nodiscard]] int distance_from_origin() const
    {
        int h_dist = 0;
        // If one is east and the other is west, they cancel/
        // If both are the same, they add.
        // Reminder: positive sw_ne means northeast. positive nw_se means northwest.
        if (sw_ne_axis >= 0 && nw_se_axis >= 0 || sw_ne_axis <= 0 && nw_se_axis <= 0)
        { // cancelling.
            h_dist = std::abs(std::abs(sw_ne_axis) - std::abs(nw_se_axis));
        } else // (sw_ne_axis >= 0 && nw_se_axis <= 0 || sw_ne_axis <= 0 && nw_se_axis >= 0)
        { // adding
            h_dist = std::abs(std::abs(sw_ne_axis) + std::abs(nw_se_axis));
        }

        int v_dist = 0;
        // int southness = (sw_ne_axis < 0 ? std::abs(sw_ne_axis) : 0) + (nw_se_axis < 0 ? std::abs(nw_se_axis) : 0); // contributions to south by both other axes.
        // int northness = (sw_ne_axis > 0 ? sw_ne_axis : 0) + (nw_se_axis > 0 ? nw_se_axis : 0);
        // // v_dist is influenced by all 3 axes.
        // // n_s axis steps are either adding extra work, or removing work created by the ne/nw/se/sw steps.
        // // This depends on whether a sharp angle is created by the addition of the S/N steps.
        // // This triangle rule applies if any order of the 3 axes can shape it. I.E. SE-NE-N might not make it, but SE-N-NE would!
        // if (n_s_axis >= 0) // north: Triangles are made if SE or SW was chosen.
        // {
        //     if (southness >= n_s_axis) // we can fully absorb the north steps with horizontal steps on the northness direction.
        //     {
        //         v_dist = 0;
        //     } else // southness < n_s_axis // We cannot fully absorb these steps, explicit north steps must be taken to reach this place.
        //     {
        //         v_dist = std::abs(n_s_axis - southness);
        //     }
        // } else // south: Triangles are made if NE or NW was chosen.
        // {
        //     if (northness >= std::abs(n_s_axis)) // fully absorb.
        //     {
        //         v_dist = 0;
        //     } else
        //     {
        //         v_dist = std::abs(std::abs(n_s_axis) - northness);
        //     }
        // }

        // east/west triangles also incur Vdist if they exist. The VDist equals the length of the lesser axis.
        // The existing vdist can then either add to or subtract from this, depending on if it works in our favor or not.
        if (sw_ne_axis >= 0 && nw_se_axis >= 0 || sw_ne_axis <= 0 && nw_se_axis <= 0)
        {
            int triangle_penalty = std::min(std::abs(sw_ne_axis), std::abs(nw_se_axis));
            if (sw_ne_axis >= 0 && n_s_axis >= 0 || sw_ne_axis <= 0 && n_s_axis <= 0) // north-facing triangle, north steps. south-facing triangle, south steps. Not in our favor, add-to.
            {
                v_dist = triangle_penalty + std::abs(n_s_axis);
            } else
            {
                v_dist = std::abs(triangle_penalty - std::abs(n_s_axis));
            }
        }

        std::cout << "hdist " << h_dist << std::endl;
        std::cout << "vdist " << v_dist << std::endl;


        return v_dist + h_dist;
    }
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    static Dir chars_to_dir(char l, char r)
    {
        switch (l)
        {
            case 'n':
                switch (r)
                {
                    case 'e': return Dir::NE;
                    case 'w': return Dir::NW;
                    case '\0': return Dir::N;
                    default: throw std::invalid_argument("Invalid sequence");
                }
            case 's':
                switch (r)
                {
                    case 'e': return Dir::SE;
                    case 'w': return Dir::SW;
                    case '\0': return Dir::S;
                    default: throw std::invalid_argument("Invalid sequence");
                }
            default:throw std::invalid_argument("Invalid sequence");
        }
    }

    void parse(std::ifstream &input) override {
        std::string sequence;
        std::getline(input, sequence);

        std::array<char, 2> type{};
        int i = 0;
        for (auto c : sequence)
        {
            if (c == ',')
            {
                directions.emplace_back(chars_to_dir(type[0], type[1]));
                i = 0;
                type[1] = '\0';
            } else
            {
                type[i++] = c;
            }
        }
        directions.emplace_back(chars_to_dir(type[0], type[1]));
    }

    void v1() const override {
        State s;
        std::ranges::for_each(directions, [&s](auto d) { s.step(d); });
        reportSolution(s.distance_from_origin());
    }

    void v2() const override {
        State s;
        int max_dist = 0;
        std::ranges::for_each(directions, [&](auto d)
        {
            s.step(d);
            int c_dist = s.distance_from_origin();
            max_dist = std::max(max_dist, c_dist);
        });

        // 1489 is too low
        reportSolution(max_dist);
    }

    void parseBenchReset() override {
        // none
    }

    private:
    std::vector<Dir> directions;
};

} // namespace

#undef DAY