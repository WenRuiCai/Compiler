//
// Created by 84443 on 2019/11/16.
//

#ifndef COMPILER_RETURNCENTENCE_H
#define COMPILER_RETURNCENTENCE_H

#include "CentenceMid.h"

class ReturnCentence : public CentenceMid {
private:
    ExpressionMidCode returnExp;

public:
    void setExp() {
        this->returnExp = ExpressionMidCode();
        this->returnExp.init();
    }

    ExpressionMidCode* getNowExp() {
        return &this->returnExp;
    }

    string toString() {
        string result  = "";
        if (this->returnExp.expHasInit()) {
            result += this->returnExp.toString();
            result += "ret " + this->returnExp.getExpResultID() + "\n";
        } else {
            result += "ret\n";
        }
        return result;
    }

    ReturnCentence() {
        this->kind = RETURN;
    }

};


#endif //COMPILER_RETURNCENTENCE_H
