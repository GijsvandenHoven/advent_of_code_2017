#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 20

NAMESPACE_DEF(DAY) {

struct XYZ
{
    double x;
    double y;
    double z;

    [[nodiscard]] double manhattan() const
    {
        return std::abs(x) + std::abs(y) + std::abs(z);
    }

    bool operator==(const XYZ& p2) const = default;
};

inline std::ostream& operator<<(std::ostream& os, const XYZ& xyz)
{
    os << "XYZ { x= " << xyz.x << " y= " << xyz.y << " z= " << xyz.z <<" }";
    return os;
}

struct PVA
{
    XYZ p;
    XYZ v;
    XYZ a;
};

inline std::ostream& operator<<(std::ostream& os, const PVA& pva)
{
    os << "PVA {\n\tp= " << pva.p << "\n\tv= " << pva.v << "\n\ta= " << pva.a <<"\n}";
    return os;
}
    
CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    // Stream is left pointing to the first character after the XYZ triplet. Assumes pointed-at the start of a triplet.
    static XYZ extract(std::istringstream& iss)
    {
        bool minus_flag = false;
        auto check_minus = [&minus_flag, &iss](){ minus_flag = iss.peek() == '-'; if (minus_flag) iss.get(); };
        auto apply_minus = [&minus_flag](const int v) { return minus_flag ? -v : v; };
        
        int x = 0;
        check_minus();
        while (iss.peek() != ',')
        {
            x = x * 10 + (iss.get() - '0');
        }
        x = apply_minus(x);
        
        iss.get();
        int y = 0;
        check_minus();
        while (iss.peek() != ',')
        {
            y = y * 10 + (iss.get() - '0');
        }
        y = apply_minus(y);
        
        iss.get();
        int z = 0;
        check_minus();
        while (iss.peek() != '>')
        {
            z = z * 10 + (iss.get() - '0');
        }
        z = apply_minus(z);

        return { static_cast<double>(x), static_cast<double>(y), static_cast<double>(z) };
    }

    void parse(std::ifstream &input) override {
        std::string formula;
        while (std::getline(input, formula))
        {
            std::istringstream iss(formula);
            iss.get(); iss.get(); iss.get();
            XYZ p = extract(iss);
            iss.get(); iss.get(); iss.get(); iss.get(); iss.get(); iss.get(); // >, v=<
            XYZ v = extract(iss);
            iss.get(); iss.get(); iss.get(); iss.get(); iss.get(); iss.get(); // >, a=<
            XYZ a = extract(iss);

            points.emplace_back(p, v, a);
        }

        // std::ranges::for_each(points, [&](auto& pva)
        // {
        //     std::cout << pva << "\n";
        // });
    }

    static double double_integral_factor(const double v)
    {
        // first integral of A to get V is just... the factor. c -> cx
        // second integral is 0.5c because cx^2' = 2cx.
        return v * 0.5;
    }

    static XYZ position_at_t(const PVA& pva, double t = 1'000'000)
    {
        // formula for a points coordinates = ∫∫a + (∫v + a_factor) + p_0. Refer to 'insanity_ramblings.txt' of this folder.
        double x_at_t = t * t * double_integral_factor(pva.a.x) + t * (pva.v.x + double_integral_factor(pva.a.x)) + pva.p.x;
        double y_at_t = t * t * double_integral_factor(pva.a.y) + t * (pva.v.y + double_integral_factor(pva.a.y)) + pva.p.y;
        double z_at_t = t * t * double_integral_factor(pva.a.z) + t * (pva.v.z + double_integral_factor(pva.a.z)) + pva.p.z;

        // std::cout << " xt = " << x_at_t << " yt = " << y_at_t << " zt = " << z_at_t << "\n";

        return { x_at_t, y_at_t, z_at_t };
    }

    void v1() const override {
        int closest_i = 0;
        double closest = position_at_t(points.at(0)).manhattan();
        
        for (int i = 1; i < points.size(); ++i)
        {
            auto dist = position_at_t(points.at(i)).manhattan();
            if (dist < closest)
            {
                closest_i = i;
                closest = dist;
            }
        }
        reportSolution(closest_i);
    }

    using CollisionMarker = std::pair<std::pair<int,int>,int>; // first: particle pairs, second: time.

    // Approach: solve the quadratic equation for just X, get the natural number times if they exist, fill it in and check if y and z are also matching. Then it is a collision.
    static std::vector<CollisionMarker> find_collision_times(const PVA& first, const PVA& second, std::pair<int,int> ids)
    {
        std::vector<double> natural_number_roots;
        auto root_qualifies = [](double r) -> bool { return r >= 0 && (static_cast<int64_t>(r) == r); }; // NOLINT(*-narrowing-conversions)
        
        // let's get the A, B, C values for these. First, the formula itself...
        const double A1 = double_integral_factor(first.a.x);
        const double B1 = first.v.x + double_integral_factor(first.a.x);
        const double C1 = first.p.x;

        const double A2 = double_integral_factor(second.a.x);
        const double B2 = second.v.x + double_integral_factor(second.a.x);
        const double C2 = second.p.x;

        const double A = A1 - A2;
        const double B = B1 - B2;
        const double C = C1 - C2;

        if (A != 0)
        {
            const double D = (B * B) - (4 * A * C);

            // std::cout << "D = " << D << " (A= " << A << " B= " << B << " C= " << C << ")\n";

            if (D < 0)
            {
                // std::cout << "()\n";
                return {}; // no roots... don't even need to check Y and Z.
            }

            const double root_1 = ((-B) + std::sqrt(D)) / (2*A);
            const double root_2 = ((-B) - std::sqrt(D)) / (2*A);

            // std::cout << "(" << root_1 << ") (" << root_2 << ")\n";

            if (root_qualifies(root_1)) { natural_number_roots.emplace_back(root_1); }
            if (root_qualifies(root_2)) { natural_number_roots.emplace_back(root_2); }
        } else
        {
            // bx + c = 0.
            // bx = -c
            // x = -c/b
            const double root = (-C)/B;
            // std::cout << "(" << root << ")\n";
            
            if (root_qualifies(root)) { natural_number_roots.emplace_back(root); }
        }

        // for the roots that qualified on X... do they qualify on all 3 axes?
        std::vector<CollisionMarker> true_connections;
        for (auto& t : natural_number_roots)
        {
            XYZ p1 = position_at_t(first, t);
            XYZ p2 = position_at_t(second, t);

            if (p1 == p2)
            {
                true_connections.emplace_back(ids, t);
            }
        }

        return true_connections;
    }

    void v2() const override {
        // for any pair of particles, check the equation for their position if they have integer solutions.
        // keep in mind that particles do not keep moving after a collision...

        // for any pairwise comparison, solve the equation (quadratic formula), and make a list for each particle:
        // collides with (p) at time (t).
        // Sort these events on (t), and then start eliminating:
        //   An elimination happens if and only if each of the particles is not destroyed yet at this time.
        //   Keep the multi-collision in mind! Do not eliminate until this (t) is solved. Multiple can crash at the same time!!

        std::vector<CollisionMarker> all_possible_collisions;
        // scuffed upper bound: number of pairs times two. The n_pairs formula division by 2 is cancelled out by the at most 2 solutions per equation pair.
        // Very casual way to just allocate 8M memory, moon lander software team would freak out once again.
        all_possible_collisions.reserve((points.size() * (points.size() - 1)));

        for (int i = 0; i < points.size(); ++i)
        {
            for (int j = i + 1; j < points.size(); ++j)
            {
                auto collisions = find_collision_times(points.at(i), points.at(j), std::make_pair(i,j));
                all_possible_collisions.insert(all_possible_collisions.end(), collisions.begin(), collisions.end());
            }
        }
        
        // sort on time.
        std::ranges::sort(all_possible_collisions, [](const CollisionMarker& cm1, const CollisionMarker& cm2)
        {
            return cm1.second < cm2.second;
        });
        
        std::vector<int> time_of_collide(points.size(), -1); // for index i, represents time particle i has collided.
        auto particle_exists_at_t = [&time_of_collide](int pid, int t) -> bool { return time_of_collide.at(pid) == -1 || time_of_collide.at(pid) >= t; };

        for (auto& [particle_pair, t] : all_possible_collisions)
        {
            const auto& [p1, p2] = particle_pair;

            std::cout << p1 << ", " << p2 << " Shall collide at: " << t << "\n";
            if (particle_exists_at_t(p1, t) && particle_exists_at_t(p2, t))
            {
                std::cout << "\tAnd it is a problem\n";
                time_of_collide.at(p1) = t;
                time_of_collide.at(p2) = t;
            } else
            {
                // the puzzle input has 0 of these, nice red herring :(
                std::cout << "\tBut not both exist at this point in time.\n";
            }
        }

        // anything that is "-1" after we have processed all possible collisions, must be left at the end...
        int answer = std::accumulate(time_of_collide.begin(), time_of_collide.end(),  0, [](int acc, auto& v)
        {
            return acc + (v == -1);
        });
        
        reportSolution(answer);
    }

    void parseBenchReset() override {
        points.clear();
    }

    private:
    std::vector<PVA> points;
};

} // namespace

#undef DAY