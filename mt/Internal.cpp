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

#include "node/Tree.h"
#include "node/Content.h"
#include "content/Layout.h"

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

	void Internal::startup(Messages& log,Db::Connection& _sql,buildKind kind) {
		Timing& times = Timing::t();
		sql = & _sql;
		if (times.show()) { times.set("Load Site Storage"); }
		storage.load(log,_sql,kind);
		if (times.show()) { times.use(log,"Load Site Storage"); }
	}

	void Internal::shutdown(Messages& log,Db::Connection& sql,buildKind kind) {
		storage.save(log,sql,kind);
	}

	plist Internal::toParms(string basis,string cutter,string sort) {
		vector<string> list;
		Support::tolist(list,basis,cutter);
		doSort(list,sort);
		plist result;
		for(auto& i : list) {
			result.push_back({Text(i)});
		}
		return result;
	}

	plist Internal::toParms(const listType* orig,string sort,size_t maxSize) {
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
			result.push_back({Text(i)});
		}
		return result;
	}

	plist Internal::toParms(vector<string>& list,string sort,size_t maxSize) {
		if(maxSize < list.size()) {
			list.resize(maxSize);
		}
		doSort(list,sort);
		plist result;
		for(auto& i : list) {
			result.push_back({Text(i)});
		}
		return result;
	}

	plist Internal::toNodeParms(Messages& e,vector<string>& list,string sort,size_t maxSize) {
		if(maxSize < list.size()) {
			list.resize(maxSize);
		}
		vector<const Node*> nodes;
		for(auto& i : list) {
			const Node* node = node::Content::editorial.byPath(e,i);
			if(node != nullptr) {
				nodes.push_back(node);
			}
		}
		doSort(e,nodes,sort);
		plist result;
		for(auto& i : nodes) {
			result.push_back({Text(i->ids())});
		}
		return result;
	}


	void Internal::doTrace(Messages& e,mstack& context) {
		for (auto& i : context) {
			if(i.first != nullptr) {
				std::string name = std::visit([](auto&& arg) -> std::string { return arg.name();},*(i.first));
				e << Message(trace,name);
			}
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
	void Internal::doSort(Messages& e,vector<const Node*>& nodelist,string sortparm) {
		bool ignore_non_existent = false;
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
				string seg_ref=sortparm.substr(1,sortparm.length()-2);       //take off 1 from the start + 1 from the end, =2
				if(seg_ref[0] == '*') {
					ignore_non_existent = true;
					seg_ref.erase(0,1);
				}
				vector<Node *> deadlist;
				e << Message(error,"node sort by segment reference is not currently supported.");
//				for (auto i : nodelist) {
//					pairtype cp(i->id(),seg_ref);
//					content_map_type::iterator it = ContentVal::content_map.find(cp);
//					if(it != ContentVal::content_map.end()) {
//						i->scratch(it->second[0]); //set the scratch field to the contents of the segment.
//					} else {
//						if(!ignore_non_existent) {
//							output << message(warn,"Node " + i->linkref() + " has no segment " + seg_ref + ". It will be removed. Suppress this message by adding a * as in '[*" + seg_ref + "]' in the sort parameter.");
//						}
//						deadlist.push_back(i);
//					}
//				}
				if(! deadlist.empty()) {
					vector<const Node *> goodNodes;
					set_difference(nodelist.begin(), nodelist.end(), deadlist.begin(), deadlist.end(), back_inserter(goodNodes));
					nodelist = goodNodes;
				}
			}
		}
		if (!sortparm.empty()) {
			sortnodes(nodelist,backwards,sortparm[0]);
		} else {
			sortnodes(nodelist,backwards,'W');
		}
	}

	void Internal::sortnodes(vector<const Node *>& nodelist,bool backwards,char function) {
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
