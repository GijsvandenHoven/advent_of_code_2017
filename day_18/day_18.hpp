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

    std::queue<int64_t> send_queue{};
    std::queue<int64_t> receive_queue{};

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

struct ReceiveInstruction : Instruction
{
    explicit ReceiveInstruction(std::unique_ptr<Operand>&& op)
    {
        operands.at(0) = std::move(op);
    }

    void Execute(Device& d) const override
    {
        if (d.receive_queue.empty()) throw std::logic_error("Receive on empty queue was called.");

        int64_t v = d.receive_queue.front();
        d.receive_queue.pop();

        std::cout << "Receive: " << operands.at(0)->get_register_name() << " (" << v << ") w/ queuesize: " << d.receive_queue.size() << "\n";
        auto& out = d.registers.at(operands.at(0)->get_register_name());
        out = v;
    }
};

struct SendInstruction : Instruction
{
    explicit SendInstruction(std::unique_ptr<Operand>&& op)
    {
        operands.at(0) = std::move(op);
    }

    void Execute(Device& d) const override
    {
        std::cout << "Send: " << operands.at(0)->get_value(d.registers) << "w/ queuesize: " << d.send_queue.size() << "\n";
        d.send_queue.push(operands.at(0)->get_value(d.registers));
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
    static std::unique_ptr<Instruction> build(const std::string& ins, const std::string& op1, const std::string& op2, bool p2 = false)
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
                if (p2)
                {
                    return std::make_unique<ReceiveInstruction>(OperandFactory::build(op1));
                } else
                {
                    return std::make_unique<RecoverInstruction>(OperandFactory::build(op1));
                }
            case "jgz"_concat:
                return std::make_unique<JumpInstruction>(OperandFactory::build(op1), OperandFactory::build(op2));
            case "snd"_concat:
                if (p2)
                {
                    return std::make_unique<SendInstruction>(OperandFactory::build(op1));
                } else
                {
                    return std::make_unique<SoundInstruction>(OperandFactory::build(op1));
                }
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
            p2_program.emplace_back(InstructionFactory::build(ins, opA, opB, true));
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

    static std::function<bool()> problem_2_halt_condition(const Device& d)
    {
        return [&]() -> bool
        { // halt when a device is pointed at a 'receive' instruction and its 'receive' queue is empty.
            auto& ins = d.program.at(d.instruction_pointer);
            bool is_rcv = dynamic_cast<const ReceiveInstruction*>(ins.get()) != nullptr;
            bool has_nothing = d.receive_queue.empty();

            return is_rcv && has_nothing;
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
        Device d1 {p2_program};
        Device d2 {p2_program};
        d2.registers.at('p') = 1;

        auto d1_halter = problem_2_halt_condition(d1);
        auto d2_halter = problem_2_halt_condition(d2);

        size_t values_sent_by_d2 = 0;
        while (! (d1.halt(d1_halter) && d2.halt(d2_halter)))
        {
            while (! d1.halt(d1_halter))
            {
                d1.execute();
            }
            d2.receive_queue = std::move(d1.send_queue);
            while (! d2.halt(d2_halter))
            {
                d2.execute();
            }
            values_sent_by_d2 += d2.send_queue.size();
            d1.receive_queue = std::move(d2.send_queue);
        }

        reportSolution(values_sent_by_d2);
    }

    void parseBenchReset() override {
        program.clear();
        p2_program.clear();
    }

    private:
    std::vector<std::unique_ptr<const Instruction>> program;
    std::vector<std::unique_ptr<const Instruction>> p2_program;
};

} // namespace

#undef DAY