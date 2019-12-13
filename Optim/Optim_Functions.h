//
// Created by 84443 on 2019/12/12.
//

#ifndef COMPILER_OPTIM_FUNCTIONS_H
#define COMPILER_OPTIM_FUNCTIONS_H

bool isConst(string name) {
    int num = 0;
    return isNum(name) || isCharCon(name, &num) || isConst(name, &num, nullptr);
}

bool IS_NOT_COMPARE_SYMBOL(string string1) {
    if (string1.find("PRINT") != string::npos && string1.find('"') != string::npos) {
        return true;
    }
    return string1.find(">") == string::npos && string1.find("<") == string::npos
    && string1.find("==") == string::npos && string1.find("!=") == string::npos;
}

/**
 * @param fourUnitExp: 单条中间代码四元式语句
 * @return : 这条四元式语句的定义变量
 * @details : 定义变量只可能出现在赋值语句当中，只需考虑赋值语句即可
 * @brief : 本函数用于活跃变量分析过程
 */
string get_SingleExp_Defined_Var(string fourUnitExp) {
    string result = "";
    string one, two, three, four, five;
    stringstream ss;
    ss << fourUnitExp; ss >> one >> two >> three >> four >> five;
    if (two == "=" && IS_NOT_COMPARE_SYMBOL(two)) {
        result = one;
    }
    return result;
}

/**
 * @param fourUnitExp: 单条中间代码四元式语句
 * @return : 这条四元式语句的使用变量
 * @details : 单条语句可能有多个使用变量，所以在这里使用一个vector存储
 * @brief : 本函数用于活跃变量分析过程
 */
vector<string> get_SingleExp_Used_Var(string fourUnitExp) {
    vector<string> result;
    string one, two, three, four, five;
    stringstream ss;
    ss << fourUnitExp; ss >> one >> two >> three >> four >> five;
    if (two == "=" && IS_NOT_COMPARE_SYMBOL(two)) {
        if (!(one.find("TEMP_VAR_CWR") != string::npos && three == "RET" && five.length() == 0)) {
            if (!isConst(three)) result.push_back(three);
            if (five.length() != 0 && !isConst(five)) result.push_back(five);
        }
    } else if (one == "push" || one == "ret") {
        if (!isConst(two) && two.length() > 0) result.push_back(two);
    } else if (one == "PRINT") {
        if (fourUnitExp.at(fourUnitExp.length() - 1) != '"') {
            string printVar = "";
            for (int x = fourUnitExp.length() - 1; x >= 0; x--) {
                if (fourUnitExp.at(x) != ' ') {
                    printVar = fourUnitExp.at(x) + printVar;
                } else break;
            }
            if (!isConst(printVar)) result.push_back(printVar);
        }
    } else if (!(IS_NOT_COMPARE_SYMBOL(two))) {
        if (!isConst(one)) result.push_back(one);
        if (!isConst(three)) result.push_back(three);
    }

    return result;
}

/**
 * @param fourUnitExps: 基本块中的所有四元式表达式
 * @return              在这个基本块中的所有需要使用的变量
 * @details: 这里导出的需要使用的变量用于DAG优化，保证优化后不会遗漏变量
 * @brief: 此函数用于基本块内优化
 * @callgraph : 此函数只在函数 getOptim_MidCode 中调用
 */
