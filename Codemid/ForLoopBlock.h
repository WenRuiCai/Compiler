//
// Created by 84443 on 2019/11/15.
//

#ifndef COMPILER_FORLOOPBLOCK_H
#define COMPILER_FORLOOPBLOCK_H
#include "ExpressionMidCode.h"
#include "CentenceMid.h"
#include "IfelseBlock.h"
#include "WhileBlock.h"

class ForLoopBlock : public CentenceMid {
private:
    string IDENFR;
    ExpressionMidCode IDENFR_VALUE;
    ExpressionMidCode conditionLeftExp;
    conditionCompare compare;
    ExpressionMidCode conditionRightExp;
    int step;
    bool isAdd;
    vector<CentenceMid*> loopBlock;

public:

    void setCmp(string type) {
        this->compare = (type == "LSS") ? LESS :
                        ((type == "LEQ") ? LESS_EQL :
                        ((type == "GRE") ? GRT :
                        ((type == "GEQ") ? GRT_EQL :
                        ((type == "EQL") ? EQL : NOTEQL)
                        )));
    }

    void setExp() {}

    ExpressionMidCode* getNowExp() {}

    string toString() {}

    ForLoopBlock() {
        this->kind = FOR;
    }

};


#endif //COMPILER_FORLOOPBLOCK_H
