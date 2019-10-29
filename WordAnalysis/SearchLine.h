//
// Created by 84443 on 2019/10/18.
//
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

#ifndef COMPILER_SEARCHLINE_H
#define COMPILER_SEARCHLINE_H

int searchLine(int streamPoint, string code) {
    stringstream codeStream;
    char a[1000];
    codeStream << code;
    int line = 0;
    while (codeStream.tellg() < streamPoint) {
        codeStream.getline(a, 1000);
        line += 1;
    }
    return line;
}

#endif //COMPILER_SEARCHLINE_H
