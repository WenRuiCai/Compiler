#include "TYPE.h"
#include "WordAnalysis/WordAnalysis.h"
#include "SyntaxAnalysis/SyntaxAnalysis.h"

using namespace std;

int main() {
    ifstream inputCode;
    inputCode.open("testfile.txt");
    ofstream outputCode;
    outputCode.open("mips.txt");
    WordAnalysis wordAnalysis(inputCode);
    SyntaxAnalysis syntaxAnalysis(wordAnalysis.getWords(), outputCode);
    return 0;
}