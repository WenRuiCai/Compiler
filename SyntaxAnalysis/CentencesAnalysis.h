//
// Created by 84443 on 2019/9/28.
//
#include "ExpressionDeal.h"
using namespace std;

#ifndef COMPILER_CENTENCESANALYSIS_H
#define COMPILER_CENTENCESANALYSIS_H

static bool hasReturnCentence = false;

bool Centence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
bool ifCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
bool whileCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
bool forCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
bool doWhileCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
bool functionCall(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output,
        int isFactor, factorMidCode* factorMid, vector<CentenceMid*>* centenceBlock);
bool assignCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
bool printfCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
bool scanfCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
bool returnCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock);
void condition(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, CentenceMid* centence);
void parameterValueList(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<string>& types,
        bool isFactor, factorMidCode* factorMid, FunctionCallMidCode* functionCall);

extern bool No_Symbol_Number(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, int* intcon);

#endif //COMPILER_CENTENCESANALYSIS_H

/*
 * 处理语句列
 * 此时的PointNum已经指向了语句列的第一个词位置
 * 因此只需考虑后续动作即可
 */

void parameterValueList(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<string>& types,
        bool isFactor, factorMidCode* factorMid, FunctionCallMidCode* functionCall) {
    if (WORD_TYPE == "RPARENT") {
        //cout << "<值参数表>" << endl;
        return;
    }

    ExpressionFlag flag;
    flag = expression(Words, PointNum, output,
            (!isFactor) ? 0 : 2,
            (isFactor) ? factorMid : nullptr,
            (isFactor) ? nullptr : functionCall);
    types.push_back((flag == INT_Express) ? "INTTK" : "CHARTK");

    while (WORD_TYPE == "COMMA") {
        PRINT_WORD_AND_ADDPOINT;
        flag = expression(Words, PointNum, output,
                (!isFactor) ? 0 : 2,
                (isFactor) ? factorMid : nullptr,
                (isFactor) ? nullptr : functionCall);
        types.push_back((flag == INT_Express) ? "INTTK" : "CHARTK");
    }
    //cout << "<值参数表>" << endl;
}

void condition(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, CentenceMid* centence) {
    int flag = 0, line;

    if (expression(Words, PointNum, output, centence->kind, nullptr, centence) == CHAR_Express)
        flag = 1;

    if (WORD_TYPE == "LSS" || WORD_TYPE == "LEQ"
        || WORD_TYPE == "GRE" || WORD_TYPE == "GEQ"
        || WORD_TYPE == "EQL" || WORD_TYPE == "NEQ") {
        line = LINE;
        switch (centence->kind) {
            case IFELSE:
                static_cast<IfelseBlock*>(centence)->setCmp(WORD_TYPE);
                break;
            case WHILE:
                static_cast<WhileBlock*>(centence)->setCmp(WORD_TYPE);
                break;
            case DOWHILE:
                static_cast<DoWhileBlock*>(centence)->setCmp(WORD_TYPE);
                break;
            case FOR:
                static_cast<ForLoopBlock*>(centence)->setCmp(WORD_TYPE);
                break;
        }
        PRINT_WORD_AND_ADDPOINT;
        ExpressionFlag flag1 = expression(Words, PointNum, output, centence->kind, nullptr, centence);
        if (flag1 == CHAR_Express) {
            if (flag != 1) {
                symbolTable.addConditionTypeError(line);
            }
        } else if (flag1 == INT_Express) {
            if (flag == 1) {
                symbolTable.addConditionTypeError(line);
            }
        }
    }
    //cout << "<条件>" << endl;
}

bool ifCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (WORD_TYPE != "IFTK")
        return false;
    PRINT_WORD_AND_ADDPOINT;
    IfelseBlock* ifelseBlock = symbolTable.getNowBlock().addCentence_IF(centenceBlock);
    if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        condition(Words, PointNum, output, ifelseBlock);
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
        Centence(Words, PointNum, output, ifelseBlock->getCentenceBlock());
        ifelseBlock->conditionSatisfieldBlock_Finished();
        if (WORD_TYPE == "ELSETK") {
            PRINT_WORD_AND_ADDPOINT;
            Centence(Words, PointNum, output, ifelseBlock->getCentenceBlock());
            //cout << "<条件语句>" << endl;
            return true;
        }
        else {
            //cout << "<条件语句>" << endl;
            return true;
        }
    }
    return false;
}

bool whileCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (WORD_TYPE != "WHILETK")
        return false;
    PRINT_WORD_AND_ADDPOINT;
    WhileBlock* whileBlock = symbolTable.getNowBlock().addCentence_WHILE(centenceBlock);
    if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        condition(Words, PointNum, output, whileBlock);
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
        Centence(Words, PointNum, output, whileBlock->getCentenceBlock());
        //cout << "<循环语句>" << endl;
        return true;
    }
    return false;
}

