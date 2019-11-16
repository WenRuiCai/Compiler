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

class FunctionBlock {
    vector<ExpressionMidCode> expressionMidCodes;
    vector<CentenceMid*> centences;
    int nowExpression;
    int nowCentence;

    void addCentence(vector<CentenceMid*>* centenceBlock, CentenceMid* centence) {
        centenceBlock->push_back(centence);
    }

public:
    vector<CentenceMid*>* getCentenceBlock() {
        return &this->centences;
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


#endif //COMPILER_FUNCTIONBLOCK_H
