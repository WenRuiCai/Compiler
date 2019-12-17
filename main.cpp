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

    string midCode = symbolTable.toString();
    cout << midCode;
    string optim_mipsCode = symbolTable.Optim_MidCode_And_Prepare_Mips();
    outputCode << ".data\n";
    for (Variable v : globalStrings) {
        string str = "";
        for (char c : v.string_var) {
            if (c == '\\')  str += '\\';
            str += c;
        }
        outputCode << "\t" + v.VariableName + ": .asciiz " + "\"" + str + "\"\n";
    }
    outputCode << ".text\n";
    outputCode << "jal main\nli $v0, 10\nsyscall\n";
    outputCode << optim_mipsCode;

    ofstream outputCode1;
    outputCode1.open("mips1.txt");
    stringstream mips_with_tmp;
    string mipsCode = symbolTable.toMips();
    mips_with_tmp << ".data\n";
    for (Variable v : globalStrings) {
        string str = "";
        for (char c : v.string_var) {
            if (c == '\\')  str += '\\';
            str += c;
        }
        mips_with_tmp << "\t" + v.VariableName + ": .asciiz " + "\"" + str + "\"\n";
    }
    mips_with_tmp << ".text\n";
    mips_with_tmp << "jal main\nli $v0, 10\nsyscall\n";
    mips_with_tmp << mipsCode;
    selectTempRegister(mips_with_tmp, outputCode1);
    return 0;
}