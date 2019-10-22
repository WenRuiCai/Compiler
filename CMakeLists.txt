cmake_minimum_required(VERSION 3.14)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(Compiler main.cpp WordAnalysis/WordAnalysis.h WordAnalysis/WordDealFunction.h TYPE.h SyntaxAnalysis/SyntaxAnalysis.h SyntaxAnalysis/SyntaxAnalyseFunction.h SyntaxAnalysis/CentencesAnalysis.h SyntaxAnalysis/ExpressionDeal.h WordAnalysis/SearchLine.h Errors/CompileError.h Errors/WordError.h Errors/NameDefineError.h SymbolTable/TableItem.h SymbolTable/SymbolTable.h Errors/FunctionParameterError.h)