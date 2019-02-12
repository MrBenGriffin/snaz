#include "Internal.h"
#include "InternalInstance.h"
#include "support/Infix.h"
#include "support/Message.h"
#include "support/Convert.h"
#include "support/Timing.h"

namespace mt {
	using namespace Support;

	void iEq::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		std::string left =  my.parm(1);
		std::string right = my.count > 1 ? my.parm(2): "";
		my.logic(left == right,3);
	}
	void iExpr::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		Infix::Evaluate expression;
		auto expr=my.parm(1);
		expression.set_expression(expr);
		for (size_t i=2; i <= my.count; i++) {
			char **end = nullptr;
			std::string parmValue = my.parm(i);
			std::ostringstream ost;
			ost << "p" << i-1; //parm by number.
			auto number = strtold(&parmValue[0], end);
			expression.add_parm(ost.str(),number);
		}
		long double value = expression.process(*my.errs);
		if(my.errs->marked()) {
			my.errs->prefix({usage,expr});
			my.errs->enscope("Within iExpr");
		}
		std::ostringstream ost; ost << value;
		my.set(ost.str());
	}
	void iForIndex::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		plist parms=toParms(my.parm(2),my.parm(1),my.parm(5));
		my.generate(parms,my.praw(6),my.parm(3),my.parm(4));
	}
	void iIndex::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		enum optype {resize,ifempty,normalise,get,size,set,push,erase,find,back,append,drop,reverse,uappend,upush,pop,retrieve,remove,contains} op = normalise;
		string result,delimiter = my.parm(1);
		if (delimiter.size() == 1) {
			if (my.count > 1) { //now process the array
				string index_string = my.parm(2);
				deque<string> idx; istringstream iss (index_string);
				for( string line; getline(iss,line,delimiter[0]); ){
					idx.push_back(line);
				}
				if (my.count > 2) { //now process the operation
					string operation = my.parm(3);
					if (operation.size() > 1) {
						switch (operation[0]) {
							case 'r': { //resize, reverse, retrieve
								switch (operation[2]) {
									case 'm': op=remove; break;
									case 's': op=resize; break;
									case 't': op=retrieve; break;
									case 'v': op=reverse; break;
									default: op=normalise; break;
								}
							} break;
							case 'i': op=ifempty; break;
							case 'n': op=normalise; break;
							case 'g': op=get; break;
							case 's': op= operation[1] == 'e' ? set : size ; break;
							case 'p': op= operation[1] == 'o' ? pop : push ; break;
							case 'e': op=erase; break;
							case 'f': op=find; break;
							case 'c': op=contains; break;
							case 'b': op=back; break;
							case 'a': op=append; break;
							case 'd': op=drop; break;
							case 'u': op= operation[1] == 'a' ? uappend : upush ; break;
							default: {
								e << Message(error,"iIndex: unknown operation '"+ operation +"' found");
							} break;
						}
					}
				}
				string p1,p2;
				if (my.count > 3) { //now process p1
					p1 = my.parm(4);
				}
				if (my.count > 4 && (op != contains)) { //now process p2
					p2 = my.parm(5);
				}
				pair<int,bool> i(0,true); //used for those who have p1 as an index.
				bool finished = false;
				switch (op) {
					case resize: {
						i = znatural(p1);
						if (i.second) { idx.resize(i.first,p2); }
					} break;
					case ifempty: {
						i = znatural(p1);
						if (i.second) {
							if ( (unsigned)i.first < idx.size()) {
								if (idx[i.first].empty()) { idx[i.first] = p2;}
							} else {
								idx.resize(i.first);
								idx.push_back(p2);
							}
						}
					} break;
					case normalise: {
					} break;
					case get: {
						i = znatural(p1);
						if (i.second && (unsigned)i.first < idx.size()) { result = idx[i.first]; }
						finished = true;
					} break;
					case set: {
						i = znatural(p1);
						if (i.second) {
							if ( (unsigned)i.first < idx.size()) {
								idx[i.first] = p2;
							} else {
								idx.resize(i.first);
								idx.push_back(p2);
							}
						}
					} break;
					case size: {
						tostring(result,idx.size());
						finished = true;
					} break;
					case pop: {
						idx.pop_front();
					} break;
					case push: {
						idx.push_front(p1);
					} break;
					case upush: {
						signed x=0,y=(signed)idx.size();
						while ((x < y) && (p1 != idx[x])) {x++;}
						if (x >= (signed)idx.size()) {
							idx.push_front(p1);
						}
					} break;
					case erase: {
						i = znatural(p1);
						if (i.second && (unsigned)i.first < idx.size()) { idx.erase(idx.begin()+i.first); }
					} break;
					case remove: { //erase all indices that match p1.
						for (size_t x=0; x < idx.size(); x++ ) {
							if (p1 == idx[x]) {
								idx.erase(idx.begin()+ x--);
							}
						}
					} break;
					case contains: {
						size_t x=0,y= idx.size();
						while ((x < y) && (p1 != idx[x])) {x++;}
						my.logic(x < idx.size(),4); //@iIndex(;,A;B,contains,A,[T,F])=4
						return;
					} break;
					case find: {
						signed x=0,y=(signed)idx.size();
						while ((x < y) && (p1 != idx[x])) {x++;}
						if (x >= (signed)idx.size()) x = -1;
						tostring(result,x);
						finished = true;
					} break;
					case retrieve: { //return all indexes that start with p1.
						size_t plen=p1.size(); //
						for (size_t x=0;x < idx.size(); x++) {
							if (idx[x].compare(0,plen,p1) != 0) {
								idx.erase(idx.begin()+ x--);
							}
						}
					} break;
					case back: {
						if (! idx.empty()) {
							result = idx.back();
						}
						finished = true;
					} break;
					case append: {
						idx.push_back(p1);
					} break;
					case uappend: {
						signed x=0,y=(signed)idx.size();
						while ((x < y) && (p1 != idx[x])) {x++;}
						if (x >= (signed)idx.size()) {
							idx.push_back(p1);
						}
					} break;
					case drop: {
						if (! idx.empty()) {
							idx.pop_back();
						}
					} break;
					case reverse: {
						ostringstream rslt;
						for(auto j=idx.rbegin(); j!=idx.rend();++j) {
							rslt << *j;
							if (j+1 != idx.rend() || j->empty() ) {
								rslt.put(delimiter[0]);
							}
						}
						result = rslt.str();
						finished = true;
					} break;
				}
				if (!finished) {
					ostringstream rslt;
					for(auto j=idx.begin(); j!=idx.end();++j) {
						rslt << *j;
						if (j+1 != idx.end() || j->empty() ) {
							rslt.put(delimiter[0]);
						}
					}
					result = rslt.str();
				}
			}
		} else {
			e << Message(error,"The delimiter must be a single character from Unicode page 0. '" + delimiter + "' was found");
		}
		my.set(result);
	}
	void iConsole::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
