//
// Created by 84443 on 2019/9/29.
//
#include "../TYPE.h"
#include "../SymbolTable/SymbolTable.h"
#include "../Codemid/FunctionCallMid.h"

#ifndef COMPILER_EXPRESSIONDEAL_H
#define COMPILER_EXPRESSIONDEAL_H

ExpressionFlag expression_work(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output,
        ExpressionMidCode& expBlock);
ExpressionFlag expression(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output,
        int expflag, factorMidCode* factorMid, CentenceMid* centence);
ExpressionFlag item(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, ExpressionMidCode& expBlock);
ExpressionFlag factor(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, ExpressionMidCode& expBlock);

extern bool Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, int* intcon);
extern bool functionCall(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output,
        int isFactor, factorMidCode* factorMid, vector<CentenceMid*>* centenceBlock);

#endif //COMPILER_EXPRESSIONDEAL_H

ExpressionFlag factor(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, ExpressionMidCode& expBlock) {
    expBlock.getNowItem().addFactor();
    /*
     * 目前已经建立完了表达式的层次结构
     * 接下来需要在factor中进行三元式的生成
     * 比如，在因子仅为一个标识符的情况下，就不用Ti来表示了
     *      这里已经处理了因子是单个纯标识符,以及纯表达式的情况
     *      已经处理了因子是数组的情况
     *      已经处理了字符常量、數字常量的情况
     *      剩余：函数调用
     *      搞定~
     */
    ExpressionFlag flag = NONE_Express;
    if (WORD_TYPE == "IDENFR") {
        string name = WORD_VALUE;
        if (Words[PointNum + 1].WORD.first == "LPARENT") {
            expBlock.getNowItem().getNowFactor().setFunctionCall(new FunctionCallMidCode(name));
            expBlock.getNowItem().getNowFactor().factor_is_functioncall();
            functionCall(Words, PointNum, output, 1,
                    expBlock.getNowItem().getNowFactorPointer(), nullptr);

            expBlock.getNowItem().getNowFactor().setString_FUNCTION(
                    expBlock.getNowItem().getNowFactor().getFunctionCall().toString(),
                    expBlock.getNowItem().getNowFactor().getFunctionCall().getFunctionReturnValueID()
            );

            flag = symbolTable.findFunctionKind(name);
        }
        else {
            symbolTable.nameHasNotDefined(WORD_VALUE, LINE);
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "LBRACK") {
                int line = LINE;
                PRINT_WORD_AND_ADDPOINT;
                if (expression(Words, PointNum, output, 1,
                        expBlock.getNowItem().getNowFactorPointer(), nullptr) != INT_Express) {
                    symbolTable.addArrayIndexError(line);
                }
                if (WORD_TYPE == "RBRACK") {
                    PRINT_WORD_AND_ADDPOINT;
                } else {
                    symbolTable.loss_RBRACK_Error(PRE_WORD_LINE);
                }
                expBlock.getNowItem().getNowFactor().factor_is_array(name);

                expBlock.getNowItem().getNowFactor().setString_ARRAY(
                        expBlock.getNowItem().getNowFactor().getFactorExp().toString(),
                        expBlock.getNowItem().getNowFactor().getFactorExp().getExpResultID()
                );
            } else {
                expBlock.getNowItem().getNowFactor().factor_is_pure_idenfr(name);

                expBlock.getNowItem().getNowFactor().setString_CHAR_INT_IDENFR();
            }
            flag = symbolTable.find_ID_Kind(name);
        }
    }
    else if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        expression(Words, PointNum, output, 1,
                expBlock.getNowItem().getNowFactorPointer(), nullptr);
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
        expBlock.getNowItem().getNowFactor().factor_is_exp();

        expBlock.getNowItem().getNowFactor().setString_EXP(
                expBlock.getNowItem().getNowFactor().getFactorExp().toString(),
                expBlock.getNowItem().getNowFactor().getFactorExp().getExpResultID()
        );
        flag = INT_Express;
    }
    else if (WORD_TYPE == "CHARCON") {
        expBlock.getNowItem().getNowFactor().factor_is_charcon(WORD_VALUE);

        expBlock.getNowItem().getNowFactor().setString_CHAR_INT_IDENFR();
        PRINT_WORD_AND_ADDPOINT;
        flag = CHAR_Express;
    }
    else {
        int intcon1 = 0;
        Number(Words, PointNum, output, &intcon1);
        expBlock.getNowItem().getNowFactor().factor_is_intcon(intcon1);

        expBlock.getNowItem().getNowFactor().setString_CHAR_INT_IDENFR();
        flag = INT_Express;
    }
    //cout << "<因子>" << endl;
    return flag;
}

