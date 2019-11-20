//
// Created by 84443 on 2019/11/18.
//

#ifndef COMPILER_VARIABLE_H
#define COMPILER_VARIABLE_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

long int addr = 0x10010000; //268500992
int globalStringCounter = 0;
enum VariableType {
    INT_VAR,
    INT_PARA,
    INT_ARRAY,
    CHAR_VAR,
    CHAR_PARA,
    CHAR_ARRAY,
    STRING_VAR
};

class Variable {
public:
    string VariableName; //变量的名字
    VariableType var_type; //变量类型

    long int var_addr;  //内存地址
    string string_var;

    string thisRegister;

    Variable(string name, string type, int flag) { //函数参数变量
        this->VariableName = name;
        this->VariableName = name;
        this->var_type = (type == "INTTK") ? INT_PARA : CHAR_PARA;
        this->thisRegister = "$" + to_string(flag);
    }

    Variable(string name, int length, string type) { //数组变量
        this->VariableName = name;
        this->var_type = (type == "INTTK") ? INT_ARRAY : CHAR_ARRAY;
        this->var_addr = (type == "INTTK") ?
                ((addr % 4 == 0) ? addr : addr + 4 - (addr % 4)) : addr;
        if (this->var_type == INT_ARRAY) {
            addr = this->var_addr + 4 * length;
        } else if (this->var_type == CHAR_ARRAY) {
            addr = this->var_addr + length;
        }
    }

    Variable(string name, string type) { //普通变量
        this->VariableName = name;
        this->var_type = (type == "INTTK") ? INT_VAR : CHAR_VAR;
        this->var_addr = (type == "INTTK") ?
                ((addr % 4 == 0) ? addr : addr + 4 - (addr % 4)) : addr;
        if (this->var_type == INT_VAR) {
            addr = this->var_addr + 4;
        } else if (this->var_type == CHAR_VAR) {
            addr = this->var_addr + 1;
        }
    }

    Variable(string s) { //字符串
        this->VariableName = "string_" + to_string(globalStringCounter++);
        this->string_var = s;
        this->var_type = STRING_VAR;
        this->var_addr = addr;
        addr += s.length() + 1;
    }
};

vector<Variable> globalVariable;
vector<Variable> globalStrings;

string getStringLabel(string s) {
    for (Variable v : globalStrings) {
        if (v.string_var == s) {
            return v.VariableName;
        }
    }
    return "";
}
#endif //COMPILER_VARIABLE_H
