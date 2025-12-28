#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 25

NAMESPACE_DEF(DAY) {

struct Transition {
    bool write;
    bool left_move;
    char next_state;
};
    
struct State {
    char name;
    std::array<Transition, 2> transitions{};

    explicit State(char name) : name(name) {}
};

struct TuringMachine {
    std::vector<State> states;

    std::map<int, bool> tape;

    char current_state;
    int current_position = 0;
    int n_step = 0;

    const State& get_state(char S) {
        return states.at(S - 'A'); // Not very robust now, is it? This is some last puzzle of the year code for real.
    }

    void step() {
        auto& state = get_state(current_state);
        bool read = tape[current_position];
        auto& rules = state.transitions.at(read);

        tape[current_position] = rules.write;
        current_position = (rules.left_move) ? (current_position - 1) : (current_position + 1);
        current_state = rules.next_state;
        ++n_step;
    }

    void print_tape() const {
        std::ranges::for_each(tape, [](auto& kvp) {
            std::cout << kvp.first << ": " << kvp.second << "\n";
        });
    }
};

inline std::ostream& operator<<(std::ostream& os, const State& s) {
    os << "State {\n";
    os << "\tname: " << s.name << "\n\ttransitions: [\n";
    std::ranges::for_each(s.transitions, [&](auto& t) {
        os << "\t\t{" << " write: " << t.write << ", left_move: " << t.left_move << ", next: " << t.next_state << " }\n";
    });
    os << "\t]\n}";
    return os;
}
    
CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        constexpr std::string_view initial_state_yapping = "Begin in State ";
        constexpr std::string_view n_steps_input_yapping = "Perform a diagnostic checksum after ";

        constexpr std::string_view state_block_yapping =            "In state ";
        constexpr std::string_view state_block_value_yapping =      "  If the current value is ";
        constexpr std::string_view state_block_write_yapping =      "    - Write the value ";
        constexpr std::string_view state_block_tape_move_yapping =  "    - Move one slot to the ";
        constexpr std::string_view state_block_transition_yapping = "    - Continue with state ";

        auto skip = [](std::istringstream& strm, std::istringstream::off_type skip) { strm.seekg(strm.tellg() + skip); };

        char initial_state;
        int64_t n_steps;
        std::vector<State> states;
        {
            std::string initial_state_line;
            std::getline(input, initial_state_line);
            std::istringstream iss(initial_state_line);
            skip(iss, initial_state_yapping.size());

            initial_state = static_cast<char>(iss.get());
        }
        {
            std::string n_step_line;
            std::getline(input, n_step_line);
            std::istringstream iss(n_step_line);
            skip(iss, n_steps_input_yapping.size());

            iss >> n_steps;
        }

        std::string current_line;
        while (std::getline(input, current_line)) {
            if (current_line.empty()) {
                std::string state_line;
                // this section could be broken up to generify for number of symbols on the tape != 2, but that complicates parsing. I don't want to.
                int n_states = 2;
                int i = 0;
                std::string state;
                std::string state_write;
                std::string state_move;
                std::string state_transition;

                char state_name;
                bool state_write_v;
                bool left_move;
                char transition_state;

                {
                    std::getline(input, state_line);
                    std::istringstream iss(state_line);
                    skip(iss, state_block_yapping.size());
                    state_name = static_cast<char>(iss.get());
                }
                states.emplace_back(state_name);
fuck_it_goto_label: // Exceeded the for-loop quota for this year's puzzles, sorry.
                {
                    std::getline(input, state);
                }
                {
                    std::getline(input, state_write);
                    std::istringstream iss(state_write);
                    skip(iss, state_block_write_yapping.size());
                    iss >> state_write_v;
                }
                {
                    std::getline(input, state_move);
                    std::istringstream iss(state_move);
                    skip(iss, state_block_tape_move_yapping.size());
                    left_move = iss.get() == 'l';
                }
                {
                    std::getline(input, state_transition);
                    std::istringstream iss(state_transition);
                    skip(iss, state_block_transition_yapping.size());
                    transition_state = static_cast<char>(iss.get());
                }

                Transition& t = states.back().transitions.at(i);
                t.write = state_write_v;
                t.left_move = left_move;
                t.next_state = transition_state;

                ++i;
                if (i < n_states) {
                    goto fuck_it_goto_label;
                }

                // std::cout << "Finished making state: " << states.back() << "\n";
            } else throw std::logic_error("Expecting to be consuming state block lines only.");
        }

        TM.current_state = initial_state;
        TM.states = std::move(states);
        
        goal_steps = n_steps;
    }

    void v1() const override {
        auto mutable_copy = TM;

        while (mutable_copy.n_step < goal_steps) {
            mutable_copy.step();
            // std::cout << "State is now: " << mutable_copy.current_state << " w/ head: " << mutable_copy.current_position << " w/ tape:\n";
            // mutable_copy.print_tape();
        }

        int ones_written = 0;
        std::ranges::for_each(mutable_copy.tape, [&](auto& kvp) {
            ones_written += kvp.second;
        });
        
        reportSolution(ones_written);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {
        TM = {};
        goal_steps = 0;
    }

    private:
    TuringMachine TM{};
    int64_t goal_steps = 0;
};

} // namespace

#undef DAY