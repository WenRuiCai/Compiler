//
// Created by 84443 on 2019/12/14.
//

#ifndef COMPILER_TRANSLATE_TO_MIPS_FUNCTIONS_H
#define COMPILER_TRANSLATE_TO_MIPS_FUNCTIONS_H

#include "../MIPSCode/Variable.h"
#include <assert.h>

vector<Variable> nowFunctionVariables;
map<string, Variable> nowFunction_GetVar_byName_Map;
vector<TableItem> nowFunctionConsts;
void update_GetVar_byName_Map();

enum temp_Reg {
    t1 = 0,
    t2,
    t3,
    t4,
    t5,
    t6,
    t7
};
string temp_reg_Enum2String(temp_Reg tempReg) {
    if (tempReg == t1) return "$t1";
    if (tempReg == t2) return "$t2";
    if (tempReg == t3) return "$t3";
    if (tempReg == t4) return "$t4";
    if (tempReg == t5) return "$t5";
    if (tempReg == t6) return "$t6";
    else return "$t7";
}

struct tmp_Variable {
    string name;
    temp_Reg reg;
    string addr;
    bool isInReg = false;
    int times = 0;

    tmp_Variable(string name) : name(name) {}
};

void getAllTemps(vector<tmp_Variable>& temps, vector<string> fourUnitExps) {
    set<string> allVar;
    map<string, int> timesCount;
    for (string fourUnitExp : fourUnitExps) {
        for (string s : get_SingleExp_Used_Var(fourUnitExp)) {
            allVar.insert(s);
            if (timesCount.count(s) != 0) timesCount.at(s) += 1;
            else timesCount.insert(make_pair(s, 1));
        }
        if (get_SingleExp_Defined_Var(fourUnitExp).length() > 0) {
            string s = get_SingleExp_Defined_Var(fourUnitExp);
            allVar.insert(s);
            if (timesCount.count(s) != 0) timesCount.at(s) += 1;
            else timesCount.insert(make_pair(s, 1));
        }
    }
    /**
     * @brief: 把分配了全局寄存器的变量刨除
     *
     */
    vector<string> new_allVar;
    for (string s : allVar) {
        int flag = 0;
        for (Variable variable : nowFunctionVariables) {
            if (variable.VariableName == s && variable.nowThisVariableIsInRegister) {
                flag = 1;
                break;
            }
        }
        if (flag == 0) new_allVar.push_back(s);
    }
    for (string s : new_allVar) {
        tmp_Variable variable = tmp_Variable(s);
        variable.times = timesCount.at(s);
        temps.push_back(variable);
    }
}

void alloc_tmp_register(vector<tmp_Variable>& temps) {
    int gp = 0x10008000;
    int k = 7;
    int reg[7] = {0};
    while (k > 0) {
        int flag = 0;
        for (tmp_Variable variable : temps) {
            if (!variable.isInReg) {
                flag = 1; break;
            }
        }
        if (flag == 0) {
            break;
        }
        int max = 0;   int place = 0;
        for (int i = 0; i < temps.size(); i++) {
            if (!temps[i].isInReg) {
                max = temps[i].times; place = i;
                break;
            }
        }

        for (int i = 0; i < temps.size(); i++) {
            tmp_Variable variable1 = temps[i];
            if (variable1.times > max && !variable1.isInReg) {
                max = variable1.times;     place = i;
            }
        }
        for (int i = 0; i < 7; i++) {
            if (reg[i] == 0) {
                temps[place].reg = (temp_Reg) i;
                reg[i] = 1;
                temps[place].isInReg = true;
                k--;
                break;
            }
        }
    }
    /**
     * @brief: 以上已经分配完毕临时寄存器，如果还有剩余的临时变量，那么就只能存内存里面了，这里存在$gp指针处
     *         同时，如果临时变量是函数内定义过的，或者是程序全局的，
     *         那么在nowFunctionVariables或者globalVariable中要注意更新信息
     *         因为我们的tmp_Variable这个结构体，在最后只处理TEMP_VAR_CWR这种类型的临时变量
     */
    for (tmp_Variable& variable2 : temps) {
        ///@details：在nowFunctionVariables或者globalVariable中更新信息
        if (variable2.isInReg) {
            int flag = 0;
            for (Variable& variable : nowFunctionVariables) {
                if (variable.VariableName == variable2.name) {
                    flag = 1;   variable.thisRegister = temp_reg_Enum2String(variable2.reg);
                    variable.nowThisVariableIsInRegister = true;
                    break;
                }
            }
            if (flag == 0) {
                for (Variable& variable1 : globalVariable) {
                    if (variable1.VariableName == variable2.name) {
                        variable1.thisRegister = temp_reg_Enum2String(variable2.reg);
                        variable1.nowThisVariableIsInRegister = true;
                        break;
                    }
                }
            }
        } else {
            int flag = 0;
            for (Variable& variable : nowFunctionVariables) {
                if (variable.VariableName == variable2.name) {
                    flag = 1;   variable2.addr = to_string(variable.var_addr);
                    variable.nowThisVariableIsInRegister = false;
                    break;
                }
            }
            if (flag == 0) {
                for (Variable& variable1 : globalVariable) {
                    if (variable1.VariableName == variable2.name) {
                        flag = 1; variable2.addr = to_string(variable1.var_addr);
                        variable1.nowThisVariableIsInRegister = false;
                        break;
                    }
                }
            }
            if (flag == 0) {
                variable2.addr = to_string(gp);
                gp += 4;
            }
        }
    }
    update_GetVar_byName_Map();
}

