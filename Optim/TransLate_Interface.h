//
// Created by 84443 on 2019/12/15.
//

#ifndef COMPILER_TRANSLATE_INTERFACE_H
#define COMPILER_TRANSLATE_INTERFACE_H

#include "TransLate_To_Mips_Functions.h"

string new_TranslateAssign(string midCode, vector<tmp_Variable>& temps, bool isFunctionReturn) {
    string result = "";
    stringstream ss;
    string left, assign, op1, op, op2;
    ss << midCode;
    ss >> left >> assign >> op1 >> op >> op2;
    string reg1, reg2;
    string action1 = "";
    string action2 = "";
    if (isFunctionReturn) {
        reg1 = "$v1";
    }
    else if (op.length() == 0 && op1.find("[") != string::npos) {
        string index = op1.substr(op1.find('[') + 1, op1.find(']') - op1.find('[') - 1);
        string array = op1.substr(0, op1.find('['));    int num = 0; int num1 = 0;
        string array_addr = get_array_begin_addr(array, &num);
        if (isNum(index)) {
            result += "li $t8, " + index + '\n';
        }
        else if (isConst(index, &num1, nullptr)) {
            result += (num == 0) ? "lb $t8, " : "lw $t8, ";
            result += to_string(num1 + atoi(array_addr.c_str())) + "($0)\n";
        } else {
            reg2 = get_VarReg(temps, index, action2, 2);
            result += action2;
            result += "li $t8, ";
            result += (num == 0) ? "1\n" : "4\n";
            result += "mult $t8, " + reg2 + '\n' + "mflo $t8\n";
            result += "add $t9, $t8, " + array_addr + '\n';
            result += (num == 0) ? "lb " : "lw ";
            result += "$t8, 0($t9)\n";
        }
        reg1 = "$t8";
    } else {
        int num = 0;
        reg1 = "$t8";
        if (isNum(op1)) {
            result += "li $t8, " + op1 + '\n';
        } else if (isCharCon(op1, &num)) {
            result += "li $t8, " + to_string(num) + '\n';
        } else if (isConst(op1, &num, nullptr)) {
            result += "li $t8, " + to_string(num) + '\n';
        } else {
            reg1 = get_VarReg(temps, op1, action1, 1);
            result += action1;
        }
        if (op.length() > 0) {
            reg2 = "$t9";
            if (isNum(op2)) {
                result += "li $t9, " + op2 + '\n';
            } else if (isCharCon(op2, &num)) {
                result += "li $t9, " + to_string(num) + '\n';
            } else if (isConst(op2, &num, nullptr)) {
                result += "li $t9, " + to_string(num) + '\n';
            } else {
                reg2 = get_VarReg(temps, op2, action2, 2);
                result += action2;
            }
        }
    }

    /////等式左边赋值的
    string action3 = "";
    string reg3 = "";
    if (left.find('[') != string::npos) {
        string left_array_item_addr = "";
        string index = left.substr(left.find('[') + 1, left.find(']') - left.find('[') - 1);
        string array = left.substr(0, left.find('['));    int num = 0; int num1 = 0;
        string array_addr = get_array_begin_addr(array, &num);

        if (isNum(index)) {
            result += "li $t8, " + index + '\n';
        } else if (isConst(index, &num1, nullptr)) {
            left_array_item_addr = to_string(num1 + atoi(array_addr.c_str()));
            result += "li $v0, " + left_array_item_addr + "($0)\n";
        } else {
            reg3 = get_VarReg(temps, index, action3, 3);
            result += action3;
            result += "li $v0, ";
            result += (num == 0) ? "1\n" : "4\n";
            result += "mult $v0, " + reg3 + '\n' + "mflo $v0\n";
            result += "add $v0, $v0, " + array_addr + '\n';
        }
        if (op.length() == 0) result += (num == 1 ? "sw " : "sb ") + reg1 + ", 0($v0)\n";
        else {
            if (op == "+") result += "add $t0, " + reg1 + ", " + reg2 + '\n';
            else if (op == "-") result += "sub $t0, " + reg1 + ", " + reg2 + '\n';
            else if (op == "*") result += "mult " + reg1 + ", " + reg2 + '\n' + "mflo $t0\n";
            else if (op == "/") result += "div " + reg1 + ", " + reg2 + '\n' + "mflo $t0\n";
            result += (num == 1 ? "sw " : "sb ");
            result += "$t0, 0($v0)\n";
        }
    } else {
        if (op.length() == 0) {
            result += "move $t0, " + reg1 + '\n';
        }
        if (op == "+") result += "add $t0, " + reg1 + ", " + reg2 + '\n';
        else if (op == "-") result += "sub $t0, " + reg1 + ", " + reg2 + '\n';
        else if (op == "*") result += "mult " + reg1 + ", " + reg2 + '\n' + "mflo $t0\n";
        else if (op == "/") result += "div " + reg1 + ", " + reg2 + '\n' + "mflo $t0\n";

        string addr1 = "";   int num = 0;
        string s = getAssignLeft(temps, left, addr1, &num);

        if (num == 0 || num == 6) {
            result += "move " + s + ", " + "$t0\n";
        }
        else if (num == 1 || num == 4 || num == 9) {
            result += "sw $t0, " + s + "($0)\n";
        } else if (num == 2 || num == 7) {
            result += "move " + s + ", " + "$t0\n";
            result += "sw $t0, " + addr1 + "($0)\n";
            assert(addr1.length() > 0 && addr1.at(0) >= '0' && addr1.at(0) <= '9');
        } else if (num == 3 || num == 8) {
            result += "move " + s + ", " + "$t0\n";
            result += "sb $t0, " + addr1 + "($0)\n";
        } else if (num == 5 || num == 10) {
            result += "sb $t0, " + s + "($0)\n";
        }
    }
    return result;
}

