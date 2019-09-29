//
// Created by 84443 on 2019/9/27.
//
#include <iostream>
using namespace std;

#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

typedef string Word_Value;

typedef string Word_Type;

typedef pair<Word_Type, Word_Value> SINGLE_WORD;

#define OUTPUT_AND_ADD(type, value) \
    word_vector.push_back(SINGLE_WORD(type, value))

#define WORD_TYPE \
    Words[PointNum].first

#define PRINT_WORD_AND_ADDPOINT \
    output << Words[PointNum].first << " " << Words[PointNum].second << endl; \
    PointNum++

bool hasReturnValue(vector<SINGLE_WORD> Words, string FunctionName) {
    for (int i = 0; i < Words.size(); i++) {
        if (Words[i].second == FunctionName) {
            return !(Words[i - 1].second == "void");
        }
    }
    return false;
}
#endif //COMPILER_TYPE_H
