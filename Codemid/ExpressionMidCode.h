//
// Created by 84443 on 2019/11/8.
//

#ifndef COMPILER_EXPRESSIONMIDCODE_H
#define COMPILER_EXPRESSIONMIDCODE_H

#include "calculateExp.h"

class ExpressionMidCode;



/////////////////////////////////////////////////
///                 因子                      ///
/////////////////////////////////////////////////
class factorMidCode {
    enum FactorKind{
        EXP,
        PURE_IDENFR,
        PURE_ARRAY,
        ARRAY_WITH_EXP,
        FUNCTION
    };
private:
    string factorResultID;
    ExpressionMidCode* factor_exp = NULL;
    string IDENFR;
    FactorKind factorKind;

public:
    void setFactor_exp(ExpressionMidCode* pointer) {
        this->factor_exp = pointer;
        this->factorKind = EXP;
    }

    void factor_is_functioncall() {}

    void factor_is_array() {}

    void factor_is_pure_idenfr(string name) {
        this->IDENFR = name;
        this->factorKind = PURE_IDENFR;
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

public:
    void init() {
        this->itemResultID = to_string(ID_counter++);
    }

    factorMidCode& getNowFactor() {
        return this->factorMidCodes[nowFactor];
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

public:
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

#endif //COMPILER_EXPRESSIONMIDCODE_H
