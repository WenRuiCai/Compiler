//
// Created by 84443 on 2019/9/29.
//
#include "../TYPE.h"
#include "../SymbolTable/SymbolTable.h"

#ifndef COMPILER_EXPRESSIONDEAL_H
#define COMPILER_EXPRESSIONDEAL_H

ExpressionFlag expression(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
ExpressionFlag item(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
ExpressionFlag factor(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);

extern void Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
extern bool functionCall(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, int isFactor);

#endif //COMPILER_EXPRESSIONDEAL_H

ExpressionFlag factor(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    ExpressionFlag flag;
    if (WORD_TYPE == "IDENFR") {
        string name = WORD_VALUE;
        if (Words[PointNum + 1].WORD.first == "LPARENT") {
            functionCall(Words, PointNum, output, 1);
            flag = symbolTable.findFunctionKind(name);
        }
        else {
            symbolTable.nameHasNotDefined(WORD_VALUE, LINE);
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "LBRACK") {
                PRINT_WORD_AND_ADDPOINT;
                expression(Words, PointNum, output);
                if (WORD_TYPE == "RBRACK") {
                    PRINT_WORD_AND_ADDPOINT;
                }
            }
            flag = symbolTable.find_ID_Kind(name);
        }
    }
    else if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        expression(Words, PointNum, output);
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
            flag = INT_Express;
        }
    }
    else if (WORD_TYPE == "CHARCON") {
        PRINT_WORD_AND_ADDPOINT;
        flag = CHAR_Express;
    }
    else {
        Number(Words, PointNum, output);
        flag = INT_Express;
    }
    output << "<因子>" << endl;
    return flag;
}

ExpressionFlag item(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    int counter = 0;
    ExpressionFlag flag = factor(Words, PointNum, output);
    counter++;
    while (WORD_TYPE == "MULT" || WORD_TYPE == "DIV") {
        PRINT_WORD_AND_ADDPOINT;
        factor(Words, PointNum, output);
        counter++;
    }
    output << "<项>" << endl;
    return (counter > 1) ? INT_Express : flag;
}

ExpressionFlag expression(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        PRINT_WORD_AND_ADDPOINT;
    }
    int counter = 0;
    ExpressionFlag flag = item(Words, PointNum, output);
    counter++;
    while (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        PRINT_WORD_AND_ADDPOINT;
        item(Words, PointNum, output);
        counter++;
    }
    output << "<表达式>" << endl;
    return (counter > 1) ? INT_Express : flag;
}