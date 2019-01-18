//
// Created by Ben on 2019-01-10.
//

#ifndef USERMACRO_H
#define USERMACRO_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Driver.h"

using namespace std;

class userMacro {
public:
    ~userMacro();
private:
    static userMacro empty;
    static unordered_map<string,userMacro> library;
    static void trim(mt::plist&,mt::plist &);
protected:
    size_t counter;
    mt::mtext expansion;

    static void terminate();

public:
    string name;
    long minParms,maxParms;
    bool iterated,trimParms,preExpand;
    std::ostream& visit(std::ostream&);
    void expand(std::ostream&,mt::plist&,mt::mstack&);
    userMacro(string,string,long,long,bool=true,bool=true,bool=false);
    static bool test_adv(string&);
    static void add(userMacro&);
    static void del(string);
    static bool has(string);
    static userMacro& get(string);
};


#endif //USERMACRO_H
