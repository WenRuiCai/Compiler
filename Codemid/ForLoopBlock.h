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
    int flag = -2;
    int step;
    bool isAdd;
    vector<CentenceMid*> loopBlock;

public:

    void set_IDENFR(string id) {
        this->IDENFR = id;
    }

    vector<CentenceMid*>* getCentenceBlock() {
        return &this->loopBlock;
    }

    void setCmp(string type) {
        this->compare = (type == "LSS") ? LESS :
                        ((type == "LEQ") ? LESS_EQL :
                        ((type == "GRE") ? GRT :
                        ((type == "GEQ") ? GRT_EQL :
                        ((type == "EQL") ? EQL : NOTEQL)
                        )));
    }

    void setExp() {
        if (flag == -2) {
            IDENFR_VALUE = ExpressionMidCode();
            IDENFR_VALUE.init();
            flag++;
        } else if (flag == -1) {
            conditionLeftExp = ExpressionMidCode();
            conditionLeftExp.init();
            flag++;
        } else if (flag == 0) {
            conditionRightExp = ExpressionMidCode();
            conditionRightExp.init();
            flag++;
        }
    }

    ExpressionMidCode* getNowExp() {
        return (flag == -1) ? &IDENFR_VALUE : ((flag == 0) ? &conditionLeftExp : &conditionRightExp);
    }

    string toString() {
        return "";
    }

    ForLoopBlock() {
        this->kind = FOR;
    }

};


#endif //COMPILER_FORLOOPBLOCK_H
