//
// Created by 84443 on 2019/9/27.
//
#include <iostream>
#include "WordAnalysis/SearchLine.h"

using namespace std;

#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

typedef string Word_Value;

typedef string Word_Type;

typedef pair<Word_Type, Word_Value> MY_WORD;

static string code;

struct SINGLE_WORD {
    MY_WORD WORD;
    int line;
    SINGLE_WORD(Word_Type type, Word_Value value, int inputPoint) {
        WORD.first = type;
        WORD.second = value;
        line = searchLine(inputPoint, code);
    }
};

#define OUTPUT_AND_ADD(type, value, sstream) \
    word_vector.push_back(SINGLE_WORD(type, value, sstream.tellg()))

#define WORD_TYPE \
    Words[PointNum].WORD.first

#define PRINT_WORD_AND_ADDPOINT \
    output << Words[PointNum].WORD.first << " " << Words[PointNum].WORD.second << endl; \
    PointNum++

bool hasReturnValue(vector<SINGLE_WORD> Words, string FunctionName) {
    for (int i = 0; i < Words.size(); i++) {
        if (Words[i].WORD.second == FunctionName) {
            return !(Words[i - 1].WORD.second == "void");
        }
    }
    return false;
}
#endif //COMPILER_TYPE_H
