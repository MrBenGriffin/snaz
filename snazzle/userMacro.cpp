//
// Created by Ben on 2019-01-10.
//

#include "userMacro.h"

#include <utility>

unordered_map<string, userMacro > userMacro::library;
userMacro userMacro::empty("__empty","",false,false,false);

userMacro::userMacro(
        string name_i, string expansion_i,
        long min, long max, bool strip, bool trimParms_i,bool preExpand_i )
        : counter(0), minParms(min), name(name_i), trimParms(trimParms_i), preExpand(preExpand_i) {
    maxParms = max == -1 ? INT_MAX : max;
    mt::Driver driver;
    std::istringstream code(expansion_i);
    bool advanced = test_adv(expansion_i);
    mt::parse_result result = driver.define(code,advanced,strip);
    expansion = result.first;
    iterated = result.second;
}

bool userMacro::test_adv(string& basis) {
    //⌽E2.8C.BD ⍟E2.8D.9F ⎣E2.8E.A3
    if(!basis.empty()) {
        size_t size = basis.size();
        size_t offset = basis.find(0xE2);
        for(;offset != string::npos && offset+2 < size; offset+=2) {
            char a=basis[offset+1]; char b=basis[offset+2];
            if(a==char(0x8C) && b==char(0xBD)) return true;
            if(a==char(0x8D) && b==char(0x9F)) return true;
            if(a==char(0x8E) && b==char(0xA3)) return true;
            offset = basis.find(0xE2,offset);
        }
    }
    return false;
}

userMacro::~userMacro() {
}

//shared_ptr<userMacro> userMacro = make_shared<userMacro>(name,expansion,min,max,bools[0]=='1',bools[1]=='1',bools[2]=='1');
userMacro& userMacro::get(string name) {
    auto good = userMacro::library.find(name);
    return good != userMacro::library.end() ? good->second : userMacro::empty;
}

void userMacro::add(userMacro& macro) {
    del(macro.name);
    library.emplace(macro.name, std::move(macro));
 }

bool userMacro::has(string name) {
    return userMacro::library.find(name) != userMacro::library.end();
}

void userMacro::del(string name) {
    auto good = userMacro::library.find(name);
    if (good != userMacro::library.end()) {
        userMacro::library.erase(good);
    }
}

std::ostream& userMacro::visit(std::ostream& o) {
    return mt::Driver::visit(expansion,o);
}

void userMacro::expand(std::ostream& o,mt::plist& myParms,mt::mstack& context) {
    if(!preExpand) {
        mt::plist parms;
        for (auto& parm : myParms) {
            ostringstream in;
            mt::Driver::expand(parm,in,context);
            std::string str = in.str(); in.str("");
            if(trimParms) trim(str);
            parms.push_back({str});
        }
        context.push_front({this,parms});
    } else {
        context.push_front({this,myParms});
    }
     mt::Driver::expand(expansion,o,context);
    context.pop_front();
}

void userMacro::trim(std::string &str) {
    char const* unwanted = " \t\r\n";
    string::size_type  notWhite = str.find_first_not_of(unwanted);
    str.erase(0,notWhite);
    notWhite = str.find_last_not_of(unwanted);
    str.erase(notWhite+1);
}


//static
void userMacro::terminate() {
}




