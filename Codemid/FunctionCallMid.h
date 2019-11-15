//
// Created by 84443 on 2019/11/14.
//

#ifndef COMPILER_FUNCTIONCALLMID_H
#define COMPILER_FUNCTIONCALLMID_H

#include "ExpressionMidCode.h"
#include "CentenceMid.h"

/////////////////////////////////////////////////
///                 函数调用                   ///
/////////////////////////////////////////////////
class FunctionCallMidCode : public CentenceMid {
private:
    vector<ExpressionMidCode> parameterValues;
    string functionName;
    string functionReturnValueID;
    int nowParameter = -1;

    void addParameterValue() {
        ExpressionMidCode expressionMidCode = ExpressionMidCode();
        expressionMidCode.init();
        this->parameterValues.push_back(expressionMidCode);
        this->nowParameter = this->parameterValues.size() - 1;
    }

public:
    FunctionCallMidCode(string name) {
        this->functionName = name;
        this->functionReturnValueID = "T" + to_string(ID_counter++);
        this->kind = FUNCTIONCALL;
    }

    string getFunctionReturnValueID() {
        return this->functionReturnValueID;
    }

    void setExp() {
        addParameterValue();
    }

    ExpressionMidCode* getNowExp() {
        return &this->parameterValues[nowParameter];
    }

    string toString() {
        string result = "";
        string call = "call " + this->functionName + "\n";
        string returnCode = this->functionReturnValueID + " = RET\n";
        for (ExpressionMidCode value : this->parameterValues) {
            if (!value.hasOnlyOneItem())
                result += value.toString();
        }
        for (ExpressionMidCode value : this->parameterValues) {
            result += "push " + value.getExpResultID() + "\n";
        }
        return result + call + returnCode;
    }
};

#endif //COMPILER_FUNCTIONCALLMID_H
