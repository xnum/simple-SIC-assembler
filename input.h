#ifndef MINPUT_H
#define MINPUT_H
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include "instru.h"
using namespace std;

class Minput {
    public:
        Minput();
        Minput(string);
        bool getCode();



        vector<Minstru> code;
        fstream fs;
        bool chk_file();
    private:
};
#endif
