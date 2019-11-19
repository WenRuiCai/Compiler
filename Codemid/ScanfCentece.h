//
// Created by 84443 on 2019/11/16.
//

#ifndef COMPILER_SCANFCENTECE_H
#define COMPILER_SCANFCENTECE_H

#include "CentenceMid.h"

class ScanfCentece : public CentenceMid {
private:
    vector<string> IDENFRS;

public:

    void setIDENFR(string s) {
        this->IDENFRS.push_back(s);
    }

    string toString() {
        string result = "";
        for (string string1 : this->IDENFRS) {
            //result += "SCANF " + string1 + "\n";
            int num = 0;
            string varAddr = getVarAddr(string1, &num);
            if (num == 1) {
                result += "li $v0, 5\n";
                result += "syscall\n";
                result += "sw $v0, " + varAddr + "($0)\n";
            } else {
                result += "li $v0, 12\n";
                result += "syscall\n";
                result += "sb $v0, " + varAddr + "($0)\n";
            }
        }
        return result;
    }

    ScanfCentece() {
        this->kind = SCANF;
    }

};


#endif //COMPILER_SCANFCENTECE_H
