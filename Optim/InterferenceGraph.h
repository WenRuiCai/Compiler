//
// Created by 84443 on 2019/12/13.
//

#ifndef COMPILER_INTERFERENCEGRAPH_H
#define COMPILER_INTERFERENCEGRAPH_H

enum global_reg {
    s0 = 0,
    s1,
    s2,
    s3,
    s4,
    s5,
    s6,
    s7
};

struct var_node {
    string name;

    global_reg reg;

    vector<var_node*> link_nodes;

    void addlink(var_node* varNode) {
        this->link_nodes.push_back(varNode);
    }

    var_node(string varname) : name(varname) {}
};

class InterferenceGraph {
public:
    vector<var_node> var_nodes;

    InterferenceGraph(set<string> var_cross) {
        for (string s : var_cross) {
            this->var_nodes.push_back(var_node(s));
        }
    }

    void add_edge(string var1, string var2) {
        var_node* a = nullptr;    var_node* b = nullptr;
        for (int i = 0; i < this->var_nodes.size(); i++) {
            if (var_nodes[i].name == var1) {
                a = &this->var_nodes[i];
            } else if (var_nodes[i].name == var2) {
                b = &this->var_nodes[i];
            }
        }
        a->addlink(b);
        b->addlink(a);
    }

    void allocate_reg() {
        if (this->var_nodes.size() < 8) {
            int i = 0;
            for (var_node node : var_nodes) {
                node.reg = global_reg (i++);
            }
            return;
        }
        /**
         * 使用图着色算法~
         */
        for (var_node& node : this->var_nodes) {
            if (node.link_nodes.size() < 8) {

            }
        }
    }
};


#endif //COMPILER_INTERFERENCEGRAPH_H
