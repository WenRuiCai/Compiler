//
// Created by 84443 on 2019/12/13.
//

#ifndef COMPILER_FLOWBLOCK_H
#define COMPILER_FLOWBLOCK_H

#include <set>
#include <algorithm>
#include "TransLate_Interface.h"

void dynamicManageTempReg(vector<tmp_Variable>& temps, vector<string> fourUnitExps, int nowLine);
class FlowBlock {
private:
    bool varHasDefined(string s) {
        for (string s1 : this->def) {
            if (s1 == s) return true;
        }
        return false;
    }

    bool varHasUsed(string s) {
        for (string s1 : this->use) {
            if (s1 == s) return true;
        }
        return false;
    }
public:
    ///@details: fourUnitExps存储这个基本块里面的所有四元式语句
    ///          nextBlock存储这个基本块的所有后继
    ///          frontBlock存储这个基本块的所有前驱
    vector<string> fourUnitExps;
    vector<FlowBlock*> nextBlock;
    vector<FlowBlock*> frontBlock;
    set<string> use;
    set<string> def;
    set<string> in;
    set<string> out;

    void addFourUnitExp(string line) {
        this->fourUnitExps.push_back(line);
    }

    /**
     * @brief:此函数用于基本块内优化，现在只做了DAG优化
     */
    void Optim() {
        cout << getOptim_MidCode(this->fourUnitExps);
    }

    string flowBlockString() {
        string result = "";
        for (string s : fourUnitExps) {
            result += s;
            result += '\n';
        }
        return result;
    }

    void addFrontBlock(FlowBlock* block) {
        this->frontBlock.push_back(block);
    }

    void addNextBlock(FlowBlock* block) {
        this->nextBlock.push_back(block);
        block->addFrontBlock(this);
    }

    void fillUse_And_Def() {
        for (string fourUnitExp : fourUnitExps) {
            for (string usedVar : get_SingleExp_Used_Var(fourUnitExp)) {
                if (!varHasDefined(usedVar)) this->use.insert(usedVar);
            }
            if (!varHasUsed(get_SingleExp_Defined_Var(fourUnitExp))) {
                if (get_SingleExp_Defined_Var(fourUnitExp).length() > 0)
                    this->def.insert(get_SingleExp_Defined_Var(fourUnitExp));
            }
        }
    }

    string translateToMipsCode() {
        /**
         * @brief： 对本块中的语句逐条翻译
         * @details1: 翻译的函数接口已经在Codemid/MiddleCode.h中基本写好
         *            中间代码的语句总共就这么几样，需要先对每种语句进行分析判断句型，再进行翻译
         * @details2: 每个基本块设一个临时寄存器池，除了享有全局寄存器的变量外，所有块内的变量都存放在临时寄存器中
         *            临时寄存器不够时则需要存入内存中，
         *            同样的，在退出基本块时，如果在基本块内有对 程序全局/函数全局 的赋值语句，那么需要回填入内存
         */

        /**
         * @brief： 为临时变量分配存储
         * @details：这里准备采用静态分配策略
         * @bug: 是否需要在开始先重新更新一边globalVariables和functionVariables？
         *         好像不用
         *         但是在每个块前必须先把分配临时寄存器的定义变量载入
         */
        vector<tmp_Variable> temps;
        getAllTemps(temps, this->fourUnitExps);
        alloc_tmp_register(temps);
        string result = "";
        /**
         * @details: 首先将分到临时寄存器的声明变量加载进寄存器里
         */
        if (!(fourUnitExps[0].find(':') != string::npos &&
              fourUnitExps[0].find("PRINT") == string::npos)) {
            for (int i = 0; i < temps.size(); i++) {
                if (temps[i].name.find("TEMP_VAR_CWR") == string::npos) {
                    if (temps[i].isInReg) {
                        int num = 0;
                        string string1 = getVarAddr(temps[i].name, &num);
                        if (num == 0) {
                            result += "lb " + temp_reg_Enum2String(temps[i].reg) +
                                      ", " + string1 + "($0)\n";
                        } else {
                            result += "lw " + temp_reg_Enum2String(temps[i].reg) +
                                      ", " + string1 + "($0)\n";
                        }
                    }
                }
            }
        }
        for (int i = 0; i < this->fourUnitExps.size(); i++) {
            string fourUnitExp = this->fourUnitExps[i];
            stringstream ss; ss << fourUnitExp; string one, two, three, four, five;
            ss >> one >> two >> three >> four >> five;
            int needAlloc = 1;
            if (three.length() == 0 && two.find("()") != string::npos && one != "PRINT") {
                /**
                 * @brief: 函数名
                 */
                int place = two.find("(");
                result += two.substr(0, place) + ":\n";
                while (i + 1 < this->fourUnitExps.size() &&
                       this->fourUnitExps[i + 1].find("para ") != string::npos) {
                    i++;
                }
            } else if (two == "=" && IS_NOT_COMPARE_SYMBOL(two)) {
                string exp = fourUnitExp + '\n';
                /**
                 * @brief: 纯赋值语句
                 */
                result += new_TranslateAssign(exp, temps, false);
            } else if (!IS_NOT_COMPARE_SYMBOL(fourUnitExp)) {
                string condition = fourUnitExp + '\n';
                string bnz = this->fourUnitExps[i + 1] + '\n';
                i++;
                /**
                 * @brief: 条件比较语句
                 */
                result += new_TranslateCondition(condition, bnz, temps);
            } else if (one == "PRINT") {
                /**
                 * @brief: 打印语句
                 */
                result += new_Print(fourUnitExp, temps);
            } else if (one == "SCANF") {
                /**
                 * @brief: 读语句
                 */
                string id = two;
                result += new_Scanf(two, temps);
            } else if (one == "ret") {
                /**
                 * @brief: 返回语句
                 */
                result += new_ret(two, temps);
            } else if (one.find(":") != string::npos && two.length() == 0) {
                /**
                 * @brief: 标签
                 */
                result += fourUnitExp + '\n';
                for (int u = 0; u < temps.size(); u++) {
                    if (temps[u].name.find("TEMP_VAR_CWR") == string::npos) {
                        if (temps[u].isInReg) {
                            int num = 0;
                            string string1 = getVarAddr(temps[u].name, &num);
                            if (num == 0) {
                                result += "lb " + temp_reg_Enum2String(temps[u].reg) +
                                          ", " + string1 + "($0)\n";
                            } else {
                                result += "lw " + temp_reg_Enum2String(temps[u].reg) +
                                          ", " + string1 + "($0)\n";
                            }
                        }
                    }
                }
            } else if (one == "GOTO") {
                /**
                 * @brief: 无条件跳转语句
                 */
                result += "j " + two + '\n';
            } else if (one == "call") {
                /**
                 * @brief: 函数调用语句
                 */
                if (i - 1 >= 0 && (fourUnitExps[i - 1].substr(0, 4) != "push" ||
                                   fourUnitExps[i - 1].find('=') != string::npos)) {
                    result += new_pushStack(temps);
                }
                string call = two;
                result += "jal " + two + '\n';
                result += new_popStack(temps);
                string retValue;
                if (i + 1 < fourUnitExps.size()) {
                    stringstream ss11; ss11 << fourUnitExps[i + 1];
                    string one_, two_, three_;    ss11 >> one_ >> two_ >> three_;
                    if (two_ == "=" && three_ == "RET") {
                        retValue = fourUnitExps[i + 1];
                        result += new_TranslateAssign(retValue, temps, true);
                        i++;
                    }
                }
            } else if (one == "push") {
                /**
                 * @brief: 送参语句
                 */
                int reg = 5;
                result += new_pushStack(temps);
                result += new_pushPara(two, temps, reg++);
                for (int j = i + 1; j < fourUnitExps.size(); j++) {
                    stringstream ss12; ss12 << fourUnitExps[j];
                    string one12, two12; ss12 >> one12 >> two12;
                    if (one12 == "push") {
                        assert(reg <= 8);
                        result += new_pushPara(two12, temps, reg++);
                    } else {
                        i = j - 1;
                        break;
                    }
                }
                needAlloc = 0;
            }
            /**
             * 这里可以使用动态临时寄存器分配，如果上一条语句的TEMP_VAR在后面没用了，
             * 那么我们可以注销掉这个TEMP_VAR, 这个寄存器可以再次分配给其他TEMP_VAR
             * @attention: 注意一定得是分配给TEMP_VAR
             */
            if (needAlloc == 1)
               dynamicManageTempReg(temps, this->fourUnitExps, i);
        }
        return result;
    }
};

