//
// Created by 84443 on 2019/11/15.
//

#ifndef COMPILER_CENTENCEMID_H
#define COMPILER_CENTENCEMID_H

#include "ExpressionMidCode.h"

enum conditionCompare {
    GRT,
    EQL,
    NOTEQL,
    GRT_EQL,
    LESS,
    LESS_EQL
};

enum CentenceKind {
    IFELSE,
    WHILE,
    DOWHILE,
    FOR,
    PRINTF,
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
     * 设置当前语句中的表达式，在FunctionBlock中可以查找到
     */
    virtual void setExp();
    /*
     * 当前语句现在正在填充的表达式
     */
    virtual ExpressionMidCode* getNowExp();
    /*
     * 输出中间代码的方法
     */
    virtual string toString();

};


#endif //COMPILER_CENTENCEMID_H
