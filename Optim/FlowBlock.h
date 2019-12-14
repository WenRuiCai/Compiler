//
// Created by 84443 on 2019/12/13.
//

#ifndef COMPILER_FLOWBLOCK_H
#define COMPILER_FLOWBLOCK_H

#include <set>
#include <algorithm>

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

    void translateToMipsCode() {
        /**
         * @brief： 对本块中的语句逐条翻译
         * @details1: 翻译的函数接口已经在Codemid/MiddleCode.h中基本写好
         *            中间代码的语句总共就这么几样，需要先对每种语句进行分析判断句型，再进行翻译
         * @details2: 每个基本块设一个临时寄存器池，除了享有全局寄存器的变量外，所有块内的变量都存放在临时寄存器中
         *            临时寄存器不够时则需要存入内存中，
         *            同样的，在退出基本块时，如果在基本块内有对 程序全局/函数全局 的赋值语句，那么需要回填入内存
         */

    }
};

#endif //COMPILER_FLOWBLOCK_H