#endif //COMPILER_FLOWBLOCK_H

void dynamicManageTempReg(vector<tmp_Variable>& temps, vector<string> fourUnitExps, int nowLine) {
    vector<string> needUseTempVar;
    for (int nextLine = nowLine + 1; nextLine < fourUnitExps.size(); nextLine++) {
        string nowFourUnitExp = fourUnitExps[nextLine];
        if (nowFourUnitExp.find("TEMP_VAR_CWR") == string::npos) {
            continue;
        } else if (nowFourUnitExp.substr(0, 6) == "PRINT " &&
                   nowFourUnitExp.find('"') != string::npos) {
            int i = nowFourUnitExp.length() - 1;
            while (nowFourUnitExp.at(i) != '"') i--;
            string string1 = nowFourUnitExp.substr(i, nowFourUnitExp.length() - i);
            if (string1.find("TEMP_VAR_CWR") == string::npos) {
                continue;
            }
        }
        for (int i = 0; i < nowFourUnitExp.length(); i++) {
            string s;
            if (i + 12 > nowFourUnitExp.length()) break;
            if ((s = nowFourUnitExp.substr(i, 12)) == "TEMP_VAR_CWR") {
                int j;
                for (j = i + 12; j < nowFourUnitExp.length() &&
                                 nowFourUnitExp.at(j) >= '0' && nowFourUnitExp.at(j) <= '9'; j++) {
                    s = s + nowFourUnitExp.at(j);
                }
                needUseTempVar.push_back(s);
                i = j;
            }
        }
    }
    for (int i = 0; i < temps.size(); i++) {
        if (temps[i].name.find("TEMP_VAR_CWR") != string::npos) {
            int flag = 0;
            for (string s : needUseTempVar) {
                if (s == temps[i].name) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                auto iter = temps.begin(); int num = i;
                while (num--) iter++;
                temps.erase(iter);  i--;
            }
        }
    }
    int reg[6] = {0};
    for (tmp_Variable variable : temps) {
        if (variable.isInReg) {
            reg[variable.reg] = 1;
        }
    }
    for (int i = 0; i < 6; i++) {
        if (reg[i] == 0) {
            for (tmp_Variable& variable : temps) {
                if (!variable.isInReg && variable.name.find("TEMP_VAR_CWR") != string::npos) {
                    variable.reg = (temp_Reg) i;
                    variable.isInReg = true;
                    break;
                }
            }
        }
    }
    return;
}