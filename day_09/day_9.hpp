#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 9

NAMESPACE_DEF(DAY) {

using Sentinel = std::function<bool()>;

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::getline(input, stream);
    }

    static int proceed_iter_until_end_of_garbage(std::string::const_iterator& iter, const Sentinel& sentinel)
    {
        int garbage_char_count = 0;
        while (sentinel())
        {
            char c = *iter;
            ++iter;

            switch (c)
            {
                case '!': ++iter; break;
                case '>': return garbage_char_count;
                default: ++garbage_char_count; break;
            }
        }

        throw std::logic_error("End of stream while consuming garbage. Never found a matching '>'");
    }

    static int get_score(std::string::const_iterator& iter, const Sentinel& sentinel, int scoring_of_self, int& garbage_chars_count)
    {
        int value_of_contents = 0;
        while (sentinel())
        {
            char c = *iter;
            ++iter;

            switch (c)
            {
                case '{':
                    value_of_contents += get_score(iter, sentinel, scoring_of_self + 1, garbage_chars_count);
                    break;
                case '}':
                    return value_of_contents + scoring_of_self;
                case '<':
                    garbage_chars_count += proceed_iter_until_end_of_garbage(iter, sentinel);
                    break;
                default:
                    break;
            }
        }

        return value_of_contents + scoring_of_self;
    }

    void v1() const override {
        auto parser = stream.begin();
        auto done = [&]() -> bool { return parser < stream.end(); };

        int _; // annoying thingy to enable part 2 while supporting 2 separate concerns with one function...
        int score = get_score(parser, done, 0, _);

        reportSolution(score);
    }

    void v2() const override {
        auto parser = stream.begin();
        auto done = [&]() -> bool { return parser < stream.end(); };

        int garbage_chars = 0;
        get_score(parser, done, 0, garbage_chars);
        reportSolution(garbage_chars);
    }

    void parseBenchReset() override {
        stream.clear();
    }

    private:
    std::string stream;
};

} // namespace

#undef DAY