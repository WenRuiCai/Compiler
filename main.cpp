#include "TYPE.h"
#include "WordAnalysis/WordAnalysis.h"
#include "SyntaxAnalysis/SyntaxAnalysis.h"

using namespace std;

int main() {
    ifstream inputCode;
    inputCode.open("testfile.txt");
    ofstream outputCode;
    outputCode.open("error.txt");
    WordAnalysis wordAnalysis(inputCode);
    SyntaxAnalysis syntaxAnalysis(wordAnalysis.getWords(), outputCode);
    vector<CompileError> allErrors;
    for (CompileError error : wordError.getErrors()) {
        allErrors.push_back(error);
    }
    for (CompileError error : nameDefineError.getErrors()) {
        allErrors.push_back(error);
    }
    for (CompileError error : functionParameterError.getErrors()) {
        allErrors.push_back(error);
    }
    for (CompileError error : symbolError.getErrors()) {
        allErrors.push_back(error);
    }
    while (!allErrors.empty()) {
        auto iterator = allErrors.begin();
        CompileError error = *(iterator);
        for (auto iter = allErrors.begin(); iter != allErrors.end(); iter++) {
            if ((*iter).error_Happened_Line < error.error_Happened_Line) {
                iterator = iter;
                error = (*iterator);
            }
        }
        outputCode << error.error_Happened_Line << " " << error.errorKind << endl;
        allErrors.erase(iterator);
    }
    return 0;
}