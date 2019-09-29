cmake_minimum_required(VERSION 3.14)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(Compiler main.cpp WordAnalysis.h WordDealFunction.h TYPE.h SyntaxAnalysis.h SyntaxAnalyseFunction.h CentencesAnalysis.h ExpressionDeal.h)