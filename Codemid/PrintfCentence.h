//
// Created by 84443 on 2019/11/16.
//

#ifndef COMPILER_PRINTFCENTENCE_H
#define COMPILER_PRINTFCENTENCE_H

#include "CentenceMid.h"

extern string getStringLabel(string s);

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
            result += translateExp(this->exp.toString());
            //result += "PRINT " + this->exp.getExpResultID() + "\n";
            int num;
            result += resultIDtoMIPS(this->exp.getExpResultID(), &num);
            result += "move $a0, $t9\n";
            if (num == 1) result += "li $v0, 1\n";
            else result += "li $v0, 11\n";
            result += "syscall\n";
        }
        if (!this->exp.expHasInit()) {
            //result += "PRINT \"" + this->print_string + "\"\n";
            result += "la $a0, " + getStringLabel(this->print_string) + '\n';
            result += "li $v0, 4\n";
            result += "syscall\n";
        }
        if (this->print_string.length() > 0 && this->exp.expHasInit()) {
            result += translateExp(this->exp.toString());
            //result += "PRINT \"" + this->print_string + "\" " + this->exp.getExpResultID() + "\n";
            int num;
            result += resultIDtoMIPS(this->exp.getExpResultID(), &num);
            result += "la $a0, " + getStringLabel(this->print_string) + '\n';
            result += "li $v0, 4\n";
            result += "syscall\n";
            result += "move $a0, $t9\n";
            if (num == 1) result += "li $v0, 1\n";
            else result += "li $v0, 11\n";
            result += "syscall\n";
        }
        return result;
    }

    PrintfCentence() {
        this->kind = PRINTF;
    }
};


#endif //COMPILER_PRINTFCENTENCE_H