//		enum channel { fatal, error, syntax, range, parms, warn, info, debug, usage, timing, trace, code };
		channel type = info;
		string message;
		if(my.count == 2) {
			std::string infoStr =  my.parm(1);
			message =  my.parm(2);
			if(!infoStr.empty()) {
				char hint = infoStr[0];
				hint = (char) tolower(hint);
				switch (hint) {
					case 'f': type=fatal; break;
					case 'x':
					case 'e': type=error; break;
					case 'w': type=warn; break;
					case 'g': type=syntax; break;
					case 'r': type=range; break;
					case 'd': type=debug; break;
					case 'u': type=usage; break;
					case 'p': type=code; break;
					case 't': doTrace(e,context); e.enscope(message); return;
					case 'b': Timing::getTiming(e,'b'); e.enscope(message); return;
					case 'n': Timing::getTiming(e,'n'); e.enscope(message); return;
					case 'c': Timing::getTiming(e,'c',message); return;
					default: break;
				}
			}
		} else {
			message =  my.parm(1);
		}
		e << Message(type,message);
	}
	void iDate::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

	void iEval::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		std::stringstream code;
		bool advanced = false;
		for(auto i=1; i <= my.count; i++) {
			string parm = my.parm(i);
			advanced = advanced || Definition::test_adv(parm);
			code << parm;
			if (i != my.count) {
				code << ",";
			}
		}
		mt::Driver driver(e,code,advanced);
		mt::mtext structure = driver.parse(e,false); //no strip
		ostringstream result;
		driver.expand(result,e,"iEval Instance");
		my.set(result.str());
	}

	void iFile::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

	void iField::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

	void iForSubs::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

	void iForQuery::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

	void iMath::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

	void iNull::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

	void iTiming::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalInstance my(this,e,o,instance,context);
		e << Message(error,_name + " is not yet implemented.");
		std::string left =  my.parm(1);
		my.logic(false,1);
	}

}