bool doWhileCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (WORD_TYPE != "DOTK")
        return false;
    PRINT_WORD_AND_ADDPOINT;
    DoWhileBlock* doWhileBlock = symbolTable.getNowBlock().addCentence_DOWHILE(centenceBlock);
    Centence(Words, PointNum, output, doWhileBlock->getCentenceBlock());
    if (WORD_TYPE == "WHILETK") {
        PRINT_WORD_AND_ADDPOINT;
    } else {
        symbolTable.loss_WHILETK_Error(PRE_WORD_LINE);
    }
    if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        condition(Words, PointNum, output, doWhileBlock);
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
        //cout << "<循环语句>" << endl;
        return true;
    }

    return false;
}

bool forCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (WORD_TYPE != "FORTK")
        return false;
    PRINT_WORD_AND_ADDPOINT;
    ForLoopBlock* forLoopBlock = symbolTable.getNowBlock().addCentence_FOR(centenceBlock);
    if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "IDENFR") {
            symbolTable.nameHasNotDefined(WORD_VALUE, LINE);
            symbolTable.hasAssignForConst(WORD_VALUE, LINE);
            forLoopBlock->set_IDENFR(WORD_VALUE);
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "ASSIGN") {
                PRINT_WORD_AND_ADDPOINT;
                expression(Words, PointNum, output, 0, nullptr, forLoopBlock);
                if (WORD_TYPE == "SEMICN") {
                    PRINT_WORD_AND_ADDPOINT;
                } else {
                    symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
                }
                condition(Words, PointNum, output, forLoopBlock);
                if (WORD_TYPE == "SEMICN") {
                    PRINT_WORD_AND_ADDPOINT; //;
                } else {
                    symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
                }
                symbolTable.nameHasNotDefined(WORD_VALUE, LINE);
                symbolTable.hasAssignForConst(WORD_VALUE, LINE);
                PRINT_WORD_AND_ADDPOINT; //IDENFR
                PRINT_WORD_AND_ADDPOINT; //=
                symbolTable.nameHasNotDefined(WORD_VALUE, LINE);
                PRINT_WORD_AND_ADDPOINT; //IDENFR
                PRINT_WORD_AND_ADDPOINT; //+ | -
                No_Symbol_Number(Words, PointNum, output, nullptr); //STEP
                //cout << "<步长>" << endl;
                if (WORD_TYPE == "RPARENT") {
                    PRINT_WORD_AND_ADDPOINT;
                } else {
                    symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
                }
                Centence(Words, PointNum, output, forLoopBlock->getCentenceBlock());
                //cout << "<循环语句>" << endl;
                return true;
            }
        }
    }
    return false;
}

bool assignCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (WORD_TYPE != "IDENFR" || Words[PointNum + 1].WORD.first == "LPARENT")
        return false;
    symbolTable.nameHasNotDefined(WORD_VALUE, LINE);
    symbolTable.hasAssignForConst(WORD_VALUE, LINE);
    AssignCentence* assignCentence = symbolTable.getNowBlock().addCentence_ASSIGN(centenceBlock);
    assignCentence->setIDENFR(WORD_VALUE);
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "LBRACK") {
        int line = LINE;
        PRINT_WORD_AND_ADDPOINT;
        assignCentence->ID_isArray();
        if (expression(Words, PointNum, output, 0, nullptr, assignCentence) != INT_Express) {
            symbolTable.addArrayIndexError(line);
        }
        if (WORD_TYPE == "RBRACK") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RBRACK_Error(PRE_WORD_LINE);
        }
    }
    if (WORD_TYPE == "ASSIGN") {
        PRINT_WORD_AND_ADDPOINT;
        expression(Words, PointNum, output, 0, nullptr, assignCentence);
    }
    //cout << "<赋值语句>" << endl;
    if (WORD_TYPE == "SEMICN") {
        PRINT_WORD_AND_ADDPOINT;
    } else {
        symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
    }
    return true;
}

bool printfCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (WORD_TYPE != "PRINTFTK")
        return false;
    PrintfCentence* printfCentence = symbolTable.getNowBlock().addCentence_printf(centenceBlock);
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        if (WORD_TYPE == "STRCON") {
            printfCentence->setString(WORD_VALUE);
            PRINT_WORD_AND_ADDPOINT;
            //cout << "<字符串>" << endl;
            if (WORD_TYPE == "COMMA") {
                PRINT_WORD_AND_ADDPOINT;
                expression(Words, PointNum, output, 0, nullptr, printfCentence);
            }
        }
        else {
            expression(Words, PointNum, output, 0, nullptr, printfCentence);
        }
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
        if (WORD_TYPE == "SEMICN") {
            //cout << "<写语句>" << endl;
            PRINT_WORD_AND_ADDPOINT;
            return true;
        } else {
            symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
            return true;
        }
    }
    return false;
}

