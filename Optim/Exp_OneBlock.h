//
// Created by 84443 on 2019/12/9.
//

#ifndef COMPILER_EXP_ONEBLOCK_H
#define COMPILER_EXP_ONEBLOCK_H
/*
 * 在我的设计中，生成中间代码和mips代码都是以语句为单位进行，递归式地翻译
 * 但是在中间代码优化过程中，由于需要构建流图，所以必然是在整个中间代码基础上优化
 * 为了保持现有的架构，需要进行标记：   建立 中间代码位置 <-> 语句 的映射
 * 优化完毕后进行回填
 */

string getExpInLine(int line, string exp) {
    int cntLine = 0;
    string result = "";
    if (line == 1) {
        stringstream ss;
        ss << exp;
        getline(ss, result);
        return result;
    }
    for (int i = 0; i < exp.length(); i++) {
        if (exp.at(i) == '\n') {
            cntLine++;
            if (cntLine == line) {
                for (int j = i - 1; exp.at(j) != '\n'; j--) {
                    result = exp.at(j) + result;
                }
                break;
            }
        }
    }
    return result;
}

int getLineNum(string exp) {
    int num = 0;
    for (int i = 0; i < exp.length(); i++) {
        if (exp.at(i) == '\n') {
            num++;
        }
    }
    return num;
}

class Exp_DAG_Optim {
public:
    map<string, char> var2Id;
    vector<string> var_GoingToBe_Used;
    char exp_oneBlock_TransChar = 'a';

    int cnt = 0;

    char ans[110][10];
    bool flag[110];

    struct Node {
        char id;
        int left = -1, right = -1;
        vector<char> var;
    } node[110];

    bool find_var(int i, char c) {
        for (char j : node[i].var)
            if (j == c) return true;
        return false;
    }

    int add_node(char c) {
        for (int i = cnt - 1; i >= 0; --i)
            if (node[i].id == c || find_var(i, c))return i;
        node[cnt].id = c;
        return cnt++;
    }

    void add_operator(char c, char op, int l, int r) {
        for (int i = cnt - 1; i >= 0; --i)
            if (node[i].left == l && node[i].right == r && node[i].id == op) {
                node[i].var.push_back(c);
                return;
            }
        node[cnt].id = op;
        node[cnt].var.push_back(c);
        node[cnt].left = l;
        node[cnt].right = r;
        cnt++;
    }

    void dfs(int x) {
        if (node[x].left != -1) {
            flag[x] = 1;
            dfs(node[x].left);
            dfs(node[x].right);
        }
    }

    void build(string exp) {
        int l = add_node(exp[2]);
        int r = add_node(exp[4]);
        add_operator(exp[0], exp[3], l, r);
    }

    void DAGOptim() {
        for (int i = 0; i < cnt; ++i) {
            if (node[i].left != -1) {
                ans[i][0] = node[i].var[0];
                ans[i][1] = '=';
                Node ll = node[node[i].left], rr = node[node[i].right];
                ans[i][2] = ll.var.size() > 0 ? ll.var[0] : ll.id;
                ans[i][3] = node[i].id;
                ans[i][4] = rr.var.size() > 0 ? rr.var[0] : rr.id;
                ans[i][5] = 0;
            }
        }
    }

    string getExp_Optim() {
        for (string varName : this->var_GoingToBe_Used) {
            for (int i = cnt - 1; i >= 0; --i) {
                if (ans[i][0] == var2Id.at(varName)) {
                    dfs(i);
                    break;
                }
            }
        }
        string result = "";
        for (int i = 0; i < cnt; ++i) {
            if (flag[i]) {
                string line = "";
                for (int j = 0; ans[i][j] != '\0'; j++) {
                    line += ans[i][j];
                }
                for (map<string, char>::iterator iter = var2Id.begin(); iter != var2Id.end(); iter++) {
                    if ((*iter).second == line.at(0)) {result += (*iter).first + " = "; break;}
                }
                for (map<string, char>::iterator iter = var2Id.begin(); iter != var2Id.end(); iter++) {
                    if ((*iter).second == line.at(2)) {result += (*iter).first; break;}
                }
                if (line.length() == 5) {
                    if (line.at(3) != '[') {
                        result +=  " ";
                        result += line.at(3);
                        result += " ";
                        for (map<string, char>::iterator iter = var2Id.begin(); iter != var2Id.end(); iter++) {
                            if ((*iter).second == line.at(4)) {result += (*iter).first; break;}
                        }
                    } else {
                        result += '[';
                        for (map<string, char>::iterator iter = var2Id.begin(); iter != var2Id.end(); iter++) {
                            if ((*iter).second == line.at(4)) {result += (*iter).first; break;}
                        }
                        result += ']';
                    }
                }
                result += '\n';
            }
        }
        return result;
    }

    void preDeal(string exp, vector<string> vars) {
        for (int i = 0; i < 110; i++) {
            flag[i] = 0;
            //for (int j = 0; j < 10; j++) ans[i][j] = '\0';
        }

        this->var_GoingToBe_Used = vars;
        for (string varName : var_GoingToBe_Used) {
            var2Id.insert(make_pair(varName, exp_oneBlock_TransChar));
            exp_oneBlock_TransChar = (exp_oneBlock_TransChar == 'z') ? 'A' : (exp_oneBlock_TransChar + 1);
        }
        for (int i = 1; i <= getLineNum(exp); i++) {
            stringstream line;
            string transLine = "";
            line << getExpInLine(i, exp);
            string left, assign, op1, op, op2;
            line >> left >> assign >> op1 >> op >> op2;
            if (op1.find('[') != string::npos) {
                string arr_id = "", index = ""; int arr_index_place = 0;
                for (int x = 0; x < op1.length(); x++) {
                    if (op1.at(x) != '[') {
                        arr_id += op1.at(x);
                    } else {
                        arr_index_place = x + 1;
                        break;
                    }
                }
                for (; op1.at(arr_index_place) != ']'; arr_index_place++) {
                    index += op1.at(arr_index_place);
                }
                op1 = arr_id;   op = "["; op2 = index;
            }
            if (var2Id.count(left) != 0) transLine += var2Id.at(left);
            else {
                transLine += exp_oneBlock_TransChar;
                var2Id.insert(make_pair(left, exp_oneBlock_TransChar));
                exp_oneBlock_TransChar = (exp_oneBlock_TransChar == 'z') ? 'A' : (exp_oneBlock_TransChar + 1);
            }
            transLine += "=";
            if (var2Id.count(op1) != 0) transLine += var2Id.at(op1);
            else {
                transLine += exp_oneBlock_TransChar;
                var2Id.insert(make_pair(op1, exp_oneBlock_TransChar));
                exp_oneBlock_TransChar = (exp_oneBlock_TransChar == 'z') ? 'A' : (exp_oneBlock_TransChar + 1);
            }
            if (op.length() != 0) {
                transLine += op;
                if (var2Id.count(op2) != 0) transLine += var2Id.at(op2);
                else {
                    transLine += exp_oneBlock_TransChar;
                    var2Id.insert(make_pair(op2, exp_oneBlock_TransChar));
                    exp_oneBlock_TransChar = (exp_oneBlock_TransChar == 'z') ? 'A' : (exp_oneBlock_TransChar + 1);
                }
            }
            //cout << transLine << endl;
            build(transLine);
        }
        //cout << "********************\n";
        DAGOptim();
        //cout << getExp_Optim();
    }
};
#endif //COMPILER_EXP_ONEBLOCK_H
