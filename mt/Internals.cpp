//
// Created by Ben on 2019-01-24.
//
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <random>
#include <variant>

#include "Internals.h"
#include "InternalsCommon.h"
#include "support/Infix.h"
#include "support/Message.h"
#include "support/Fandr.h"
#include "support/Comparison.h"

namespace mt {
    using namespace Support;
	using namespace std;

    Storage Internal::storage;
    Library Internal::library;
    Library Internal::cache;
    LStore Internal::lStore;

// Must split the string into text tokens with marker/count tokens being substituted by the i and the k.
// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c

	void Internal::split1(mtext& result,string& s,string& d1,Injection t1) {
		size_t pos_start = 0, pos_end, dSize = d1.size();
		while ((pos_end=s.find (d1,pos_start)) != string::npos) {
			string txt = s.substr (pos_start, pos_end - pos_start);
			pos_start = pos_end + dSize;
			if(!txt.empty()) { result.push_back(Text(txt)); }		//store the left-string (if it's not empty).
			result.push_back(t1);									//push back the count
		}
		string txt = s.substr(pos_start);
		if(!txt.empty()) { result.push_back(Text(txt)); }			//store the left-string (if it's not empty).
	}

	void Internal::split2(mtext& result,string& s,string& d1,Injection t1,string& d2,Injection t2) {
		size_t pos_start = 0, pos_end, dSize = d1.size();
		while ((pos_end=s.find (d1,pos_start)) != string::npos) {
			string txt = s.substr (pos_start, pos_end - pos_start);
			pos_start = pos_end + dSize;
			if(!txt.empty()) {
				split1(result,txt,d2,t2);
			}
			result.push_back(t1);										//push back the count
		}
		string txt = s.substr(pos_start);
		if(!txt.empty()) {
			split1(result,txt,d2,t2);
		}
	}

	void Internal::generate(Messages& e,mtext& o,mstack& c,plist& list,const mtext* pbase,string marker,string count) {
		//So, we need to iterate through the program - and replace text instances 'marker' and 'count' with injection instances.
		Injection ivalue; Injection icount; ivalue.seti(); icount.setk();
		mtext program;
		//TODO:: Fix this. Currently this does not go into (and does not deal with) the contents of the parms held by macros.
		//TODO:: Each macro token itself has it's own set of parms as tokens.
		//TODO:: So, we should really store this to prevent too much re-working.
		//TODO:: Or, write a serialiser / deserialiser.
		for(auto& t : *pbase) {
			//but we need to go into macros..
			if (std::holds_alternative<Text>(t)) {
				std::string text = std::get<Text>(t).get();
				split2(program,text,marker,ivalue,count,icount);
			} else {
				program.push_back(t);
			}
		}
		cout << marker << " " << count << endl;
		Driver::visit(*pbase,cout);
		cout << endl;
		Driver::visit(program,cout);
		cout << endl;
		Definition macro(":generated:",program);
		Instance instance(&list,{1,list.size()});
		macro.expand(e,o,instance,c);
	}

	void Internal::doSort(vector<string>& idx,string sortstr) {
		// [+-](d|n|i|c)[(numeric offset)]
		char dir='+',sortfn=' ';	//default = ascending,none.
		enum stype {numeric,date,caseinsensitive,casesensitive,random,none} sorter = none;

		//get optional sort order
		if (sortstr.size() > 0 && (sortstr[0]=='-' || sortstr[0]=='+' ) ) {
			dir = sortstr[0];
			sortstr.erase(0,1);
		}
		//get optional sort function
		if (sortstr.size() > 0) {
			sortfn=sortstr[0];
			sortstr.erase(0,1);
		} else {
			sortfn=' ';
		}

		if (sortstr.size() > 0) {
			size_t offset = 0;
			try {
				offset = stoul(sortstr.c_str());
			} catch(...) {}
			Sort::offset = offset;
		}

		switch (sortfn) {
			case 'D':
			case 'd': sorter = date; break;
			case 'N':
			case 'n': sorter = numeric; break;
			case 'I':
			case 'i': sorter = caseinsensitive; break;
			case 'C':
			case 'c': sorter = casesensitive; break;
			case 'R':
			case 'r': sorter = random; break;
			default: sorter = none; break;
		}
		switch (sorter) {
			case none: {
				if (dir == '-') {
					std::reverse(idx.begin(), idx.end());
				}
			} break;
			case date: {
				if (dir == '+') {
					std::sort(idx.begin(), idx.end(), asc_date);
				} else {
					std::sort(idx.begin(), idx.end(), desc_date);
				}
			} break;
			case numeric: {
				if (dir == '+') {
					std::sort(idx.begin(), idx.end(), asc_numeric);
				} else {
					std::sort(idx.begin(), idx.end(), desc_numeric);
				}
			} break;
			case caseinsensitive: {
				if (dir == '+') {
					std::sort(idx.begin(), idx.end(), asc_caseinsensitive);
				} else {
					std::sort(idx.begin(), idx.end(), desc_caseinsensitive);
				}
			} break;
			case casesensitive: {
				if (dir == '+') {
					std::sort(idx.begin(), idx.end(), asc_casesensitive);
				} else {
					std::sort(idx.begin(), idx.end(), desc_casesensitive);
				}
			} break;
			case random: {
				std::random_device rd;
				std::mt19937 g(rd());
				std::shuffle(idx.begin(), idx.end(), g);
			}
		}
		Sort::offset = 0;
	}

}
