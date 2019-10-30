//
// Created by 84443 on 2019/10/19.
//
enum ExpressionFlag {
    INT_Express,
    CHAR_Express,
    NONE_Express
};
#include "TableItem.h"
#include "../Errors/NameDefineError.h"
#include "../Errors/FunctionParameterError.h"
#include "../Errors/SymbolError.h"

class SymbolTable {
private:
    vector<Level> symbolStackTable;
    int nowlevel = 0;

    bool nameDuplicated(string name) {
        Level level = symbolStackTable[nowlevel];
        for (TableItem item : level.getItems()) {
            if (item.name == name)
                return true;
        }
        return false;
    }

    Level findFunctionSymbols(string functionName) {
        int rank = 0;
        for (TableItem item : symbolStackTable[0].getItems()) {
            if (item.kind == FUNC) {
                rank++;
                if (item.name == functionName) break;
            }
        }
        return symbolStackTable[rank];
    }

public:
    void addLevel() {
        Level newLevel = Level();
        this->symbolStackTable.push_back(newLevel);
        this->nowlevel = symbolStackTable.size() - 1;
    }

    void dropOutLevel() {
        this->nowlevel = 0;
    }

    vector<Level> getSymbolTable() {
        return this->symbolStackTable;
    }

    void addConditionTypeError(int line) {
        functionParameterError.addError('f', line);
    }

    void nowLevelAddItem(string type, string name, KIND kind, int line, int dimension) {
        if (nameDuplicated(name)) {
            nameDefineError.addError('b', line);
            return;
        }
        this->symbolStackTable[nowlevel].addItem(type, name, kind, line, dimension, symbolStackTable.size() - 1);
        //cout << symbolStackTable[symbolStackTable.size() - 1].getItems().size() << endl;
    }

    bool parameterNumHasError(string functionName, int line, int paraNum) {
        int counter = 0;
        for (TableItem item1 : findFunctionSymbols(functionName).getItems()) {
            if (item1.kind == PARA) counter++;
        }
        if (paraNum != counter) {
            functionParameterError.addError('d', line);
            return true;
        }
        return false;
    }

    /*
     * 本函数应该要在参数个数一致的情况下进行
     */
    bool parameterTypeHasError(string functionName, int line, vector<string>& types) {
        for (TableItem item : findFunctionSymbols(functionName).getItems()) {
            if (types.size() == 0) break;
            if (item.kind == PARA) {
                if (item.type != types[0]) {
                    functionParameterError.addError('e', line);
                    return true;
                }
                types.erase(types.begin());
            }
        }
        return false;
    }

    bool hasAssignForConst(string name, int line) {
        int flag = 0;
        for (TableItem item : symbolStackTable[nowlevel].getItems()) {
            if (item.name == name) {
                flag = 1;
                if (item.kind == CONST) {
                    nameDefineError.addError('j', line);
                    return true;
                }
            }
        }
        if (flag == 0) {
            for (TableItem item : symbolStackTable[0].getItems()) {
                if (item.name == name && item.kind == CONST) {
                    nameDefineError.addError('j', line);
                    return true;
                }
            }
        }
        return false;
    }

    bool functionHasNotDefined(string functionName, int line) {
        for (TableItem item : symbolStackTable[0].getItems()) {
            if (item.name == functionName && item.kind == FUNC) {
                return false;
            }
        }
        nameDefineError.addError('c', line);
        return true;
    }

    bool nameHasNotDefined(string name, int line) {
        Level level = symbolStackTable[nowlevel];
        for (TableItem item : level.getItems()) {
            if (item.name == name) {
                return false;
            }
        }
        for (TableItem item1 : symbolStackTable[0].getItems()) {
            if (item1.name == name) {
                return false;
            }
        }
        nameDefineError.addError('c', line);
        return true;
    }

    ExpressionFlag findFunctionKind(string name) {
        for (TableItem item : symbolStackTable[0].getItems()) {
            if (item.kind == FUNC && item.name == name) {
                if (item.type == "INTTK") {
                    return INT_Express;
                } else if (item.type == "CHARTK") {
                    return CHAR_Express;
                }
            }
        }
        return INT_Express;
    }

    ExpressionFlag find_ID_Kind(string name) {
        for (TableItem item : symbolStackTable[nowlevel].getItems()) {
            if (item.name == name && ((item.kind == VAR) || (item.kind == CONST))) {
                if (item.type == "INTTK") return INT_Express;
                else if (item.type == "CHARTK") return CHAR_Express;
            }
        }
        for (TableItem item1 : symbolStackTable[0].getItems()) {
            if (item1.name == name && ((item1.kind == VAR) || (item1.kind == CONST))) {
                if (item1.type == "INTTK") return INT_Express;
                else if (item1.type == "CHARTK") return CHAR_Express;
            }
        }
        return INT_Express;
    }

    void addArrayIndexError(int line) {
        nameDefineError.addError('i', line);
    }

    bool noReturnCentenceError(bool hasReturnCentence, int line) {
        int counter = 0;
        for (TableItem item : symbolStackTable[0].getItems()) {
            if (item.kind == FUNC) {
                counter++;
                if (counter == nowlevel) {
                    if (item.type != "VOIDTK" && !hasReturnCentence) {
                        functionParameterError.addError('h', line);
                        return true;
                    } else {
                        return false;
                    }
                }
            }
        }
        return false;
    }

    bool returnCentenceIsCorrect(ExpressionFlag flag, int line) {
        int counter = 0;
        for (TableItem item : symbolStackTable[0].getItems()) {
            if (item.kind == FUNC) {
                counter++;
                if (counter == nowlevel) {
                    if (flag != NONE_Express && item.type == "VOIDTK") {
                        functionParameterError.addError('g', line);
                        return false;
                    }
                    else if ((item.type == "CHARTK" && flag != CHAR_Express) ||
                            (item.type == "INTTK" && flag != INT_Express)) {
                        functionParameterError.addError('h', line);
                        return false;
                    }
                    return true;
                }
            }
        }
        return true;
    }

    /*
     * k和o同时不报的情况下，第一个点0
     * 报o不报k的情况下，第一个点60
     * 所以第一个错误是o型错误
     * 报k且报o的情况下，第一个点13
     * 所以后面一定有k型错误，而且最重要的是，可能是o型错误导致的k型错误，所以会多报错.
     * 以下代码段会造成这种情况
     * const int a = +1,b = -20,c =
     * 这里确实同时具有k和o两种情况，我调整输出后得到60，但是后面的13分又得不到了
     * 真jb操蛋啊
     */
    void loss_SEMICN_Error(int line) {
        symbolError.addError('k', line);
    }

    void loss_RBRACK_Error(int line) {
        symbolError.addError('m', line);
    }

    void loss_RPARENT_Error(int line) {
        symbolError.addError('l', line);
    }

    void loss_WHILETK_Error(int line) {
        symbolError.addError('n', line);
    }

    void addAssignValueError(int line) {
        nameDefineError.addError('o', line);
    }
};

SymbolTable symbolTable;