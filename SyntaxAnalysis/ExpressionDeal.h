//
// Created by 84443 on 2019/9/29.
//

#ifndef COMPILER_EXPRESSIONDEAL_H
#define COMPILER_EXPRESSIONDEAL_H

void expression(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void item(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void factor(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);

extern void Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
extern bool functionCall(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, int isFactor);

#endif //COMPILER_EXPRESSIONDEAL_H

void factor(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "IDENFR") {
        if (Words[PointNum + 1].first == "LPARENT") {
            functionCall(Words, PointNum, output, 1);
        }
        else {
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "LBRACK") {
                PRINT_WORD_AND_ADDPOINT;
                expression(Words, PointNum, output);
                if (WORD_TYPE == "RBRACK") {
                    PRINT_WORD_AND_ADDPOINT;
                }
            }
        }
    }
    else if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        expression(Words, PointNum, output);
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        }
    }
    else if (WORD_TYPE == "CHARCON") {
        PRINT_WORD_AND_ADDPOINT;
    }
    else {
        Number(Words, PointNum, output);
    }
    output << "<因子>" << endl;
}

void item(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    factor(Words, PointNum, output);
    while (WORD_TYPE == "MULT" || WORD_TYPE == "DIV") {
        PRINT_WORD_AND_ADDPOINT;
        factor(Words, PointNum, output);
    }
    output << "<项>" << endl;
}

void expression(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        PRINT_WORD_AND_ADDPOINT;
    }
    item(Words, PointNum, output);
    while (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        PRINT_WORD_AND_ADDPOINT;
        item(Words, PointNum, output);
    }
    output << "<表达式>" << endl;
}