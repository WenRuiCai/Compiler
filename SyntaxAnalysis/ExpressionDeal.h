//
// Created by 84443 on 2019/9/29.
//
#include "../TYPE.h"
#include "../SymbolTable/SymbolTable.h"
#include "../Codemid/ExpressionMidCode.h"

#ifndef COMPILER_EXPRESSIONDEAL_H
#define COMPILER_EXPRESSIONDEAL_H

ExpressionFlag expression(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, int expflag);
ExpressionFlag item(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, ExpressionMidCode& expBlock);
ExpressionFlag factor(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, ExpressionMidCode& expBlock);

extern bool Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, int* intcon);
extern bool functionCall(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, int isFactor);

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
     */
    ExpressionFlag flag = NONE_Express;
    if (WORD_TYPE == "IDENFR") {
        string name = WORD_VALUE;
        if (Words[PointNum + 1].WORD.first == "LPARENT") {
            expBlock.getNowItem().getNowFactor().setFunctionCall(new FunctionCallMidCode(name));
            expBlock.getNowItem().getNowFactor().factor_is_functioncall();
            functionCall(Words, PointNum, output, 1);
            flag = symbolTable.findFunctionKind(name);
        }
        else {
            symbolTable.nameHasNotDefined(WORD_VALUE, LINE);
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "LBRACK") {
                int line = LINE;
                PRINT_WORD_AND_ADDPOINT;
                if (expression(Words, PointNum, output, 1) != INT_Express) {
                    symbolTable.addArrayIndexError(line);
                }
                if (WORD_TYPE == "RBRACK") {
                    PRINT_WORD_AND_ADDPOINT;
                } else {
                    symbolTable.loss_RBRACK_Error(PRE_WORD_LINE);
                }
                expBlock.getNowItem().getNowFactor().factor_is_array(name);
            } else {
                expBlock.getNowItem().getNowFactor().factor_is_pure_idenfr(name);
            }
            flag = symbolTable.find_ID_Kind(name);
        }
    }
    else if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        expression(Words, PointNum, output, 1);
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
        expBlock.getNowItem().getNowFactor().factor_is_exp();
        flag = INT_Express;
    }
    else if (WORD_TYPE == "CHARCON") {
        expBlock.getNowItem().getNowFactor().factor_is_charcon(WORD_VALUE);
        PRINT_WORD_AND_ADDPOINT;
        flag = CHAR_Express;
    }
    else {
        int intcon1 = 0;
        Number(Words, PointNum, output, &intcon1);
        expBlock.getNowItem().getNowFactor().factor_is_intcon(intcon1);
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
    return (counter > 1) ? INT_Express : flag;
}

ExpressionFlag expression(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, int expflag) {
    if (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        PRINT_WORD_AND_ADDPOINT;
    }
    if (expflag == 0) {
        symbolTable.getNowBlock().addExpressionMidCode();
    } else if (expflag == 1) {
        symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().setFactor_exp(new ExpressionMidCode());
        symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().getFactorExp().init();
    } else {
        symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().getFunctionCall().addParameterValue();
    }
    //得到当前的表达式块，给表达式最终的结果一个标号Ti
    int counter = 0;
    ExpressionFlag flag = item(Words, PointNum, output,
            (expflag == 0) ?
            symbolTable.getNowBlock().getNowExp() :
            ((expflag == 1) ?
            symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().getFactorExp() :
            symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().getFunctionCall().getNowParameterExp())
            );
    /*
     * 表达式是由多个项通过加法运算符连接起来的，所以我们需要每个项
     * 这里只需要每个项的标号，然后通过加法运算符连接起来（自顶向下的思想）
     */
    counter++;
    while (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {

        ((expflag == 0) ?
        symbolTable.getNowBlock().getNowExp() :
        ((expflag == 1) ?
        symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().getFactorExp() :
        symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().getFunctionCall().getNowParameterExp())
        ).
        addOP((WORD_TYPE == "PLUS") ? ADD : SUB);

        PRINT_WORD_AND_ADDPOINT;
        item(Words, PointNum, output,
                (expflag == 0) ?
                symbolTable.getNowBlock().getNowExp() :
                ((expflag == 1) ?
                symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().getFactorExp() :
                symbolTable.getNowBlock().getNowExp().getNowItem().getNowFactor().getFunctionCall().getNowParameterExp())
                );
        counter++;
    }
    //cout << "<表达式>" << endl;
    return (counter > 1) ? INT_Express : flag;
}