//
// Created by 84443 on 2019/9/27.
//

#ifndef COMPILER_WORDDEALFUNCTION_H
#define COMPILER_WORDDEALFUNCTION_H

#include "../TYPE.h"

using namespace std;

bool dealString(stringstream& input, ofstream& output, string data,
                vector<SINGLE_WORD> &word_vector) {
    int flag = 0;
    if (data[0] == '"') {
        int i;
        for (i = 1; i < data.length(); i++) {
            if (data[i] == '"') {
                flag = 1;
                break;
            }
        }
        if (flag == 1) { //字符串不含空格，可以直接返回
            OUTPUT_AND_ADD("STRCON", data.substr(1, i - 1), input);
        } else { //此时字符串可是含有空格的哟
            string thisString = data.substr(1);
            char getChar;
            while ((getChar = input.get()) != '"') {
                thisString += getChar;
                ////词法错误：包含非法字符
                if (!(getChar == 32 || getChar == 33 || (getChar >= 35 && getChar <= 126))) {
                    wordError.addError(searchLine(input.tellg(), code));
                }
                ////////////////////////
            }
            OUTPUT_AND_ADD("STRCON", thisString, input);
        }
        return true;
    } else {
        return false;
    }
}

bool isString_PureNumber_Or_PureIdentify_And_Deal(stringstream& input, ofstream& output,
        string data, vector<SINGLE_WORD> &word_vector) {
    if ((data[0] >= '0' && data[0] <= '9') || (data[0] == '-' || data[0] == '+')) {
        //纯数字串
        for (int i = 1; i < data.length(); i++) {
            if (!(data[i] >= '0' && data[i] <= '9')) {
                return false;
            }
        }
        switch (data[0]) {
            case '+':
                OUTPUT_AND_ADD("PLUS", "+", input);
                OUTPUT_AND_ADD("INTCON", data.substr(1), input);
                break;
            case '-':
                OUTPUT_AND_ADD("MINU", "-", input);
                OUTPUT_AND_ADD("INTCON", data.substr(1), input);
                break;
            default:
                OUTPUT_AND_ADD("INTCON", data, input);
                break;
        }
        return true;
    } else if ((data[0] >= 'a' && data[0] <= 'z') || (data[0] >= 'A' && data[0] <= 'Z') || data[0] == '_') {
        //纯标识符
        for (int i = 1; i < data.length(); i++) {
            if (!((data[i] >= 'a' && data[i] <= 'z') ||
                  (data[i] >= 'A' && data[i] <= 'Z') || (data[i] == '_') || (data[i] >= '0' && data[i] <= '9'))) {
                return false;
            }
        }
        OUTPUT_AND_ADD("IDENFR", data, input);
    } else {
        return false;
    }
    return true;
}

