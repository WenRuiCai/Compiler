#include <string>
#include "WordAnalysis.h"
#include "SyntaxAnalysis.h"

using namespace std;

int main() {
    ifstream inputCode;
    inputCode.open("testfile.txt");
    ofstream outputCode;
    outputCode.open("output.txt");
    WordAnalysis wordAnalysis(inputCode, outputCode);
    SyntaxAnalysis syntaxAnalysis(wordAnalysis.getWords(), outputCode);
    return 0;
}