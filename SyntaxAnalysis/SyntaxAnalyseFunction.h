﻿//
// Created by 84443 on 2019/9/28.
//
#include "CentencesAnalysis.h"
#ifndef COMPILER_SYNTAXANALYSEFUNCTION_H
#define COMPILER_SYNTAXANALYSEFUNCTION_H

void Program_Analysis(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Const_Analysis(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Variable_Analysis(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Const_Define(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Variable_Define(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
bool Function_With_Return_Value(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
bool Function_Not_With_Return_Value(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Function_Main(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
bool No_Symbol_Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
bool Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Parameters(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Component_Centences(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
extern bool hasReturnCentence;
#endif //COMPILER_SYNTAXANALYSEFUNCTION_H

//复合语句
void Component_Centences(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    Const_Analysis(Words, PointNum, output);
    Variable_Analysis(Words, PointNum, output);
    ////////////////////////////////////
    ///                              ///
    ///   接下来是最复杂的语句列分析    ///
    ///                              ///
    ////////////////////////////////////
    hasReturnCentence = false;
    while (Centence(Words, PointNum, output));
    cout << "<语句列>" << endl;
    cout << "<复合语句>" << endl;
    symbolTable.noReturnCentenceError(hasReturnCentence, LINE);
}

//整数
bool Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        PRINT_WORD_AND_ADDPOINT;
    }
    if (No_Symbol_Number(Words, PointNum, output)) {
        cout << "<整数>" << endl;
        return true;
    }
    return false;
}

//无符号整数
bool No_Symbol_Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "INTCON") {
        PRINT_WORD_AND_ADDPOINT;
        cout << "<无符号整数>" << endl;
        return true;
    }
    return false;
}

//常量声明
void Const_Analysis(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE != "CONSTTK")
        return;
    while ("CONSTTK" == WORD_TYPE) {
        PRINT_WORD_AND_ADDPOINT;
        Const_Define(Words, PointNum, output);
    }
    cout << "<常量说明>" << endl;
}

//常量定义
void Const_Define(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if ("INTTK" == WORD_TYPE) {
        PRINT_WORD_AND_ADDPOINT;
        while (true) {
            if (WORD_TYPE == "IDENFR") {
                symbolTable.nowLevelAddItem("INTTK", WORD_VALUE, CONST, LINE, 0);
                PRINT_WORD_AND_ADDPOINT;
            }
            if (WORD_TYPE == "ASSIGN") {
                PRINT_WORD_AND_ADDPOINT;
            }
            if (!Number(Words, PointNum, output)) {
                if (WORD_TYPE == "CHARCON") {
                    PRINT_WORD_AND_ADDPOINT;
                } else {
                    symbolTable.addAssignValueError(LINE);
                }
            }
            if (WORD_TYPE == "SEMICN") {
                cout << "<常量定义>" << endl;
                PRINT_WORD_AND_ADDPOINT;
                break;
            }
            else if (WORD_TYPE == "COMMA") { PRINT_WORD_AND_ADDPOINT; continue; }
            else {
                symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
                break;
            }
        }
    }
    else if ("CHARTK" == WORD_TYPE) {
        PRINT_WORD_AND_ADDPOINT;
        while (true) {
            if (WORD_TYPE == "IDENFR") {
                symbolTable.nowLevelAddItem("CHARTK", WORD_VALUE, CONST, LINE, 0);
                PRINT_WORD_AND_ADDPOINT;
            }
            if (WORD_TYPE == "ASSIGN") {
                PRINT_WORD_AND_ADDPOINT;
            }
            if (WORD_TYPE == "CHARCON") {
                PRINT_WORD_AND_ADDPOINT;
            } else if (!Number(Words, PointNum, output)) {
                symbolTable.addAssignValueError(LINE);
            }
            if (WORD_TYPE == "SEMICN") {
                cout << "<常量定义>" << endl;
                PRINT_WORD_AND_ADDPOINT;
                break;
            }
            else if (WORD_TYPE == "COMMA") { PRINT_WORD_AND_ADDPOINT; continue; }
            else {
                symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
                break;
            }
        }
    }
}

//变量声明
void Variable_Analysis(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if ((WORD_TYPE != "INTTK" && WORD_TYPE != "CHARTK") ||
        (Words[PointNum + 2].WORD.first == "LPARENT")) {
        return;
    }
    while ("INTTK" == WORD_TYPE || "CHARTK" == WORD_TYPE) {
        if (Words[PointNum + 2].WORD.first == "LPARENT") {
            cout << "<变量说明>" << endl;
            return;
        }
        PRINT_WORD_AND_ADDPOINT;
        Variable_Define(Words, PointNum, output);
    }
    cout << "<变量说明>" << endl;
}

//变量定义
void Variable_Define(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    string thistype = Words[PointNum - 1].WORD.first;
    ///如果标识符出现词法错误，在此处我的处理是将其删除，
    /// 所以如果标识符出现了词法错误后，语法分析就不能够检测到标识符
    /// Update ：应该解决了
    bool hasWrongId = (WORD_TYPE == "COMMA" || WORD_TYPE == "SEMICN" || WORD_TYPE == "LBRACK");
    while (WORD_TYPE == "IDENFR" || hasWrongId) {
        string thisValue = WORD_VALUE;
        int thisline = LINE;
        if (!hasWrongId) {
            PRINT_WORD_AND_ADDPOINT;
        }
        if (WORD_TYPE == "COMMA") {
            if (!hasWrongId)
                symbolTable.nowLevelAddItem(thistype, thisValue, VAR, thisline, 0);
            PRINT_WORD_AND_ADDPOINT;
            hasWrongId = (WORD_TYPE == "COMMA" || WORD_TYPE == "SEMICN" || WORD_TYPE == "LBRACK");
            continue;
        }
        else if (WORD_TYPE == "SEMICN") {
            if (!hasWrongId)
                symbolTable.nowLevelAddItem(thistype, thisValue, VAR, thisline, 0);
            cout << "<变量定义>" << endl;
            PRINT_WORD_AND_ADDPOINT;
            break;
        }
        else if (WORD_TYPE == "LBRACK") {
            if (!hasWrongId)
                symbolTable.nowLevelAddItem(thistype, thisValue, VAR, thisline, 1);
            PRINT_WORD_AND_ADDPOINT;
            No_Symbol_Number(Words, PointNum, output);
            if (WORD_TYPE == "RBRACK") {
                PRINT_WORD_AND_ADDPOINT;
            } else {
                symbolTable.loss_RBRACK_Error(PRE_WORD_LINE);
            }
            if (WORD_TYPE == "COMMA") {
                PRINT_WORD_AND_ADDPOINT;
                hasWrongId = (WORD_TYPE == "COMMA" || WORD_TYPE == "SEMICN" || WORD_TYPE == "LBRACK");
                continue;
            }
            else if (WORD_TYPE == "SEMICN") {
                cout << "<变量定义>" << endl;
                PRINT_WORD_AND_ADDPOINT;
                break;
            } else {
                symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
                break;
            }
        } else {
            symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
            break;
        }
    }
}

//函数参数列
void Parameters(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "RPARENT") {
        cout << "<参数表>" << endl;
        return;
    }
    //参数声明
    while (WORD_TYPE == "CHARTK" || WORD_TYPE == "INTTK") {
        string thistype = WORD_TYPE;
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "IDENFR") {
            symbolTable.nowLevelAddItem(thistype, WORD_VALUE, PARA, LINE, 0);
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "COMMA") { PRINT_WORD_AND_ADDPOINT; }
            else {
                cout << "<参数表>" << endl;
                return;
            }
        }
    }
}

//有返回值函数
bool Function_With_Return_Value(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE != "INTTK" && WORD_TYPE != "CHARTK")
        return false;
    string thistype = WORD_TYPE;
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "IDENFR") {
        symbolTable.nowLevelAddItem(thistype, WORD_VALUE, FUNC, LINE, 0);
        symbolTable.addLevel();
        PRINT_WORD_AND_ADDPOINT;
        cout << "<声明头部>" << endl;
        if (WORD_TYPE == "LPARENT") {
            PRINT_WORD_AND_ADDPOINT;
            Parameters(Words, PointNum, output);
            if (WORD_TYPE == "RPARENT") {
                PRINT_WORD_AND_ADDPOINT;
            } else {
                symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
            }
            if (WORD_TYPE == "LBRACE") {
                PRINT_WORD_AND_ADDPOINT;
                Component_Centences(Words, PointNum, output);
                if (WORD_TYPE == "RBRACE") {
                    PRINT_WORD_AND_ADDPOINT;
                    cout << "<有返回值函数定义>" << endl;
                    symbolTable.dropOutLevel();
                    return true;
                }
            }

        }
    }
    return false;
}