bool Deal_Single_Word_Part(stringstream& input, ofstream& output,
                    string data, vector<SINGLE_WORD> &word_vector) {
    if (data == "const") {
        OUTPUT_AND_ADD("CONSTTK", data, input);
    } else if (data == "int") {
        OUTPUT_AND_ADD("INTTK", data, input);
    } else if (data == "char") {
        OUTPUT_AND_ADD("CHARTK", data, input);
    } else if (data == "void") {
        OUTPUT_AND_ADD("VOIDTK", data, input);
    } else if (data == "main") {
        OUTPUT_AND_ADD("MAINTK", data, input);
    } else if (data == "if") {
        OUTPUT_AND_ADD("IFTK", data, input);
    } else if (data == "else") {
        OUTPUT_AND_ADD("ELSETK", data, input);
    } else if (data == "do") {
        OUTPUT_AND_ADD("DOTK", data, input);
    } else if (data == "while") {
        OUTPUT_AND_ADD("WHILETK", data, input);
    } else if (data == "for") {
        OUTPUT_AND_ADD("FORTK", data, input);
    } else if (data == "scanf") {
        OUTPUT_AND_ADD("SCANFTK", data, input);
    } else if (data == "printf") {
        OUTPUT_AND_ADD("PRINTFTK", data, input);
    } else if (data == "return") {
        OUTPUT_AND_ADD("RETURNTK", data, input);
    } else if (data == "+") {
        OUTPUT_AND_ADD("PLUS", data, input);
    } else if (data == "-") {
        OUTPUT_AND_ADD("MINU", data, input);
    } else if (data == "*") {
        OUTPUT_AND_ADD("MULT", data, input);
    } else if (data == "/") {
        OUTPUT_AND_ADD("DIV", data, input);
    } else if (data == "<") {
        OUTPUT_AND_ADD("LSS", data, input);
    } else if (data == "<=") {
        OUTPUT_AND_ADD("LEQ", data, input);
    } else if (data == ">") {
        OUTPUT_AND_ADD("GRE", data, input);
    } else if (data == ">=") {
        OUTPUT_AND_ADD("GEQ", data, input);
    } else if (data == "==") {
        OUTPUT_AND_ADD("EQL", data, input);
    } else if (data == "!=") {
        OUTPUT_AND_ADD("NEQ", data, input);
    } else if (data == "=") {
        OUTPUT_AND_ADD("ASSIGN", data, input);
    } else if (data == ";") {
        OUTPUT_AND_ADD("SEMICN", data, input);
    } else if (data == ",") {
        OUTPUT_AND_ADD("COMMA", data, input);
    } else if (data == "(") {
        OUTPUT_AND_ADD("LPARENT", data, input);
    } else if (data == ")") {
        OUTPUT_AND_ADD("RPARENT", data, input);
    } else if (data == "[") {
        OUTPUT_AND_ADD("LBRACK", data, input);
    } else if (data == "]") {
        OUTPUT_AND_ADD("RBRACK", data, input);
    } else if (data == "{") {
        OUTPUT_AND_ADD("LBRACE", data, input);
    } else if (data == "}") {
        OUTPUT_AND_ADD("RBRACE", data, input);
    } else if (data[0] == '\'' && data[2] == '\'' && data.length() == 3) {
        if (!(data[1] == '+' || data[1] == '-' || data[1] == '*' || data[1] == '/' ||
              data[1] == '_' || (data[1] >= 'a' && data[1] <= 'z') ||
              (data[1] >= 'A' && data[1] <= 'Z') || (data[1] >= '0' && data[1] <= '9'))) {
            wordError.addError(searchLine(input.tellg(), code));
        }
        OUTPUT_AND_ADD("CHARCON", data.substr(1, 1), input);
    } else if (!isString_PureNumber_Or_PureIdentify_And_Deal(input, output, data, word_vector) &&
                !dealString(input, output, data, word_vector)) {
        return false;
    }
    return true;
}

void Cut_Component_With_Space(stringstream& stringstream1, string nowData) {
    string newData = "";
    for (int pointNum = 0; pointNum < nowData.length(); pointNum++) {
        char i = nowData[pointNum];
        switch (i) {
            case ';':
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case ',':
            case '+':
            case '-':
            case '*':
            case '/':
                newData = newData + " " + i + " ";
                break;
            case '\'':
                newData = newData + " '" + nowData[pointNum + 1] + "' ";
                pointNum += 2;
                break;
            case '=':
                if (nowData[pointNum + 1] == '=') {
                    newData = newData + " == ";
                    pointNum++;
                } else {
                    newData = newData + " " + i + " ";
                }
                break;
            case '<':
            case '>':
                if (nowData[pointNum + 1] == '=') {
                    newData = newData + " " + i + "=" + " ";
                    pointNum++;
                } else {
                    newData = newData + " " + i + " ";
                }
                break;
            case '!':
                newData = newData + " != ";
                pointNum += 1;
                break;
            default:
                newData = newData + i;
                break;
        }
    }
    stringstream1 << newData;
    return;
}

#endif //COMPILER_WORDDEALFUNCTION_H
