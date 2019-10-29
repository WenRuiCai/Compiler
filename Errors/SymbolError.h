//
// Created by 84443 on 2019/10/23.
//
#include "CompileError.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

#ifndef COMPILER_SYMBOLERROR_H
#define COMPILER_SYMBOLERROR_H


class SymbolError {
private:
    vector<CompileError> symbolErrors;

public:
    void addError(char kind, int line) {
        CompileError compileError(kind, line);
        for (int i = 0; i < symbolErrors.size(); i++) {
            if (symbolErrors[i].errorKind == kind &&
                symbolErrors[i].error_Happened_Line == line) {
                return;
            }
        }
        symbolErrors.push_back(compileError);
    }

    vector<CompileError> getErrors() {
        return this->symbolErrors;
    }
};

SymbolError symbolError;

#endif //COMPILER_SYMBOLERROR_H
