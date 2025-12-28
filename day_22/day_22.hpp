#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 22

NAMESPACE_DEF(DAY) {

enum class Facing {
    UP, DOWN, LEFT, RIGHT
};

enum class State {
    CLEAN = 0, WEAK, INFECTED, FLAGGED
};
    
using Position = std::pair<int,int>;
using Grid = std::map<Position, State>;
    
inline Position to_pos(int x, int y) { return std::make_pair(x,y); }
    
struct Carrier {
    Position p = to_pos(0,0);
    Facing f = Facing::UP;

    virtual ~Carrier() = default;

    void step(Grid& g) {
        auto& node_state = g[p];
        rotate(node_state);
        modify_node(node_state);
        move();
    }

    virtual void rotate(const State node_state) {
        switch (f) {
            case Facing::UP:
                f = (node_state == State::INFECTED) ? Facing::RIGHT : Facing::LEFT;
                break;
            case Facing::DOWN:
                f = (node_state == State::INFECTED) ? Facing::LEFT : Facing::RIGHT;
                break;
            case Facing::LEFT:
                f = (node_state == State::INFECTED) ? Facing::UP : Facing::DOWN;
                break;
            case Facing::RIGHT:
                f = (node_state == State::INFECTED) ? Facing::DOWN : Facing::UP;
                break;
            default: throw std::logic_error("Unknown facing.");
        }
    }

    virtual void modify_node(State& node_state_ref) {
        switch (node_state_ref) {
            case State::CLEAN: node_state_ref = State::INFECTED; break;
            case State::INFECTED: node_state_ref = State::CLEAN; break;
            default: throw std::logic_error("Unsupported state");
        }
    }

    void move() {
        switch (f) {
            case Facing::UP:
                p = to_pos(p.first, p.second - 1);
                break;
            case Facing::DOWN:
                p = to_pos(p.first, p.second + 1);
                break;
            case Facing::LEFT:
                p = to_pos(p.first - 1, p.second);
                break;
            case Facing::RIGHT:
                p = to_pos(p.first + 1, p.second);
                break;
            default: throw std::logic_error("Unknown facing.");
        }
    }
};

struct EvolvedCarrier : Carrier {
    void rotate(const State node_state) override {
        switch (node_state) {
            case State::CLEAN:
            case State::INFECTED:
                Carrier::rotate(node_state);
                break;
            case State::WEAK:
                break; // do not rotate, just keep moving.
            case State::FLAGGED:
                switch (f) {
                    case Facing::UP: f = Facing::DOWN; break;
                    case Facing::DOWN: f = Facing::UP; break;
                    case Facing::LEFT: f = Facing::RIGHT; break;
                    case Facing::RIGHT: f = Facing::LEFT; break;
                    default: throw std::logic_error("Unknown facing.");
                }
                break;
            default: throw std::logic_error("Unknown state.");
        }
    }

    void modify_node(State& node_state_ref) override {
        switch (node_state_ref) {
            case State::CLEAN: node_state_ref = State::WEAK; break;
            case State::WEAK: node_state_ref = State::INFECTED; break;
            case State::INFECTED: node_state_ref = State::FLAGGED; break;
            case State::FLAGGED: node_state_ref = State::CLEAN; break;
            default: throw std::logic_error("Unknown state");
        }
    }
};
    
CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(input, line)) {
            lines.emplace_back(std::move(line));
        }

        size_t grid_length = lines.back().size();
        size_t grid_height = lines.size();

        for (int y = 0; y < lines.size(); ++y) {
            for (int x = 0; x < lines.at(y).size(); ++x) {
                int ny = y - static_cast<int>(grid_height / 2);
                int nx = x - static_cast<int>(grid_length / 2);

                if (lines.at(y).at(x) == '#') {
                    node_status.emplace(to_pos(nx, ny), State::INFECTED);
                }
            }
        }
    }

    void v1() const override {
        auto mutable_copy = node_status;
        Carrier agent;

        int infections_caused = 0;
        for (int i = 0; i < 10000; ++i) {
            Position old = agent.p;
            agent.step(mutable_copy);
            infections_caused += (mutable_copy.at(old) == State::INFECTED);
        }
        
        reportSolution(infections_caused);
    }

    void v2() const override {
        auto mutable_copy = node_status;
        EvolvedCarrier agent;

        int infections_caused = 0;
        for (int i = 0; i < 10'000'000; ++i) {
            Position old = agent.p;
            agent.step(mutable_copy);
            infections_caused += (mutable_copy.at(old) == State::INFECTED);
        }
        
        reportSolution(infections_caused);
    }

    void parseBenchReset() override {
        node_status.clear();
    }

    private:
    Grid node_status;
};

} // namespace

#undef DAY