string new_TranslateCondition(string midCode, string bnz, vector<tmp_Variable>& temps) {
    stringstream ss;
    ss << midCode;
    string left, op, right; ss >> left >> op >> right;
    string result = "";
    string reg1 = "$t9";
    string reg2 = "$t8";
    if (isNum(left)) {
        result += "li $t9, " + left + '\n';
    } else {
        int num = 0;
        if (isCharCon(left, &num)) {
            result += "li $t9, " + to_string(num) + '\n';
        }
        else if (isConst(left, &num, nullptr)) {
            result += "li $t9, " + to_string(num) + '\n';
        } else {
            string action = "";
            reg1 = get_VarReg(temps, left, action, 2);
            result += action;
        }
    }
    if (isNum(right)) {
        result += "li $t8, " + right + '\n';
    } else {
        int num = 0;
        if (isCharCon(right, &num)) {
            result += "li $t8, " + to_string(num) + '\n';
        }
        else if (isConst(right, &num, nullptr)) {
            result += "li $t8, " + to_string(num) + '\n';
        } else {
            string action = "";
            reg2 = get_VarReg(temps, right, action, 1);
            result += action;
        }
    }

    stringstream ss2;
    ss2 << bnz;
    string jumpIns; ss2 >> jumpIns;
    string label; ss2 >> label;

    if (op == "==") result += "beq " + reg1 + ", " + reg2 + ", " + label + '\n';
    else if (op == "!=") result += "bne " + reg1 + ", " + reg2 + ", " + label + '\n';
    else if (op == ">") { // t9 > t8
        result += "slt $t8, " + reg2 + ", " + reg1 + '\n';
        result += "bne $t8, $0, " + label + '\n';
    }
    else if (op == "<=") { // t9 <= t8
        result += "slt $t8, " + reg2 + ", " + reg1 + '\n';
        result += "beq $t8, $0, " + label + '\n';
    }
    else if (op == ">=") { // t9 >= t8
        result += "slt $t8, " + reg1 + ", " + reg2 + '\n';
        result += "beq $t8, $0, " + label + '\n';
    }
    else if (op == "<") { //t9 < t8
        result += "slt $t8, " + reg1 + ", " + reg2 + '\n';
        result += "bne $t8, $0, " + label + '\n';
    }
    return result;
}

string new_Print(string midCode, vector<tmp_Variable>& temps) {
    string result = "";
    string pureString = "";
    string id = "";
    int i, j;
    if (midCode.find('"') != string::npos) {
        for (i = 0; i < midCode.size(); i++) {
            if (midCode.at(i) == '"') {
                for (j = i + 1; midCode.at(j) != '"'; j++) {
                    pureString += midCode.at(j);
                }
                break;
            }
        }
        for (j = j + 2; j < midCode.length(); j++) {
            id += midCode.at(j);
        }
    } else {
        stringstream ss; ss << midCode;
        string one, two;
        ss >> one >> two;
        id = two;
    }
    string action = "";
    if (pureString.length() == 0 && id.length() > 0) {
        int num = 0;
        string reg = new_resultIDtoMIPS(id, &num, action, temps);
        result += action;
        result += "move $a0, " + reg + "\n";
        if (num == 1) result += "li $v0, 1\n";
        else result += "li $v0, 11\n";
        result += "syscall\n";
    }
    if (id.length() == 0) {
        result += "la $a0, " + getStringLabel(pureString) + '\n';
        result += "li $v0, 4\n";
        result += "syscall\n";
    }
    if (pureString.length() > 0 && id.length() > 0) {
        int num = 0;
        result += "la $a0, " + getStringLabel(pureString) + '\n';
        result += "li $v0, 4\n";
        result += "syscall\n";
        string reg = new_resultIDtoMIPS(id, &num, action, temps);
        result += action;
        result += "move $a0, " + reg + "\n";
        if (num == 1) result += "li $v0, 1\n";
        else result += "li $v0, 11\n";
        result += "syscall\n";
    }
    result += "li $a0, 10\n";
    result += "li $v0, 11\n";
    result += "syscall\n";
    return result;
}

