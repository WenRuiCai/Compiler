//
// Created by 84443 on 2019/11/8.
//

#ifndef COMPILER_MIDDLECODE_H
#define COMPILER_MIDDLECODE_H
int label_counter = 0;
string getVarAddr(string var, int* flag);
extern bool isConst(string name, int* num, int* type);
string getArrayItemAddr(string s, int* num);
string getArrayOpNum(string s, int* num, bool op1);
string translateExp(string exp_midCode);

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

string resultIDtoMIPS(string resultID, int* type) {
    string result = "";
    if (isNum(resultID)) {
        result += "li $t9, " + resultID + '\n';
        if (type != nullptr) *type = 1;
    } else {
        int num = 0;
        if (isCharCon(resultID, &num)) {
            result += "li $t9, " + to_string(num) + '\n';
            if (type != nullptr) *type = 0;
        }
        else if (isConst(resultID, &num, type)) {
            result += "li $t9, " + to_string(num) + '\n';
        } else if (resultID.at(resultID.length() - 1) != ']') {
            string varAddr = getVarAddr(resultID, &num);
            if (varAddr[0] == '$') {
                result += "move $t9, " + varAddr + '\n';
                if (type != nullptr) *type = 1;
            } else {
                if (num) result += "lw $t9, " + varAddr + "($0)\n";
                else result += "lb $t9, " + varAddr + "($0)\n";
                if (type != nullptr) *type = num;
            }
        } else {
            int num111 = 0;
            result += getArrayOpNum(resultID, &num111, true);
            result += "move $t9, $t7\n";
            if (type != nullptr) *type = num111;
        }
    }
    return result;
}

/*
 * $t9 :条件左部
 * $t8 :条件右部
 * 不可中途更改t8 t9
 */
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
    result = translateExp(result);
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
            result += "li $t9, " + to_string(num) + '\n';
        }
        else if (isConst(left, &num, nullptr)) {
            result += "li $t9, " + to_string(num) + '\n';
        } else if (left.at(left.length() - 1) != ']') {
            string varAddr = getVarAddr(left, &num);
            if (varAddr[0] == '$') {
                result += "move $t9, " + varAddr + '\n';
            } else {
                if (num) result += "lw $t9, " + varAddr + "($0)\n";
                else result += "lb $t9, " + varAddr + "($0)\n";
            }
        } else {
            int num111 = 0;
            result += getArrayOpNum(left, &num111, true);
            result += "move $t9, $t7\n";
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
        } else if (right.at(right.length() - 1) != ']') {
            string varAddr = getVarAddr(right, &num);
            if (varAddr[0] == '$') {
                result += "move $t8, " + varAddr + '\n';
            } else {
                if (num) result += "lw $t8, " + varAddr + "($0)\n";
                else result += "lb $t8, " + varAddr + "($0)\n";
            }
        } else {
            int num111 = 0;
            result += getArrayOpNum(right, &num111, true);
            result += "move $t8, $t7\n";
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

/*
 * $s7 :数组元素的地址 得到后在使用前不可更改
 * $t6, $t7可更改
 */
string getArrayItemAddr(string s, int* num) {
    string arrayName = ""; int place = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s.at(i) != '[') {
            place = i + 1;
            arrayName += s.at(i);
        } else break;
    }
    string index = "";
    for (int i = place + 1; i < s.length(); i++) {
        if (s.at(i) == ']') break;
        else index += s.at(i);
    }
    string beginAddr = getVarAddr(arrayName, num);
    string result = "";
    int num1 = 0;
    result += "li $t6, " + beginAddr + '\n';
    if (*num == 1) {
        result += "li $t7, 4\n";
    } else result += "li $t7, 1\n";

    if (isNum(index)) {
        result += "li $s7, " + index + '\n';
    } else if (isConst(index, &num1, nullptr)) {
        result += "li $s7, " + to_string(num1) + '\n';
    } else {
        int num_var_index = 0;
        string indexAddr = getVarAddr(index, &num_var_index);
        if (indexAddr[0] == '$') {
            result += "move $s7, " + indexAddr + '\n';
        } else {
            if (num_var_index == 1) result += "lw $s7, " + indexAddr + "($0)\n";
            else result += "lb $s7, " + indexAddr + "($0)\n";
        }
    }
    result += "mult $t7, $s7\n";
    result += "mflo $t7\n";
    result += "add $s7, $t7, $t6\n";
    return result;
}
/*
 * 函数功能，将数组符号代表的元素取出并存在t6或t7寄存器中
 * 数组元素作为赋值符号右部已完成，但是数组元素作为print、push均未实现
 * $t7 : 存左操作数
 * $t6 : 存右操作数
 * 不可更改t8 t9
 */
