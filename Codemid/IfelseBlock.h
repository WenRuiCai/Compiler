//
// Created by 84443 on 2019/11/15.
//

#ifndef COMPILER_IFELSEBLOCK_H
#define COMPILER_IFELSEBLOCK_H

#include "ExpressionMidCode.h"
#include "CentenceMid.h"
/*
 * 语句列中的语句可以是任何，但是在本类中无法将其转换成别的语句，所以需要在最顶层中设置接口
 */
extern string get_centences_component_string(vector<CentenceMid*>& centencesBlock);
extern string get_centences_component_mips(vector<CentenceMid*>& centencesBlock);

class IfelseBlock : public CentenceMid {
private:
    ExpressionMidCode conditionLeftExp;
    conditionCompare compare;
    ExpressionMidCode conditionRightExp;
    int flag = -1;

    vector<CentenceMid*> conditionSatisfieldBlock;
    vector<CentenceMid*> conditionUnsatisfieldBlock;
    int isElseBlock = 0;

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

    void conditionSatisfieldBlock_Finished() {
        this->isElseBlock = 1;
    }

    vector<CentenceMid*>* getCentenceBlock() {
        return (isElseBlock) ? &conditionUnsatisfieldBlock : &conditionSatisfieldBlock;
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

    void setCmp(string type) {
        this->compare = (type == "LSS") ? LESS :
                ((type == "LEQ") ? LESS_EQL :
                 ((type == "GRE") ? GRT :
                 ((type == "GEQ") ? GRT_EQL :
                 ((type == "EQL") ? EQL : NOTEQL)
                 )));
    }

    string toString() {
        string conditionLabel = "condition_" + to_string(label_counter++);
        string satisfieldLabel = "Satisfield_" + to_string(label_counter++);
        string unsatisfieldLabel = "Unsatisfield_" + to_string(label_counter++);
        string leaveLabel = "label_" + to_string(label_counter++);

        string result = "";
        result += conditionLabel + ":\n";
        result += this->getConditionString();
        result += "BNZ " + satisfieldLabel + "\n";
        if (this->conditionUnsatisfieldBlock.size() > 0) {
            result += unsatisfieldLabel + ":\n";
            result += get_centences_component_string(this->conditionUnsatisfieldBlock);
        }
        result += "GOTO " + leaveLabel + "\n";

        result += satisfieldLabel + ":\n";
        result += get_centences_component_string(this->conditionSatisfieldBlock);
        result += leaveLabel + ":\n";

        return result;
    }

    string toMips() {
        string conditionLabel = "condition_" + to_string(label_counter++);
        string satisfieldLabel = "Satisfield_" + to_string(label_counter++);
        string unsatisfieldLabel = "Unsatisfield_" + to_string(label_counter++);
        string leaveLabel = "label_" + to_string(label_counter++);

        string result = "";
        result += conditionLabel + ":\n";

        //result += this->getConditionString();
        //result += "BNZ " + satisfieldLabel + "\n";
        result +=
                translateConditionCentence(this->getConditionString() + "BNZ " + satisfieldLabel + "\n");


        if (this->conditionUnsatisfieldBlock.size() > 0) {
            result += unsatisfieldLabel + ":\n";
            result += get_centences_component_mips(this->conditionUnsatisfieldBlock);
        }
        //result += "GOTO " + leaveLabel + "\n";
        result += "j " + leaveLabel + "\n";

        result += satisfieldLabel + ":\n";
        result += get_centences_component_mips(this->conditionSatisfieldBlock);
        result += leaveLabel + ":\n";

        return result;
    }

    IfelseBlock() {
        this->kind = IFELSE;
    }
};


#endif //COMPILER_IFELSEBLOCK_H
