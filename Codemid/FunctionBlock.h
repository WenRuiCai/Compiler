//
// Created by 84443 on 2019/11/8.
//

#ifndef COMPILER_FUNCTIONBLOCK_H
#define COMPILER_FUNCTIONBLOCK_H

#include "ExpressionMidCode.h"
#include "FunctionCallMid.h"
#include "IfelseBlock.h"
#include "WhileBlock.h"
#include "DoWhileBlock.h"
#include "ForLoopBlock.h"
#include "AssignCentence.h"
#include "PrintfCentence.h"
#include "ReturnCentence.h"
#include "ScanfCentece.h"
#include "../MIPSCode/Variable.h"

vector<Variable> nowFunctionVariables;
map<string, Variable> nowFunction_GetVar_byName_Map;
vector<TableItem> nowFunctionConsts;

class FunctionBlock {
    string kind;
    string functionName;
    vector<TableItem> parameters;
    vector<ExpressionMidCode> expressionMidCodes;
    vector<CentenceMid*> centences;
    int nowExpression;
    int nowCentence;
    vector<Variable> functionVariables;
    map<string, Variable> function_GetVar_byName_Map;
    vector<TableItem> functionConsts;

    void addCentence(vector<CentenceMid*>* centenceBlock, CentenceMid* centence) {
        centenceBlock->push_back(centence);
    }

public:
    string getName() { return this->functionName; }

    vector<TableItem>& getFunctionConsts() { return this->functionConsts;}

    vector<Variable>& getFunctionVariables() {
        return this->functionVariables;
    }

    map<string, Variable>& getVarMap() {
        return this->function_GetVar_byName_Map;
    }

    void functionBlock_addParameter(TableItem para) {
        this->parameters.push_back(para);
    }

    void set_Function_Kind(string kind) {
        this->kind = kind;
    }

    void set_Function_Name(string name) {
        this->functionName = name;
    }

    string toString() {
        nowFunction_GetVar_byName_Map = this->function_GetVar_byName_Map;
        nowFunctionConsts = this->functionConsts;
        nowFunctionVariables = this->functionVariables;

        string result = "";
        result += (this->kind == "INTTK") ? "int " :
                ((this->kind == "CHARTK") ? "char " : "void ");
        result += this->functionName + "()\n";
        for (TableItem item : this->parameters) {
            result += "para ";
            result += (item.type == "INTTK") ? "int " : "char ";
            result += item.name + "\n";
        }
        result += get_centences_component_string(this->centences);
        return result;
    }

    vector<CentenceMid*>* getCentenceBlock() {
        return &this->centences;
    }

    FunctionCallMidCode* addCentence_FunctionCall(vector<CentenceMid*>* centenceBlock, string s) {
        FunctionCallMidCode* functionCall = new FunctionCallMidCode(s);
        addCentence(centenceBlock, functionCall);
        this->nowCentence = this->centences.size() - 1;
        return functionCall;
    }

    IfelseBlock* addCentence_IF(vector<CentenceMid*>* centenceBlock) {
        IfelseBlock* ifelseBlock = new IfelseBlock();
        addCentence(centenceBlock, ifelseBlock);
        this->nowCentence = this->centences.size() - 1;
        return ifelseBlock;
    }

    WhileBlock* addCentence_WHILE(vector<CentenceMid*>* centenceBlock) {
        WhileBlock* whileBlock = new WhileBlock();
        addCentence(centenceBlock, whileBlock);
        this->nowCentence = this->centences.size() - 1;
        return whileBlock;
    }

    DoWhileBlock* addCentence_DOWHILE(vector<CentenceMid*>* centenceBlock) {
        DoWhileBlock* doWhileBlock = new DoWhileBlock();
        addCentence(centenceBlock, doWhileBlock);
        this->nowCentence = this->centences.size() - 1;
        return doWhileBlock;
    }

    ForLoopBlock* addCentence_FOR(vector<CentenceMid*>* centenceBlock) {
        ForLoopBlock* forLoopBlock = new ForLoopBlock();
        addCentence(centenceBlock, forLoopBlock);
        this->nowCentence = this->centences.size() - 1;
        return forLoopBlock;
    }

    AssignCentence* addCentence_ASSIGN(vector<CentenceMid*>* centenceBlock) {
        AssignCentence* assignCentence = new AssignCentence();
        addCentence(centenceBlock, assignCentence);
        this->nowCentence = this->centences.size() - 1;
        return assignCentence;
    }

    ReturnCentence* addCentence_RETURN(vector<CentenceMid*>* centenceBlock) {
        ReturnCentence* returnCentence = new ReturnCentence();
        addCentence(centenceBlock, returnCentence);
        this->nowCentence = this->centences.size() - 1;
        return returnCentence;
    }

    ScanfCentece* addCentence_SCANF(vector<CentenceMid*>* centenceBlock) {
        ScanfCentece* scanfCentece = new ScanfCentece();
        addCentence(centenceBlock, scanfCentece);
        this->nowCentence = this->centences.size() - 1;
        return scanfCentece;
    }

    PrintfCentence* addCentence_printf(vector<CentenceMid*>* centenceBlock) {
        PrintfCentence* printfCentence = new PrintfCentence();
        addCentence(centenceBlock, printfCentence);
        this->nowCentence = this->centences.size() - 1;
        return printfCentence;
    }

    CentenceMid* getNowCentence() {
        return this->centences[nowCentence];
    }

    ExpressionMidCode& getNowExp() {
        return this->expressionMidCodes[nowExpression];
    }

    void addExpressionMidCode() {
        ExpressionMidCode expMidCode = ExpressionMidCode();
        expMidCode.init();
        this->expressionMidCodes.push_back(expMidCode);
        this->nowExpression = expressionMidCodes.size() - 1;
    }

};

string get_centences_component_string(vector<CentenceMid*>& centencesBlock);

string get_centences_component_string(vector<CentenceMid*>& centencesBlock) {
    string result = "";
    for (CentenceMid* centence : centencesBlock) {
        switch (centence->kind) {
            case IFELSE:
                result += static_cast<IfelseBlock*>(centence)->toString();
                break;
            case WHILE:
                result += static_cast<WhileBlock*>(centence)->toString();
                break;
            case DOWHILE:
                result += static_cast<DoWhileBlock*>(centence)->toString();
                break;
            case FOR:
                result += static_cast<ForLoopBlock*>(centence)->toString();
                break;
            case PRINTF:
                result += static_cast<PrintfCentence*>(centence)->toString();
                break;
            case SCANF:
                result += static_cast<ScanfCentece*>(centence)->toString();
                break;
            case FUNCTIONCALL:
                result += static_cast<FunctionCallMidCode*>(centence)->toString();
                break;
            case ASSIGN:
                result += static_cast<AssignCentence*>(centence)->toString();
                break;
            case RETURN:
                result += static_cast<ReturnCentence*>(centence)->toString();
                break;
        }
    }
    return result;
}

#endif //COMPILER_FUNCTIONBLOCK_H
