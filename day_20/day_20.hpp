#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 20

NAMESPACE_DEF(DAY) {

struct XYZ
{
    int x;
    int y;
    int z;
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
        auto apply_minus = [&minus_flag, &iss](const int v) { return minus_flag ? -v : v; };
        
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

        return { x, y, z };
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

    static double double_integral_factor(const int v)
    {
        // first integral of A to get V is just... the factor. c -> cx
        // second integral is 0.5c because cx^2' = 2cx.
        return static_cast<double>(v) * 0.5;
    }

    static double manhattan_dist_at_t(const PVA& pva, double t = 1'000'000'000)
    {
        // formula for a points coordinates = ∫∫a + (∫v + a_factor) + p_0. Refer to 'insanity_ramblings.txt' of this folder.
        double x_at_t = t * t * double_integral_factor(pva.a.x) + t * (pva.v.x + double_integral_factor(pva.a.x)) + pva.p.x;
        double y_at_t = t * t * double_integral_factor(pva.a.y) + t * (pva.v.y + double_integral_factor(pva.a.y)) + pva.p.y;
        double z_at_t = t * t * double_integral_factor(pva.a.z) + t * (pva.v.z + double_integral_factor(pva.a.z)) + pva.p.z;

        // std::cout << " xt = " << x_at_t << " yt = " << y_at_t << " zt = " << z_at_t << "\n";

        return std::abs(x_at_t) + std::abs(y_at_t) + std::abs(z_at_t);
    }

    void v1() const override {
        int closest_i = 0;
        double closest = manhattan_dist_at_t(points.at(0));
        
        for (int i = 1; i < points.size(); ++i)
        {
            std::cout << i << ": ";
            auto dist = manhattan_dist_at_t(points.at(i));
            if (dist < closest)
            {
                closest_i = i;
                closest = dist;
            }
        }
        reportSolution(closest_i);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {
        points.clear();
    }

    private:
    std::vector<PVA> points;
};

} // namespace

#undef DAY