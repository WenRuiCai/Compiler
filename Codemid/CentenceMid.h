//
// Created by 84443 on 2019/11/15.
//

#ifndef COMPILER_CENTENCEMID_H
#define COMPILER_CENTENCEMID_H

#include "ExpressionMidCode.h"
#include "MiddleCode.h"

enum conditionCompare {
    GRT,
    EQL,
    NOTEQL,
    GRT_EQL,
    LESS,
    LESS_EQL
};

enum CentenceKind {
    IFELSE = -1,
    WHILE = -2,
    DOWHILE = -3,
    FOR = -4,
    PRINTF = 0,
    SCANF,
    FUNCTIONCALL,
    ASSIGN,
    RETURN
};

/*
 * 所有语句（除函数调用语句）的共同父类
 * 循环语句、赋值语句、条件语句、写语句、返回语句必须实现这里的方法
 * 读语句、函数调用语句不用
 * kind表示具体的语句种类
 */
class CentenceMid {
public:
    CentenceKind kind;

    /*
     * if、循环等语句都包含语句列，我们要建立语句列的递归结构，这里可以得到当前语句的直接下属语句列
     */
    vector<CentenceMid*>* getCentenceBlock();
    /*
     * 设置当前语句中的表达式，在FunctionBlock中可以查找到
     */
    void setExp();
    /*
     * 当前语句现在正在填充的表达式
     */
    ExpressionMidCode* getNowExp();
    /*
     * 输出中间代码的方法
     */
    string toString();

};


#endif //COMPILER_CENTENCEMID_H