/**
 * @brief： 得到use变量的寄存器
 */
string get_VarReg(vector<tmp_Variable>& temps, string var, string& pre_action, int place) {
    int flag = 0;   string reg = "";
    if (var.find("TEMP_VAR_CWR") != string::npos) {
        for (tmp_Variable variable : temps) {
            if (variable.name == var) {
                if (variable.isInReg) return temp_reg_Enum2String(variable.reg);
                else {
                    if (place == 3) {
                        pre_action += "lw $t0, " + variable.addr + "($0)\n";
                        return "$t0";
                    }
                    pre_action += "lw $t" +
                                  to_string((place == 1) ? 8 : 9) + ", " + variable.addr + "($0)\n";
                    return "$t" + to_string((place == 1) ? 8 : 9);
                }
            }
        }
    }
    if (nowFunction_GetVar_byName_Map.count(var) == 0) {
        for (Variable variable : globalVariable) {
            if (variable.VariableName == var) {
                if (variable.nowThisVariableIsInRegister) {
                    return variable.thisRegister;
                }
                flag = (variable.var_type == INT_VAR) ? 1 : 0;
                if (flag == 1) {
                    pre_action += "lw ";
                } else
                    pre_action += "lb ";

                if (place == 3) {
                    pre_action += "$t0, " +
                                  to_string(variable.var_addr) + "($0)\n";
                    return "$t0";
                }
                pre_action += "$t" +
                        to_string((place == 1) ? 8 : 9) + ", " +
                        to_string(variable.var_addr) + "($0)\n";
                return "$t" + to_string((place == 1) ? 8 : 9);
            }
        }
        assert(1 == 0);
    }
    if (nowFunction_GetVar_byName_Map.at(var).nowThisVariableIsInRegister) {
        return nowFunction_GetVar_byName_Map.at(var).thisRegister;
    }
    flag = (nowFunction_GetVar_byName_Map.at(var).var_type == INT_VAR) ? 1 : 0;
    if (flag == 1) {
        pre_action += "lw ";
    } else
        pre_action += "lb ";

    if (place == 3) {
        pre_action += "$t0, " +
                      to_string(nowFunction_GetVar_byName_Map.at(var).var_addr) + "($0)\n";
        return "$t0";
    }
    pre_action += "$t" +
                  to_string((place == 1) ? 8 : 9) + ", " +
                  to_string(nowFunction_GetVar_byName_Map.at(var).var_addr) + "($0)\n";
    return "$t" + to_string((place == 1) ? 8 : 9);
}

/**
 * @brief： 得到数组的起始地址
 */
string get_array_begin_addr(string var, int* type) {
    if (nowFunction_GetVar_byName_Map.count(var) == 0) {
        for (Variable variable : globalVariable) {
            assert(variable.var_type == INT_ARRAY || variable.var_type == CHAR_ARRAY);
            if (variable.VariableName == var) {
                if (variable.var_type == INT_ARRAY) *type = 1; else *type = 0;
                return to_string(variable.var_addr);
            }
        }
    }
    assert(nowFunction_GetVar_byName_Map.at(var).var_type == INT_ARRAY ||
    nowFunction_GetVar_byName_Map.at(var).var_type == CHAR_ARRAY);

    if (nowFunction_GetVar_byName_Map.at(var).var_type == INT_ARRAY) *type = 1;
    else *type = 0;

    return to_string(nowFunction_GetVar_byName_Map.at(var).var_addr);
}

/**
 * @brief： 处理等式左边的待赋值元素
 */
