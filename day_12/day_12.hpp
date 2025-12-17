#pragma once

#include <iostream>
#include <queue>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 12

NAMESPACE_DEF(DAY) {

struct Node
{
    int id = -1;
    std::vector<const Node*> cons;
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        std::map<int, std::vector<int>> connections;
        while (std::getline(input, line))
        {
            std::istringstream iss(line);
            int node_id;
            iss >> node_id; // technically redundant, input is ordered for the left side of the <->.
            iss.get();
            iss.get();
            iss.get();
            iss.get();
            iss.get(); // ' <-> '

            std::vector<int> connections_of_node;
            int digit = 0;
            int read;
            while ((read = iss.get()) != EOF)
            {
                if (read == ',')
                {
                    iss.get(); // space
                    connections_of_node.emplace_back(digit);
                    digit = 0;
                } else
                {
                    digit = digit * 10 + (read - '0');
                }
            }
            // after loop exit by EOF.
            connections_of_node.emplace_back(digit);

            connections.emplace(node_id, std::move(connections_of_node));
        }

        for (const auto& k : connections | std::views::keys)
        {
            graph.emplace(k, Node{ .id = k});
        }
        for (const auto& [k, v] : connections)
        {
            for (auto& con : v)
            {
                auto it = graph.find(con);
                if (it == graph.end()) throw std::logic_error("Node referenced on right-hand side of input (" + std::to_string(it->first) + ") while not present on left-hand side.");
                graph[k].cons.emplace_back(& it->second);
            }
        }

        // std::ranges::for_each(graph, [&](auto& kvp)
        // {
        //     std::cout << kvp.first << " <-> " << " {";
        //     std::ranges::for_each(kvp.second.cons, [&](auto* con) { std::cout << " " << con->id; });
        //     std::cout << " } " << "\n";
        // });
    }

    static int count_network_members(const Node& s, std::set<const Node*>& seen_set_out)
    {
        std::queue<const Node*> work;
        seen_set_out.clear();

        work.emplace(&s);
        while (! work.empty())
        {
            const Node* unit = work.front();
            work.pop();

            for (const auto& k : unit->cons)
            {
                if (! seen_set_out.contains(k))
                {
                    seen_set_out.insert(k);
                    work.emplace(k);
                }
            }
        }

        return static_cast<int>(seen_set_out.size());
    }

    void v1() const override {
        // use DFS or BFS to find the size of the network that has node '0' in it.
        std::set<const Node*> _; // used for P2 only.
        reportSolution(count_network_members(graph.at(0), _));
    }

    void v2() const override {
        std::set<int> in_group;

        int groups = 0;
        for (auto& v : graph | std::views::values)
        {
            if (! in_group.contains(v.id))
            {
                std::set<const Node*> group_members;
                ++groups;
                count_network_members(v, group_members);

                std::ranges::for_each(group_members, [&in_group](auto* k){ in_group.emplace(k->id); });
            }
        }
        reportSolution(groups);
    }

    void parseBenchReset() override {
        graph.clear();
    }

    private:
    std::map<int, Node> graph;
};

} // namespace

#undef DAY