ExpressionFlag item(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, ExpressionMidCode& expBlock) {
    expBlock.addItem();
    int counter = 0;
    ExpressionFlag flag = factor(Words, PointNum, output, expBlock);
    counter++;
    /*
     * 每个item都由多个factor通过乘法运算符连接而成，同样的道理
     */
    while (WORD_TYPE == "MULT" || WORD_TYPE == "DIV") {
        expBlock.getNowItem().addOp((WORD_TYPE == "MULT") ? MULT :DIV);
        PRINT_WORD_AND_ADDPOINT;
        factor(Words, PointNum, output, expBlock);
        counter++;
    }
    //cout << "<项>" << endl;
    expBlock.getNowItem().toString();
    return (counter > 1) ? INT_Express : flag;
}

ExpressionFlag expression(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output,
        int expflag, factorMidCode* factorMid, CentenceMid* centence) {
    if (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        PRINT_WORD_AND_ADDPOINT;
    }
    if (expflag <= 0) {
        //expflag < 0仅存在于condition的情况
        if (expflag == -1) {
            IfelseBlock* ifelseBlock = static_cast<IfelseBlock *>(centence);
            ifelseBlock->setExp();
            ExpressionMidCode& xi = *ifelseBlock->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            cout << xi.toString();
            return f;
        } else if (expflag == -2) {
            WhileBlock* whileBlock = static_cast<WhileBlock *>(centence);
            whileBlock->setExp();
            ExpressionMidCode& xi = *whileBlock->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            return f;
        } else if (expflag == -3) {
            DoWhileBlock* doWhileBlock = static_cast<DoWhileBlock*>(centence);
            doWhileBlock->setExp();
            ExpressionMidCode& xi = *doWhileBlock->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            return f;
        } else if (expflag == -4) {
            ForLoopBlock* forLoopBlock = static_cast<ForLoopBlock*>(centence);
            forLoopBlock->setExp();
            ExpressionMidCode& xi = *forLoopBlock->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            cout << xi.toString();
            return f;
        }
        if (centence != nullptr && centence->kind == FOR) {
            ForLoopBlock* forLoopBlock = static_cast<ForLoopBlock*>(centence);
            forLoopBlock->setExp();
            ExpressionMidCode& xi = *forLoopBlock->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            return f;
        }
        if (centence != nullptr && centence->kind == ASSIGN) {
            AssignCentence* assignCentence = static_cast<AssignCentence*>(centence);
            assignCentence->setExp();
            ExpressionMidCode& xi = *assignCentence->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            return f;
        }
        if (centence != nullptr && centence->kind == PRINTF) {
            PrintfCentence* printfCentence = static_cast<PrintfCentence*>(centence);
            printfCentence->setExp();
            ExpressionMidCode& xi = *printfCentence->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            return f;
        }
        if (centence != nullptr && centence->kind == RETURN) {
            ReturnCentence* returnCentence = static_cast<ReturnCentence*>(centence);
            returnCentence->setExp();
            ExpressionMidCode& xi = *returnCentence->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            return f;
        }
        if (centence != nullptr && centence->kind == FUNCTIONCALL) {
            FunctionCallMidCode* functionCall = static_cast<FunctionCallMidCode*>(centence);
            functionCall->setExp();
            ExpressionMidCode& xi = *functionCall->getNowExp();
            ExpressionFlag f = expression_work(Words, PointNum, output, xi);
            return f;
        }
        cout << "\n-------------------Now is EXP--------------------------\n";
        symbolTable.getNowBlock().addExpressionMidCode();
        ExpressionFlag f = expression_work(Words, PointNum, output, symbolTable.getNowBlock().getNowExp());
        string s = symbolTable.getNowBlock().getNowExp().toString();
        if (s != "") cout << "exp :  " << s;
        else cout << "exp :  " << symbolTable.getNowBlock().getNowExp().getExpResultID();
        cout << "\n-------------------Now this EXP finished--------------------------\n";
        return f;
    } else if (expflag == 1) {
        factorMid->setFactor_exp(new ExpressionMidCode());
        factorMid->getFactorExp().init();
        return expression_work(Words, PointNum, output, factorMid->getFactorExp());
    }
    else {
        factorMid->getFunctionCall().setExp();
        return expression_work(Words, PointNum, output,
                               *factorMid->getFunctionCall().getNowExp());
    }
}

ExpressionFlag expression_work(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output,
        ExpressionMidCode& expBlock) {
    int counter = 0;
    ExpressionFlag flag = item(Words, PointNum, output, expBlock);
    /*
     * 表达式是由多个项通过加法运算符连接起来的，所以我们需要每个项
     * 这里只需要每个项的标号，然后通过加法运算符连接起来（自顶向下的思想）
     */
    counter++;
    while (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        expBlock.addOP((WORD_TYPE == "PLUS") ? ADD : SUB);
        PRINT_WORD_AND_ADDPOINT;
        item(Words, PointNum, output, expBlock);
        counter++;
    }
    //cout << "<表达式>" << endl;
    expBlock.toString();
    return (counter > 1) ? INT_Express : flag;
}