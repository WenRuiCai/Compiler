//
// Created by 84443 on 2019/12/11.
//

#ifndef COMPILER_FLOWGRAPH_H
#define COMPILER_FLOWGRAPH_H

#include "Exp_OneBlock.h"
#include "Optim_Functions.h"
#include "FlowBlock.h"
#include "InterferenceGraph.h"
#include "../MIPSCode/Variable.h"
#include "InlineOptim.h"
#include <assert.h>

string nowFunctionMidCode;

int arrayTypeIsInt(string tempVarName) {
    string line;
    int lineNum = getLineNum(nowFunctionMidCode);
    for (int i = 1; i <= lineNum; i++) {
        if (getExpInLine(i, nowFunctionMidCode).find(tempVarName) != string::npos) {
            string one, two, three;     stringstream ss; ss << getExpInLine(i, nowFunctionMidCode);
            ss >> one >> two >> three;
            if (three.find('[') != string::npos) {
                string arrayName = three.substr(0, three.find('['));
                if (nowFunction_GetVar_byName_Map.count(arrayName) > 0) {
                    if (nowFunction_GetVar_byName_Map.at(arrayName).var_type == INT_ARRAY) {
                        return 1;
                    } else if (nowFunction_GetVar_byName_Map.at(arrayName).var_type == CHAR_ARRAY) {
                        return 0;
                    } else
                        return -1;
                } else {
                    for (Variable variable : globalVariable) {
                        if (variable.VariableName == arrayName) {
                            if (variable.var_type == INT_ARRAY) {
                                return 1;
                            } else if (variable.var_type == CHAR_ARRAY) {
                                return 0;
                            } else
                                return -1;
                        }
                    }
                }
            } else {
                return -1;
            }
        }
    }
    return -1;
}

class Function_Flow_Blocks {
private:
    void calcu_in_and_out() {
        while (true) {
            int flag = 0;
            for (int i = this->flowBlocks.size() - 1; i >= 0; i--) {
                FlowBlock &block = this->flowBlocks[i];
                for (FlowBlock *nextflowBlock : block.nextBlock) {
                    for (string in : nextflowBlock->in) {
                        block.out.insert(in);
                    }
                }
                set <string> chaji;
                set_difference(block.out.begin(), block.out.end(),
                               block.def.begin(), block.def.end(), inserter(chaji, chaji.begin()));
                set<string> previousIn = block.in;
                set_union(block.use.begin(), block.use.end(),
                          chaji.begin(), chaji.end(), inserter(block.in, block.in.begin()));
                /*
                 * 检查previousIn和现在的in是否相等
                 */
                if (previousIn != block.in) {
                    flag = 1;
                }
            }
            if (flag == 0) {
                break;
            }
        }
    }
public:
    vector<FlowBlock> flowBlocks;
    InterferenceGraph* interferenceGraph;

