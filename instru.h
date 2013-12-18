#ifndef MINSTRU_H
#define MINSTRU_H

#include <string>

using namespace std;

class Minstru {
    public:
        Minstru(){};
        Minstru(string _label ,string _ins ,string _value ,string _extra )
          : label(_label) , ins(_ins) , value(_value) , extra(_extra)
        {
        }
        string label;
        string ins;
        string value;
        string extra;
};
#endif
