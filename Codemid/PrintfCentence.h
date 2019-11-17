//
// Created by 84443 on 2019/11/16.
//

#ifndef COMPILER_PRINTFCENTENCE_H
#define COMPILER_PRINTFCENTENCE_H

#include "CentenceMid.h"
class PrintfCentence : public CentenceMid {
private:
    string print_string;
    ExpressionMidCode exp;

public:
    void setString(string s) {
        this->print_string = s;
    }

    void setExp() {
        exp = ExpressionMidCode();
        exp.init();
    }

    ExpressionMidCode* getNowExp() {
        return &this->exp;
    }

    string toString() {
        string result = "";
        if (this->print_string.length() == 0 && this->exp.expHasInit()) {
            result += this->exp.toString();
            result += "PRINT " + this->exp.getExpResultID() + "\n";
        }
        if (!this->exp.expHasInit()) {
            result += "PRINT \"" + this->print_string + "\"\n";
        }
        if (this->print_string.length() > 0 && this->exp.expHasInit()) {
            result += this->exp.toString();
            result += "PRINT \"" + this->print_string + "\" " + this->exp.getExpResultID() + "\n";
        }
        return result;
    }

    PrintfCentence() {
        this->kind = PRINTF;
    }
};


#endif //COMPILER_PRINTFCENTENCE_H