    Function_Flow_Blocks(string functionMidCode) {
        nowFunctionMidCode = functionMidCode;
        //nowFunctionMidCode = inlineMidCode(functionMidCode);
        //functionMidCode = inlineMidCode(functionMidCode);
        //cout << inlineMidCode(functionMidCode);
        vector<int> entryCentence;
        entryCentence.push_back(1);
        int lineNum = getLineNum(functionMidCode);

        ///@brief: 找出所有入口语句
        for (int i = 2; i <= lineNum; i++) {
            if (getExpInLine(i, functionMidCode).find(':') != string::npos &&
                getExpInLine(i, functionMidCode).find("PRINT ") == string::npos) {
                int flag = 0;
                for (int x : entryCentence) {if (x == i) flag = 1;}
                if (flag == 0) entryCentence.push_back(i);
            }
            else if (getExpInLine(i, functionMidCode).substr(0, 4) == "BNZ " ||
                getExpInLine(i, functionMidCode).substr(0, 5) == "GOTO " ||
                getExpInLine(i, functionMidCode).substr(0, 4) == "ret ") {
                int flag = 0;
                for (int x : entryCentence) {if (x == i + 1) flag = 1;}
                if (flag == 0 && i + 1 <= lineNum) entryCentence.push_back(i + 1);
            }
        }

        ///@brief: 划分基本块
        for (int i = 0; i < entryCentence.size(); i++) {
            FlowBlock flowBlock = FlowBlock();
            if (i != entryCentence.size() - 1) {
                for (int j = entryCentence[i]; j < entryCentence[i + 1]; j++) {
                    flowBlock.addFourUnitExp(getExpInLine(j, functionMidCode));
                }
            } else {
                for (int j = entryCentence[i]; j <= lineNum; j++) {
                    flowBlock.addFourUnitExp(getExpInLine(j, functionMidCode));
                }
            }
            this->flowBlocks.push_back(flowBlock);
        }

        ///@brief: 找出每个基本块的后继，构建流图
        for (int i = 0; i < flowBlocks.size(); i++) {
            FlowBlock* flowBlock1 = &flowBlocks[i];
            string last = flowBlock1->fourUnitExps[flowBlock1->fourUnitExps.size() - 1];
            if (last.find("GOTO") != string::npos || last.find("BNZ") != string::npos) {
                string action, label;
                stringstream ss;    ss << last;
                ss >> action >> label;
                for (int y = 0; y < flowBlocks.size(); y++) {
                    string String1 = flowBlocks[y].fourUnitExps[0];
                    if (String1.find(label) != string::npos && String1 != last) {
                        flowBlock1->addNextBlock(&flowBlocks[y]);
                    }
                }
                if (last.find("BNZ") != string::npos) {
                    if (i + 1 < flowBlocks.size()) flowBlock1->addNextBlock(&flowBlocks[i + 1]);
                }
            } else if (last.find("ret") == string::npos) {
                if (i + 1 < flowBlocks.size()) flowBlock1->addNextBlock(&flowBlocks[i + 1]);
            }
        }

        ///@brief: 针对每个基本块做块内表达式优化
        //for (int i = 0; i < flowBlocks.size(); i++) {
        //    flowBlocks[i].Optim();
        //}

        ///@brief: 活跃变量分析
        for (FlowBlock& block : this->flowBlocks) {
            block.fillUse_And_Def();
        }
        calcu_in_and_out();

        ///@brief: 构建冲突图
            ///@details: 找出跨基本块活跃的变量
        set<string> var_cross;
        for (int i = 0; i < this->flowBlocks.size(); i++) {
            for (string var : flowBlocks[i].in) {
                for (int j = i + 1; j < this->flowBlocks.size(); j++) {
                    int flag = 0;
                    for (string var1 : flowBlocks[j].in) {
                        if (var == var1) {
                            var_cross.insert(var);  flag = 1;
                            break;
                        }
                    }
                    if (flag == 1) break;
                }
            }
        }
            ///@details: 用正确的方法计算出冲突图的每条边
            ///@bug: 还是感觉不对啊啊啊啊，操蛋的
        /*
        vector<pair<string, string>> edges;
        for (int i = 0; i < this->flowBlocks.size(); i++) {
            set<string> live = this->flowBlocks[i].out;
            for (int j = this->flowBlocks[i].fourUnitExps.size() - 1; j >= 0; j--) {
                set<string> def;
                string defined = get_SingleExp_Defined_Var(this->flowBlocks[i].fourUnitExps[j]);
                if (this->flowBlocks[i].def.find(defined) != this->flowBlocks[i].def.end())
                    def.insert(defined);

                for (string y : live) {
                    for (string x : def) {
                        int flag = 0;
                        for (pair<string, string> edge : edges) {
                            if ((edge.first == y && edge.second == x) ||
                                (edge.first == x && edge.second == y)) {
                                flag = 1;
                                break;
                            }
                        }
                        if (flag == 0 && y != x) edges.push_back(make_pair(y, x));
                    }
                }

                set<string> diffSet;
                set_difference(live.begin(), live.end(),
                        def.begin(), def.end(), inserter(diffSet, diffSet.begin()));

                set<string> use;    set<string> newLive;
                for (string string1 : get_SingleExp_Used_Var(this->flowBlocks[i].fourUnitExps[j])) {
                    if (this->flowBlocks[i].use.find(string1) != this->flowBlocks[i].use.end())
                        use.insert(string1);
                }
                set_union(use.begin(), use.end(), diffSet.begin(), diffSet.end(),
                        inserter(newLive, newLive.begin()));
                live = newLive;
            }
        }

            ///@details: 构建冲突图
        this->interferenceGraph = new InterferenceGraph(var_cross);
        for (FlowBlock block1 : this->flowBlocks) {
            for (string in_var : block1.in) {
                for (string def_var : block1.def) {
                    assert(in_var != def_var);
                    if (var_cross.find(in_var) != var_cross.end() &&
                        var_cross.find(def_var) != var_cross.end()) {
                        interferenceGraph->add_edge(in_var, def_var);
                    }
                }
            }
        }
        ///@brief: 图着色分析寄存器分配
        interferenceGraph->allocate_reg();

        ///@brief: 完成操作，可以开始翻译了
        */
    }

    void registerGlobalVarReg(vector<Variable>& functionVariables) {
        /**
         * @details1: 预备动作，先给分好了全局寄存器的变量注册寄存器
         * @details2: 在完成预备动作后，进入每个基本块中进行代码翻译
         *           由于需要对variable进行注册，所以和functionBlock类必须有交互
         * @bug: 图染色法分配寄存器存在bug，先姑且认为分配寄存器成功吧。。。
         * @bug: 图染色是真的全是bug，所以先尝试用暴力全局分配吧。。。
         */
         int arr[8] = {0};
         for (Variable& variable : functionVariables) {
             if (!variable.nowThisVariableIsInRegister) {
                 for (int i = 0; i < 8; i++) {
                     if (arr[i] == 0) {
                         variable.thisRegister = global_reg2string((global_reg) i);
                         variable.nowThisVariableIsInRegister = true;
                         arr[i] = 1;
                         break;
                     }
                 }
             }
         }
        return;
    }

    string translateToMips() {
        string result = "";
        for (FlowBlock& block : this->flowBlocks) {
            result += block.translateToMipsCode();
        }
        return result;
    }
};


#endif //COMPILER_FLOWGRAPH_H
