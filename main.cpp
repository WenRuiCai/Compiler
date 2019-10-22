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
    int i = 0;
    return 0;
}