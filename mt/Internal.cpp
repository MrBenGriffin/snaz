//
// Created by Ben on 2019-01-24.
//
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <random>
#include <variant>
#include <algorithm>

#include "mt.h"
#include "support/Definitions.h"
#include "support/Infix.h"
#include "support/Message.h"
#include "support/Fandr.h"
#include "support/Comparison.h"
#include "support/Convert.h"
#include "support/Timing.h"

#include "node/Node.h"
#include "node/Tree.h"
#include "node/Content.h"
#include "node/Metrics.h"
#include "content/Layout.h"
#include "content/Template.h"
#include "content/Editorial.h"

#include "Internal.h"
#include "InternalInstance.h"

namespace mt {
    using namespace Support;
	using namespace node;
	using namespace std;

    Storage Internal::storage;
    Library Internal::library;
    LStore Internal::lStore;
	Db::Connection* Internal::sql;

	Internal::Internal(std::string name, size_t min, size_t max) :
		Handler(),_name(std::move(name)),minParms(min),maxParms(max) {}

	bool Internal::inRange(size_t i) const {
		return (minParms <= i) && (i <= maxParms);
	}
//	struct iList : public Internal {

	std::string Internal::name() const {
		return _name;
	}

void Internal::startup(Messages& log,Db::Connection& _sql,buildKind kind) {
		Timing& times = Timing::t();
		sql = & _sql;
		if (times.show()) { times.set("Load Site Storage"); }
		storage.load(log,_sql,kind);
		if (times.show()) { times.use(log,"Load Site Storage"); }
	}

	void Internal::shutdown(Messages& log,Db::Connection&,buildKind kind) {
		storage.save(log,*sql,kind);
	}

	plist Internal::toParms(string basis,string cutter,string sort) const {
		vector<string> list;
		Support::tolist(list,basis,cutter);
		doSort(list,sort);
		plist result;
		for(auto& i : list) {
			MacroText parm;
			parm.add(i);
			result.emplace_back(std::move(parm));
		}
		return result;
	}

	plist Internal::toParms(const listType* orig,string sort,size_t maxSize) const {
		vector<string> list;
		long count = min(maxSize,list.size());
		for(auto& i : *orig) {
			if(count > 0) {
				list.push_back(i);
				count--;
			}
		}
		doSort(list,sort);
		plist result;
		for(auto& i : list) {
			MacroText parm;
			parm.add(i);
			result.emplace_back(std::move(parm));
		}
		return result;
	}

	plist Internal::toParms(vector<string>& list,string sort,size_t maxSize) const {
		if(maxSize < list.size()) {
			list.resize(maxSize);
		}
		doSort(list,sort);
		plist result;
		for(auto& i : list) {
			MacroText parm;
			parm.add(i);
			result.emplace_back(std::move(parm));
		}
		return result;
	}

	void Internal::doTrace(Messages& e,mstack& context) const {
		for (auto& i : context) {
			if(i.first != nullptr) {
				e << Message(trace,i.first->name());
			}
		}
		auto& x = context.back().second;
		if(x->metrics != nullptr  && (x->metrics)->currentTemplate != nullptr) {
			e << Message(trace,x->metrics->currentTemplate->name);
		} else {
			e << Message(trace,"nil");
		}

	}

/*
 +------+--------------+------------+----------+-----+
 | code | content      | taxonomy   | files    | int |
 +------+--------------+------------+----------+-----+
 |Ll    | linkref      | scope      | suffix   |  0  |
 |Tt    | title        | title      | script   |  1  |
 |Ss    | shorttitle   | shorttitle |  -       |  2  |
 |C     | tierref      | classcode  |  -       |  3  |
 |F     | basefilename | synonyms   |  -       |  4  |
 |B     | birthdate    | keywords   |  -       |  5  |
 |D     | deathdate    | descr      |  -       |  6  |
 |W     | treewalk     | treewalk   | treewalk |  -  |
 |[     | segment      |  -         |  -       |  -  |
 |R     | RANDOM       |  -         |  -       |  -  |
 +------+--------------+------------+----------+-----+
 */
	void Internal::doSort(Messages& e,vector<const Node*>& nodelist,string sortparm,mstack* context,Metrics* metrics) const {
		bool backwards = false;
		if (!sortparm.empty()) {
			if (sortparm[0] == '-') {
				backwards = true;
				sortparm.erase(0,1);
			} else { //remove the positive, if it is there.
				if (sortparm[0] == '+') {
					sortparm.erase(0,1);
				}
			}
			if ((sortparm[0] == '[') && (sortparm.length() > 2)) { 			// This is a segment reference. +[aaa]
				Messages suppress;
				Messages* errs;
				string seg_ref=sortparm.substr(1,sortparm.length()-2);       //take off 1 from the start + 1 from the end, =2
				if(seg_ref[0] == '*') {
					errs = &suppress;
					seg_ref.erase(0,1);
				} else {
					errs = &e;
				}
				vector<const Node *>nodes(nodelist); //used by segment sort..
				nodelist.clear();
				auto& edit = content::Editorial::e();
				for (auto* item : nodes) {
					auto *node = const_cast<node::Content *>(item->content());
					if (node && node->layout()) {
						const content::Segment *segment = node->layout()->segment(*errs, seg_ref);
						if (segment) { //"NLTBB"
							auto code = edit.get(*errs, node, segment);
							string result;
							if (!code.first) { //need to do IO as well..
								if (context && metrics) {
									metrics->push(node, segment);
									code.second->str(*errs, result, *context);
									metrics->pop();
								}
							} else {
								code.second->str(*errs, result, *context);
							}
							if (!result.empty()) {
								node->setComment(result);
								nodelist.push_back(node);
							}
						}
					}
				}
			}
		}
		if (!sortparm.empty()) {
			sortnodes(nodelist,backwards,sortparm[0]);
		} else {
			sortnodes(nodelist,backwards,'W');
		}
	}

	void Internal::sortnodes(vector<const Node *>& nodelist,bool backwards,char function) const {
		switch(function) {
			case('B'): sort(nodelist.begin(), nodelist.end(), sortbirth); break;
			case('b'): sort(nodelist.begin(), nodelist.end(), sortbirth); break;
			case('C'): sort(nodelist.begin(), nodelist.end(), sorttier); break;
			case('D'): sort(nodelist.begin(), nodelist.end(), sortdeath); break;
			case('d'): sort(nodelist.begin(), nodelist.end(), sortdeath); break;
			case('F'): sort(nodelist.begin(), nodelist.end(), sortfname); break;
			case('T'): sort(nodelist.begin(), nodelist.end(), sorttitle); break;
			case('t'): sort(nodelist.begin(), nodelist.end(), sorttitlei); break;
			case('L'): sort(nodelist.begin(), nodelist.end(), sortlinkref); break;
			case('l'): sort(nodelist.begin(), nodelist.end(), sortlinkrefi); break;
			case('S'): sort(nodelist.begin(), nodelist.end(), sortshorttitle); break;
			case('s'): sort(nodelist.begin(), nodelist.end(), sortshorttitlei); break;
			case('['): sort(nodelist.begin(), nodelist.end(), sortscratch); break;
			case('W'): sort(nodelist.begin(), nodelist.end(), sorttw); break;
			case('R'): {
				static std::random_device rng;
				static std::mt19937 urng(rng());
				std::shuffle(nodelist.begin(), nodelist.end(), urng);
			} break;
			default: break;
		}
		if(backwards) {
			reverse(nodelist.begin(), nodelist.end());
		}
	}

	void Internal::doSort(vector<string>& idx,string sortstr) const {
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
