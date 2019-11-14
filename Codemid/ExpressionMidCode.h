//
// Created by 84443 on 2019/11/8.
//

#ifndef COMPILER_EXPRESSIONMIDCODE_H
#define COMPILER_EXPRESSIONMIDCODE_H

#include "calculateExp.h"

class ExpressionMidCode;
class FunctionCallMidCode;
enum FactorKind{
    EXP,
    PURE_IDENFR,
    ARRAY,
    FUNCTION,
    INTCON,
    CHARCON
};

/////////////////////////////////////////////////
///                 因子                      ///
/////////////////////////////////////////////////
class factorMidCode {
private:
    string factorResultID;
    ExpressionMidCode* factor_exp = nullptr;
    string IDENFR;
    FactorKind factorKind;
    char charcon;
    int intcon;
    FunctionCallMidCode* functionCallMid = nullptr;
    string thisString;

public:
    char getCharcon() { return this->charcon; }
    int getIntcon() { return this->intcon; }
    /*
     * 由于超前引用，这里不能调用functionCallMid和exp的toString方法
     * 所以必须增加外部接口，在外部进行操作
     */
    void setString_CHAR_INT_IDENFR() {
        if (this->factorKind == CHARCON) {
            this->factorResultID = &"" [ charcon];
        } else if (this->factorKind == INTCON) {
            this->factorResultID = to_string(intcon);
        } else if (this->factorKind == PURE_IDENFR) {
            this->factorResultID = this->IDENFR;
        }
    }

    void setString_ARRAY(string arrayIndexString, string expID) {
        this->thisString = arrayIndexString;    //如果数组索引是表达式，那么需要先进行表达式运算
        this->factorResultID = this->IDENFR + "[" + expID + "]";
    }

    void setString_FUNCTION(string functionString, string functionReturnValueID) {
        this->thisString = functionString;
        this->factorResultID = functionReturnValueID;
    }

    void setString_EXP(string expString, string expID) {
        this->thisString = expString;
        this->factorResultID = expID;
    }

    string toString() { return this->thisString; }

    string getFactorResultID() {
        return this->factorResultID;
    }

    FactorKind getFactorKind() {
        return this->factorKind;
    }

    void setFactor_exp(ExpressionMidCode* pointer) {
        this->factor_exp = pointer;
    }

    void setFunctionCall(FunctionCallMidCode* pointer) {this->functionCallMid = pointer;}

    FunctionCallMidCode& getFunctionCall() {
        return *this->functionCallMid;
    }

    void factor_is_functioncall() {
        this->factorKind = FUNCTION;
    }

    void factor_is_exp() {
        this->factorKind = EXP;
    }

    void factor_is_intcon(int num) {
        this->factorKind = INTCON;  this->intcon = num;
    }

    void factor_is_charcon(string char_value) {
        this->factorKind = CHARCON;  this->charcon = char_value[0];
    }

    void factor_is_array(string name) {
        this->factorKind = ARRAY;   this->IDENFR = name;
    }

    void factor_is_pure_idenfr(string name) {
        this->IDENFR = name;    this->factorKind = PURE_IDENFR;
    }

    ExpressionMidCode& getFactorExp() {
        return *this->factor_exp;
    }

    void init() {
        factorResultID = to_string(ID_counter++);
    }
};



/////////////////////////////////////////////////
///                  项                       ///
/////////////////////////////////////////////////
class itemMidCode {
private:
    string itemResultID;
    vector<factorMidCode> factorMidCodes;
    vector<OP> ops;
    vector<calculateExp> mid_three_unit_Exps;
    int nowFactor;