string getAssignLeft(vector<tmp_Variable>& temps, string var, string& addr, int* type) {
    if (var.find("TEMP_VAR_CWR") != string::npos) {
        //如果是没声明的临时变量，那么只需要得到它分配的寄存器或地址，写入即可
        for (tmp_Variable variable : temps) {
            if (variable.name == var) {
                if (variable.isInReg) {
                    *type = 0;
                    return temp_reg_Enum2String(variable.reg);
                } else {
                    *type = 1;
                    return variable.addr;
                }
            }
        }
    }
    else {
        //如果是声明了的，如果分配了全局寄存器，那么写全局寄存器
        //如果没有分配全局寄存器，那么先要写临时寄存器，后再写内存
        //如果是数组，直接写内存
        if (nowFunction_GetVar_byName_Map.count(var) == 0) {
            for (Variable variable : globalVariable) {
                if (variable.VariableName == var) {
                    if (variable.nowThisVariableIsInRegister) {
                        addr = to_string(variable.var_addr);
                        if (variable.var_type == INT_VAR || variable.var_type == INT_ARRAY) {
                            *type = 2;
                        } else
                            *type = 3;
                        return variable.thisRegister;
                    } else {
                        if (variable.var_type == INT_VAR || variable.var_type == INT_ARRAY) {
                            *type = 4;
                        } else
                            *type = 5;
                        return to_string(variable.var_addr);
                    }
                }
            }
            assert(1 == 0);
        }
        if (nowFunction_GetVar_byName_Map.at(var).nowThisVariableIsInRegister) {
            if (nowFunction_GetVar_byName_Map.at(var).thisRegister.find('s') != string::npos ||
                nowFunction_GetVar_byName_Map.at(var).var_type == INT_PARA ||
                    nowFunction_GetVar_byName_Map.at(var).var_type == CHAR_PARA) {
                *type = 6;
                return nowFunction_GetVar_byName_Map.at(var).thisRegister;
            }
            if (nowFunction_GetVar_byName_Map.at(var).var_type == INT_VAR ||
                    nowFunction_GetVar_byName_Map.at(var).var_type == INT_ARRAY) {
                *type = 7;
            } else
                *type = 8;
            addr = to_string(nowFunction_GetVar_byName_Map.at(var).var_addr);
            return nowFunction_GetVar_byName_Map.at(var).thisRegister;
        } else {
            if (nowFunction_GetVar_byName_Map.at(var).var_type == INT_VAR ||
                    nowFunction_GetVar_byName_Map.at(var).var_type == INT_ARRAY) {
                *type = 9;
            } else
                *type = 10;
            return to_string(nowFunction_GetVar_byName_Map.at(var).var_addr);
        }
    }
    return nullptr;
}

string new_resultIDtoMIPS(string resultID, int* type, string& action, vector<tmp_Variable>& temps) {
    string reg = "$t9";
    if (isNum(resultID)) {
        action += "li $t9, " + resultID + '\n';
        if (type != nullptr) *type = 1;
    } else {
        int num = 0;
        if (isCharCon(resultID, &num)) {
            action += "li $t9, " + to_string(num) + '\n';
            if (type != nullptr) *type = 0;
        }
        else if (isConst(resultID, &num, type)) {
            action += "li $t9, " + to_string(num) + '\n';
        } else {
            num = 1;
            string varAddr = getVarAddr(resultID, &num);
            reg = get_VarReg(temps, resultID, action, 2);

            if (type != nullptr) *type = num;
        }
    }
    return reg;
}

string new_paraIDtoMIPS(string resultID, int* type, string& action, vector<tmp_Variable>& temps) {
    string reg = "$t9";
    if (isNum(resultID)) {
        action += "li $t9, " + resultID + '\n';
    } else {
        int num = 0;
        if (isCharCon(resultID, &num)) {
            action += "li $t9, " + to_string(num) + '\n';
        }
        else if (isConst(resultID, &num, type)) {
            action += "li $t9, " + to_string(num) + '\n';
        } else {
            num = 1;
            string varAddr = getVarAddr(resultID, &num);
            if (varAddr[0] == '$' && varAddr.length() == 2) {
                //函数参数作为函数内函数调用，此时必须从栈里取参数，不然会出现覆盖现象
                int regNum = varAddr[1] - '0';
                int offset = (regNum - 4) * 4 + 4;
                action += "lw $t9, " + to_string(offset) + "($sp)\n";
            } else {
                reg = get_VarReg(temps, resultID, action, 2);
            }
        }
    }
    return reg;
}
#endif //COMPILER_TRANSLATE_TO_MIPS_FUNCTIONS_H
