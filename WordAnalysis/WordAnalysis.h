//
// Created by 84443 on 2019/9/27.
//

#ifndef COMPILER_WORDANALYSIS_H
#define COMPILER_WORDANALYSIS_H

#include <iostream>
#include <fstream>
#include <string>
#include "WordDealFunction.h"
#include <vector>
#include "../TYPE.h"

class WordAnalysis {
private:
    vector<SINGLE_WORD> Words;

    void beginBuild(stringstream& input, ofstream& outputFile) {
        string data;
        while (input >> data) {
            if (!Deal_Single_Word_Part(input, outputFile, data, Words)) {
                stringstream buffer;
                Cut_Component_With_Space(buffer, data);
                while (buffer >> data) {
                    Deal_Single_Word_Part(input, outputFile, data, Words);
                }
            }
        }
    }

public:
    WordAnalysis(ifstream& inputCode, ofstream& outputFile) {
        char each; int flag = 0;
        while ((each = inputCode.get()) != EOF) {
            if (each == '"') {
                code += ((flag == 0) ? " \"" : "\" ");
                flag = (flag == 0) ? 1 : 0;
            } else {
                code += each;
            }
        }
        stringstream input;
        input << code;
        beginBuild(input, outputFile);
    }

    vector<SINGLE_WORD> getWords() {
        return this->Words;
    }
};


#endif //COMPILER_WORDANALYSIS_H