    void fill_mid_three_unit_exps() {
        //将factor和op组合填入三元式中
        int num = 0;
        for (int i = 0; i < ops.size(); i++) {
            int left, right;
            int left1, right1;
            string left_s, right_s;
            if (mid_three_unit_Exps.size() == 0) {
                int flag = 0;
                if (factorMidCodes[num].getFactorKind() == INTCON) {
                    left = factorMidCodes[num].getIntcon();
                } else if (factorMidCodes[num].getFactorKind() == CHARCON) {
                    left1 = factorMidCodes[num].getCharcon();   flag = 1;
                } else {
                    left_s = factorMidCodes[num].getFactorResultID();   flag = 2;
                }
                num++;
                if (factorMidCodes[num].getFactorKind() == INTCON) {
                    right = factorMidCodes[num].getIntcon();
                    if (flag == 0)
                        mid_three_unit_Exps.push_back(calculateExp(left, right, ops[i], ID_counter++));
                    else if (flag == 1)
                        mid_three_unit_Exps.push_back(calculateExp(left1, right, ops[i], ID_counter++));
                    else
                        mid_three_unit_Exps.push_back(calculateExp(left_s, right, ops[i], ID_counter++));
                } else if (factorMidCodes[num].getFactorKind() == CHARCON) {
                    right1 = factorMidCodes[num].getCharcon();
                    if (flag == 0)
                        mid_three_unit_Exps.push_back(calculateExp(left, right1, ops[i], ID_counter++));
                    else if (flag == 1)
                        mid_three_unit_Exps.push_back(calculateExp(left1, right1, ops[i], ID_counter++));
                    else
                        mid_three_unit_Exps.push_back(calculateExp(left_s, right1, ops[i], ID_counter++));
                } else {
                    right_s = factorMidCodes[num].getFactorResultID();
                    if (flag == 0)
                        mid_three_unit_Exps.push_back(calculateExp(left, right_s, ops[i], ID_counter++));
                    else if (flag == 1)
                        mid_three_unit_Exps.push_back(calculateExp(left1, right_s, ops[i], ID_counter++));
                    else
                        mid_three_unit_Exps.push_back(calculateExp(left_s, right_s, ops[i], ID_counter++));
                }
                num++;
            } else {
                left_s = mid_three_unit_Exps[mid_three_unit_Exps.size() - 1].getResultID();
                if (factorMidCodes[num].getFactorKind() == INTCON) {
                    right = factorMidCodes[num].getIntcon();
                    mid_three_unit_Exps.push_back(calculateExp(left_s, right, ops[i], ID_counter++));
                } else if (factorMidCodes[num].getFactorKind() == CHARCON) {
                    right1 = factorMidCodes[num].getCharcon();
                    mid_three_unit_Exps.push_back(calculateExp(left_s, right1, ops[i], ID_counter++));
                } else {
                    right_s = factorMidCodes[num].getFactorResultID();
                    mid_three_unit_Exps.push_back(calculateExp(left_s, right_s, ops[i], ID_counter++));
                }
                num++;
            }
        }
    }

public:
    string getItemResultID() {
        return this->itemResultID;
    }

    string toString() {
        fill_mid_three_unit_exps();
        //把每个因子的运算表达式输出，同时将所有calculatExp的string表示输出
        string result = "";
        int factorNum = 0;
        if (factorMidCodes.size() == 1) {
            if (!(factorMidCodes[0].getFactorKind() == INTCON ||
                  factorMidCodes[0].getFactorKind() == CHARCON ||
                  factorMidCodes[0].getFactorKind() == PURE_IDENFR)) {
                result += factorMidCodes[0].toString();
            }
            this->itemResultID = factorMidCodes[0].getFactorResultID();
            return result;
        }
        for (int i = 0; i < mid_three_unit_Exps.size(); i++) {
            if (i == 0) {
                if (!(factorMidCodes[factorNum].getFactorKind() == INTCON ||
                    factorMidCodes[factorNum].getFactorKind() == CHARCON ||
                    factorMidCodes[factorNum].getFactorKind() == PURE_IDENFR)) {
                    result += factorMidCodes[factorNum].toString();
                }
                factorNum++;
                if (!(factorMidCodes[factorNum].getFactorKind() == INTCON ||
                      factorMidCodes[factorNum].getFactorKind() == CHARCON ||
                      factorMidCodes[factorNum].getFactorKind() == PURE_IDENFR)) {
                    result += factorMidCodes[factorNum].toString();
                }
                factorNum++;
                result += mid_three_unit_Exps[i].toString();
            } else {
                if (!(factorMidCodes[factorNum].getFactorKind() == INTCON ||
                      factorMidCodes[factorNum].getFactorKind() == CHARCON ||
                      factorMidCodes[factorNum].getFactorKind() == PURE_IDENFR)) {
                    result += factorMidCodes[factorNum].toString();
                }
                factorNum++;
                result += mid_three_unit_Exps[i].toString();
            }
        }
        this->itemResultID =
                this->mid_three_unit_Exps[mid_three_unit_Exps.size() - 1].getResultID();
        return result;
    }

