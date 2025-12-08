#pragma once

#include <iostream>
#include <ranges>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 8

NAMESPACE_DEF(DAY) {

enum class Opcode
{
    INC,
    DEC,
};

struct Instruction
{
    std::string target;
    Opcode operation;
    int operation_value;
    std::string condition_register;
    std::string condition_comparator;
    int condition_value;

    void execute(std::map<std::string, int>& registers) const
    {
        static const std::map<Opcode, std::function<int(int, int)>> OPERATIONS {
            { Opcode::INC, std::plus{} },
            { Opcode::DEC, std::minus{} }
        };

        if (evaluate_condition(registers))
        {
            auto functor = OPERATIONS.find(operation)->second;
            int result = functor(registers[target], operation_value);
            registers[target] = result;
            // std::cout << "Wrote " << result << " to " << target << "\n";
        }
    }

    bool evaluate_condition(std::map<std::string, int>& registers) const
    {
        static const std::map<std::string, std::function<bool(int, int)>> COMPARATORS {
                { "==", std::equal_to{} },
                { "!=", std::not_equal_to{} },
                { ">=", std::greater_equal{} },
                { ">", std::greater{} },
                { "<=", std::less_equal{} },
                { "<", std::less{} }
        };

        auto iter = COMPARATORS.find(condition_comparator);
        if (iter == COMPARATORS.end()) throw std::logic_error("Unknown operator: " + condition_comparator);

        return iter->second(registers[condition_register], condition_value); // operator[] default-inserts zero and that is OK.
    };
};

inline std::ostream& operator<<(std::ostream& os, const Instruction& i)
{
    os << i.target << " " << (i.operation == Opcode::DEC ? "dec" : "inc") << " " << i.operation_value << " (if) " << i.condition_register << " "  << i.condition_comparator << " " << i.condition_value;
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line))
        {
            std::string the_register;
            std::string opcode;
            std::string value;
            std::string if_statement_ignored;
            std::string conditional_register;
            std::string comparator;
            std::string compared_value;

            std::istringstream iss(line);
            iss >> the_register;
            iss >> opcode;
            iss >> value;
            iss >> if_statement_ignored;
            iss >> conditional_register;
            iss >> comparator;
            iss >> compared_value;

            Opcode the_opcode;
            if (opcode == "dec")
            {
                the_opcode = Opcode::DEC;
            } else if (opcode == "inc")
            {
                the_opcode = Opcode::INC;
            } else throw std::logic_error("Unknown opcde: " + opcode);

            int the_value = std::stoi(value);
            int the_compared_value = std::stoi(compared_value);

            instructions.emplace_back(the_register, the_opcode, the_value, conditional_register, comparator, the_compared_value);
        }

        // std::ranges::for_each(instructions, [](auto& i) { std::cout << i << "\n"; });
    }

    void v1() const override {
        std::map<std::string, int> registers;
        std::ranges::for_each(instructions, [&registers](const Instruction& instruction)
        {
            instruction.execute(registers);
        });

        reportSolution(std::ranges::max(registers | std::views::values));
    }

    void v2() const override {
        int highest_ever_value = std::numeric_limits<int>::min();
        std::map<std::string, int> registers;
        std::ranges::for_each(instructions, [&registers, &highest_ever_value](const Instruction& instruction)
        {
            instruction.execute(registers);
            auto new_register_value = registers[instruction.target];
            highest_ever_value = std::max(highest_ever_value, new_register_value);
        });

        reportSolution(highest_ever_value);
    }

    void parseBenchReset() override {
        instructions.clear();
    }

    private:
    std::vector<Instruction> instructions;
};

} // namespace

#undef DAY