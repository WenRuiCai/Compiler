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
    bool will_allocate_global_reg = false;
    global_reg reg;

    vector<string> link_nodes_name;

    void addlink(string link_node_name) {
        for (string s : this->link_nodes_name) {
            if (s == link_node_name)
                return;
        }
        this->link_nodes_name.push_back(link_node_name);
    }

    var_node(string varname) : name(varname) {}
};

class InterferenceGraph {
private:
    var_node& getNodeByName(string name) {
        for (var_node& node : this->var_nodes) {
            if (node.name == name) {
                return node;
            }
        }
    }

    bool hasNodeLink_LessThan_regNum() {
        for (var_node node : this->var_nodes) {
            if (node.link_nodes_name.size() < 8)
                return true;
        }
        return false;
    }
public:
    vector<var_node> var_nodes;

    InterferenceGraph(set<string> var_cross) {
        for (string s : var_cross) {
            this->var_nodes.push_back(var_node(s));
        }
    }

    void add_edge(string var1, string var2) {
        int a = 0;    int b = 0;
        for (int i = 0; i < this->var_nodes.size(); i++) {
            if (var_nodes[i].name == var1) {
                a = i;
            } else if (var_nodes[i].name == var2) {
                b = i;
            }
        }
        this->var_nodes[a].addlink(this->var_nodes[b].name);
        this->var_nodes[b].addlink(this->var_nodes[a].name);
    }

    void allocate_reg() {
        if (this->var_nodes.size() < 8) {
            int i = 0;
            for (var_node& node : var_nodes) {
                node.reg = global_reg (i++);
            }
            return;
        }
        /**
         * @brief: 使用图着色算法~
         * @item: removed_node 移除点的集合
         * @item: copy_VarNodes 复制一份副本，便于后面恢复边
         */
            ///@details: 构造移除点的集合，判断是否分配全局寄存器
        vector<var_node> removed_node;
        while (true) {
            if (this->var_nodes.size() == 1) {
                break;
            }
            while (hasNodeLink_LessThan_regNum() && this->var_nodes.size() > 1) {
                for (int i = 0; i < this->var_nodes.size() && this->var_nodes.size() > 1; i++) {
                    var_node& node = this->var_nodes[i];
                    if (node.link_nodes_name.size() < 8) {
                        for (string node1_name : node.link_nodes_name) {
                            var_node& node1 = getNodeByName(node1_name);
                            for (int j = 0; j < node1.link_nodes_name.size(); j++) {
                                if (node1.link_nodes_name[j] == node.name) {
                                    auto iter1 = node1.link_nodes_name.begin(); int num1 = j;
                                    while (num1--) iter1++; node1.link_nodes_name.erase(iter1);
                                    break;
                                }
                            }
                        }
                        node.will_allocate_global_reg = true;
                        removed_node.push_back(node);
                        auto iter = this->var_nodes.begin(); int number = i;
                        while (number--) iter++;
                        this->var_nodes.erase(iter);
                        i--;
                    }
                }
            }
            if (this->var_nodes.size() == 1) {
                break;
            }
            var_node& node2 = this->var_nodes[0];
            for (string node3_name : node2.link_nodes_name) {
                var_node& node3 = getNodeByName(node3_name);
                for (int i = 0; i < node3.link_nodes_name.size(); i++) {
                    if (node3.link_nodes_name[i] == node2.name) {
                        auto iter3 = node3.link_nodes_name.begin(); int number1 = i;
                        while (number1--) iter3++;  node3.link_nodes_name.erase(iter3);
                        break;
                    }
                }
            }
            node2.will_allocate_global_reg = false;
            removed_node.push_back(node2);
            this->var_nodes.erase(this->var_nodes.begin());
        }
            ///@details: 分配寄存器，染色
            ///@bug: 此部分待检查，可能存在bug
        this->var_nodes[0].reg = s0;
        for (int i = removed_node.size() - 1; i >= 0; i--) {    //反向将节点和边添加
            if (removed_node[i].will_allocate_global_reg) {
                int arr[8] = {0};
                for (string node_name : removed_node[i].link_nodes_name) {
                    var_node& node = getNodeByName(node_name);
                    if (node.will_allocate_global_reg) arr[node.reg] = 1;
                }
                for (int j = 0; j < 8; j++) {
                    if (arr[j] == 0) {
                        removed_node[i].reg = (global_reg) j;
                        break;
                    }
                }
            }
            this->var_nodes.insert(this->var_nodes.begin(), removed_node[i]);
        }
        return;
    }
};


#endif //COMPILER_INTERFERENCEGRAPH_H