string getArrayOpNum(string s, int* num, bool op1) {
    string result = getArrayItemAddr(s, num);
    if (op1) {
        if (*num == 1) result += "lw $t7, 0($s7)\n";
        else result += "lb $t7, 0($s7)\n";
    }
    else {
        if (*num == 1) result += "lw $t6, 0($s7)\n";
        else result += "lb $t6, 0($s7)\n";
    }
    return result;
}

/*
 * 调用完后可以更改任何寄存器
 */
string transLateExp_leftIsArray(string left, string right_op_1, string right_op_2, string op) {
    string result = "";
    int num = 0, num1 = 0, num2 = 0;
    result += getArrayItemAddr(left, &num);
    if (op.length() == 0) {
        //纯赋值语句
        if (isNum(right_op_1)) {

            result += "li $t7, " + right_op_1 + '\n';
            if (num == 1) result += "sw $t7, 0($s7)\n";
            else result += "sb $t7, 0($s7)\n";

        } else if (isCharCon(right_op_1, &num1)) {
            result += "li $t7, " + to_string(num1) + '\n';
            if (num == 1) result += "sw $t7, 0($s7)\n";
            else result += "sb $t7, 0($s7)\n";
        } else if (isConst(right_op_1, &num1, nullptr)) {
            result += "li $t7, " + to_string(num1) + '\n';
            if (num == 1) result += "sw $t7, 0($s7)\n";
            else result += "sb $t7, 0($s7)\n";
        } else if (right_op_1.at(right_op_1.length() - 1) != ']') {
            string rightAddr = getVarAddr(right_op_1, &num1);
            if (rightAddr[0] == '$') {

                if (num == 1) result += "sw " + rightAddr + ", " + "0($s7)\n";
                else result += "sb " + rightAddr + ", " + "0($s7)\n";

            } else {
                if (num1 == 1) result += "lw $t6, " + rightAddr + "($0)\n";
                else result += "lb $t6, " + rightAddr + "($0)\n";

                if (num == 1) result += "sw $t6, 0($s7)\n";
                else result += "sb $t6, 0($s7)\n";

            }
        } else {
            int num_array = 0;
            result += "move $t5, $s7\n";
            result += getArrayOpNum(right_op_1, &num_array, true);

            if (num == 1) result += "sw $t7, 0($t5)\n";
            else result += "sb $t7, 0($t5)\n";
        }
    } else {
        string op1_addr;
        string op2_addr;
        string reg_op1 = "$t7", reg_op2 = "$t6";
        result += "move $t5, $s7\n";
        if (isNum(right_op_1)) {
            result += "li $t7, " + right_op_1 + '\n';
        } else if (isCharCon(right_op_1, &num1)) {
            result += "li $t7, " + to_string(num1) + '\n';
        } else if (isConst(right_op_1, &num1, nullptr)) {
            result += "li $t7, " + to_string(num1) + '\n';
        } else if (right_op_1.at(right_op_1.size() - 1) != ']') {
            op1_addr = getVarAddr(right_op_1, &num1);
            if (op1_addr[0] == '$') { reg_op1 = op1_addr; }
            else {
                if (num1 == 1) result += "lw $t7, " + op1_addr + "($0)\n";
                else result += "lb $t7, " + op1_addr + "($0)\n";
            }
        } else {
            int num_array = 0;
            result += getArrayOpNum(right_op_1, &num_array, true);
        }

        if (isNum(right_op_2)) {
            result += "li $t6, " + right_op_2 + '\n';
        } else if (isCharCon(right_op_2, &num2)) {
            result += "li $t6, " + to_string(num2) + '\n';
        } else if (isConst(right_op_2, &num2, nullptr)) {
            result += "li $t6, " + to_string(num2) + '\n';
        } else if (right_op_2.at(right_op_2.length() - 1) != ']') {
            op2_addr = getVarAddr(right_op_2, &num2);
            if (op2_addr[0] == '$') reg_op2 = op2_addr;
            else {
                if (num2 == 1) result += "lw $t6, " + op2_addr + "($0)\n";
                else result += "lb $t6, " + op2_addr + "($0)\n";
            }
        } else {
            int num_array = 0;
            result += getArrayOpNum(right_op_2, &num_array, false);
        }

        if (op == "+") {

            result += "add $t6, " + reg_op1 + ", " + reg_op2 + "\n";
            result += "sw $t6, 0($t5)\n";

        } else if (op == "-") {

            result += "sub $t6, " + reg_op1 + ", " + reg_op2 + "\n";
            result += "sw $t6, 0($t5)\n";

        } else if (op == "*") {
            result += "mult " + reg_op1 + ", " + reg_op2 + '\n';

            result += "mflo $t6\n";
            result += "sw $t6, 0($t5)\n";

        } else if (op == "/") {
            result += "div " + reg_op1 + ", " + reg_op2 + '\n';

            result += "mflo $t6\n";
            result += "sw $t6, 0($t5)\n";

        }
    }
    return result;
}

