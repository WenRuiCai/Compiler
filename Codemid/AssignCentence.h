//
// Created by 84443 on 2019/11/16.
//

#ifndef COMPILER_ASSIGNCENTENCE_H
#define COMPILER_ASSIGNCENTENCE_H

#include "CentenceMid.h"

class AssignCentence : public CentenceMid {
private:
    string IDENFR;
    ExpressionMidCode ID_EXP;
    ExpressionMidCode ID_VALUE;
    /*
     * 0表示不是数组型元素
     * 1表示是数组，ID_EXP还未填充
     * 2表示ID_EXP填充完,ID_VALUE未填充
     * 3表示ID_VALUE填充完
     */
    int isArray_need_assign = 0;

public:
    AssignCentence() {
        this->kind = ASSIGN;
    }

    void ID_isArray() {
        this->isArray_need_assign = 1;
    }

    void setIDENFR(string id) {
        this->IDENFR = id;
    }

    void setExp() {
        if (this->isArray_need_assign == 1) {
            this->ID_EXP = ExpressionMidCode();
            ID_EXP.init();
            this->isArray_need_assign = 2;
        } else if (this->isArray_need_assign == 2 || this->isArray_need_assign == 0){
            this->ID_VALUE = ExpressionMidCode();
            this->ID_VALUE.init();
            this->isArray_need_assign = 3;
        }
    }

    ExpressionMidCode* getNowExp() {
        return (isArray_need_assign == 2) ? &this->ID_EXP : &this->ID_VALUE;
    }

    string toString() {
        string result = "";
        if (ID_EXP.expHasInit()) {
            result += ID_EXP.toString();
            result += ID_VALUE.toString();
            result += IDENFR + "[" + ID_EXP.getExpResultID() + "]" + " = " +
                      ID_VALUE.getExpResultID() + "\n";
        } else {
            result += ID_VALUE.toString();
            result += IDENFR + " = " + ID_VALUE.getExpResultID() + "\n";
        }
        return result;
    }

    string toMips() {
        string result = "";
        if (ID_EXP.expHasInit()) {
            //result += ID_EXP.toString();
            //result += ID_VALUE.toString();
            result += translateExp(ID_EXP.toString() + ID_VALUE.toString());

            /*
             * 数组的情况还要处理~
             * 处理完成！
             */
            result += translateExp(IDENFR + "[" + ID_EXP.getExpResultID() + "]" + " = " +
                    ID_VALUE.getExpResultID() + "\n");
        } else {
            //result += ID_VALUE.toString();
            //result += IDENFR + " = " + ID_VALUE.getExpResultID() + "\n";
            result +=
                    translateExp(ID_VALUE.toString() +
                    IDENFR + " = " + ID_VALUE.getExpResultID() + "\n");
        }
        return result;
    }

};


#endif //COMPILER_ASSIGNCENTENCE_H
