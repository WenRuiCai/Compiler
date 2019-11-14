//
// Created by 84443 on 2019/11/14.
//

#ifndef COMPILER_FACTORMID_H
#define COMPILER_FACTORMID_H


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
        factorResultID = "t" + to_string(ID_counter++);
    }
};


#endif //COMPILER_FACTORMID_H
