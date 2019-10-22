//
// Created by 84443 on 2019/10/18.
//

#ifndef COMPILER_COMPILEERROR_H
#define COMPILER_COMPILEERROR_H


class CompileError {
public:
    char errorKind;
    int error_Happened_Line;
    CompileError(char errorKind, int line):errorKind(errorKind), error_Happened_Line(line) {}
};


#endif //COMPILER_COMPILEERROR_H
