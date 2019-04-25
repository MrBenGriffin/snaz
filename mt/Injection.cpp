//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
#include "Definition.h"
#include "node/Metrics.h"
#include "content/Template.h"

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

	Injection::Injection(const std::string src) :
			type(It::plain), value(0), sValue(0), offset(0),
			modulus(false), stack(false), list(false), iterator(false), basis(src) {
		// ⍟^*([ijk]|[0-9]+|\(^*([ijkn]([.+-][0-9]+)?|[0-9]++?|(p(s|[0-9]?)))\))
		parseStart();
	}

	void Injection::expand(Messages& errs,mtext &result,mstack &context) const {
		if (type == It::text) {
			Text(basis).expand(errs,result,context);
		} else {
			if(sValue < context.size()) {
				auto &contextMacro = context[sValue].first;
				auto &instance = context[sValue].second;
				auto &parms = instance.parms;
				auto &iter = instance.it;
				size_t parmCount = parms->size();
				if (stack) {
					for(auto& s : context) {
						if(s.first!= nullptr) {
							std::visit([&result](auto const &a){ result.push_back(Text(a.name() + ";")); },*(s.first));
						}
					}
					auto& x = context.back().second;
					if(x.metrics != nullptr  && (x.metrics)->currentTemplate != nullptr) {
						result.push_back(Text(x.metrics->currentTemplate->name));
					} else {
						result.push_back(Text("nil"));
					}
				}
				if (list) {
					plist* p_parms = const_cast <plist *>(context.back().second.parms);
					for(size_t i=value; i <= parmCount; i++) {
						mtext tmp;
						Driver::inject((*parms)[i - 1],errs,tmp,context);
						p_parms->push_back(std::move(tmp));
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
										Driver::expand((*parms)[value - 1],errs, result, context);
									} else {
										mstack subContext(context.begin() + sValue, context.end());
										Driver::expand((*parms)[value - 1],errs, result, subContext);
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
									Driver::expand((*parms)[posi - 1],errs, result, context);
								} else {
									mstack subContext(context.begin() + sValue, context.end());
									Driver::expand((*parms)[posi - 1],errs, result, subContext);
								}
							} else {
								if (posi == 0) {
									std::visit([&result](auto const &a){ result.push_back(Text(a.name())); },*contextMacro);
								} else {
									//Do nothing, I guess.
								}
							}
						}  break;
						case It::count: {
							std::ostringstream a;
							size_t value = iter.first;
							adjust(value);
							a << value;
							Text(a.str()).expand(errs,result,context);
						} break;
						case It::size: {
							std::ostringstream a;
							size_t value = iter.second;
							adjust(value);
							a << value;
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


