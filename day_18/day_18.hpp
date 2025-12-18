#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 18

NAMESPACE_DEF(DAY) {

using Register_t = std::map<char, int64_t>;

struct Operand
{
    virtual ~Operand() = default;
    [[nodiscard]] virtual int64_t get_value(const Register_t& registers) const = 0;
    [[nodiscard]] virtual char get_register_name() const { throw std::logic_error("This operand type does not have a register!"); }
};

struct NOP : Operand { [[nodiscard]] int64_t get_value(const Register_t&) const override { throw std::logic_error("Accessed NOP"); } };

struct DirectOperand : Operand
{
    explicit DirectOperand(int _v) : v(_v) {}

    int v;
    [[nodiscard]] int64_t get_value(const Register_t& registers) const override
    {
        return v;
    }
};

struct IndirectOperand : Operand
{
    explicit IndirectOperand(char _c) : c(_c) {}

    char c;
    [[nodiscard]] int64_t get_value(const Register_t& registers) const override
    {
        auto iter = registers.find(c);
        if (iter == registers.end()) throw std::logic_error("Operand has unknown register.");

        return iter->second;
    }

    [[nodiscard]] char get_register_name() const override { return c; }
};

struct Device;

struct Instruction
{
    std::array<std::unique_ptr<Operand>, 2> operands;
    Instruction() : operands({ std::make_unique<NOP>(), std::make_unique<NOP>() }) {}

    virtual ~Instruction() = default;
    virtual void Execute(Device& d) const = 0;
};

struct Device
{
    explicit Device(const std::vector<std::unique_ptr<const Instruction>>& p) : program(p)
    {
        for (char c = 'a'; c < 'z'; ++c)
        {
            registers[c] = 0;
        }
    }

    Register_t registers;
    const std::vector<std::unique_ptr<const Instruction>>& program;
    int64_t instruction_pointer = 0;
    int64_t last_played_sound = -1;

    bool halt(const std::function<bool()>& termination_cond = []{ return false; }) const
    {
        return instruction_pointer < 0 || instruction_pointer >= program.size() || termination_cond();
    }

    void execute()
    {
        program.at(instruction_pointer)->Execute(*this);
        ++instruction_pointer;
    }
};

struct SoundInstruction : Instruction
{
    explicit SoundInstruction(std::unique_ptr<Operand>&& op)
    {
        operands.at(0) = std::move(op);
    }

    void Execute(Device& d) const override
    {
        std::cout << "Play sound with " << operands.at(0)->get_register_name() << " (" << operands.at(0)->get_value(d.registers) << ")" << std::endl;
        d.last_played_sound = operands.at(0)->get_value(d.registers);
    }
};

struct SetInstruction : Instruction
{
    SetInstruction(std::unique_ptr<Operand>&& opA, std::unique_ptr<Operand>&& opB)
    {
        operands.at(0) = std::move(opA);
        operands.at(1) = std::move(opB);
    }

    void Execute(Device& d) const override
    {
        std::cout << "Set value with " << operands.at(0)->get_register_name() << " (" << operands.at(0)->get_value(d.registers) << ") - (" << operands.at(1)->get_value(d.registers) << ") " << std::endl;
        d.registers.at(operands.at(0)->get_register_name()) = operands.at(1)->get_value(d.registers);
    }
};

struct AddInstruction : Instruction
{
    AddInstruction(std::unique_ptr<Operand>&& opA, std::unique_ptr<Operand>&& opB)
    {
        operands.at(0) = std::move(opA);
        operands.at(1) = std::move(opB);
    }

    void Execute(Device& d) const override
    {
        std::cout << "Add value with " << operands.at(0)->get_register_name() << " (" << operands.at(0)->get_value(d.registers) << ") - (" << operands.at(1)->get_value(d.registers) << ") " << std::endl;
        auto& out = d.registers.at(operands.at(0)->get_register_name());
        out = operands.at(0)->get_value(d.registers) + operands.at(1)->get_value(d.registers);
    }
};

struct MultiplyInstruction : Instruction
{
    MultiplyInstruction(std::unique_ptr<Operand>&& opA, std::unique_ptr<Operand>&& opB)
    {
        operands.at(0) = std::move(opA);
        operands.at(1) = std::move(opB);
    }

    void Execute(Device& d) const override
    {
        std::cout << "Mult value with " << operands.at(0)->get_register_name() << " (" << operands.at(0)->get_value(d.registers) << ") - (" << operands.at(1)->get_value(d.registers) << ") " << std::endl;
        auto& out = d.registers.at(operands.at(0)->get_register_name());
        out = operands.at(0)->get_value(d.registers) * operands.at(1)->get_value(d.registers);
    }
};

struct ModuloInstruction : Instruction
{
    ModuloInstruction(std::unique_ptr<Operand>&& opA, std::unique_ptr<Operand>&& opB)
    {
        operands.at(0) = std::move(opA);
        operands.at(1) = std::move(opB);
    }

    void Execute(Device& d) const override
    {
        std::cout << "Mod value with " << operands.at(0)->get_register_name() << " (" << operands.at(0)->get_value(d.registers) << ") - (" << operands.at(1)->get_value(d.registers) << ") " << std::endl;
        auto& out = d.registers.at(operands.at(0)->get_register_name());
        out = operands.at(0)->get_value(d.registers) % operands.at(1)->get_value(d.registers);
    }
};

struct RecoverInstruction : Instruction
{
    explicit RecoverInstruction(std::unique_ptr<Operand>&& op)
    {
        operands.at(0) = std::move(op);
    }

    void Execute(Device& d) const override
    {
        if (0 != d.registers.at(operands.at(0)->get_register_name())) // assumes this is always a register. Observed in puzzle input to be true.
        {
            std::cout << "RCV: " << d.last_played_sound << "\n";
        } else
        {
            std::cout << "zero RCV\n";
        }
    }
};

struct JumpInstruction : Instruction
{
    JumpInstruction(std::unique_ptr<Operand>&& opA, std::unique_ptr<Operand>&& opB)
    {
        operands.at(0) = std::move(opA);
        operands.at(1) = std::move(opB);
    }

    void Execute(Device& d) const override
    {
        const int64_t XValue = operands.at(0)->get_value(d.registers);
        const int64_t YValue = operands.at(1)->get_value(d.registers);

        std::cout << "Jump instruction with " << XValue << " " << YValue << "\n";

        if (XValue > 0)
        {
            d.instruction_pointer += YValue;
            d.instruction_pointer -= 1; // offset for the increment of the fetch-decode-execute cycle. Similar to real computers and jump instructions, actually.
        }
    }
};

constexpr uint64_t operator ""_concat(const char* s, std::size_t n)
{
    if (n >= 8) throw std::logic_error("Operand overflow");

    int bit = 0;
    uint64_t res = 0;
    while (bit < n)
    {
        const char v = *(s + bit);
        res |= static_cast<uint64_t>(v) << (bit * 8);
        ++bit;
    }

    return res;
}

class OperandFactory
{
public:
    static std::unique_ptr<Operand> build(const std::string& op)
    {
        if (op.at(0) >= 'a' && op.at(0) <= 'z')
        {
            return std::make_unique<IndirectOperand>(op.at(0));
        } else
        {
            return std::make_unique<DirectOperand>(std::stoi(op));
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
            case "set"_concat:
                return std::make_unique<SetInstruction>(OperandFactory::build(op1), OperandFactory::build(op2));
            case "add"_concat:
                return std::make_unique<AddInstruction>(OperandFactory::build(op1), OperandFactory::build(op2));
            case "mul"_concat:
                return std::make_unique<MultiplyInstruction>(OperandFactory::build(op1), OperandFactory::build(op2));
            case "mod"_concat:
                return std::make_unique<ModuloInstruction>(OperandFactory::build(op1), OperandFactory::build(op2));
            case "rcv"_concat:
                return std::make_unique<RecoverInstruction>(OperandFactory::build(op1));
            case "jgz"_concat:
                return std::make_unique<JumpInstruction>(OperandFactory::build(op1), OperandFactory::build(op2));
            case "snd"_concat:
                return std::make_unique<SoundInstruction>(OperandFactory::build(op1));
            default:
                throw std::logic_error("Unknown instruction conversion");
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

            std::cout << ins << ", " << opA << ", " << opB << "\n";
            program.emplace_back(InstructionFactory::build(ins, opA, opB));
        }
    }

    static std::function<bool()> problem_1_halt_condition(const Device& d)
    {
        return [&]() -> bool
        {
            auto& ins = d.program.at(d.instruction_pointer);
            bool is_rcv = dynamic_cast<const RecoverInstruction*>(ins.get()) != nullptr;
            bool is_nonzero = ins->operands.at(0)->get_value(d.registers) != 0;

            return is_rcv && is_nonzero;
        };
    }

    void v1() const override {
        Device d {program};
        const auto halter = problem_1_halt_condition(d);

        while (! d.halt(halter))
        {
            d.execute();
        }
        d.execute(); // execute the RCV itself as well, just because.

        reportSolution(d.last_played_sound);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {
        program.clear();
    }

    private:
    std::vector<std::unique_ptr<const Instruction>> program;
};

} // namespace

#undef DAY