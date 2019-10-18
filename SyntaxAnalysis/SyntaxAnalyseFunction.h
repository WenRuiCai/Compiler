//
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
void No_Symbol_Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Parameters(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);
void Component_Centences(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output);

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
    while (Centence(Words, PointNum, output));
    output << "<语句列>" << endl;
    output << "<复合语句>" << endl;
}

//整数
void Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "PLUS" || WORD_TYPE == "MINU") {
        PRINT_WORD_AND_ADDPOINT;
    }
    No_Symbol_Number(Words, PointNum, output);
    output << "<整数>" << endl;
}

//无符号整数
void No_Symbol_Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "INTCON") {
        PRINT_WORD_AND_ADDPOINT;
        output << "<无符号整数>" << endl;
    }
}

//常量声明
void Const_Analysis(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE != "CONSTTK")
        return;
    while ("CONSTTK" == WORD_TYPE) {
        PRINT_WORD_AND_ADDPOINT;
        Const_Define(Words, PointNum, output);
    }
    output << "<常量说明>" << endl;
}

//常量定义
void Const_Define(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if ("INTTK" == WORD_TYPE) {
        PRINT_WORD_AND_ADDPOINT;
        while (true) {
            if (WORD_TYPE == "IDENFR") {
                PRINT_WORD_AND_ADDPOINT;
                if (WORD_TYPE == "ASSIGN") {
                    PRINT_WORD_AND_ADDPOINT;
                    Number(Words, PointNum, output);
                    if (WORD_TYPE == "SEMICN") {
                        output << "<常量定义>" << endl;
                        PRINT_WORD_AND_ADDPOINT;
                        break;
                    }
                    else if (WORD_TYPE == "COMMA") { PRINT_WORD_AND_ADDPOINT; continue; }
                }
            }
        }
    }
    else if ("CHARTK" == WORD_TYPE) {
        PRINT_WORD_AND_ADDPOINT;
        while (true) {
            if (WORD_TYPE == "IDENFR") {
                PRINT_WORD_AND_ADDPOINT;
                if (WORD_TYPE == "ASSIGN") {
                    PRINT_WORD_AND_ADDPOINT;
                    if (WORD_TYPE == "CHARCON") {
                        PRINT_WORD_AND_ADDPOINT;
                        if (WORD_TYPE == "SEMICN") {
                            output << "<常量定义>" << endl;
                            PRINT_WORD_AND_ADDPOINT;
                            break;
                        }
                        else if (WORD_TYPE == "COMMA") { PRINT_WORD_AND_ADDPOINT; continue; }
                    }
                }
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
            output << "<变量说明>" << endl;
            return;
        }
        PRINT_WORD_AND_ADDPOINT;
        Variable_Define(Words, PointNum, output);
    }
    output << "<变量说明>" << endl;
}

//变量定义
void Variable_Define(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    while (WORD_TYPE == "IDENFR") {
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "COMMA") { PRINT_WORD_AND_ADDPOINT; continue; }
        else if (WORD_TYPE == "SEMICN") {
            output << "<变量定义>" << endl;
            PRINT_WORD_AND_ADDPOINT;
            break;
        }
        else if (WORD_TYPE == "LBRACK") {
            PRINT_WORD_AND_ADDPOINT;
            No_Symbol_Number(Words, PointNum, output);
            if (WORD_TYPE == "RBRACK") {
                PRINT_WORD_AND_ADDPOINT;
                if (WORD_TYPE == "COMMA") {
                    PRINT_WORD_AND_ADDPOINT;
                    continue;
                }
                else if (WORD_TYPE == "SEMICN") {
                    output << "<变量定义>" << endl;
                    PRINT_WORD_AND_ADDPOINT;
                    break;
                }
            }
        }
    }
}

//函数参数列
void Parameters(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE == "RPARENT") {
        output << "<参数表>" << endl;
        return;
    }
    //参数声明
    while (WORD_TYPE == "CHARTK" || WORD_TYPE == "INTTK") {
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "IDENFR") {
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "COMMA") { PRINT_WORD_AND_ADDPOINT; }
            else {
                output << "<参数表>" << endl;
                return;
            }
        }
    }
}

//有返回值函数
bool Function_With_Return_Value(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    if (WORD_TYPE != "INTTK" && WORD_TYPE != "CHARTK")
        return false;
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "IDENFR") {
        PRINT_WORD_AND_ADDPOINT;
        output << "<声明头部>" << endl;
        if (WORD_TYPE == "LPARENT") {
            PRINT_WORD_AND_ADDPOINT;
            Parameters(Words, PointNum, output);
            if (WORD_TYPE == "RPARENT") {
                PRINT_WORD_AND_ADDPOINT;
                if (WORD_TYPE == "LBRACE") {
                    PRINT_WORD_AND_ADDPOINT;
                    Component_Centences(Words, PointNum, output);
                    if (WORD_TYPE == "RBRACE") {
                        PRINT_WORD_AND_ADDPOINT;
                        output << "<有返回值函数定义>" << endl;
                        return true;
                    }
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
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "LPARENT") {
            PRINT_WORD_AND_ADDPOINT;
            Parameters(Words, PointNum, output);
            if (WORD_TYPE == "RPARENT") {
                PRINT_WORD_AND_ADDPOINT;
                if (WORD_TYPE == "LBRACE") {
                    PRINT_WORD_AND_ADDPOINT;
                    Component_Centences(Words, PointNum, output);
                    if (WORD_TYPE == "RBRACE") {
                        PRINT_WORD_AND_ADDPOINT;
                        output << "<无返回值函数定义>" << endl;
                        return true;
                    }
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
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "LPARENT") {
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "RPARENT") {
                PRINT_WORD_AND_ADDPOINT;
                if (WORD_TYPE == "LBRACE") {
                    PRINT_WORD_AND_ADDPOINT;
                    Component_Centences(Words, PointNum, output);
                    if (WORD_TYPE == "RBRACE") {
                        PRINT_WORD_AND_ADDPOINT;
                        output << "<主函数>" << endl;
                        return;
                    }
                }
            }
        }
    }
}

//整个程序
void Program_Analysis(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output) {
    Const_Analysis(Words, PointNum, output);
    Variable_Analysis(Words, PointNum, output);
    while (Function_With_Return_Value(Words, PointNum, output) ||
           Function_Not_With_Return_Value(Words, PointNum, output));
    Function_Main(Words, PointNum, output);
    output << "<程序>" << endl;
}