//无返回值函数
bool Function_Not_With_Return_Value(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE != "VOIDTK" || Words[PointNum + 1].WORD.first == "MAINTK")
        return false;
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "IDENFR") {
        symbolTable.nowLevelAddItem("VOIDTK", WORD_VALUE, FUNC, LINE, 0);
        symbolTable.addLevel();
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "LPARENT") {
            PRINT_WORD_AND_ADDPOINT;
            Parameters(Words, PointNum, output);
            if (WORD_TYPE == "RPARENT") {
                PRINT_WORD_AND_ADDPOINT;
            } else {
                symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
            }
            if (WORD_TYPE == "LBRACE") {
                PRINT_WORD_AND_ADDPOINT;
                Component_Centences(Words, PointNum, output);
                if (WORD_TYPE == "RBRACE") {
                    PRINT_WORD_AND_ADDPOINT;
                    cout << "<无返回值函数定义>" << endl;
                    symbolTable.dropOutLevel();
                    return true;
                }
            }
        }
    }
    return false;
}

//主函数
void Function_Main(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "MAINTK") {
        symbolTable.nowLevelAddItem("VOIDTK", WORD_VALUE, MAIN, LINE, 0);
        symbolTable.addLevel();
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "LPARENT") {
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "RPARENT") {
                PRINT_WORD_AND_ADDPOINT;
            } else {
                symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
            }
            if (WORD_TYPE == "LBRACE") {
                PRINT_WORD_AND_ADDPOINT;
                Component_Centences(Words, PointNum, output);
                if (WORD_TYPE == "RBRACE") {
                    PRINT_WORD_AND_ADDPOINT;
                    cout << "<主函数>" << endl;
                    symbolTable.dropOutLevel();
                    return;
                }
            }
        }
    }
}

//整个程序
void Program_Analysis(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    symbolTable.addLevel();
    Const_Analysis(Words, PointNum, output);
    Variable_Analysis(Words, PointNum, output);
    while (Function_With_Return_Value(Words, PointNum, output) ||
           Function_Not_With_Return_Value(Words, PointNum, output));
    Function_Main(Words, PointNum, output);
    cout << "<程序>" << endl;
}