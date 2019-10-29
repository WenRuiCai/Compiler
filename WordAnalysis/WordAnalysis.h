//
// Created by 84443 on 2019/9/27.
//

#ifndef COMPILER_WORDANALYSIS_H
#define COMPILER_WORDANALYSIS_H

#include "WordDealFunction.h"

class WordAnalysis {
private:
    vector<SINGLE_WORD> Words;

    void beginBuild(stringstream& input) {
        string data;
        while (input >> data) {
            if (!Deal_Single_Word_Part(input, data, Words)) {
                stringstream buffer;
                Cut_Component_With_Space(buffer, data, input.tellg());
                while (buffer >> data) {
                    if (!Deal_Single_Word_Part(input, data, Words)) {
                        wordError.addError(searchLine(input.tellg(), code));
                    }
                }
            }
        }
    }

public:
    WordAnalysis(ifstream& inputCode) {
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
        beginBuild(input);
    }

    vector<SINGLE_WORD> getWords() {
        return this->Words;
    }
};


#endif //COMPILER_WORDANALYSIS_H
