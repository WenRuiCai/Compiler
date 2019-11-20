//
// Created by 84443 on 2019/11/18.
//

#ifndef COMPILER_MIPSGENERATOR_H
#define COMPILER_MIPSGENERATOR_H

#include "Variable.h"

enum regForTmp {
    s6 = 0,
    s5,
    s4,
    s3,
    s2,
    s1,
    s0,
    t4,
    t3
};

int reg[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
map<string, int> temp_valid_register;

string getEnumString(int i) {
    if (i == 0) return "s6";    if (i == 3) return "s3";
    if (i == 1) return "s5";    if (i == 4) return "s2";
    if (i == 2) return "s4";    if (i == 5) return "s1";
    if (i == 6) return "s0";    if (i == 7) return "t4";
    if (i == 8) return "t3";
    return "";
}

int searchValidReg() {
    for (int i = 0; i < 9; i++) {
        if (reg[i] == 0)
            return i;
    }
    return 0;
}

void selectTempRegister(stringstream& mips_with_tmp, ofstream& output) {
    string line;
    int place = 0;
    while (getline(mips_with_tmp, line)) {
        if ((place = line.find("TEMP_VAR_CWR")) == string::npos) {
            output << line << '\n';
        } else {
            string nowLine = line.substr(0, place);

            int numplace = place + 12;
            string num = "";
            while ((numplace < line.length()) &&
                    (line.at(numplace) >= '0' && line.at(numplace) <= '9')) {
                num += line.at(numplace); numplace++;
            }
            if (temp_valid_register.count(num) == 0) {
                temp_valid_register.insert(make_pair(num, searchValidReg()));
                nowLine += getEnumString(searchValidReg());
                nowLine += line.substr(numplace, line.length());
                reg[temp_valid_register.at(num)] = 1;
            } else {
                nowLine += getEnumString(temp_valid_register.at(num));
                nowLine += line.substr(numplace, line.length());
                reg[temp_valid_register.at(num)] = 0;
                temp_valid_register.erase(num);
            }

            output << nowLine << '\n';
        }
    }
}



#endif //COMPILER_MIPSGENERATOR_H
