//
// Created by 84443 on 2019/11/15.
//

#ifndef COMPILER_DOWHILEBLOCK_H
#define COMPILER_DOWHILEBLOCK_H
#include "ExpressionMidCode.h"
#include "CentenceMid.h"

/*
 * 语句列中的语句可以是任何，但是在本类中无法将其转换成别的语句，所以需要在最顶层中设置接口
 */
extern string get_centences_component_string(vector<CentenceMid*>& centencesBlock);
extern string get_centences_component_mips(vector<CentenceMid*>& centencesBlock);

class DoWhileBlock : public CentenceMid {
private:
    ExpressionMidCode conditionLeftExp;
    conditionCompare compare;
    ExpressionMidCode conditionRightExp;
    int flag = -1;

    vector<CentenceMid*> loopBlock;

    string getConditionString() {
        string conditionString = "";
        conditionString += conditionLeftExp.toString();
        if (conditionRightExp.expHasInit()) {
            conditionString += conditionRightExp.toString();
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
        string loopLabel = "dowhileloop_" + to_string(label_counter++);
        string conditionlabel = "condition_" + to_string(label_counter++);

        string result = loopLabel + ":\n";

        result += get_centences_component_string(this->loopBlock);    //语句列
        result += conditionlabel + ":\n";
        result += this->getConditionString();
        result += "BNZ " + loopLabel + "\n";

        return result;
    }

    string toMips() {
        string loopLabel = "dowhileloop_" + to_string(label_counter++);
        string conditionlabel = "condition_" + to_string(label_counter++);

        string result = loopLabel + ":\n";

        result += get_centences_component_mips(this->loopBlock);    //语句列
        result += conditionlabel + ":\n";
        //result += this->getConditionString();
        //result += "BNZ " + loopLabel + "\n";
        result +=
                translateConditionCentence(this->getConditionString() + "BNZ " + loopLabel + "\n");

        return result;
    }

    DoWhileBlock() {
        this->kind = DOWHILE;
    }
};


#endif //COMPILER_DOWHILEBLOCK_H
