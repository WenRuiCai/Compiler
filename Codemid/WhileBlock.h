//
// Created by 84443 on 2019/11/15.
//

#ifndef COMPILER_WHILEBLOCK_H
#define COMPILER_WHILEBLOCK_H
#include "ExpressionMidCode.h"
#include "CentenceMid.h"

class WhileBlock : public CentenceMid {
private:
    ExpressionMidCode conditionLeftExp;
    conditionCompare compare;
    ExpressionMidCode conditionRightExp;
    int flag = -1;

    vector<CentenceMid> loopBlock;

public:
    void setCmp(string type) {
        this->compare = (type == "LSS") ? LESS :
                        ((type == "LEQ") ? LESS_EQL :
                        ((type == "GRE") ? GRT :
                        ((type == "GEQ") ? GRT_EQL :
                        ((type == "EQL") ? EQL : NOTEQL)
                        )));
    }

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

    string toString() {
        return "";
    }

    WhileBlock() {
        this->kind = WHILE;
    }
};


#endif //COMPILER_WHILEBLOCK_H
