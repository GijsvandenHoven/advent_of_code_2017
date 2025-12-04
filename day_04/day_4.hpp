#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 4

NAMESPACE_DEF(DAY) {

    CLASS_DEF(DAY) {
        public:
        DEFAULT_CTOR_DEF(DAY)

        void parse(std::ifstream &input) override {
            std::string line;
            while (std::getline(input, line))
            {
                passphrases.emplace_back(line);
            }
        }

        static bool do_anagram_check(const std::set<std::string>& words)
        {
            std::set<std::string> sorted_words;
            for (auto str: words)
            {
                std::ranges::sort(str);
                sorted_words.emplace(str);
            }

            return sorted_words.size() == words.size();
        }

        static bool is_passphrase_valid(const std::string& s, bool anagram_check = false)
        {
            std::set<std::string> words;
            std::string tmp;
            std::istringstream ss(s);
            while (ss.good())
            {
                ss >> tmp;
                auto [_, new_word] = words.emplace(tmp);

                if (!new_word) return false;
            }

            return anagram_check ? do_anagram_check(words) : true;
        }

        void v1() const override {
            auto r = std::accumulate(passphrases.begin(), passphrases.end(), 0, [](int a, const std::string& passphrase)
            {
                return a + is_passphrase_valid(passphrase);
            });
            reportSolution(r);
        }

        // 435 is too high
        void v2() const override {
            auto r = std::accumulate(passphrases.begin(), passphrases.end(), 0, [](int a, const std::string& passphrase)
            {
                return a + is_passphrase_valid(passphrase, true);
            });
            reportSolution(r);
        }

        void parseBenchReset() override {
            passphrases.clear();
        }

        private:
        std::vector<std::string> passphrases;
    };

} // namespace

#undef DAY