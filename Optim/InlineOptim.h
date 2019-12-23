//
// Created by 84443 on 2019/12/23.
//

#ifndef COMPILER_INLINEOPTIM_H
#define COMPILER_INLINEOPTIM_H

string getFunctionMidCode(string name);
bool getFunctionCanBeInlined(string name, vector<string>& changedGlobalVars, vector<string>& parasName);

vector<string> global_Variable_Changed(string midCode) {
    vector<string> result;
    for (int i = 1; i <= getLineNum(midCode); i++) {
        stringstream ss;    ss << getExpInLine(i, midCode);
        string one, two;    ss >> one >> two;
        string changedVar;
        if (two == "=" && IS_NOT_COMPARE_SYMBOL(two)) {
            changedVar = one;
            if (changedVar.find('[') != string::npos) {
                changedVar = changedVar.substr(0, changedVar.find('['));
            }
        } else if (one == "SCANF") {
            changedVar = two;
        }
        for (Variable variable : globalVariable) {
            if (variable.VariableName == changedVar) {
                result.push_back(changedVar);
            }
        }
    }
    return result;
}

bool functionHasNoCall_Or_MidReturn(string midCode) {
    for (int i = 1; i <= getLineNum(midCode); i++) {
        string nowLine = getExpInLine(i, midCode);
        string one, two; stringstream ss; ss << nowLine; ss >> one >> two;
        if (one == "call" && two != "=") {
            return false;
        }
        if (one == "ret" && two != "=" && i != getLineNum(midCode)) {
            return false;
        }
    }
    return true;
}

/**
 * @param functionName: 检查是否可以内联的函数名字functionName
 * @details: 要满足能够内联，首先函数的canBeInlined属性必须为真
 *           其次，要被内联的函数如果修改了全局变量，那么全局变量一定不能和要使用内联的函数局部变量重名
 */
bool functionCanBeInlined(string functionName, vector<string>& parasName) {
    vector<string> changedGlobalVars;
    changedGlobalVars.clear();
    if (!getFunctionCanBeInlined(functionName, changedGlobalVars, parasName)) {
        return false;
    }
    for (string s : changedGlobalVars) {
        for (Variable variable : nowFunctionVariables) {
            if (variable.VariableName == s) {
                return false;
            }
        }
        for (TableItem item : nowFunctionConsts) {
            if (item.name == s) {
                return false;
            }
        }
    }
    return true;
}

