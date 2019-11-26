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
extern string pushStack();
extern string popStack();

class FunctionCallMidCode : public CentenceMid {
private:
    vector<ExpressionMidCode> parameterValues;
    string functionName;
    string functionReturnValueID;
    int nowParameter = -1;
    bool hasReturnValue;

    void addParameterValue() {
        ExpressionMidCode expressionMidCode = ExpressionMidCode();
        expressionMidCode.init();
        this->parameterValues.push_back(expressionMidCode);
        this->nowParameter = this->parameterValues.size() - 1;
    }

public:
    FunctionCallMidCode(string name) {
        this->functionName = name;
        this->functionReturnValueID = "TEMP_VAR_CWR" + to_string(ID_counter++);
        this->kind = FUNCTIONCALL;
    }

    void functionHasReturnValue() {
        this->hasReturnValue = true;
    }

    void functionHasNotReturnValue() {
        this->hasReturnValue = false;
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
        if (this->hasReturnValue)
            return result + call + returnCode;
        else
            return result + call;
    }

    string toMips() {
        string result = "";
        /*
         * 调用函数前要压栈
         */
        //string call = "call " + this->functionName + "\n";
        string call = "jal " + this->functionName + "\n";
        //string returnCode = this->functionReturnValueID + " = RET\n";
        string returnCode = "move $" + this->functionReturnValueID + ", $v1\n";
        for (ExpressionMidCode value : this->parameterValues) {
            //if (!value.hasOnlyOneItem())
            result += translateExp(value.toString());
        }
        int reg = 5;
        for (ExpressionMidCode value : this->parameterValues) {
            result += resultIDtoMIPS(value.getExpResultID(), nullptr, true);
            //result += "push " + value.getExpResultID() + "\n";
            result += "move $" + to_string(reg) + ", $t9\n";
            reg++;
        }
        /*
         * 函数调用之后要恢复变量哦
         */
        if (this->hasReturnValue)
            return pushStack() + result + call + returnCode + popStack();
        else
            return pushStack() + result + call + popStack();
    }
};

#endif //COMPILER_FUNCTIONCALLMID_H
