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
            result += translateExp(this->returnExp.toString());
            //result += "ret " + this->returnExp.getExpResultID() + "\n";
            result += resultIDtoMIPS(this->returnExp.getExpResultID(), nullptr);
            result += "move $v1, $t9\n";
            result += "jr $ra\n";
        } else {
            //result += "ret\n";
            result += "jr $ra\n";
        }
        return result;
    }

    ReturnCentence() {
        this->kind = RETURN;
    }

};


#endif //COMPILER_RETURNCENTENCE_H
