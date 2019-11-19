//
// Created by 84443 on 2019/11/8.
//

#ifndef COMPILER_MIDDLECODE_H
#define COMPILER_MIDDLECODE_H
int label_counter = 0;
string getVarAddr(string var, int* flag);
extern bool isConst(string name, int* num);

bool isNum(string s) {
    for (int i = 0; i < s.length(); i++) {
        if (!(s[i] >= '0' && s[i] <= '9')) {
            return false;
        }
    }
    return true;
}

bool isCharCon(string s, int* num) {
    if (s.length() == 3 && s[0] == '\'' && s[2] == '\'') {
        (*num) = (int)s[1];
        return true;
    }
    return false;
}

string translateConditionCentence(string midCode) {
    stringstream ss;
    ss << midCode;
    string result = "";
    string tmp;
    int counter = 0;
    for (int i = 0; i < midCode.length(); i++) {
        if (midCode.at(i) == '\n') {
            counter++;
        }
    }
    for (int i = 0; i < counter - 2; i++) {
        getline(ss, tmp);
        result += tmp + '\n';
    }
    ////////////////////////////////////////////////将判断以及跳转语句翻译
    getline(ss, tmp);

    stringstream ss1;
    ss1 << tmp;
    string left;    ss1 >> left;
    string op;  ss1 >> op;
    string right; ss1 >> right;

    if (isNum(left)) {
        result += "li $t9, " + left + '\n';
    } else {
        int num = 0;
        if (isCharCon(left, &num)) {
            result += "li $t8, " + to_string(num) + '\n';
        }
        else if (isConst(left, &num)) {
            result += "li $t9, " + to_string(num) + '\n';
        } else {
            string varAddr = getVarAddr(left, &num);
            if (varAddr[0] == '$') {
                result += "move $t9, " + varAddr + '\n';
            } else {
                if (num) result += "lw $t9, " + varAddr + "($0)\n";
                else result += "lb $t9, " + varAddr + "($0)\n";
            }
        }
    }
    if (isNum(right)) {
        result += "li $t8, " + right + '\n';
    } else {
        int num = 0;
        if (isCharCon(right, &num)) {
            result += "li $t8, " + to_string(num) + '\n';
        }
        else if (isConst(right, &num)) {
            result += "li $t8, " + to_string(num) + '\n';
        } else {
            string varAddr = getVarAddr(right, &num);
            if (varAddr[0] == '$') {
                result += "move $t8, " + varAddr + '\n';
            } else {
                if (num) result += "lw $t8, " + varAddr + "($0)\n";
                else result += "lb $t8, " + varAddr + "($0)\n";
            }
        }
    }

    getline(ss, tmp);
    stringstream ss2;
    ss2 << tmp;
    string jumpIns; ss2 >> jumpIns;
    string label; ss2 >> label;

    if (op == "==") result += "beq $t8, $t9, " + label + '\n';
    else if (op == "!=") result += "bne $t8, $t9, " + label + '\n';
    else if (op == ">") { // t9 > t8
        result += "slt $t8, $t8, $t9\n";
        result += "bne $t8, $0, " + label + '\n';
    }
    else if (op == "<=") { // t9 <= t8
        result += "slt $t8, $t8, $t9\n";
        result += "beq $t8, $0, " + label + '\n';
    }
    else if (op == ">=") { // t9 >= t8
        result += "slt $t8, $t9, $t8\n";
        result += "beq $t8, $0, " + label + '\n';
    }
    else if (op == "<") { //t9 < t8
        result += "slt $t8, $t9, $t8\n";
        result += "bne $t8, $0, " + label + '\n';
    }
    return result;

}

string translateExp(string exp_midCode) {
    stringstream ss;
    ss << exp_midCode;
    string tmp;
    while (getline(ss, tmp)) {
        stringstream ss1;
        ss1 << tmp;
        string left, right_op_1, right_op_2, op;
        ss1 >> left; ss1 >> tmp; ss1 >> right_op_1; ss1 >> op; ss1 >> right_op_2;

    }
}

#endif //COMPILER_MIDDLECODE_H
