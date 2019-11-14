//
// Created by 84443 on 2019/11/8.
//

#ifndef COMPILER_EXPRESSIONMIDCODE_H
#define COMPILER_EXPRESSIONMIDCODE_H

#include "ItemMid.h"

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
    bool mid_three_unit_Exps_hasFilled = false;

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
        this->mid_three_unit_Exps_hasFilled = true;
    }

public:
    bool hasOnlyOneItem() { return this->itemMidCodes.size() == 1;}

    string toString() {
        if (!mid_three_unit_Exps_hasFilled) {
            fill_mid_three_unit_exps();
        }
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
        this->expResultID =
                mid_three_unit_Exps[mid_three_unit_Exps.size() - 1].getResultID();
        return result;
    }

    string getExpResultID() { return this->expResultID; }

    void init() {
        this->expResultID = "T" + to_string(ID_counter++);
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
