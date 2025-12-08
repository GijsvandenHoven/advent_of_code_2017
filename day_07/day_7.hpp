#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 7

NAMESPACE_DEF(DAY) {

struct Node
{
    std::string name;
    std::vector<Node*> children;
    int weight = 0;

    [[nodiscard]] bool has_child_with_name(const std::string& s) const
    {
        return std::ranges::any_of(children, [&s](auto& node) { return node->name == s; });
    }

    [[nodiscard]] int get_total_weight() const
    {
        int child_weight = std::accumulate(children.begin(), children.end(), 0, [](int v, auto& node)
        {
            return v + node->get_total_weight();
        });
        return child_weight + weight;
    }

    [[nodiscard]] bool are_children_balanced() const
    {
        if (children.empty()) return true;

        const int the_weight = (*children.begin())->get_total_weight();

        // no std::ranges, want to continue even if an imbalance is found.
        bool ok = true;
        std::ranges::for_each(children, [&ok, &the_weight](auto& node)
        {
            std::cout << "For node " << node->name << " The total weight is " << node->get_total_weight() << std::endl;
            ok &= the_weight == node->get_total_weight();
        });

        return ok;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Node& node)
{
    os << "Node {\n\t";
    os << "name: " << node.name << ", weight: " << node.weight << " children:\n\t\t";
    std::ranges::for_each(node.children, [&os](auto& sub_node) { os << sub_node->name << ", "; });
    os << "\n}";
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        std::map<std::string, std::vector<std::string>> post_process_step;
        while (std::getline(input, line))
        {
            std::istringstream iss(line);
            std::string name;
            iss >> name;
            std::string value;
            iss >> value;
            int weight = std::stoi(value.substr(1, value.length() - 2));

            Node new_node { name, {}, weight };

            std::vector<std::string> node_child_by_name;
            if (iss.peek() != EOF) // this has children
            {
                std::string child;
                iss >> child; // eat the arrow symbol.
                while (iss.good())
                {
                    iss >> child;
                    // hate dealing with that comma.
                    child = child.substr(0, ',' == *child.rbegin() ? child.length() - 1 : child.length());
                    node_child_by_name.emplace_back(child);
                }
            }

            graph.emplace(name, std::move(new_node));

            if (! node_child_by_name.empty())
            {
                post_process_step.emplace(name, std::move(node_child_by_name));
            }
        }

        for (auto& [name, child_names] : post_process_step)
        {
            auto iter = graph.find(name);
            if (iter != graph.end())
            {
                for (auto& child : child_names)
                {
                    auto c_iter = graph.find(child);
                    if (c_iter != graph.end())
                    {
                        auto* p = &(c_iter->second);
                        iter->second.children.push_back(p);
                    } else
                    {
                        throw std::logic_error("Unknown Child Node: " + name);
                    }
                }
            } else
            {
                throw std::logic_error("Unknown Child Node: " + name);
            }
        }

        std::ranges::for_each(graph, [](auto& kvp)
        {
            std::cout << kvp.second << "\n";
        });
    }

    std::string get_root_node() const
    {
        auto current_node_name = graph.begin()->first;
        bool exists_child_with_node;

        do
        {
            exists_child_with_node = false;
            for (const auto& [name, node] : graph)
            {
                if (node.has_child_with_name(current_node_name))
                {
                    exists_child_with_node = true;
                    current_node_name = node.name;
                    break;
                }
            }
        } while (exists_child_with_node);

        return current_node_name;
    }

    void v1() const override {
        reportSolution(get_root_node());
    }

    void v2() const override {
        // Worked out by hand by filling in manually in this line the node that is unbalanced program output.
        auto [root_name, root_node] = *graph.find("lahahn");
        root_node.are_children_balanced();

        // Workout:
        //
        // 82373 -> bad (zklwp)
        // 82364 -> balance
        //
        // 8612 -> bad (lahahn)
        // 8603 -> balance
        //
        // 1960 -> bad (utnrb)
        // 1951 -> balance
        //
        // utnrb is itself balanced.
        // utnrb is too heavy by 9.
        // Its weight should be 9 less
        // utnrb currently weighs 538
        // utnrb should weigh 529
        // Verified by modifying program input: utnrb weight to 529 makes it balanced.

        reportSolution(529);
    }

    void parseBenchReset() override {

    }

    private:
    std::map<std::string, Node> graph;
};

} // namespace

#undef DAY