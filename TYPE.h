//
// Created by 84443 on 2019/9/27.
//
#include <bits/stdc++.h>
#include "WordAnalysis/SearchLine.h"
#include "Errors/WordError.h"

using namespace std;

//////////////////////////////////////////////////////
///                                                ///
///                  全局变量定义                   ///
///                                                ///
//////////////////////////////////////////////////////
#ifndef COMPILER_TYPE_H_VAR
#define COMPILER_TYPE_H_VAR
/*
 * 其实从设计角度考虑这里是不应该放置变量的
 * 但是为了方便，这里只放置两个变量，一个存储整体代码，一个存储词法错误
 * 接下来的活可以放心交给语法部分了
 */
static string code;
WordError wordError;

#endif
//////////////////////////////////////////////////////
///                                                ///
///                  全局类型定义                   ///
///                                                ///
//////////////////////////////////////////////////////
#ifndef COMPILER_TYPE_H_TYPEDEF
#define COMPILER_TYPE_H_TYPEDEF

typedef string Word_Value;

typedef string Word_Type;

typedef pair<Word_Type, Word_Value> MY_WORD;

struct SINGLE_WORD {
    MY_WORD WORD;
    int line;
    SINGLE_WORD(Word_Type type, Word_Value value, int inputPoint) {
        WORD.first = type;
        WORD.second = value;
        line = searchLine(inputPoint, code);
    }
};

#endif
//////////////////////////////////////////////////////
///                                                ///
///                  全局宏定义                     ///
///                                                ///
//////////////////////////////////////////////////////

#define OUTPUT_AND_ADD(type, value, sstream) \
    word_vector.push_back(SINGLE_WORD(type, value, sstream.tellg()));

#define WORD_TYPE \
    Words[PointNum].WORD.first

#define WORD_VALUE \
    Words[PointNum].WORD.second

#define LINE \
    Words[PointNum].line

#define PRINT_WORD_AND_ADDPOINT \
    output << Words[PointNum].WORD.first << " " << Words[PointNum].WORD.second << endl; \
    PointNum++

//////////////////////////////////////////////////////
///                                                ///
///                  全局函数定义                   ///
///                                                ///
//////////////////////////////////////////////////////
#ifndef COMPILER_TYPE_H_FUNC
#define COMPILER_TYPE_H_FUNC

bool hasReturnValue(vector<SINGLE_WORD> Words, string FunctionName) {
    for (int i = 0; i < Words.size(); i++) {
        if (Words[i].WORD.second == FunctionName) {
            return !(Words[i - 1].WORD.second == "void");
        }
    }
    return false;
}

#endif