bool scanfCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (WORD_TYPE != "SCANFTK")
        return false;
    ScanfCentece* scanfCentece = symbolTable.getNowBlock().addCentence_SCANF(centenceBlock);
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        while (WORD_TYPE == "IDENFR") {
            symbolTable.nameHasNotDefined(WORD_VALUE, LINE);
            symbolTable.hasAssignForConst(WORD_VALUE, LINE);
            scanfCentece->setIDENFR(WORD_VALUE);
            PRINT_WORD_AND_ADDPOINT;
            if (WORD_TYPE == "COMMA") {
                PRINT_WORD_AND_ADDPOINT;
            }
        }
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
        if (WORD_TYPE == "SEMICN") {
            //cout << "<读语句>" << endl;
            PRINT_WORD_AND_ADDPOINT;
            return true;
        } else {
            symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
            return true;
        }
    }
    return false;
}

bool returnCentence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (WORD_TYPE != "RETURNTK") {
        return false;
    }
    ReturnCentence* returnCentence = symbolTable.getNowBlock().addCentence_RETURN(centenceBlock);
    hasReturnCentence = true;
    ExpressionFlag flag = NONE_Express;
    int line = LINE;
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        flag = expression(Words, PointNum, output, 0, nullptr, returnCentence);
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
    }
    if (WORD_TYPE == "SEMICN") {
        //cout << "<返回语句>" << endl;
        PRINT_WORD_AND_ADDPOINT;
        symbolTable.returnCentenceIsCorrect(flag, line);
        return true;
    } else {
        symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
        return true;
    }
    return false;
}

/*
 * 如何区分有返回值和无返回值？？？？？
 * 搜索标识符前的类型符
 */
bool functionCall(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output,
        int isFactor, factorMidCode* factorMid, vector<CentenceMid*>* centenceBlock) {
    string name = WORD_VALUE;
    int line = LINE;
    if (WORD_TYPE != "IDENFR" || Words[PointNum + 1].WORD.first != "LPARENT")
        return false;
    symbolTable.functionHasNotDefined(name, line);
    FunctionCallMidCode* functionCall = nullptr;
    if (!isFactor) {
        functionCall = symbolTable.getNowBlock().addCentence_FunctionCall(centenceBlock, name);
    }
    PRINT_WORD_AND_ADDPOINT;
    if (WORD_TYPE == "LPARENT") {
        PRINT_WORD_AND_ADDPOINT;
        vector<string> types;
        parameterValueList(Words, PointNum, output, types, isFactor, factorMid, functionCall);
        if (!symbolTable.parameterNumHasError(name, line, types.size())) {
            symbolTable.parameterTypeHasError(name, line, types);
        }
        if (WORD_TYPE == "RPARENT") {
            PRINT_WORD_AND_ADDPOINT;
        } else {
            symbolTable.loss_RPARENT_Error(PRE_WORD_LINE);
        }
        if (hasReturnValue(Words, name)) {
            if (!isFactor) functionCall->functionHasReturnValue();
            //cout << "<有返回值函数调用语句>" << endl;
        }
        else {
            if (!isFactor) functionCall->functionHasNotReturnValue();
            //cout << "<无返回值函数调用语句>" << endl;
        }
        if (!isFactor) {
            if (WORD_TYPE == "SEMICN") {
                PRINT_WORD_AND_ADDPOINT;
            } else {
                symbolTable.loss_SEMICN_Error(PRE_WORD_LINE);
            }
        }
    }
    return true;
}

bool Centence(vector<SINGLE_WORD>& Words, int& PointNum, ofstream& output, vector<CentenceMid*>* centenceBlock) {
    if (ifCentence(Words, PointNum, output, centenceBlock)) { /*cout << "<语句>" << endl;*/ return true; }
    if (whileCentence(Words, PointNum, output, centenceBlock)) { /*cout << "<语句>" << endl;*/ return true; }
    if (forCentence(Words, PointNum, output, centenceBlock)) { /*cout << "<语句>" << endl;*/ return true; }
    if (doWhileCentence(Words, PointNum, output, centenceBlock)) { /*cout << "<语句>" << endl;*/ return true; }
    if (functionCall(Words, PointNum, output, 0, nullptr, centenceBlock)) {
        /*cout << "<语句>" << endl;*/ return true;
    }
    if (assignCentence(Words, PointNum, output, centenceBlock)) { /*cout << "<语句>" << endl;*/ return true; }
    if (printfCentence(Words, PointNum, output, centenceBlock)) { /*cout << "<语句>" << endl;*/ return true; }
    if (scanfCentence(Words, PointNum, output, centenceBlock)) { /*cout << "<语句>" << endl;*/ return true; }
    if (returnCentence(Words, PointNum, output, centenceBlock)) { /*cout << "<语句>" << endl;*/ return true; }
    if (WORD_TYPE == "LBRACE") {
        PRINT_WORD_AND_ADDPOINT;
        while (Centence(Words, PointNum, output, centenceBlock));
        //cout << "<语句列>" << endl;
        if (WORD_TYPE == "RBRACE") {
            PRINT_WORD_AND_ADDPOINT;
            //cout << "<语句>" << endl;
            return true;
        }
    }
    if (WORD_TYPE == "SEMICN") {
        PRINT_WORD_AND_ADDPOINT;
        //cout << "<语句>" << endl;
        return true;
    }
    return false;
}