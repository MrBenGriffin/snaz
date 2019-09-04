#include <utility>

//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
#include "Definition.h"
#include "node/Metrics.h"
#include "content/Template.h"
#include "mt/Token.h"

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
			if(!basis.empty()) {
				parseStart();                 // now work out the rest.
				sValue = value;
				value = 0;
			} else {
				sValue++;
			}
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

	Injection::Injection(std::string src) :
			type(It::plain), value(0), sValue(0), offset(0),
			modulus(false), stack(false), list(false), iterator(false), basis(std::move(src)) {
		// ⍟^*([ijk]|[0-9]+|\(^*([ijkn]([.+-][0-9]+)?|[0-9]++?|(p(s|[0-9]?)))\))
		parseStart();
	}

	std::string Injection::get() const {
		return basis; //not sure this is correct.
	}

	bool Injection::empty() const {
		return basis.empty();
	}

	void Injection::inject(Messages& errs,MacroText &result,mstack &context) const {
		expand(errs,result,context);
	}

	void Injection::doFor(Messages& errs,MacroText &result,const forStuff&,mstack &context) const {
		expand(errs,result,context);
	}

	void Injection::clone(MacroText &out) const {
		auto token= make_unique<Injection>(*this);
		out.emplace(token);
	}

	void Injection::final(std::ostream& o) const {
		visit(o);
	}

	void Injection::subs(MacroText& out,const std::deque<std::string>&,const std::string&) const {
		auto token=make_unique<Injection>(*this);
		out.emplace(token);
	}

	void Injection::expand(Messages& errs,MacroText &result,mstack &context) const {
		if (type == It::text) {
			result.add(basis);
		} else {
			if(!context.empty() && sValue < context.size()) {
				auto &contextMacro = context[sValue].first;
				auto* instance = context[sValue].second;  //Carriage passes instance by pointer.
				auto &parms = instance->parms;
				auto &iter = instance->it;
				size_t parmCount = parms.size();
				if (stack) {
					for(auto& s : context) {
						if(s.first!= nullptr) {
							result.add((s.first)->name() + ";"); //macro name
						}
					}
					auto& x = context.back().second;
					if(x->metrics != nullptr  && (x->metrics)->currentTemplate != nullptr) {
						result.add(x->metrics->currentTemplate->name);
					} else {
						result.add("nil");
					}

				}
				if (list) {
					for(size_t i=value; i <= parmCount; i++) {
						MacroText tmp;
						parms[i - 1].inject(errs,tmp,context);
						context.back().second->parms.emplace_back(std::move(tmp)); //plist is a std::deque<MacroText>;
					}
				}
				if(!stack && !list) {
					switch (type) {
						case It::plain:
							if(value == 0) {
								if(contextMacro != nullptr) {
									result.add(contextMacro->name());
								} else {
									errs << Message(trace,"Reference to Parm 0 with no context present.");
								}
							} else {
								bool legal = contextMacro->inRange(value);
								if(legal && (value <= parmCount)) {
									if (sValue == 0) {
											parms[value - 1].expand(errs,result,context);
									} else {
										mstack subContext(context.begin() + sValue, context.end());
										parms[value - 1].expand(errs,result,subContext);
									}
								}
								/**
								// else do nothing. Pop-back of empty parameters causes this.
								else {
									if(!(parmCount == 0 && value == 1)) {
										//This happens due to pop-back of empty parameters
										ostringstream estr;
										estr << " parameter value " << value << " is beyond current range of " << parmCount;
										errs << Message(debug,estr.str());
									}
								}
								 **/
							}
							break;
						case It::current: {
							auto posi = iter.first;
							adjust(posi);
							if ( (0 < posi) &&  (posi <=  parmCount)) {
								if (sValue == 0) {
									parms[posi-1].expand(errs,result,context);
								} else {
									mstack subContext(context.begin() + sValue, context.end());
									parms[posi-1].expand(errs,result,subContext);
								}
							} else {
								if (posi == 0) {
									result.add(contextMacro->name());
								} else {
									//Do nothing, I guess.
								}
							}
						}  break;
						case It::count: {
							std::ostringstream a;
							size_t number = iter.first;
							adjust(number);
							a << number;
							Text(a.str()).expand(errs,result,context);
						} break;
						case It::size: {
							std::ostringstream a;
							size_t number = iter.second;
							adjust(number);
							a << number;
							Text(a.str()).expand(errs,result,context);
						} break;
						default:
							break;
					}
				}
			} else {
				std::ostringstream ers;
				ers << " Offset " << sValue << "is greater than the current stack size of " << context.size();
				errs << Message(error,ers.str());

			}

		}
	}

	void Injection::adjust(size_t& base) const { //modulus etc.
		base = modulus ? base % offset : base + offset;
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