string transLateInlineCode(string functionMidCode, vector<string> para,
        vector<string> fake_para, vector<string> functionPara, string& retId) {
    string result = "";
    map<string, string> functionPara2fake_para;
    assert(functionPara.size() == fake_para.size());
    for (int i = 0; i < functionPara.size(); i++) {
        functionPara2fake_para.insert(make_pair(functionPara[i], fake_para[i]));
    }
    for (int i = 0; i < para.size(); i++) {
        result += fake_para[i] + " = " + para[i] + '\n';
    }
    for (int i = 1; i <= getLineNum(functionMidCode); i++) {
        string fourUnitExp = getExpInLine(i, functionMidCode);
        stringstream ss; ss << fourUnitExp; string one, two, three, four, five;
        ss >> one >> two >> three >> four >> five;
        if (three.length() == 0 && two.find("()") != string::npos && one != "PRINT") {
            /**
             * @brief: 函数名, 全跳
             */
            while (i + 1 <= getLineNum(functionMidCode) &&
                   getExpInLine(i + 1, functionMidCode).find("para ") != string::npos) {
                i++;
            }
        } else if (two == "=" && IS_NOT_COMPARE_SYMBOL(two)) {
            /**
             * @brief: 纯赋值语句, 要替换
             */
             string id;
             if (one.find('[') != string::npos) {
                 id = one.substr(one.find('[') + 1, one.find(']') - one.find('[') - 1);
                 result += one.substr(0, one.find('[')) + '[';

                 if (functionPara2fake_para.count(id) > 0) result += functionPara2fake_para.at(id);
                 else result += id;

                 result += ']';
             } else {
                 if (functionPara2fake_para.count(one) > 0) result += functionPara2fake_para.at(one);
                 else result += one;
             }

             result += " " + two + " ";

            if (three.find('[') != string::npos) {
                id = three.substr(three.find('[') + 1, three.find(']') - three.find('[') - 1);
                result += three.substr(0, three.find('[')) + '[';

                if (functionPara2fake_para.count(id) > 0) result += functionPara2fake_para.at(id);
                else result += id;

                result += ']';
            } else {
                if (functionPara2fake_para.count(three) > 0) result += functionPara2fake_para.at(three);
                else result += three;
            }

            if (four.length() == 0) {
                result += '\n';
            } else {
                result += ' ' + four + ' ';
                if (functionPara2fake_para.count(five) > 0) result += functionPara2fake_para.at(five);
                else result += five;

                result += '\n';
            }
        } else if (!IS_NOT_COMPARE_SYMBOL(fourUnitExp)) {
            /**
             * @brief: 条件比较语句
             */
            if (functionPara2fake_para.count(one) > 0) result += functionPara2fake_para.at(one);
            else result += one;

            result += ' ' + two + ' ';

            if (functionPara2fake_para.count(three) > 0) result += functionPara2fake_para.at(three);
            else result += three;

            result += '\n';
        } else if (one == "PRINT") {
            /**
             * @brief: 打印语句
             */
             if (fourUnitExp.at(fourUnitExp.length() - 1) != '"') {
                 string id = "";
                 int place = 0;
                 for (int u = fourUnitExp.length() - 1; u >= 0; u--) {
                     if (fourUnitExp.at(u) == ' ') {
                         place = u;
                         break;
                     }
                     id = fourUnitExp.at(u) + id;
                 }
                 result += fourUnitExp.substr(0, place) + ' ';

                 if (functionPara2fake_para.count(id) > 0) result += functionPara2fake_para.at(id);
                 else result += id;
             } else {
                 result += fourUnitExp + '\n';
             }
            result += '\n';
        } else if (one == "SCANF") {
            /**
             * @brief: 读语句
             */
            string id = two;
            result += "SCANF ";
            if (functionPara2fake_para.count(id) > 0) result += functionPara2fake_para.at(id);
            else result += id;

            result += '\n';
        } else if (one == "ret") {
            if (two.length() != 0) {
                if (functionPara2fake_para.count(two) > 0) retId = functionPara2fake_para.at(two);
                else retId = two;
            }
        } else {
            result += fourUnitExp + '\n';
        }
    }
    return result;
}

string inlineMidCode(string midCode) {
    string result = "";
    for (int i = 1; i <= getLineNum(midCode); i++) {
        string nowLine = getExpInLine(i, midCode);
        string one, two; stringstream ss; ss << nowLine; ss >> one >> two;
        bool canInline = false;
        vector<string> functionParasName;
        functionParasName.clear();
        if ((one == "push" || one == "call") && two != "=") {
            for (int j = i; j <= getLineNum(midCode); j++) {
                stringstream ss1; ss1 << getExpInLine(j, midCode);
                string op, functionName; ss1 >> op >> functionName;
                if (op == "call" && functionName != "=") {
                    canInline = functionCanBeInlined(functionName, functionParasName);
                    break;
                }
            }
        }
        if (!canInline) {
            result += getExpInLine(i, midCode) + '\n';
        } else {
            vector<string> para;
            string functionToBeInlined_midCode;
            int j;
            for (j = i; j <= getLineNum(midCode); j++) {
                stringstream ss2; ss2 << getExpInLine(j, midCode);
                string one1, two1;  ss2 >> one1 >> two1;
                if (one1 == "push" && two1 != "=") {
                    para.push_back(two1);
                } else if (one1 == "call" && two1 != "=") {
                    functionToBeInlined_midCode = getFunctionMidCode(two1);
                    break;
                }
            }
            vector<string> fake_para; //由于被内联的函数可能修改参数，所以必须用temp_var替换参数
            for (int x = 0; x < para.size(); x++) {
                fake_para.push_back("TEMP_VAR_CWR" + to_string(ID_counter++));
            }
            //开始内联
            string retId = "";
            result += transLateInlineCode(functionToBeInlined_midCode, para,
                    fake_para, functionParasName, retId);
            //如果有返回值的话，可以优化
            stringstream retSS; retSS << getExpInLine(j + 1, midCode);
            string one_, two_, three_; retSS >> one_ >> two_ >> three_;
            if (two_ == "=" && three_ == "RET") {
                result += one_ + " = " + retId + '\n';
                i = j + 1;
            } else {
                i = j;
            }
        }
    }
    return result;
}


#endif //COMPILER_INLINEOPTIM_H
