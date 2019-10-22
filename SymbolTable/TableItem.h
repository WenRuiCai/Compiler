//
// Created by 84443 on 2019/10/19.
//
#include "../TYPE.h"
#ifndef COMPILER_TABLEITEM_H
#define COMPILER_TABLEITEM_H

enum KIND {
    CONST,
    VAR,
    FUNC,
    MAIN,
    PARA
};

struct TableItem {
    string type;       ///数据类型
    string name;       ///数据标识符
    KIND kind;         ///数据种类
    int line;          ///所在行数
    int dimension;     ///维数
    int level;         ///所在程序层数
    TableItem(string type, string name, KIND kind, int line, int dimension, int level):
        type(type), name(name), kind(kind), line(line), dimension(dimension), level(level) {}
};

class Level {
private:
    vector<TableItem> items;

public:
    void addItem(string type, string name, KIND kind, int line, int dimension, int level) {
        items.push_back(TableItem(type, name, kind, line, dimension, level));
    }

    vector<TableItem> getItems() {
        return this->items;
    }
};


#endif //COMPILER_TABLEITEM_H
