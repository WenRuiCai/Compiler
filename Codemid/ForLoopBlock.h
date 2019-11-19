//
// Created by 84443 on 2019/11/15.
//

#ifndef COMPILER_FORLOOPBLOCK_H
#define COMPILER_FORLOOPBLOCK_H
#include "ExpressionMidCode.h"
#include "CentenceMid.h"
/*
 * 语句列中的语句可以是任何，但是在本类中无法将其转换成别的语句，所以需要在最顶层中设置接口
 */
extern string get_centences_component_string(vector<CentenceMid*>& centencesBlock);

class ForLoopBlock : public CentenceMid {
private:
    string IDENFR;
    ExpressionMidCode IDENFR_VALUE;
    ExpressionMidCode conditionLeftExp;
    conditionCompare compare;
    ExpressionMidCode conditionRightExp;
    int flag = -2;

    string IDENFR_STEP;
    string IDENFR_STEP1;
    int step;
    bool isAdd = false;
    vector<CentenceMid*> loopBlock;

    string getConditionString() {
        string conditionString = "";
        conditionString += translateExp(conditionLeftExp.toString());
        if (conditionRightExp.expHasInit()) {
            conditionString += translateExp(conditionRightExp.toString());
            switch (this->compare) {
                case LESS:
                    conditionString += conditionLeftExp.getExpResultID() + " < " +
                                       conditionRightExp.getExpResultID() + "\n";
                    break;
                case LESS_EQL:
                    conditionString += conditionLeftExp.getExpResultID() + " <= " +
                                       conditionRightExp.getExpResultID() + "\n";
                    break;
                case GRT:
                    conditionString += conditionLeftExp.getExpResultID() + " > " +
                                       conditionRightExp.getExpResultID() + "\n";
                    break;
                case GRT_EQL:
                    conditionString += conditionLeftExp.getExpResultID() + " >= " +
                                       conditionRightExp.getExpResultID() + "\n";
                    break;
                case EQL:
                    conditionString += conditionLeftExp.getExpResultID() + " == " +
                                       conditionRightExp.getExpResultID() + "\n";
                    break;
                case NOTEQL:
                    conditionString += conditionLeftExp.getExpResultID() + " != " +
                                       conditionRightExp.getExpResultID() + "\n";
                    break;
            }
        } else {
            conditionString += conditionLeftExp.getExpResultID() + " != 0\n";
        }
        return conditionString;
    }
public:

    void setIDENFR_STEP(string s) {
        this->IDENFR_STEP = s;
    }

    void setIDENFR_STEP1(string s) {
        this->IDENFR_STEP1 = s;
    }

    void set_STEP(int num) {
        this->step = num;
    }

    void setIsAdd() {
        this->isAdd = true;
    }

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
        string result = "";
        //result += this->IDENFR_VALUE.toString();
        //result += this->IDENFR + " = " + this->IDENFR_VALUE.getExpResultID() + "\n";
        result += translateExp(this->IDENFR_VALUE.toString() +
                this->IDENFR + " = " + this->IDENFR_VALUE.getExpResultID() + "\n");

        string conditionLabel = "condition_" + to_string(label_counter++);
        string loopLabel = "ForLoop_" + to_string(label_counter++);
        string leavelLabel = "label_" + to_string(label_counter);

        result += conditionLabel + ":\n";
        //result += getConditionString();
        //result += "BNZ " + loopLabel + "\n";
        result +=
                translateConditionCentence(this->getConditionString() + "BNZ " + loopLabel + "\n");

        //result += "GOTO " + leavelLabel + "\n";
        result += "j " + leavelLabel + "\n";

        result += loopLabel + ":\n";
        result += get_centences_component_string(this->loopBlock);

        //result += this->IDENFR_STEP + " = " +
        //        this->IDENFR_STEP1 + ((this->isAdd) ? " + " : " - ") + to_string(step) + "\n";
        result += translateExp(this->IDENFR_STEP + " = " +
                this->IDENFR_STEP1 + ((this->isAdd) ? " + " : " - ") + to_string(step) + "\n");

        //result += "GOTO " + conditionLabel + "\n";
        result += "j " + conditionLabel + "\n";

        result += leavelLabel + ":\n";
        return result;
    }

    ForLoopBlock() {
        this->kind = FOR;
    }

};


#endif //COMPILER_FORLOOPBLOCK_H
