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
    string result = "";
    string tmp;
    while (getline(ss, tmp)) {
        stringstream ss1;
        ss1 << tmp;
        string left, assignSymbol, right_op_1, right_op_2, op;
        ss1 >> left; ss1 >> assignSymbol; ss1 >> right_op_1; ss1 >> op; ss1 >> right_op_2;
        if ((left == "push" || left == "call") && assignSymbol != "=") {
            result += tmp + '\n';
            continue;
        }
        else {
            int hasTranslated = 0;
            for (int i = 0; i < tmp.length(); i++)
                if (tmp.at(i) == '$') {
                    hasTranslated = 1; break;
                }
            if (hasTranslated == 1) {result += tmp + '\n'; continue;}
        }
        int num = 0, num1 = 0, num2 = 0;
        string leftAddr = getVarAddr(left, &num);
        if (op.length() == 0) {
            //纯赋值语句
            if (isNum(right_op_1)) {
                if (leftAddr[0] == '$') {result += "li " + leftAddr + ", " + right_op_1 + '\n';}
                else {
                    result += "li $t7, " + right_op_1 + '\n';
                    if (num == 1) result += "sw $t7, " + leftAddr + "($0)\n";
                    else result += "sb $t7, " + leftAddr + "($0)\n";
                }
            } else if (isCharCon(right_op_1, &num1)) {
                if (leftAddr[0] == '$') {result += "li " + leftAddr + ", " + to_string(num1) + '\n';}
                else {
                    result += "li $t7, " + to_string(num1) + '\n';
                    if (num == 1) result += "sw $t7, " + leftAddr + "($0)\n";
                    else result += "sb $t7, " + leftAddr + "($0)\n";
                }
            } else if (isConst(right_op_1, &num1)) {
                if (leftAddr[0] == '$') {result += "li " + leftAddr + ", " + to_string(num1) + '\n';}
                else {
                    result += "li $t7, " + to_string(num1) + '\n';
                    if (num == 1) result += "sw $t7, " + leftAddr + "($0)\n";
                    else result += "sb $t7, " + leftAddr + "($0)\n";
                }
            } else {
                string rightAddr = getVarAddr(right_op_1, &num1);
                if (rightAddr[0] == '$') {
                    if (leftAddr[0] == '$') result += "move " + leftAddr + ", " + rightAddr + '\n';
                    else {
                        if (num == 1) result += "sw " + rightAddr + ", " + leftAddr + "($0)\n";
                        else result += "sb " + rightAddr + ", " + leftAddr + "($0)\n";
                    }
                } else {
                    if (num1 == 1) result += "lw $t6, " + rightAddr + "($0)\n";
                    else result += "lb $t6, " + rightAddr + "($0)\n";
                    if (leftAddr[0] == '$') result += "move " + leftAddr + ", $t6\n";
                    else {
                        if (num == 1) result += "sw $t6, " + leftAddr + "($0)\n";
                        else result += "sb $t6, " + leftAddr + "($0)\n";
                    }
                }
            }
        } else {
            string op1_addr;
            string op2_addr;
            string reg_op1 = "$t7", reg_op2 = "$t6";
            if (isNum(right_op_1)) {
                result += "li $t7, " + right_op_1 + '\n';
            } else if (isCharCon(right_op_1, &num1)) {
                result += "li $t7, " + to_string(num1) + '\n';
            } else if (isConst(right_op_1, &num1)) {
                result += "li $t7, " + to_string(num1) + '\n';
            } else {
                op1_addr = getVarAddr(right_op_1, &num1);
                if (op1_addr[0] == '$') { reg_op1 = op1_addr; }
                else {
                    if (num1 == 1) result += "lw $t7, " + op1_addr + "($0)\n";
                    else result += "lb $t7, " + op1_addr + "($0)\n";
                }
            }

            if (isNum(right_op_2)) {
                result += "li $t6, " + right_op_2 + '\n';
            } else if (isCharCon(right_op_2, &num2)) {
                result += "li $t6, " + to_string(num2) + '\n';
            } else if (isConst(right_op_2, &num2)) {
                result += "li $t6, " + to_string(num2) + '\n';
            } else {
                op2_addr = getVarAddr(right_op_2, &num2);
                if (op2_addr[0] == '$') reg_op2 = op2_addr;
                else {
                    if (num2 == 1) result += "lw $t6, " + op2_addr + "($0)\n";
                    else result += "lb $t6, " + op2_addr + "($0)\n";
                }
            }
            if (op == "+") {
                if (leftAddr[0] == '$') {
                    result += "add " + leftAddr + ", " + reg_op1 + ", " + reg_op2 + "\n";
                } else {
                    result += "add $t6, " + reg_op1 + ", " + reg_op2 + "\n";
                    result += "sw $t6, " + leftAddr + "($0)\n";
                }
            } else if (op == "-") {
                if (leftAddr[0] == '$') {
                    result += "sub " + leftAddr + ", " + reg_op1 + ", " + reg_op2 + '\n';
                } else {
                    result += "sub $t6, " + reg_op1 + ", " + reg_op2 + "\n";
                    result += "sw $t6, " + leftAddr + "($0)\n";
                }
            } else if (op == "*") {
                result += "mult " + reg_op1 + ", " + reg_op2 + '\n';
                if (leftAddr[0] == '$') result += "mflo " + leftAddr + '\n';
                else {
                    result += "mflo $t6\n";
                    result += "sw $t6, " + leftAddr + "($0)\n";
                }
            } else if (op == "/") {
                result += "div " + reg_op1 + ", " + reg_op2 + '\n';
                if (leftAddr[0] == '$') result += "mflo " + leftAddr + '\n';
                else {
                    result += "mflo $t6\n";
                    result += "sw $t6, " + leftAddr + "($0)\n";
                }
            }
        }
    }
    return result;
}

#endif //COMPILER_MIDDLECODE_H
