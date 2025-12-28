#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 23

NAMESPACE_DEF(DAY) {

using Operand = Day18::Operand;
using OperandFactory = Day18::OperandFactory;
using Device = Day18::Device; // includes some stuff that is redundant, but not bad enough to refactor it for use here. (e.g. the send queue, or the sound variable).
using Instruction = Day18::Instruction;
using SetInstruction = Day18::SetInstruction;
using MulInstruction = Day18::MultiplyInstruction;
using Day18::operator ""_concat;

struct SubtractInstruction : Instruction {
    SubtractInstruction(std::unique_ptr<Operand>&& opA, std::unique_ptr<Operand>&& opB)
    {
        operands.at(0) = std::move(opA);
        operands.at(1) = std::move(opB);
    }

    void Execute(Device& d) const override
    {
        // std::cout << "Sub value with " << operands.at(0)->get_register_name() << " (" << operands.at(0)->get_value(d.registers) << ") - (" << operands.at(1)->get_value(d.registers) << ") " << std::endl;
        auto& out = d.registers.at(operands.at(0)->get_register_name());
        out = operands.at(0)->get_value(d.registers) - operands.at(1)->get_value(d.registers);
    }
};

struct JumpNotZeroInstruction : Instruction {
    JumpNotZeroInstruction(std::unique_ptr<Operand>&& opA, std::unique_ptr<Operand>&& opB) {
        operands.at(0) = std::move(opA);
        operands.at(1) = std::move(opB);
    }

    void Execute(Device& d) const override {
        auto& op = operands.at(0);
        // Uh oh. Can't get the name unless it is an indirect operand.
        char register_name = (dynamic_cast<const Day18::IndirectOperand*>(op.get()) ? op->get_register_name() : '~');
        // std::cout << "JNZ with " << register_name << " (" << operands.at(0)->get_value(d.registers) << ") - (" << operands.at(1)->get_value(d.registers) << ") " << std::endl;

        if (op->get_value(d.registers) != 0) {
            int64_t offset = operands.at(1)->get_value((d.registers));
            d.instruction_pointer += offset;
            d.instruction_pointer -= 1;// Same reason as Day18::JumpInstruction.
        }
    }
};

class InstructionFactory
{
public:
    static std::unique_ptr<Instruction> build(const std::string& ins, const std::string& op1, const std::string& op2)
    {
        switch (operator""_concat(ins.c_str(), ins.size()))
        {
            case "sub"_concat:
                return std::make_unique<SubtractInstruction>(OperandFactory::build(op1), OperandFactory::build(op2));
            case "jnz"_concat:
                return std::make_unique<JumpNotZeroInstruction>(OperandFactory::build(op1), OperandFactory::build(op2));
            default:
                return Day18::InstructionFactory::build(ins, op1, op2);
        }
    }
};
    
CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line))
        {
            std::string ins;
            std::string opA;
            std::string opB;

            std::istringstream iss(line);
            iss >> ins >> opA >> opB;

            // std::cout << ins << ", " << opA << ", " << opB << "\n";
            program.emplace_back(InstructionFactory::build(ins, opA, opB));
        }
    }

    void v1() const override {
        Device d {program};

        int mul_count = 0;
        while (! d.halt())
        {
            auto& ins = d.program.at(d.instruction_pointer);
            bool is_mul = dynamic_cast<const MulInstruction*>(ins.get()) != nullptr;
            mul_count += is_mul;
            
            d.execute();
        }

        reportSolution(mul_count);
    }

    // P2 is a mostly human investigation of the input type problem, so there is not much code.
    // Refer to insanity_ramblings.txt of this folder to see the deductions.
    // Also included is some code commented out that is used to aid the deductions.
        
    // Device d {program};
    // d.registers.at('a') = 1;
        
    // std::ofstream debugger_out;
    // debugger_out.open("p2_out.txt");
    //
    // std::function debugger = [&d, &debugger_out] {
    //     auto ip = d.instruction_pointer;
    //     auto& ins = d.program.at(ip);
    //
    //     if (ip == 25) {
    //         debugger_out << "IP: 25, STATUS:\t";
    //         std::ranges::for_each(d.registers, [&](auto& kvp) {
    //             debugger_out << kvp.first << ": " << kvp.second << " | ";
    //         });
    //         debugger_out << "\n";
    //     }
    //     
    //     return false;
    // };
        
    // while (! d.halt(debugger))
    // {
    //     d.execute();
    // }

    void v2() const override {
        Device d {program};
        d.registers.at('a') = 1;

        // execute just the setup, so that the B and C constants are set up.
        while (! d.halt([&d](){ return d.instruction_pointer >= 8; })) {
            d.execute();
        }
        auto offset_of_b_register = d.program.at(d.program.size() - 2)->operands.at(1)->get_value(d.registers);

        // Mimic the ASM program: composite_number_count is the h register. We subtract from B instead of adding, because the program we mimic can only subtract (Which is why the extracted offset/'increment' is negative)
        // The program has 3 loops: do ... while 'b != c, b += <offset>', inside that it increments d from 2 to b, inside that e from 2 to b.
        // If it finds d * e == b, it increments h.
        // In other words, For 1000 numbers, n * k + b  (n = 1000, k = 17 in our puzzle input), Count the composite numbers. 
        int composite_number_count = 0;
        for (int64_t b = d.registers.at('b'); b <= d.registers.at('c'); b -= offset_of_b_register) {
            composite_number_count += ! is_prime(b);
        }
        
        reportSolution(composite_number_count);
    }

    static bool is_prime(const int64_t N) {
        if (N > 2 && N%2 == 0) return false;

        for (int64_t i = 3; i * i <= N; i += 2) {
            if (N % i == 0) return false;
        }

        return true;
    }

    void parseBenchReset() override {
        program.clear();
    }

    private:
    std::vector<std::unique_ptr<const Instruction>> program;
};

} // namespace

#undef DAY