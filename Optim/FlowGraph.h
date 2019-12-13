//
// Created by 84443 on 2019/12/11.
//

#ifndef COMPILER_FLOWGRAPH_H
#define COMPILER_FLOWGRAPH_H

#include "Exp_OneBlock.h"
#include "Optim_Functions.h"
#include "FlowBlock.h"

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

    Function_Flow_Blocks(string functionMidCode) {
        vector<int> entryCentence;
        entryCentence.push_back(1);
        int lineNum = getLineNum(functionMidCode);

        ///@brief: 找出所有入口语句
        for (int i = 2; i <= lineNum; i++) {
            if (getExpInLine(i, functionMidCode).find(':') != string::npos) {
                int flag = 0;
                for (int x : entryCentence) {if (x == i) flag = 1;}
                if (flag == 0) entryCentence.push_back(i);
            }
            else if (getExpInLine(i, functionMidCode).find("BNZ") != string::npos ||
                getExpInLine(i, functionMidCode).find("GOTO") != string::npos ||
                getExpInLine(i, functionMidCode).find("call") != string::npos ||
                getExpInLine(i, functionMidCode).find("ret") != string::npos) {
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
            if (last.find("call") != string::npos) {
                if (i + 1 < flowBlocks.size()) flowBlock1->addNextBlock(&flowBlocks[i + 1]);
            }
            else if (last.find("GOTO") != string::npos || last.find("BNZ") != string::npos) {
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
    }
};


#endif //COMPILER_FLOWGRAPH_H
