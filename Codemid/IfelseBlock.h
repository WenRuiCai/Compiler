//
// Created by 84443 on 2019/11/15.
//

#ifndef COMPILER_IFELSEBLOCK_H
#define COMPILER_IFELSEBLOCK_H

#include "ExpressionMidCode.h"
#include "CentenceMid.h"

class IfelseBlock : public CentenceMid {
private:
    ExpressionMidCode conditionLeftExp;
    conditionCompare compare;
    ExpressionMidCode conditionRightExp;
    int flag = -1;

    vector<CentenceMid*> conditionSatisfieldBlock;
    vector<CentenceMid*> conditionUnsatisfieldBlock;
public:

    void setExp() {
        if (flag == -1) {
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
        return (this->flag == 0) ? (&this->conditionLeftExp) : (&this->conditionRightExp);
    }

    void setCmp(string type) {
        this->compare = (type == "LSS") ? LESS :
                ((type == "LEQ") ? LESS_EQL :
                 ((type == "GRE") ? GRT :
                 ((type == "GEQ") ? GRT_EQL :
                 ((type == "EQL") ? EQL : NOTEQL)
                 )));
    }


    string toString() {
        return "";
    }

    IfelseBlock() {
        this->kind = IFELSE;
    }
};


#endif //COMPILER_IFELSEBLOCK_H