string new_Scanf(string id, vector<tmp_Variable>& temps) {
    string result = "";
    string addr1 = "";   int num = 0;
    string varAddr = getVarAddr(id, &num);
    if (num == 1) {
        result += "li $v0, 5\n";
        result += "syscall\n";
    } else {
        result += "li $v0, 12\n";
        result += "syscall\n";
    }
    string s = getAssignLeft(temps, id, addr1, &num);

    if (num == 0 || num == 6) {
        result += "move " + s + ", " + "$v0\n";
    }
    else if (num == 1 || num == 4 || num == 9) {
        result += "sw $v0, " + s + "($0)\n";
    } else if (num == 2 || num == 7) {
        result += "move " + s + ", " + "$v0\n";
        result += "sw $v0, " + addr1 + "($0)\n";
    } else if (num == 3 || num == 8) {
        result += "move " + s + ", " + "$v0\n";
        result += "sb $v0, " + addr1 + "($0)\n";
    } else if (num == 5 || num == 10) {
        result += "sb $v0, " + s + "($0)\n";
    }
    return result;
}

string new_ret(string id, vector<tmp_Variable>& temps) {
    string result = "";
    if (id.length() == 0) {
        result += "jr $ra\n";
    } else {
        string action = "";     int num = 0;
        string reg = new_resultIDtoMIPS(id, &num, action, temps);
        result += action;
        result += "move $v1, " + reg + '\n';
        result += "jr $ra\n";
    }
    return result;
}

string new_pushPara(string id, vector<tmp_Variable>& temps, int paraReg) {
    string result = "";
    string action = "";     int num = 0;
    string reg = new_paraIDtoMIPS(id, &num, action, temps);
    result += action;
    result += "move $" + to_string(paraReg) + ", " + reg + '\n';
    return result;
}

string new_pushStack() {
    string result = "";
    for (int i = 9; i <= 23; i++) {
        result += "sw $" + to_string(i) + ", 0($sp)\n";
        result += "sub $sp, $sp, 4\n";
    }
    for (int i = nowFunctionVariables.size() - 1; i >= 0; i--) {
        int type = 0;
        string varAddr = getVarAddr(nowFunctionVariables[i].VariableName, &type);
        if (varAddr.at(0) != '$') {
            if (type == 1) result += "lw $t9, " + varAddr + "($0)\n";
            else result += "lb $t9, " + varAddr + "($0)\n";
            result += "sw $t9, 0($sp)\n";
        } else {
            result += "sw " + varAddr + ", 0($sp)\n";
        }
        result += "sub $sp, $sp, 4\n";
    }
    result += "sw $ra, 0($sp)\n";
    result += "sub $sp, $sp, 4\n";
    return result;
}

string new_popStack() {
    string result = "";
    result += "add $sp, $sp, 4\n";
    result += "lw $ra, 0($sp)\n";
    for (int i = 0; i < nowFunctionVariables.size(); i++) {
        result += "add $sp, $sp, 4\n";
        int type = 0;
        string varAddr = getVarAddr(nowFunctionVariables[i].VariableName, &type);
        if (varAddr[0] != '$') {
            result += "lw $t9, 0($sp)\n";
            if (type == 1) result += "sw $t9, " + varAddr + "($0)\n";
            else result += "sb $t9, " + varAddr + "($0)\n";
        } else {
            result += "lw " + varAddr + ", 0($sp)\n";
        }
    }
    for (int i = 23; i >= 9; i--) {
        result += "add $sp, $sp, 4\n";
        result += "lw $" + to_string(i) + ", 0($sp)\n";
    }

    return result;
}
#endif //COMPILER_TRANSLATE_INTERFACE_H