extern string pushStack();
extern string popStack();
string translateFunctionFactor(string functionFactorMid, int reg) {
    string result = "";
    stringstream ss;
    ss << functionFactorMid;
    string one, two, three;
    ss >> one;  ss >> two; ss >> three;
    if (one == "push") {
        result += resultIDtoMIPS(two, nullptr);
        result += "move $" + to_string(reg) + ", $t9\n";
    } else if (one == "call") {
        result += "jal " + two + '\n';
    } else {
        result += popStack();
        result += "move $" + one + ", $v1\n";
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
        if (left == "push" || left == "call") {
            result += pushStack();
            int reg = 5;
            result += translateFunctionFactor(tmp + '\n', reg++);
            while (true) {
                getline(ss, tmp);
                result += translateFunctionFactor(tmp + '\n', reg++);
                if (tmp.find("RET") != string::npos && tmp.find("=") != string::npos) {
                    break;
                }
            }
            continue;
        }
        if (assignSymbol != "=") {
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
        if (left.at(left.length() - 1) == ']') {
            result += transLateExp_leftIsArray(left, right_op_1, right_op_2, op);
            continue;
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
            } else if (isConst(right_op_1, &num1, nullptr)) {
                if (leftAddr[0] == '$') {result += "li " + leftAddr + ", " + to_string(num1) + '\n';}
                else {
                    result += "li $t7, " + to_string(num1) + '\n';
                    if (num == 1) result += "sw $t7, " + leftAddr + "($0)\n";
                    else result += "sb $t7, " + leftAddr + "($0)\n";
                }
            } else if (right_op_1.at(right_op_1.length() - 1) != ']') {
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
            } else {
                int num_array = 0;
                result += getArrayOpNum(right_op_1, &num_array, true);
                if (leftAddr[0] == '$') result += "move " + leftAddr + ", $t7\n";
                else {
                    if (num == 1) result += "sw $t7, " + leftAddr + "($0)\n";
                    else result += "sb $t7, " + leftAddr + "($0)\n";
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
            } else if (isConst(right_op_1, &num1, nullptr)) {
                result += "li $t7, " + to_string(num1) + '\n';
            } else if (right_op_1.at(right_op_1.size() - 1) != ']') {
                op1_addr = getVarAddr(right_op_1, &num1);
                if (op1_addr[0] == '$') { reg_op1 = op1_addr; }
                else {
                    if (num1 == 1) result += "lw $t7, " + op1_addr + "($0)\n";
                    else result += "lb $t7, " + op1_addr + "($0)\n";
                }
            } else {
                int num_array = 0;
                result += getArrayOpNum(right_op_1, &num_array, true);
            }

            if (isNum(right_op_2)) {
                result += "li $t6, " + right_op_2 + '\n';
            } else if (isCharCon(right_op_2, &num2)) {
                result += "li $t6, " + to_string(num2) + '\n';
            } else if (isConst(right_op_2, &num2, nullptr)) {
                result += "li $t6, " + to_string(num2) + '\n';
            } else if (right_op_2.at(right_op_2.length() - 1) != ']') {
                op2_addr = getVarAddr(right_op_2, &num2);
                if (op2_addr[0] == '$') reg_op2 = op2_addr;
                else {
                    if (num2 == 1) result += "lw $t6, " + op2_addr + "($0)\n";
                    else result += "lb $t6, " + op2_addr + "($0)\n";
                }
            } else {
                int num_array = 0;
                result += getArrayOpNum(right_op_2, &num_array, false);
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
