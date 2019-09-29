//
// Created by 84443 on 2019/9/27.
//

#ifndef COMPILER_SYNTAXANALYSIS_H
#define COMPILER_SYNTAXANALYSIS_H
#include "WordAnalysis.h"
#include "SyntaxAnalyseFunction.h"
#include <vector>

using namespace std;

class SyntaxAnalysis {
private:
    vector<SINGLE_WORD> Words;
    int PointNum;

    void BeginAnalyse(ofstream& output) {
        Program_Analysis(Words, PointNum, output);
    }

public:
    SyntaxAnalysis(vector<SINGLE_WORD> Words, ofstream& output) {
        this->Words = Words;
        PointNum = 0;
        BeginAnalyse(output);
    }
};


#endif //COMPILER_SYNTAXANALYSIS_H
