//
// Created by Ben on 2019-01-24.
//
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <random>
#include <variant>

#include "mt.h"
#include "support/Infix.h"
#include "support/Message.h"
#include "support/Fandr.h"
#include "support/Comparison.h"
#include "support/Convert.h"

#include "Internal.h"
#include "InternalInstance.h"

namespace mt {
    using namespace Support;
	using namespace std;

    Storage Internal::storage;
    Library Internal::library;
    Library Internal::cache;
    LStore Internal::lStore;


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

	plist Internal::toParms(const listType* orig,string sort) {
		vector<string> list;
		for(auto& i : *orig) {
			list.push_back(i);
		}
		doSort(list,sort);
		plist result;
		for(auto& i : list) {
			result.push_back({Text(i)});
		}
		return result;
	}

	void Internal::doTrace(Support::Messages& e,mstack& context) {
		for (auto& i : context) {
			if(i.first != nullptr) {
				std::string name = std::visit([](auto&& arg) -> std::string { return arg.name();},*(i.first));
				e << Message(trace,name);
			}
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
