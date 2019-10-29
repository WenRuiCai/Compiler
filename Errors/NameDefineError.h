//
// Created by 84443 on 2019/10/18.
//
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "CompileError.h"
using namespace std;

#ifndef COMPILER_NAMEDEFINEERROR_H
#define COMPILER_NAMEDEFINEERROR_H
/*
 * 包括了：
 *      名字重定义
 *      名字未定义
 *      给const常量赋值
 */
class NameDefineError {
private:
    vector<CompileError> nameDefineErrors;

public:
    void addError(char kind, int line) {
        CompileError compileError(kind, line);
        for (int i = 0; i < nameDefineErrors.size(); i++) {
            if (nameDefineErrors[i].errorKind == kind &&
                nameDefineErrors[i].error_Happened_Line == line) {
                return;
            }
        }
        nameDefineErrors.push_back(compileError);
    }

    vector<CompileError> getErrors() {
        return this->nameDefineErrors;
    }
};

NameDefineError nameDefineError;

#endif //COMPILER_NAMEDEFINEERROR_H