    void init() {
        this->itemResultID = to_string(ID_counter++);
    }

    factorMidCode& getNowFactor() {
        return this->factorMidCodes[nowFactor];
    }

    factorMidCode* getNowFactorPointer() {
        return &(this->factorMidCodes[nowFactor]);
    }

    void addFactor() {
        factorMidCode factor = factorMidCode();
        factor.init();
        this->factorMidCodes.push_back(factor);
        this->nowFactor = factorMidCodes.size() - 1;
    }

    void addOp(OP op) {
        this->ops.push_back(op);
    }
};



/////////////////////////////////////////////////
///                 表达式                     ///
/////////////////////////////////////////////////
class ExpressionMidCode {
private:
    string expResultID;
    vector<itemMidCode> itemMidCodes;
    vector<OP> ops;
    vector<calculateExp> mid_three_unit_Exps;
    int nowItem;

    void fill_mid_three_unit_exps() {
        int num = 0;
        for (int i = 0; i < ops.size(); i++) {
            if (i == 0) {
                string left = itemMidCodes[num++].getItemResultID();
                string right = itemMidCodes[num++].getItemResultID();
                mid_three_unit_Exps.push_back(calculateExp(left, right, ops[i], ID_counter++));
            } else {
                string a = itemMidCodes[num++].getItemResultID();
                mid_three_unit_Exps.push_back(calculateExp(a,
                                mid_three_unit_Exps[mid_three_unit_Exps.size() - 1].getResultID(),
                                ops[i],
                                ID_counter++));
            }
        }
    }

public:
    string toString() {
        fill_mid_three_unit_exps();
        string result = "";
        if (this->itemMidCodes.size() == 1) {
            result += itemMidCodes[0].toString();
            this->expResultID = itemMidCodes[0].getItemResultID();
            return result;
        }
        int num = 0;
        for (int i = 0; i < mid_three_unit_Exps.size(); i++) {
            if (i == 0) {
                result += this->itemMidCodes[num++].toString();
                result += this->itemMidCodes[num++].toString();
                result += mid_three_unit_Exps[i].toString();
            } else {
                result += this->itemMidCodes[num++].toString();
                result += mid_three_unit_Exps[i].toString();
            }
        }
        return result;
    }

    string getExpResultID() { return this->expResultID; }

    void init() {
        this->expResultID = to_string(ID_counter++);
    }

    void addItem() {
        itemMidCode item = itemMidCode();
        item.init();
        this->itemMidCodes.push_back(item);
        this->nowItem = itemMidCodes.size() - 1;
    }

    void addOP(OP op) {
        this->ops.push_back(op);
    }

    itemMidCode& getNowItem() {
        return this->itemMidCodes[nowItem];
    }
};



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
        this->functionReturnValueID = to_string(ID_counter++);
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
            result += value.toString();
        }
        for (ExpressionMidCode value : this->parameterValues) {
            result += "push " + value.getExpResultID() + "\n";
        }
        return result + call + returnCode;
    }
};
#endif //COMPILER_EXPRESSIONMIDCODE_H
