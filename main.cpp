#include "TYPE.h"
#include "WordAnalysis/WordAnalysis.h"
#include "SyntaxAnalysis/SyntaxAnalysis.h"

using namespace std;

int main() {
    ifstream inputCode;
    inputCode.open("testfile.txt");
    ofstream outputCode;
    outputCode.open("output.txt");
    WordAnalysis wordAnalysis(inputCode, outputCode);
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
    int a;
    /*
     * 错误项h还存在很多问题，每个if分支如果不存在return语句那么就会报错
     * 错误项f存在问题，char和char类型比较是允许出现的（会不会是我只要表达式是char就判错？？）
     */
    return 0;
}