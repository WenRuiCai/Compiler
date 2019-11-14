//
// Created by 84443 on 2019/11/8.
//

#ifndef COMPILER_CALCULATEEXP_H
#define COMPILER_CALCULATEEXP_H

#include "bits/stdc++.h"
using namespace std;

static int ID_counter = 0;

enum OP{
    ADD,
    MULT,
    SUB,
    DIV
};

//  z = x op y
class calculateExp {
private:
    int result;     string result_id;
    int leftNum;    string leftNum_id;
    OP op;
    int rightNum;   string rightNum_id;
    bool isLeftId, isRightId;

public:
    calculateExp(string left, string right, OP op, int resultId) {
        this->result_id = "T" + to_string(resultId); this->op = op;
        this->leftNum_id = left;  this->rightNum_id = right;
        this->isLeftId = true;  this->isRightId = true;
    }

    calculateExp(string left, int right, OP op, int resultId) {
        this->result_id = "T" + to_string(resultId); this->op = op;
        this->leftNum_id = left;  this->rightNum = right;
        this->isLeftId = true;  this->isRightId = false;
    }

    calculateExp(int left, string right, OP op, int resultId) {
        this->result_id = "T" + to_string(resultId); this->op = op;
        this->leftNum = left;  this->rightNum_id = right;
        this->isLeftId = false; this->isRightId = true;
    }

    calculateExp(int left, int right, OP op, int resultId) {
        this->result_id = "T" + to_string(resultId); this->op = op;
        this->leftNum = left;  this->rightNum = right;
        this->isLeftId = false; this->isRightId = false;
    }

    string getResultID() { return this->result_id; }

    string toString() {
        string a = result_id + " = ";
        if (isLeftId) a += leftNum_id; else a += leftNum;
        a += " ";
        if (op == ADD) a += "+"; else if (op == SUB) a += '-'; else if (op == DIV) a += '/'; else if (op == MULT) a += '*';
        a += " ";
        if (isRightId) a += rightNum_id; else a += rightNum;
        return a + '\n';
    }
};

#endif //COMPILER_CALCULATEEXP_H