vector<string> getVar_GoingToBeUsed_InOneBlock(vector<string> fourUnitExps) {
    vector<string> var_GoingToBeUsed;
    for (string item : fourUnitExps) {
        string one, two, three, four, five;
        stringstream ss;    ss << item;
        ss >> one >> two >> three >> four >> five;
        if (two == "=" && IS_NOT_COMPARE_SYMBOL(two) && one.find("TEMP_VAR_CWR") == string::npos) {
            var_GoingToBeUsed.push_back(one);
        }
        if (one == "push" || one == "ret") {
            var_GoingToBeUsed.push_back(two);
        }
        if (one == "PRINT") {
            if (item.at(item.length() - 1) != '"') {
                string printVar = "";
                for (int x = item.length() - 1; x >= 0; x--) {
                    if (item.at(x) != ' ') {
                        printVar = item.at(x) + printVar;
                    } else break;
                }
                var_GoingToBeUsed.push_back(printVar);
            }
        }
        if (!(IS_NOT_COMPARE_SYMBOL(two))) {
            var_GoingToBeUsed.push_back(one);
            var_GoingToBeUsed.push_back(three);
        }
    }
    /*
     * 检查是否存在重复
     */
    for (int i = 0; i < var_GoingToBeUsed.size(); i++) {
        for (int j = i + 1; j < var_GoingToBeUsed.size(); j++) {
            if (var_GoingToBeUsed[j] == var_GoingToBeUsed[i]) {
                auto iter = var_GoingToBeUsed.begin();  int x = j;
                while (x--) iter++; var_GoingToBeUsed.erase(iter); j--;
            }
        }
    }
    /*
     * 检查是否是常量
     */
    for (int i = 0; i < var_GoingToBeUsed.size(); i++) {
        if (isConst(var_GoingToBeUsed[i])) {
            auto iter = var_GoingToBeUsed.begin();  int x = i;
            while (x--) iter++; var_GoingToBeUsed.erase(iter); i--;
        }
    }
    return var_GoingToBeUsed;
}

/**
 * @param fourUnitExps：      提取出的表达式片段
 * @param var_GoingToBeUsed:  基本块中的所有需要使用的变量
 * @return                    送入DAG优化中进行优化
 * @details:需要特别注意的是可能会出现跨基本块使用变量的情形（函数调用返回值可能在后面的基本块中使用）
 *          此时这条四元式形似于 "xxx = RET"
 * @brief: 此函数用于基本块内优化
 * @callgraph : 此函数只在函数 getOptim_MidCode 中调用
 */
string fourUnitFormat_To_DAG(vector<string> fourUnitExps, vector<string> var_GoingToBeUsed) {
    vector<string> var_GoingToBeUsed_InThisPart;
    for (string item : var_GoingToBeUsed) {
        int flag = 0;
        for (string exp : fourUnitExps) {
            stringstream ss; ss << exp;
            string one, two, three, four, five;
            ss >> one >> two >> three >> four >> five;
            if (one == item) {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            var_GoingToBeUsed_InThisPart.push_back(item);
        }
    }
    string DAGString = "";
    for (string s : fourUnitExps) DAGString += s + '\n';
    Exp_DAG_Optim expDagOptim = Exp_DAG_Optim();
    expDagOptim.preDeal(DAGString, var_GoingToBeUsed_InThisPart);
    if (fourUnitExps.size() == 1) { return DAGString; }
    return expDagOptim.getExp_Optim();
}

/*
 * 作用：一个基本块中可能有很多表达式，同时有printf或者scanf插入在中间，所以如果全部优化的话可能会造成中间丢失
 */
string getOptim_MidCode(vector<string> fourUnitExps) {
    string result = "";
    vector<string> var_GoingToBeUsed = getVar_GoingToBeUsed_InOneBlock(fourUnitExps);
    for (int i = 0; i < fourUnitExps.size(); i++) {
        if (fourUnitExps[i].find("PRINT") == string::npos &&
            fourUnitExps[i].find("=") != string::npos && IS_NOT_COMPARE_SYMBOL(fourUnitExps[i])) {
            vector<string> part;    int j;
            for (j = i; j < fourUnitExps.size(); j++) {
                if (fourUnitExps[j].find("PRINT") == string::npos &&
                    fourUnitExps[j].find("=") != string::npos &&
                    IS_NOT_COMPARE_SYMBOL(fourUnitExps[j])) {
                    part.push_back(fourUnitExps[j]);
                } else {
                    i = j - 1;
                    break;
                }
            }
            /*
             * 送去优化
             */
            result += fourUnitFormat_To_DAG(part, var_GoingToBeUsed);
            if (j == fourUnitExps.size()) {
                break;
            }
        } else {
            result += fourUnitExps[i] + '\n';
        }
    }
    return result;
}



#endif //COMPILER_OPTIM_FUNCTIONS_H
