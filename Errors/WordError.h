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

#ifndef COMPILER_WORDERROR_H
#define COMPILER_WORDERROR_H


class WordError {
private:
    vector<CompileError> WordErrors;

public:
    void addError(int line) {
        CompileError error('a', line);
        for (int i = 0; i < WordErrors.size(); i++) {
            if (WordErrors[i].error_Happened_Line == line) {
                return;
            }
        }
        WordErrors.push_back(error);
        //cout << "word error : " << line << endl;
    }

    vector<CompileError> getErrors() {
        return this->WordErrors;
    }
};


#endif //COMPILER_WORDERROR_H
