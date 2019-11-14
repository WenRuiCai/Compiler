//
// Created by 84443 on 2019/11/14.
//

#ifndef COMPILER_FUNCTIONCALLMID_H
#define COMPILER_FUNCTIONCALLMID_H

#include "ExpressionMidCode.h"

/////////////////////////////////////////////////
///                 函数调用                   ///
/////////////////////////////////////////////////
class FunctionCallMidCode {
private:
    vector<ExpressionMidCode> parameterValues;
    string functionName;
    string functionReturnValueID;
    int nowParameter = -1;

public:
    FunctionCallMidCode(string name) {
        this->functionName = name;
        this->functionReturnValueID = "t" + to_string(ID_counter++);
    }

    string getFunctionReturnValueID() {
        return this->functionReturnValueID;
    }

    ExpressionMidCode& getNowParameterExp() {
        return this->parameterValues[nowParameter];
    }

    void addParameterValue() {
        ExpressionMidCode expressionMidCode = ExpressionMidCode();
        expressionMidCode.init();
        this->parameterValues.push_back(expressionMidCode);
        this->nowParameter = this->parameterValues.size() - 1;
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
