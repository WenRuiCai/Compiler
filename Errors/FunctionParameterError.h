//
// Created by 84443 on 2019/10/22.
//
#include "bits/stdc++.h"
#include "CompileError.h"
using namespace std;

#ifndef COMPILER_FUNCTIONPARAMETERERROR_H
#define COMPILER_FUNCTIONPARAMETERERROR_H


class FunctionParameterError {
private:
    vector<CompileError> functionParameterErrors;

public:
    void addError(char kind, int line) {
        CompileError compileError(kind, line);
        for (int i = 0; i < functionParameterErrors.size(); i++) {
            if (functionParameterErrors[i].errorKind == kind &&
                    functionParameterErrors[i].error_Happened_Line == line) {
                return;
            }
        }
        functionParameterErrors.push_back(compileError);
    }

    vector<CompileError> getErrors() {
        return this->functionParameterErrors;
    }
};

FunctionParameterError functionParameterError;
#endif //COMPILER_FUNCTIONPARAMETERERROR_H
