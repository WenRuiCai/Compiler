//
// Created by 84443 on 2019/11/14.
//

#ifndef COMPILER_ITEMMID_H
#define COMPILER_ITEMMID_H

#include "FactorMid.h"
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
    bool mid_three_unit_Exps_hasFilled = false;

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
        mid_three_unit_Exps_hasFilled = true;
    }

public:
    string getItemResultID() {
        return this->itemResultID;
    }

    string toString() {
        if (!mid_three_unit_Exps_hasFilled) {
            fill_mid_three_unit_exps();
        }
        //把每个因子的运算表达式输出，同时将所有calculatExp的string表示输出
        string result = "";
        int factorNum = 0;
        //如果项只由一个因子组成，那么当前项的string就是factor的string，当前项的id就是factor的id
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
        this->itemResultID = "T" + to_string(ID_counter++);
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


#endif //COMPILER_ITEMMID_H
