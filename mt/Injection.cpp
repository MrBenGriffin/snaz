//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
#include "Definition.h"

namespace mt {

    void Injection::parseStart() {
        switch (basis[0]) {
            case '^' :
                parseParent();
                break;
            case '(' :
                parseBrackets();
                break;
            case 'p' :
                parsePName();
                break; //same as ^0
            case 'i' :
            case 'j' :
            case 'k' :
            case 'n' :
                parseIterated();
                break;
            case '0' :
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' :
            case '9' : {
                value = stoul(basis);
            }
                break;
            default: {
                type = It::text;
            }
        }
    }
    void Injection::parseIterated() {
        switch (basis[0]) {
            case 'i':
                iterator = true;
                type = It::current;
                break;
            case 'j':
                iterator = true;
                type = It::current;
                offset = 1;
                break;
            case 'k':
                iterator = true;
                type = It::count;
                break;
            case 'n':
                type = It::size;
                break;
            default:
                break;
        }
        basis.erase(0, 1);
        if (!basis.empty()) {
            if (basis[0] == '.') {
                modulus = true;
                basis[0] = '+';
            }
            offset = stol(basis);
        }
    }
    void Injection::parsePName() {
        basis.erase(0, 1);
        if (basis[0] == 's') {
            stack = true;
            basis.erase(0, 1);
        } else {
            sValue++;
        }
    }
    void Injection::parseBrackets() {
        basis.erase(0, 1);
        basis.pop_back();
        if (!basis.empty()) {
            if (basis.back() == '+') { // is it a list of parameters?
                basis.pop_back();      // remove the list marker
                list = true;           // mark as a list.
                value = stoul(basis);  // so get the offset.
            } else {
                parseStart();         // needs more work.
            }
        }
    }
    void Injection::parseParent() {
        while (basis[0] == '^') {       // go up the stack.
            sValue++;
            basis.erase(0, 1);
        }
        parseStart();                 // now work out the rest.
    }

    Injection::Injection() :
        type(It::plain), value(0), sValue(0), offset(0),
        modulus(false), stack(false), list(false), iterator(false), basis("") {
    }

    Injection::Injection(const std::string src) :
            type(It::plain), value(0), sValue(0), offset(0),
            modulus(false), stack(false), list(false), iterator(false), basis(src) {
        // ⍟^*([ijk]|[0-9]+|\(^*([ijkn]([.+-][0-9]+)?|[0-9]++?|(p(s|[0-9]?)))\))
        parseStart();
    }

    void Injection::expand(mtext &result,mstack &context) const {
        if (type == It::text) {
            Text(basis).expand(result,context);
         } else {
            auto &contextMacro = context[sValue].first;
            auto &instance = context[sValue].second;
            auto &parms = instance.parms;
            auto &iter = instance.it;
            size_t parmCount = parms->size();
            if (stack) {
                 for(auto& s : context) {
                    std::visit([&result](auto const &a){ result.push_back(Text(a.name() + ";")); },*(s.first));
                 }
            }
            if (list) {
                for(size_t i=value; i <= parmCount; i++) {
                    mtext tmp;
                    Driver::expand((*parms)[i - 1], tmp, context);
                    result.push_back(tmp.front()); //if tmp has MORE than one result.. what then?!
                }
            }
            if(!stack && !list) {
                switch (type) {
                    case It::plain:
                        if(value == 0) {
                            std::visit([&result](auto const &a){ result.push_back(Text(a.name())); },*contextMacro);
                         } else {
                            bool legal=false; size_t index=value;
                            std::visit([&legal,&index](auto const &a){ legal = a.inRange(index); },*contextMacro);
                            if(legal && (value <= parmCount)) {
                                if (sValue == 0) {
                                    Driver::expand((*parms)[value - 1], result, context);
                                } else {
                                    mstack subContext(context.begin() + sValue, context.end());
                                    Driver::expand((*parms)[value - 1], result, subContext);
                                }
                            } else {
                                //TODO: Report value being asked for is out of the range of the definition.
                            }
                        }
                        break;
                    case It::current: {
                        auto posi = iter.first + offset;
                        if ( 0 < posi <  parmCount) {
                            if (sValue == 0) {
                                Driver::expand((*parms)[posi - 1], result, context);
                            } else {
                                //TODO: Work out what the hell this means...
                            }
                        } else {
                            if (posi == 0) {
                                std::visit([&result](auto const &a){ result.push_back(Text(a.name())); },*contextMacro);
                            } else {
                                //TODO: value being asked for is out of the range of those supplied.
                            }
                        }
                    }  break;
                    case It::count: {
                        std::ostringstream a;
                        a << iter.first;
                        Text(a.str()).expand(result, context);
                    } break;
                    case It::size: {
                        std::ostringstream a;
                        a << iter.second;
                        Text(a.str()).expand(result, context);
                    } break;
                    default:
                        break;
                }
            }
        }
    }

    std::ostream &Injection::visit(std::ostream &result) const {
        result << "⁅";
        if (type == It::text) {
            result << basis;
        } else {
            if (sValue != 0) {
                result << sValue;
                result << ":";
            }
            if (stack) {
                result << "s";
            }
            switch (type) {
                case It::plain:
                    result << value;
                    break;
                case It::current:
                    result << "i";
                    break;
                case It::count:
                    result << "k";
                    break;
                case It::size:
                    result << "n";
                    break;
                default:
                    break;
            }
            if (offset != 0) {
                if (modulus) {
                    result << "." << offset;
                } else {
                    result << std::showpos << offset;
                }
            }
            if (list) {
                result << "+";
            }
        }
        result << "⁆" << std::flush;
        return result;
    }
}


